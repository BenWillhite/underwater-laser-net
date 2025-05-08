#include "underwater-laser-propagation-loss-model.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/simulator.h"
#include "ns3/mobility-model.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <iostream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserPropagationLossModel);

TypeId
UnderwaterLaserPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserPropagationLossModel")
    .SetParent<PropagationLossModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<UnderwaterLaserPropagationLossModel> ()
    .AddAttribute ("AlphaCsv",
                   "Path to the alpha_depth_time.csv file (time,z_mid,alpha_m^-1).",
                   StringValue ("scratch/alpha_depth_time.csv"),
                   MakeStringAccessor (&UnderwaterLaserPropagationLossModel::SetAlphaCsv),
                   MakeStringChecker ())
    .AddAttribute ("BeamDivergence",
                   "Half-angle divergence in radians",
                   DoubleValue (0.001),
                   MakeDoubleAccessor (&UnderwaterLaserPropagationLossModel::SetBeamDivergence),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

UnderwaterLaserPropagationLossModel::UnderwaterLaserPropagationLossModel ()
  : m_divergenceRad (0.001),
    m_w0 (0.001)
{
  //std::cout << "[UWL-PropModel] Constructor" << std::endl;
}

UnderwaterLaserPropagationLossModel::~UnderwaterLaserPropagationLossModel ()
{
  //std::cout << "[UWL-PropModel] Destructor" << std::endl;
}

void
UnderwaterLaserPropagationLossModel::SetAlphaCsv (const std::string &alphaCsv)
{
    //std::cout << "[PropagationLossModel] Loading Alpha CSV file=" << alphaCsv << std::endl;
    //std::cout << "[UWL-PropModel] SetAlphaCsv: " << alphaCsv << std::endl;
  m_alphaCsvFilename = alphaCsv;
  m_alphaTable2D.clear ();

  std::ifstream in (alphaCsv);
  if (!in.is_open ())
    {
      std::cerr << "[UWL-PropModel] WARNING: Could not open alpha_depth_time CSV: "
                << alphaCsv << std::endl;
      return;
    }

  // Skip header
  std::string line;
  std::getline (in, line);

  std::map<double, std::vector<DepthAlpha>> tempMap;
  while (std::getline (in, line))
    {
      std::istringstream iss (line);
      double tVal, zVal, alphaVal;
      char comma1, comma2;
      if (!(iss >> tVal >> comma1 >> zVal >> comma2 >> alphaVal))
        {
          continue; // skip malformed lines
        }

      // Flip sign if negative
      DepthAlpha da;
      da.zMid  = zVal;
      da.alpha = std::fabs(alphaVal);
      tempMap[tVal].push_back (da);
    }
  in.close ();

  for (auto &kv : tempMap)
    {
      auto &depthVec = kv.second;
      std::sort (depthVec.begin (), depthVec.end (),
                 [] (const DepthAlpha &a, const DepthAlpha &b) {
                   return a.zMid < b.zMid;
                 });
      m_alphaTable2D.push_back (std::make_pair (kv.first, depthVec));
    }
  std::sort (m_alphaTable2D.begin (), m_alphaTable2D.end (),
             [] (auto &a, auto &b) { return a.first < b.first; });

  std::cout << "[UWL-PropModel] Loaded alpha table with "
            << m_alphaTable2D.size() << " distinct times." << std::endl;
}

void
UnderwaterLaserPropagationLossModel::SetBeamDivergence (double divergenceRad)
{
  m_divergenceRad = divergenceRad;
  std::cout << "[UWL-PropModel] BeamDivergence set to " << m_divergenceRad << " rad" << std::endl;
}

double
UnderwaterLaserPropagationLossModel::GetAlphaAtTimeAndDepth (Time t, double z) const
{
  if (m_alphaTable2D.empty ())
    {
      return 0.0;
    }

  double now = t.GetSeconds ();
  if (now <= m_alphaTable2D.front ().first)
    {
      return InterpDepth (m_alphaTable2D.front ().second, z);
    }
  if (now >= m_alphaTable2D.back ().first)
    {
      return InterpDepth (m_alphaTable2D.back ().second, z);
    }

  for (size_t i = 1; i < m_alphaTable2D.size (); ++i)
    {
      double t0 = m_alphaTable2D[i-1].first;
      double t1 = m_alphaTable2D[i].first;
      if (now <= t1)
        {
          double alpha0 = InterpDepth (m_alphaTable2D[i-1].second, z);
          double alpha1 = InterpDepth (m_alphaTable2D[i].second, z);
          double frac = (now - t0) / (t1 - t0);
          return alpha0 + frac * (alpha1 - alpha0);
        }
    }
  return InterpDepth (m_alphaTable2D.back ().second, z);
}

double
UnderwaterLaserPropagationLossModel::InterpDepth (const std::vector<DepthAlpha> &vec, double z) const
{
  if (vec.empty ())
    {
      return 0.0;
    }
  if (z <= vec.front ().zMid)
    {
      return vec.front ().alpha;
    }
  if (z >= vec.back ().zMid)
    {
      return vec.back ().alpha;
    }

  for (size_t i = 1; i < vec.size(); ++i)
    {
      double z0 = vec[i-1].zMid;
      double a0 = vec[i-1].alpha;
      double z1 = vec[i].zMid;
      double a1 = vec[i].alpha;
      if (z <= z1)
        {
          double frac = (z - z0) / (z1 - z0);
          return a0 + frac*(a1 - a0);
        }
    }
  return vec.back ().alpha;
}

double
UnderwaterLaserPropagationLossModel::GetBeamGain (double distance) const
{
  double beamRadius = m_w0 + m_divergenceRad * distance;
  double beamArea = M_PI * beamRadius * beamRadius;
  double A_rx = 1e-5; // Hard-coded
  return (A_rx / beamArea);
}

double
UnderwaterLaserPropagationLossModel::DoCalcRxPower (double txPowerDbm,
                                                    Ptr<MobilityModel> a,
                                                    Ptr<MobilityModel> b) const
{
  double txPowerMw = std::pow(10.0, txPowerDbm / 10.0);
  Vector posA = a->GetPosition ();
  Vector posB = b->GetPosition ();
  double dist = CalculateDistance (posA, posB);

  double zMid = 0.5 * (posA.z + posB.z);
  double alpha = GetAlphaAtTimeAndDepth (Simulator::Now (), zMid);

  double attenuation = std::exp(-alpha * dist);
  double gain = GetBeamGain(dist);

  double rxPowerMw = txPowerMw * attenuation * gain;
  double rxPowerDbm = 10.0 * std::log10(rxPowerMw + 1e-20);

  // Minimal debug
  std::cout << "[UWL-PropModel] DoCalcRxPower: dist=" << dist
            << " alpha=" << alpha
            << " => rxPowerDbm=" << rxPowerDbm << std::endl;

  return rxPowerDbm;
}

int64_t
UnderwaterLaserPropagationLossModel::DoAssignStreams (int64_t stream)
{
  return 0; 
}

} // namespace ns3
