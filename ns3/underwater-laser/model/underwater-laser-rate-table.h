#ifndef UNDERWATER_LASER_RATE_TABLE_H
#define UNDERWATER_LASER_RATE_TABLE_H

#include <vector>
#include <string>
#include <utility>
#include "ns3/object.h"

namespace ns3 {

/**
 * \class UnderwaterLaserRateTable
 * \brief A table mapping distance (m) to maximum data rate (bits/s).
 */
class UnderwaterLaserRateTable : public ns3::Object
{
public:
  static TypeId GetTypeId ();
  UnderwaterLaserRateTable ();
  virtual ~UnderwaterLaserRateTable ();

  void   Load (const std::string &filename);
  double GetMaxRate (double distance) const;

private:
  std::vector<std::pair<double,double>> m_table; // (distance, rate_bps)
};

} // namespace ns3

#endif // UNDERWATER_LASER_RATE_TABLE_H
