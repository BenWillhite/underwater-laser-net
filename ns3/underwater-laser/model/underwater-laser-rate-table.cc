#include "underwater-laser-rate-table.h"
#include "ns3/log.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserRateTable");
NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserRateTable);

TypeId
UnderwaterLaserRateTable::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserRateTable")
    .SetParent<Object> ()
    .SetGroupName ("UnderwaterLaser")  // or another group
    .AddConstructor<UnderwaterLaserRateTable> ();
  return tid;
}

UnderwaterLaserRateTable::UnderwaterLaserRateTable ()
{
  NS_LOG_FUNCTION (this);
}

UnderwaterLaserRateTable::~UnderwaterLaserRateTable ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserRateTable::Load (const std::string &filename)
{
  NS_LOG_FUNCTION (this << filename);
  m_table.clear ();

  std::ifstream in (filename.c_str ());
  if (!in.is_open ())
    {
      NS_LOG_WARN ("Could not open distance->rate CSV: " << filename);
      return;
    }

  std::string line;
  // (Optional) skip a header line
  std::getline (in, line);

  while (std::getline (in, line))
    {
      std::istringstream iss (line);
      double dist, rate;
      char comma;
      if (!(iss >> dist >> comma >> rate))
        {
          NS_LOG_WARN ("Skipping invalid line: " << line);
          continue;
        }
      m_table.push_back (std::make_pair (dist, rate));
    }
  in.close ();

  // sort by distance ascending
  std::sort (m_table.begin (), m_table.end (),
             [] (auto &a, auto &b) {
               return a.first < b.first;
             });

  NS_LOG_INFO ("Loaded rate table with " << m_table.size () << " entries");
}

double
UnderwaterLaserRateTable::GetMaxRate (double distance) const
{
  if (m_table.empty ())
    {
      // fallback if table is empty
      return 1e6; // 1 Mbps
    }

  // clamp at extremes
  if (distance <= m_table.front ().first)
    {
      return m_table.front ().second;
    }
  if (distance >= m_table.back ().first)
    {
      return m_table.back ().second;
    }

  // else linear interpolation
  for (size_t i = 1; i < m_table.size (); ++i)
    {
      double d0 = m_table[i-1].first;
      double r0 = m_table[i-1].second;
      double d1 = m_table[i].first;
      double r1 = m_table[i].second;

      if (distance <= d1)
        {
          double frac = (distance - d0) / (d1 - d0);
          return r0 + frac * (r1 - r0);
        }
    }

  // fallback if we somehow never returned
  return m_table.back ().second;
}

} // namespace ns3
