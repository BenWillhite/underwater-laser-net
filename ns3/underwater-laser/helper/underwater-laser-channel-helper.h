#ifndef UNDERWATER_LASER_CHANNEL_HELPER_H
#define UNDERWATER_LASER_CHANNEL_HELPER_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"


namespace ns3 {

class UnderwaterLaserChannel;
class UnderwaterLaserPropagationLossModel;

/**
 * \brief Helper to create an UnderwaterLaserChannel, attach a PropagationLossModel, etc.
 */
class UnderwaterLaserChannelHelper
{
public:
  UnderwaterLaserChannelHelper ();
  ~UnderwaterLaserChannelHelper ();

  /**
   * \brief Set an attribute to be configured in the channel or model
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * \brief Actually create the channel
   */
  Ptr<UnderwaterLaserChannel> Create ();

private:
  ObjectFactory m_channelFactory;
  ObjectFactory m_lossModelFactory;
};

} // namespace ns3

#endif // UNDERWATER_LASER_CHANNEL_HELPER_H
