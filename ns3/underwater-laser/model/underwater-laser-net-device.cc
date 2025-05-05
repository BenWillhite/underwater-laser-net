/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "underwater-laser-net-device.h"
#include "ns3/node.h"
#include "ns3/names.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/mobility-model.h"
#include "underwater-laser-channel.h"
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
    .SetGroupName ("PointToPoint")
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
}

UnderwaterLaserNetDevice::~UnderwaterLaserNetDevice ()
{
  std::cout << "[UnderwaterLaserNetDevice] Destructor" << std::endl;
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
  // Return true so IP can send broadcast (e.g. OLSR Hellos, ARP).
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
  // Just map IPv4 multicast to an Ethernet-like address if needed
  return Mac48Address ("33:33:00:00:00:00");
}

Address
UnderwaterLaserNetDevice::GetMulticast (Ipv6Address addr) const
{
  // Same placeholder
  return Mac48Address ("33:33:00:00:00:00");
}

/* If you want a multi-access link for OLSR, set this to false so IP 
   sees it as broadcast-capable. If you prefer a strict p2p design, 
   set IsPointToPoint()=true but then also handle broadcast carefully. */
bool
UnderwaterLaserNetDevice::IsPointToPoint () const
{
  return false;
}

/* If you want no ARP overhead, return false. If you want ARP, return true. */
bool
UnderwaterLaserNetDevice::NeedsArp () const
{
  return false;
}

/* We assume the link is always up in this simple design. */
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
UnderwaterLaserNetDevice::Send (Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
{
  return DoSend (packet, GetAddress (), dest, protocolNumber);
}

bool
UnderwaterLaserNetDevice::SendFrom (Ptr<Packet> packet, const Address &source,
                                    const Address &dest, uint16_t protocolNumber)
{
  return DoSend (packet, source, dest, protocolNumber);
}

bool
UnderwaterLaserNetDevice::DoSend (Ptr<Packet> packet,
                                  const Address &src,
                                  const Address &dst,
                                  uint16_t protocol)
{
  // Debug check for pointers
  if (!m_mobility || !m_rateTable || !m_channel)
    {
      std::cout << "[UnderwaterLaserNetDevice::DoSend DEBUG] "
                << "m_mobility=" << m_mobility
                << " m_rateTable=" << m_rateTable
                << " m_channel="   << m_channel
                << " => FAIL\n";
      return false;
    }

  DataRate dr = GetCurrentDataRate ();
  Time serialization = Seconds (packet->GetSize () * 8.0 / dr.GetBitRate ());

  std::cout << "[UnderwaterLaserNetDevice] Queueing packet size=" << packet->GetSize ()
            << " bytes, dataRate=" << dr.GetBitRate()
            << " => TxTime=" << serialization.GetSeconds() << "s" << std::endl;

  Simulator::Schedule (serialization, &UnderwaterLaserChannel::TransmitStart,
                       m_channel, packet, src, dst, protocol, this);

  return true;
}

DataRate
UnderwaterLaserNetDevice::GetCurrentDataRate () const
{
  if (!m_rateTable || !m_mobility)
    {
      return m_currentDataRate;
    }

  double maxDist = 0.0;
  for (uint32_t i = 0; i < m_channel->GetNDevices (); ++i)
    {
      Ptr<UnderwaterLaserNetDevice> dev = DynamicCast<UnderwaterLaserNetDevice> (m_channel->GetDevice (i));
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

/* -------------- New: Mtu methods that the linker was complaining about -------------- */
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
/* -------------------------------------------------------------------------- */

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

void
UnderwaterLaserNetDevice::Attach (Ptr<UnderwaterLaserChannel> channel,
                                  Ptr<UnderwaterLaserPropagationLossModel> lossModel,
                                  Ptr<UnderwaterLaserErrorRateModel> errorModel,
                                  Ptr<UnderwaterLaserRateTable> rateTable)
{
  m_channel    = channel;
  m_lossModel  = lossModel;
  m_errorModel = errorModel;
  m_rateTable  = rateTable;
}

void
UnderwaterLaserNetDevice::ReceiveFromChannel (Ptr<Packet> packet, double rxSnrDb)
{
  std::cout << "[UnderwaterLaserNetDevice] ReceiveFromChannel: rxSnrDb=" << rxSnrDb
            << " packetSize=" << packet->GetSize() << std::endl;

  uint64_t nbits = packet->GetSize () * 8;
  double chunkSuccess = 1.0;
  if (m_errorModel)
    {
      chunkSuccess = m_errorModel->DoGetChunkSuccessRate (rxSnrDb, nbits);
    }

  double r = (double) std::rand () / (double) RAND_MAX;
  if (r > chunkSuccess)
    {
      std::cout << "[UnderwaterLaserNetDevice] Packet dropped by PER model at SNR="
                << rxSnrDb << " dB" << std::endl;
      return;
    }

  // If not dropped, pass upward
  if (!m_rxCallback.IsNull ())
    {
      m_rxCallback (this, packet, 0, Address ());
    }
  if (!m_promiscCallback.IsNull ())
    {
      m_promiscCallback (this, packet, 0, Address (), Address (), PacketType::PACKET_HOST);
    }
}

void
UnderwaterLaserNetDevice::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
}

} // namespace ns3
