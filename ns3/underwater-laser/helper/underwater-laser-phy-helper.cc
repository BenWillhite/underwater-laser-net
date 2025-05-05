#include "ns3/underwater-laser-phy-helper.h"
#include "ns3/log.h"
#include "ns3/object-factory.h"
#include "ns3/pointer.h"

#include "ns3/underwater-laser-error-rate-model.h"
#include "ns3/underwater-laser-net-device.h"
#include "ns3/underwater-laser-channel.h"
#include "ns3/underwater-laser-propagation-loss-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserPhyHelper");

UnderwaterLaserPhyHelper::UnderwaterLaserPhyHelper ()
{
  NS_LOG_FUNCTION (this);
  m_errorModelFactory.SetTypeId ("ns3::UnderwaterLaserErrorRateModel");
}

UnderwaterLaserPhyHelper::~UnderwaterLaserPhyHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserPhyHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  m_errorModelFactory.Set (name, value);
}

void
UnderwaterLaserPhyHelper::CreatePhy (Ptr<UnderwaterLaserNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  NS_ASSERT (device);

  Ptr<UnderwaterLaserErrorRateModel> errorModel =
    m_errorModelFactory.Create<UnderwaterLaserErrorRateModel> ();
  NS_ASSERT (errorModel);

  // 2) pull out our channel and loss-model (now that we have their full definitions)
  Ptr<UnderwaterLaserChannel> channel =
    DynamicCast<UnderwaterLaserChannel> (device->GetChannel ());
  NS_ASSERT (channel);
  Ptr<UnderwaterLaserPropagationLossModel> loss = channel->GetPropagationLossModel ();

  // 3) attach to the device
  device->Attach (channel, loss, errorModel, 0);
}

} // namespace ns3
