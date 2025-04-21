#ifndef UNDERWATER_LASER_ERROR_MODEL_H
#define UNDERWATER_LASER_ERROR_MODEL_H

#include "ns3/error-model.h"
#include "ns3/packet.h"
#include "ns3/random-variable-stream.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace ns3 {

class UnderwaterLaserErrorModel : public ErrorModel
{
public:
  static TypeId GetTypeId(void)
  {
    static TypeId tid = TypeId("ns3::UnderwaterLaserErrorModel")
      .SetParent<ErrorModel>()
      .SetGroupName("Network")
      .AddConstructor<UnderwaterLaserErrorModel>();
    return tid;
  }

  UnderwaterLaserErrorModel()
    : m_rand(CreateObject<UniformRandomVariable>()) {}
  virtual ~UnderwaterLaserErrorModel() {}

  // Load a CSV whose first column is SNR_dB, second column is PER
  void LoadChannelTrace(const std::string &filename)
  {
    std::ifstream in(filename);
    if (!in.is_open()) {
      NS_FATAL_ERROR("Could not open PER table file: " << filename);
    }
    std::string line;
    std::getline(in, line); // skip header
    while (std::getline(in, line)) {
      std::istringstream iss(line);
      std::string tok;
      double snr, per;
      if (!std::getline(iss, tok, ',')) continue;
      snr = std::stod(tok);
      if (!std::getline(iss, tok, ',')) continue;
      per = std::stod(tok);
      m_perTable.emplace_back(snr, per);
    }
    std::sort(m_perTable.begin(), m_perTable.end(),
              [](auto &a, auto &b){ return a.first < b.first; });
  }

private:
  // Decide drop‐vs‐pass by sampling Bernoulli(per)
  virtual bool DoCorrupt(Ptr<Packet> /*p*/) override
  {
    double snrDb = LookupCurrentSnr(Simulator::Now().GetSeconds());
    double per   = LookupPer(snrDb);
    return (m_rand->GetValue() < per);
  }

  virtual void DoReset() override {}

  double LookupPer(double snrDb) const
  {
    if (m_perTable.empty()) return 1.0;
    if (snrDb <= m_perTable.front().first) return m_perTable.front().second;
    if (snrDb >= m_perTable.back().first)  return m_perTable.back().second;
    for (size_t i = 1; i < m_perTable.size(); ++i) {
      auto [s0,p0] = m_perTable[i-1];
      auto [s1,p1] = m_perTable[i];
      if (snrDb < s1) {
        double frac = (snrDb - s0)/(s1 - s0);
        return p0 + frac*(p1 - p0);
      }
    }
    return m_perTable.back().second;
  }

  double LookupCurrentSnr(double /*time_s*/) const
  {
    if (m_perTable.empty()) return -100.0;
    return m_perTable[m_perTable.size()/2].first;
  }

  std::vector<std::pair<double,double>> m_perTable;
  Ptr<UniformRandomVariable>            m_rand;
};

} // namespace ns3

#endif // UNDERWATER_LASER_ERROR_MODEL_H
