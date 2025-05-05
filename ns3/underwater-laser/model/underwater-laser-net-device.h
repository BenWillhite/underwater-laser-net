#ifndef UNDERWATER_LASER_NET_DEVICE_H
#define UNDERWATER_LASER_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/ptr.h"
#include "ns3/channel.h"
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/mac48-address.h"
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/queue-item.h"
#include "ns3/traced-callback.h"

#include "underwater-laser-rate-table.h"
#include "ns3/mobility-model.h"


namespace ns3 {

class UnderwaterLaserChannel;
class UnderwaterLaserPropagationLossModel;
class UnderwaterLaserErrorRateModel;
class Node;
class MobilityModel;

/**
 * \class UnderwaterLaserNetDevice
 * \brief A custom NetDevice that uses an UnderwaterLaserChannel for connectivity
 *        and references a RateTable, PropagationLossModel, and ErrorRateModel
 *        to shape transmissions and drop packets.
 */
class UnderwaterLaserNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);

  UnderwaterLaserNetDevice ();
  virtual ~UnderwaterLaserNetDevice ();

  // Inherited from NetDevice base class
  virtual void SetIfIndex (const uint32_t index) override;
  virtual uint32_t GetIfIndex () const override;

  virtual Ptr<Channel> GetChannel () const override;
  virtual void SetAddress (Address address) override;
  virtual Address GetAddress () const override;
  virtual bool SetMtu (const uint16_t mtu) override;
  virtual uint16_t GetMtu () const override;
  virtual bool IsLinkUp () const override;
  virtual void AddLinkChangeCallback (Callback<void> callback) override;
  virtual bool IsBroadcast () const override;
  virtual Address GetBroadcast () const override;
  virtual bool IsMulticast () const override;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const override;
  virtual bool IsPointToPoint () const override;
  virtual bool Send (Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber) override;
  virtual bool SendFrom (Ptr<Packet> packet, const Address &source, const Address &dest, uint16_t protocolNumber) override;
  virtual Ptr<Node> GetNode () const override;
  virtual void SetNode (Ptr<Node> node) override;
  virtual bool NeedsArp () const override;
  virtual void SetReceiveCallback (ReceiveCallback cb) override;
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb) override;
  virtual bool SupportsSendFrom () const override;

  // -----------------------------
  // NEW OVERRIDES for completeness
  // -----------------------------
  virtual Address GetMulticast (Ipv6Address addr) const override;
  virtual bool    IsBridge     (void) const override;

  /**
   * \brief Attach to a channel and the associated models.
   */
  void Attach (Ptr<UnderwaterLaserChannel> channel,
               Ptr<UnderwaterLaserPropagationLossModel> lossModel,
               Ptr<UnderwaterLaserErrorRateModel> errorModel,
               Ptr<UnderwaterLaserRateTable> rateTable);

  /**
   * \brief Called by the channel to notify that a packet has been received.
   *        We run the error model to see if bits are dropped.
   */
  void ReceiveFromChannel (Ptr<Packet> packet, double rxSnrDb);

  /**
   * \brief Set the underlying MobilityModel for distance calculations.
   */
  void SetMobility (Ptr<MobilityModel> mobility);

  /**
   * \brief Get the attached MobilityModel.
   */
  Ptr<MobilityModel> GetMobility (void) const { return m_mobility; }

private:
  /**
   * \brief Internal method that does the actual packet enqueue + shaping.
   */
  bool DoSend (Ptr<Packet> packet, const Address &src, const Address &dst, uint16_t protocol);

  /**
   * \brief A small function to compute the instantaneous data rate from the RateTable.
   */
  DataRate GetCurrentDataRate () const;

  uint32_t       m_ifIndex;
  bool           m_linkUp;
  uint16_t       m_mtu;
  Mac48Address   m_address;

  Ptr<Node>      m_node;
  Ptr<MobilityModel> m_mobility;

  Ptr<UnderwaterLaserChannel>                m_channel;
  Ptr<UnderwaterLaserPropagationLossModel>   m_lossModel;
  Ptr<UnderwaterLaserErrorRateModel>         m_errorModel;
  Ptr<UnderwaterLaserRateTable>              m_rateTable;

  // Receive callbacks
  NetDevice::ReceiveCallback        m_rxCallback;
  NetDevice::PromiscReceiveCallback m_promiscCallback;

  // For queueing/shaping (simplistic).
  DataRate m_currentDataRate;
};

} // namespace ns3

#endif // UNDERWATER_LASER_NET_DEVICE_H
