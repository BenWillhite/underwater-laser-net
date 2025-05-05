// underwater-laser-helper.h

#ifndef UNDERWATER_LASER_HELPER_H
#define UNDERWATER_LASER_HELPER_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"

#include "underwater-laser-channel-helper.h"
#include "ns3/underwater-laser-propagation-loss-model.h"
#include "ns3/underwater-laser-phy-helper.h"
#include "ns3/underwater-laser-mac-helper.h"
#include "ns3/mobility-model.h"

namespace ns3 {

class UnderwaterLaserRateTable;

/**
 * \brief Top-level helper that wires together the channel, PHY, MAC, and NetDevices.
 */
class UnderwaterLaserHelper : public Object
{
public:
  static TypeId GetTypeId (void);

  UnderwaterLaserHelper ();
  ~UnderwaterLaserHelper ();

  /**
   * \brief Let the user set channel or phy or mac attributes,
   *        which pass through to the underlying factories.
   */
  void SetChannelAttribute (std::string name, const AttributeValue &value);
  void SetPhyAttribute     (std::string name, const AttributeValue &value);
  void SetMacAttribute     (std::string name, const AttributeValue &value);

  /**
   * \brief Install devices on a set of nodes.
   */
  NetDeviceContainer Install (NodeContainer c);

  /**
   * \brief Provide a RateTable CSV path to all devices
   */
  void SetRateTableCsv (std::string csvPath);

private:
  // underlying factory helpers
  UnderwaterLaserChannelHelper  m_channelHelper;
  UnderwaterLaserPhyHelper      m_phyHelper;
  UnderwaterLaserMacHelper      m_macHelper;

  // channel + rate table shared by all devices
  Ptr<UnderwaterLaserChannel>   m_channel;
  Ptr<UnderwaterLaserRateTable> m_rateTable;

  bool                          m_channelCreated;
  std::string                   m_rateTableCsv;

  // --- NEW: user-configurable propagation-loss parameters ---
  std::string                   m_alphaCsv;
  double                        m_divergenceRad;
};

} // namespace ns3

#endif // UNDERWATER_LASER_HELPER_H
