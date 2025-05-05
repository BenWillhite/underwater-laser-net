#include "underwater-laser-channel.h"
#include "ns3/simulator.h"
#include "underwater-laser-net-device.h"
#include "underwater-laser-propagation-loss-model.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/object-factory.h"

#include <iostream> // for std::cout

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserChannel);

TypeId
UnderwaterLaserChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserChannel")
    .SetParent<Channel> ()
    .SetGroupName ("Underwater")
    .AddConstructor<UnderwaterLaserChannel> ()
    .AddAttribute ("TxPowerDbm",
                   "Transmit power in dBm for all transmissions.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&UnderwaterLaserChannel::SetTxPowerDbm,
                                       &UnderwaterLaserChannel::GetTxPowerDbm),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

UnderwaterLaserChannel::UnderwaterLaserChannel ()
  : m_txPowerDbm (0.0)
{
  std::cout << "[UnderwaterLaserChannel] Constructor" << std::endl;
}

UnderwaterLaserChannel::~UnderwaterLaserChannel ()
{
  std::cout << "[UnderwaterLaserChannel] Destructor" << std::endl;
}

void
UnderwaterLaserChannel::SetPropagationLossModel (Ptr<UnderwaterLaserPropagationLossModel> loss)
{
  m_lossModel = loss;
}

Ptr<UnderwaterLaserPropagationLossModel>
UnderwaterLaserChannel::GetPropagationLossModel () const
{
  return m_lossModel;
}

void
UnderwaterLaserChannel::SetTxPowerDbm (double txPowerDbm)
{
  m_txPowerDbm = txPowerDbm;
}

double
UnderwaterLaserChannel::GetTxPowerDbm () const
{
  return m_txPowerDbm;
}

void
UnderwaterLaserChannel::AddDevice (Ptr<NetDevice> dev)
{
  m_devices.push_back(dev);
}

std::size_t
UnderwaterLaserChannel::GetNDevices () const
{
  return m_devices.size();
}

Ptr<NetDevice>
UnderwaterLaserChannel::GetDevice (std::size_t i) const
{
  return m_devices[i];
}

void
UnderwaterLaserChannel::TransmitStart (Ptr<Packet> packet,
                                       Address src,
                                       Address dst,
                                       uint16_t protocol,
                                       Ptr<UnderwaterLaserNetDevice> sender)
{
  double txPowerDbm = m_txPowerDbm;

  std::cout << "[UnderwaterLaserChannel] TransmitStart; packetSize=" << packet->GetSize ()
            << " TxPowerDbm=" << txPowerDbm << std::endl;

  // Deliver to all other NetDevices
  for (uint32_t i = 0; i < GetNDevices (); ++i)
    {
      Ptr<NetDevice> nd = GetDevice (i);
      Ptr<UnderwaterLaserNetDevice> dev = DynamicCast<UnderwaterLaserNetDevice> (nd);
      if (dev && dev != sender)
        {
          double rxPowerDbm = txPowerDbm;
          if (m_lossModel)
            {
              rxPowerDbm = m_lossModel->CalcRxPower (txPowerDbm,
                                                     sender->GetMobility (),
                                                     dev->GetMobility ());
            }

          std::cout << "[UnderwaterLaserChannel] Delivering to Dev=" << dev->GetIfIndex()
                    << " => rxPowerDbm=" << rxPowerDbm << std::endl;

          dev->ReceiveFromChannel (packet->Copy (), rxPowerDbm);
        }
    }
}

} // namespace ns3
