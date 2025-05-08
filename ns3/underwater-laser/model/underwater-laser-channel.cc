#include "underwater-laser-channel.h"
#include "ns3/simulator.h"
#include "underwater-laser-net-device.h"
#include "underwater-laser-propagation-loss-model.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/node.h"
#include "ns3/data-rate.h"
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
    .AddAttribute ("MinRate",
                   "Minimum sustainable data-rate (bps) for a link to be considered alive.",
                   DataRateValue (DataRate ("0bps")), // default 0 => no pruning
                   MakeDataRateAccessor (&UnderwaterLaserChannel::SetMinRate,
                                         &UnderwaterLaserChannel::GetMinRate),
                   MakeDataRateChecker ()
                  );
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
UnderwaterLaserChannel::SetMinRate (DataRate r)
{
  m_minRate = r;
}

DataRate
UnderwaterLaserChannel::GetMinRate () const
{
  return m_minRate;
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
  return m_devices.size ();
}

Ptr<NetDevice>
UnderwaterLaserChannel::GetDevice (std::size_t i) const
{
  // (use .at(i) for range-check, or [i] if you prefer no overhead)
  return m_devices.at (i);
}

void
UnderwaterLaserChannel::TransmitStart (Ptr<Packet> packet,
                                       Address src,
                                       Address dst,
                                       uint16_t protocol,
                                       Ptr<UnderwaterLaserNetDevice> sender)
{
  for (uint32_t i = 0; i < GetNDevices (); ++i)
    {
      Ptr<UnderwaterLaserNetDevice> dev = DynamicCast<UnderwaterLaserNetDevice>(GetDevice(i));
      if (dev && dev != sender)
        {
          if (!dev->GetNode ())
            {
              std::cout << "[TransmitStart ERROR] Device " << i
                        << " (pointer=" << dev 
                        << ") has NULL node pointer.\n";
              continue; 
            }

          if (!dev->GetMobility ())
            {
              std::cout << "[TransmitStart ERROR] Device " << i
                        << " (Node=" << dev->GetNode()->GetId()
                        << ") has NULL mobility pointer.\n";
              continue;
            }

          // Apply pathloss model
          double rxPowerDbm = m_lossModel->CalcRxPower (m_txPowerDbm,
                                                        sender->GetMobility (),
                                                        dev->GetMobility ());
          // Check distance‐based link capacity
          double distance   = dev->GetMobility ()->GetDistanceFrom (sender->GetMobility ());
          DataRate linkRate = dev->GetRateTable ()->GetMaxRate (distance);  // <-- key fix

          // If link below threshold, skip
          if (linkRate < m_minRate)
            {
              // This neighbor's distance-based data rate is too low → skip
              continue;
            }

          double propagationDelaySec = distance / 2.25e8;

          std::cout << "[TransmitStart] Scheduling ReceiveFromChannel on Node " 
                    << dev->GetNode()->GetId()
                    << ", Device pointer=" << dev 
                    << ", distance=" << distance << " m"
                    << ", linkRate=" << linkRate.GetBitRate() << " bps"
                    << ", propagationDelay=" << propagationDelaySec << " s\n";

          Simulator::ScheduleWithContext (
              dev->GetNode ()->GetId (),
              Seconds (propagationDelaySec),
              &UnderwaterLaserNetDevice::ReceiveFromChannel,
              dev,
              packet->Copy (),
              rxPowerDbm,
              sender->GetAddress (),
              protocol);
        }
    }
}

} // namespace ns3
