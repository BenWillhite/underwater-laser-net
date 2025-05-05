#include "underwater-laser-mac-helper.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserMacHelper");

UnderwaterLaserMacHelper::UnderwaterLaserMacHelper ()
{
  NS_LOG_FUNCTION (this);
}

UnderwaterLaserMacHelper::~UnderwaterLaserMacHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserMacHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  // Not implemented. For a real MAC, you'd store attributes in an ObjectFactory.
}

void
UnderwaterLaserMacHelper::CreateMac (Ptr<UnderwaterLaserNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  // Not implemented. If you want e.g. a random backoff, you'd do so here.
}

} // namespace ns3
