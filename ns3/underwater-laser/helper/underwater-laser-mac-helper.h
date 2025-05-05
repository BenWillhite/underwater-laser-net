#ifndef UNDERWATER_LASER_MAC_HELPER_H
#define UNDERWATER_LASER_MAC_HELPER_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/attribute.h"

namespace ns3 {

class UnderwaterLaserNetDevice;

/**
 * \brief (Optional) A minimal helper that sets up a CSMA/CA MAC.  
 *        If your lasers are highly directional, you may skip this.
 */
class UnderwaterLaserMacHelper
{
public:
  UnderwaterLaserMacHelper ();
  ~UnderwaterLaserMacHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * \brief Configure the NetDevice's MAC side
   */
  void CreateMac (Ptr<UnderwaterLaserNetDevice> device);

private:
  // For demonstration, no real CSMA logic. If you want a real MAC, implement a separate object here.
};

} // namespace ns3

#endif // UNDERWATER_LASER_MAC_HELPER_H
