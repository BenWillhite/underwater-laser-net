#include "underwater-laser-error-rate-model.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/type-id.h"
#include "ns3/attribute.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserErrorRateModel");
NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserErrorRateModel);

TypeId
UnderwaterLaserErrorRateModel::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserErrorRateModel")
    .SetParent<Object> ()
    .SetGroupName ("UnderwaterLaser")
    .AddConstructor<UnderwaterLaserErrorRateModel> ()
    .AddAttribute ("TraceFilename",
                   "Path to the SNR→PER CSV file (PER data by default)",
                   StringValue (""),
                   MakeStringAccessor (&UnderwaterLaserErrorRateModel::SetTraceFilename,
                                      &UnderwaterLaserErrorRateModel::GetTraceFilename),
                   MakeStringChecker ());
  return tid;
}

UnderwaterLaserErrorRateModel::UnderwaterLaserErrorRateModel ()
  : m_isBerData (false)
{
  NS_LOG_FUNCTION (this);
}

UnderwaterLaserErrorRateModel::~UnderwaterLaserErrorRateModel ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserErrorRateModel::Load (const std::string &filename, bool isBerData)
{
  NS_LOG_FUNCTION (this << filename << isBerData);
  m_isBerData = isBerData;
  m_table.clear ();

  std::ifstream in (filename.c_str ());
  if (!in.is_open ())
    {
      NS_LOG_WARN ("Could not open SNR->(PER/BER) CSV: " << filename);
      return;
    }

  std::string line;
  std::getline (in, line); // skip header

  while (std::getline (in, line))
    {
      std::istringstream iss (line);
      double snr, val;
      char comma;
      if (!(iss >> snr >> comma >> val))
        {
          NS_LOG_WARN ("Skipping invalid line: " << line);
          continue;
        }
      m_table.emplace_back (snr, val);
    }
  in.close ();

  std::sort (m_table.begin (), m_table.end (),
             [] (auto &a, auto &b) { return a.first < b.first; });

  NS_LOG_INFO ("Loaded " << m_table.size ()
               << " points from " << filename
               << " (isBerData=" << m_isBerData << ")");
}

void
UnderwaterLaserErrorRateModel::SetTraceFilename (const std::string &filename)
{
  m_traceFilename = filename;
  Load (filename, false);
}

std::string
UnderwaterLaserErrorRateModel::GetTraceFilename () const
{
  return m_traceFilename;
}

double
UnderwaterLaserErrorRateModel::InterpolateRawValue (double snrDb)
{
  if (m_table.empty ())
    {
      return 1.0;
    }
  if (snrDb <= m_table.front ().first) return m_table.front ().second;
  if (snrDb >= m_table.back ().first)  return m_table.back ().second;

  for (size_t i = 1; i < m_table.size (); ++i)
    {
      double s0 = m_table[i-1].first, v0 = m_table[i-1].second;
      double s1 = m_table[i].first,   v1 = m_table[i].second;
      if (snrDb <= s1)
        {
          double frac = (snrDb - s0) / (s1 - s0);
          return v0 + frac * (v1 - v0);
        }
    }
  return m_table.back ().second;
}

double
UnderwaterLaserErrorRateModel::ComputeChunkPer (double snrDb, uint64_t nbits)
{
  double raw = InterpolateRawValue (snrDb);
  if (!m_isBerData)
    {
      return raw;
    }
  double ber = raw;
  double chunkPer = 1.0 - std::pow (1.0 - ber, static_cast<double>(nbits));
  return std::clamp (chunkPer, 0.0, 1.0);
}

double
UnderwaterLaserErrorRateModel::DoGetChunkSuccessRate (double snrDb, uint64_t nbits)
{
  double per = ComputeChunkPer (snrDb, nbits);
  return 1.0 - per;
}

} // namespace ns3
