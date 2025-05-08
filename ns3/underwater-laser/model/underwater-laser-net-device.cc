/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "underwater-laser-net-device.h"
#include "ns3/node.h"
#include "ns3/names.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/net-device-queue-interface.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/mobility-model.h"
#include "underwater-laser-channel.h"
#include "underwater-laser-rate-table.h"
#include "underwater-laser-propagation-loss-model.h"
#include "underwater-laser-error-rate-model.h"

#include <iostream> // for std::cout

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserNetDevice);

TypeId
UnderwaterLaserNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserNetDevice")
    .SetParent<NetDevice> ()
    .SetGroupName ("UnderwaterLaser")
    .AddConstructor<UnderwaterLaserNetDevice> ();
  return tid;
}

UnderwaterLaserNetDevice::UnderwaterLaserNetDevice ()
  : m_ifIndex (0),
    m_linkUp (true),
    m_mtu (1500),
    m_node (nullptr),
    m_mobility (nullptr),
    m_channel (nullptr),
    m_lossModel (nullptr),
    m_errorModel (nullptr),
    m_rateTable (nullptr),
    m_currentDataRate (DataRate (1e6)) // default 1 Mbps
{
  std::cout << "[UnderwaterLaserNetDevice] Constructor" << std::endl;

  // (1) Optionally create a placeholder NetDeviceQueueInterface aggregator.
  // In older ns-3, we won't do CreateTxQueues() or SetQueue() because they don't exist.
  m_queueInterface = CreateObject<NetDeviceQueueInterface> ();
  this->AggregateObject (m_queueInterface);

  // (2) Create our own internal queue for actual packet storage
  m_queue = CreateObject<DropTailQueue<Packet>> ();
}

void
UnderwaterLaserNetDevice::SetQueue (Ptr<Queue<Packet>> txQueue)
{
    m_queue = txQueue;
}

UnderwaterLaserNetDevice::~UnderwaterLaserNetDevice ()
{
    std::cout << "[Destructor] UnderwaterLaserNetDevice at node " << (m_node ? m_node->GetId() : -1) << std::endl;
}

void
UnderwaterLaserNetDevice::SetIfIndex (uint32_t index)
{
  m_ifIndex = index;
}

uint32_t
UnderwaterLaserNetDevice::GetIfIndex () const
{
  return m_ifIndex;
}

Ptr<Channel>
UnderwaterLaserNetDevice::GetChannel () const
{
  return m_channel;
}

void
UnderwaterLaserNetDevice::SetAddress (Address address)
{
  Mac48Address mac = Mac48Address::ConvertFrom (address);
  std::cout << "[UWLNetDevice::SetAddress] this=" << this
            << " node=" << (m_node ? m_node->GetId() : -1)
            << " old=" << m_address
            << " new=" << mac << std::endl;
  m_address = Mac48Address::ConvertFrom (address);
}

Address
UnderwaterLaserNetDevice::GetAddress () const
{
  return m_address;
}

/* For OLSR or ARP broadcast frames, we do want to allow L2 broadcast. */
bool
UnderwaterLaserNetDevice::IsBroadcast () const
{
  // Return true so IP can do broadcast frames if needed.
  return true;
}

Address
UnderwaterLaserNetDevice::GetBroadcast () const
{
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
UnderwaterLaserNetDevice::IsMulticast () const
{
  return true;
}

Address
UnderwaterLaserNetDevice::GetMulticast (Ipv4Address addr) const
{
  return Mac48Address::GetMulticast(addr);
}

Address
UnderwaterLaserNetDevice::GetMulticast (Ipv6Address addr) const
{
  return Mac48Address::GetMulticast(addr);
}

/* If you want multi-access for OLSR, set IsPointToPoint()=false. */
bool
UnderwaterLaserNetDevice::IsPointToPoint () const
{
  return false;
}

/* Return true if we need ARP for IPv4. */
bool
UnderwaterLaserNetDevice::NeedsArp () const
{
  return true;
}

/* Assume link is always up. */
bool
UnderwaterLaserNetDevice::IsLinkUp () const
{
  return m_linkUp;
}

void
UnderwaterLaserNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  // No link-change events in this design
}

bool
UnderwaterLaserNetDevice::IsBridge () const
{
  return false;
}

bool
UnderwaterLaserNetDevice::Send (Ptr<Packet> packet,
                                const Address &dest,
                                uint16_t protocolNumber)
{
  return DoSend (packet, GetAddress (), dest, protocolNumber);
}

bool
UnderwaterLaserNetDevice::SendFrom (Ptr<Packet> packet,
                                    const Address &source,
                                    const Address &dest,
                                    uint16_t protocolNumber)
{
  return DoSend (packet, source, dest, protocolNumber);
}

bool UnderwaterLaserNetDevice::DoSend (Ptr<Packet> packet,
                                       const Address &src,
                                       const Address &dst,
                                       uint16_t protocol)
{
    if (!m_mobility || !m_rateTable || !m_channel)
    {
        std::cout << "[DoSend ERROR] missing components: "
                  << "m_mobility=" << m_mobility << ", "
                  << "m_rateTable=" << m_rateTable << ", "
                  << "m_channel=" << m_channel << std::endl;
        return false;
    }

    DataRate dr = GetCurrentDataRate ();
    Time serialization = Seconds (packet->GetSize () * 8.0 / dr.GetBitRate ());
        //std::cout << "[DoSend] Node=" << m_node->GetId()
        //          << " Position: x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;

    //std::cout << "[DoSend] Node=" << m_node->GetId()
      //        << ", Sending packet, Protocol=" << protocol
      //        << ", from MAC=" << Mac48Address::ConvertFrom(src)
      //        << ", to MAC=" << Mac48Address::ConvertFrom(dst)
      //        << ", Size=" << packet->GetSize()
      //        << ", Serialization Time=" << serialization.GetSeconds() << "s"
      //        << std::endl;

    Simulator::Schedule (serialization,
                         &UnderwaterLaserChannel::TransmitStart,
                         m_channel, packet, src, dst, protocol, this);

    return true;
}


DataRate
UnderwaterLaserNetDevice::GetCurrentDataRate () const
{
    if (m_channel->GetNDevices() <= 1) {
        std::cout << "[ERROR] Channel has only 1 device connected, no other nodes reachable!" << std::endl;
    }    
  if (!m_rateTable || !m_mobility)
    {
      return m_currentDataRate;
    }

  double maxDist = 0.0;
  for (uint32_t i = 0; i < m_channel->GetNDevices (); ++i)
    {
      Ptr<UnderwaterLaserNetDevice> dev =
        DynamicCast<UnderwaterLaserNetDevice> (m_channel->GetDevice (i));

      if (dev && dev != GetObject<UnderwaterLaserNetDevice> ())
        {
          double dist = CalculateDistance (m_mobility->GetPosition (),
                                           dev->GetMobility ()->GetPosition ());
          if (dist > maxDist)
            {
              maxDist = dist;
            }
        }
    }

  double rateBps = m_rateTable->GetMaxRate (maxDist);
  return DataRate (rateBps);
}

/* -------------- Mtu methods to satisfy the linker -------------- */
bool
UnderwaterLaserNetDevice::SetMtu (const uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}

uint16_t
UnderwaterLaserNetDevice::GetMtu () const
{
  return m_mtu;
}
/* --------------------------------------------------------------- */

Ptr<Node>
UnderwaterLaserNetDevice::GetNode () const
{
  return m_node;
}

void
UnderwaterLaserNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
UnderwaterLaserNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  m_rxCallback = cb;
}

void
UnderwaterLaserNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  m_promiscCallback = cb;
}

bool
UnderwaterLaserNetDevice::SupportsSendFrom () const
{
  return true;
}

// — after:
void UnderwaterLaserNetDevice::SetChannelModels (
    Ptr<UnderwaterLaserChannel> channel,
    Ptr<UnderwaterLaserPropagationLossModel> lossModel,
    Ptr<UnderwaterLaserErrorRateModel> errorModel,
    Ptr<UnderwaterLaserRateTable> rateTable)
{
    NS_ASSERT(channel && lossModel && errorModel && rateTable);
    m_channel    = channel;
    m_lossModel  = lossModel;
    m_errorModel = errorModel;
    m_rateTable  = rateTable;
}


void
UnderwaterLaserNetDevice::SetErrorModel (Ptr<UnderwaterLaserErrorRateModel> errorModel)
{
  m_errorModel = errorModel;
}

void UnderwaterLaserNetDevice::ReceiveFromChannel(Ptr<Packet> packet, 
    double rxSnrDb, 
    Address src, 
    uint16_t protocol)
{
    if (!m_node || !m_errorModel) {
        std::cout << "[ReceiveFromChannel ERROR] Node or error model pointer is NULL.\n";
        return;
    }

    uint64_t nbits = packet->GetSize() * 8;
    double chunkSuccess = m_errorModel->DoGetChunkSuccessRate(rxSnrDb, nbits);
    double r = (double) std::rand() / RAND_MAX;

    std::cout << "[ReceiveFromChannel] Node=" << m_node->GetId()
              << ", Device=" << this
              << ", SNR=" << rxSnrDb
              << ", ChunkSuccess=" << chunkSuccess
              << ", Rand=" << r
              << ", Protocol=" << protocol
              << ", PacketSize=" << packet->GetSize() << std::endl;

    std::cout << "    My Mac48=" << m_address << std::endl;

    if (r > chunkSuccess) {
        std::cout << "[ReceiveFromChannel] Packet dropped due to error model." << std::endl;
        return;
    }

    if (!m_rxCallback.IsNull()) {
        std::cout << "[ReceiveFromChannel] Calling ReceiveCallback for node=" << m_node->GetId() << std::endl;
        m_rxCallback(this, packet, protocol, src);
    } else {
        std::cout << "[ReceiveFromChannel WARNING] ReceiveCallback is NULL for node=" << m_node->GetId() << std::endl;
    }
}


Ptr<UnderwaterLaserRateTable>
UnderwaterLaserNetDevice::GetRateTable() const
 {
    return m_rateTable;
 }

void
UnderwaterLaserNetDevice::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
}

} // namespace ns3
