#ifndef UNDERWATER_LASER_PHY_HELPER_H
#define UNDERWATER_LASER_PHY_HELPER_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"  // <-- Needed to fix "ObjectFactory is undefined"

#include "ns3/underwater-laser-error-rate-model.h"
#include "ns3/underwater-laser-channel.h"
#include "ns3/underwater-laser-propagation-loss-model.h"

namespace ns3 {

class UnderwaterLaserErrorRateModel;
class UnderwaterLaserNetDevice;

/**
 * \brief A minimal "PHY helper" that configures the NetDevice's ErrorRateModel, detector parameters, etc.
 */
class UnderwaterLaserPhyHelper
{
public:
  UnderwaterLaserPhyHelper ();
  ~UnderwaterLaserPhyHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * \brief Instantiate and attach an UnderwaterLaserErrorRateModel to the device
   */
  void CreatePhy (Ptr<UnderwaterLaserNetDevice> device);

private:
  ObjectFactory m_errorModelFactory;
};

} // namespace ns3

#endif // UNDERWATER_LASER_PHY_HELPER_H
