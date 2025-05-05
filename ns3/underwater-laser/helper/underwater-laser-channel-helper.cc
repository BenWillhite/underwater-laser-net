#include "underwater-laser-channel-helper.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/object-factory.h"

#include "underwater-laser-channel.h"
#include "ns3/underwater-laser-propagation-loss-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserChannelHelper");

UnderwaterLaserChannelHelper::UnderwaterLaserChannelHelper ()
{
  NS_LOG_FUNCTION (this);
  m_channelFactory.SetTypeId ("ns3::UnderwaterLaserChannel");
  m_lossModelFactory.SetTypeId ("ns3::UnderwaterLaserPropagationLossModel");
}

UnderwaterLaserChannelHelper::~UnderwaterLaserChannelHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserChannelHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  // Decide which factory the attribute belongs to. For this example,
  // we'll assume most attributes refer to the UnderwaterLaserPropagationLossModel
  // or the channel. If you want to differentiate, you can parse the name or
  // create separate SetChannelAttribute() / SetLossAttribute().
  if (name.find ("AlphaCsv") != std::string::npos ||
      name.find ("BeamDivergence") != std::string::npos)
    {
      m_lossModelFactory.Set (name, value);
    }
  else
    {
      m_channelFactory.Set (name, value);
    }
}

Ptr<UnderwaterLaserChannel>
UnderwaterLaserChannelHelper::Create ()
{
  NS_LOG_FUNCTION (this);
  Ptr<UnderwaterLaserChannel> channel = m_channelFactory.Create<UnderwaterLaserChannel> ();

  // create the propagation loss model
  Ptr<UnderwaterLaserPropagationLossModel> lossModel =
    m_lossModelFactory.Create<UnderwaterLaserPropagationLossModel> ();

  // associate them
  channel->SetPropagationLossModel (lossModel);

  return channel;
}

} // namespace ns3
