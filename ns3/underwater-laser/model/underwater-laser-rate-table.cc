/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "underwater-laser-rate-table.h"
#include "ns3/log.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cctype>

namespace ns3 {

// simple trim helper (now actually used)
static inline std::string
Trim (const std::string &s)
{
  auto wsfront = std::find_if_not(s.begin(), s.end(),
                                  [](int c){ return std::isspace(c); });
  auto wsback  = std::find_if_not(s.rbegin(), s.rend(),
                                  [](int c){ return std::isspace(c); }).base();
  return (wsback <= wsfront
          ? std::string()
          : std::string(wsfront, wsback));
}

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserRateTable");
NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserRateTable);

TypeId
UnderwaterLaserRateTable::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserRateTable")
    .SetParent<Object> ()
    .SetGroupName ("UnderwaterLaser")
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

    std::cout << "[RateTable::Load] called with file=" << filename << std::endl;

  NS_LOG_FUNCTION (this << filename);
  m_table.clear ();

  std::ifstream in (filename);
  if (!in.is_open ())
    {
      NS_LOG_WARN ("Could not open distance->rate CSV: " << filename);
      return;
    }

  const double desiredLambdaNm = 532.0;  // ← only keep rows for 532 nm

  std::string line;
  std::getline (in, line); // skip header
  bool fromCleanCsv = false;      // <-- add this

  while (std::getline (in, line))
    {
      // 1) tokenize + trim
      std::vector<std::string> tok;
      std::stringstream        ss(line);
      std::string              field;
      while (std::getline (ss, field, ','))
        {
          tok.push_back( Trim(field) );
        }

      // DEBUG: dump every token we just read
      NS_LOG_INFO("RateTable CSV row: \"" << line << "\"");
      for (size_t i = 0; i < tok.size(); ++i)
        {
          NS_LOG_INFO("  tok[" << i << "] = \"" << tok[i] << "\"");
        }

      // 2) must have at least 5 fields, must be OFDM+QAM, *and* λ==532
      /* -----------------------------------------------------------
        * Two CSV layouts are now accepted:
        *   (A) “full” table: λ_nm , distance_m , mod_scheme , … , rate_Mbps , …
        *   (B) “clean” table: distance_m , rate_Mbps       (your Python output)
        * --------------------------------------------------------- */

        double dist     = 0.0;
        double rateMbps = 0.0;

        if (tok.size () == 2)            // layout B
        {
            dist     = std::stod (tok[0]);
            rateMbps = std::stod (tok[1]);
            fromCleanCsv = true;
        }
        else if (tok.size () >= 5)       // layout A
        {
            if (tok[2] != "OFDM+QAM")   { continue; }

            double lambdaCsv = std::stod (tok[0]);
            if (std::abs (lambdaCsv - desiredLambdaNm) > 1e-6) { continue; }

            dist     = std::stod (tok[1]);   // distance_m
            rateMbps = std::stod (tok[4]);   // rate_Mbps
            fromCleanCsv = false;
        }
        else                              // anything else → skip
        {
            continue;
        }

        double rateBps = rateMbps * 1e6;   // bits /sec
        m_table.emplace_back (dist, rateBps);

    }
  in.close ();

  std::stable_sort (m_table.begin (), m_table.end (),
                  [] (auto &a, auto &b) { return a.first < b.first; });
                  
        std::string flavour = fromCleanCsv ? "clean-CSV" : "full-CSV";

        // Explicitly print out all loaded entries to verify correctness
        NS_LOG_INFO ("Loaded 532 nm rate table with " << m_table.size () << " entries (" << flavour << ")");
        for (auto &entry : m_table)
        {
            NS_LOG_INFO("  Entry loaded: distance=" << entry.first << " m, rate=" << entry.second << " bps");
        }

}

double
UnderwaterLaserRateTable::GetMaxRate (double distance) const
{
  if (m_table.empty ())
    {
      // fallback if table is empty: at least 1 Mbps
      //return 1e6;
    }

  double rawRateBps = 0.0;

  // clamp at extremes
  if (distance <= m_table.front ().first)
    {
      rawRateBps = m_table.front ().second;
    }
  else if (distance >= m_table.back ().first)
    {
      rawRateBps = m_table.back ().second;
    }
  else
    {
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
              rawRateBps = r0 + frac * (r1 - r0);
              NS_LOG_INFO("Interpolating rate for distance=" << distance
                           << "m: using points (" << d0 << "m, " << r0 << "bps) to ("
                           << d1 << "m, " << r1 << "bps), interpolated rate="
                           << rawRateBps << " bps");
              break;
          }
        }
    }

  // never return zero (or negative)—clamp to at least 1 bps
  return std::max(rawRateBps, 1.0);
}

} // namespace ns3
