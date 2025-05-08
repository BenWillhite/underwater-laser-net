#ifndef UNDERWATER_LASER_CHANNEL_H
#define UNDERWATER_LASER_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/address.h"
#include "ns3/ptr.h"
#include <vector>
#include "ns3/net-device.h"
#include "ns3/data-rate.h"


namespace ns3 {

class UnderwaterLaserNetDevice;
class UnderwaterLaserPropagationLossModel;

/**
 * \brief A simple channel for our underwater laser net-devices.
 *
 * When a sender calls TransmitStart(), we look up each
 * attached NetDevice, compute Rx power via the PropagationLossModel,
 * and hand the packet off to the device’s ReceiveFromChannel().
 */
class UnderwaterLaserChannel : public Channel
{
public:
  static TypeId GetTypeId (void);
  UnderwaterLaserChannel ();
  virtual ~UnderwaterLaserChannel ();

  /**
   * \brief Add a device (NetDevice) to this channel.
   */
  void AddDevice (Ptr<NetDevice> dev);

  /**
   * \brief Return how many devices are attached to this channel.
   *        (MUST use uint32_t, per ns-3 base Channel.)
   */
  virtual std::size_t GetNDevices () const override;

  /**
   * \brief Get the i-th attached device.
   *        (MUST use uint32_t, per ns-3 base Channel.)
   */
  virtual Ptr<NetDevice> GetDevice (std::size_t i) const override;

  /**
   * \brief Install the loss model created by the helper.
   */
  void SetPropagationLossModel (Ptr<UnderwaterLaserPropagationLossModel> loss);
  Ptr<UnderwaterLaserPropagationLossModel> GetPropagationLossModel () const;

  /**
   * \brief Set the transmit power in dBm for all transmissions. 
   *        (Can also be set via Attribute "TxPowerDbm".)
   */
  void SetTxPowerDbm (double txPowerDbm);

  /**
   * \brief Set/Get the minimum data rate (bps) below which links are pruned.
   */
  void SetMinRate (DataRate r);
  DataRate GetMinRate () const;

  /**
   * \brief Get the current transmit power in dBm.
   */
  double GetTxPowerDbm () const;

  /**
   * \brief Called (via Simulator::Schedule) to deliver a packet from the sender 
   *        to all other devices on this channel.
   *
   * @param packet   the packet to send
   * @param src      the source MAC address
   * @param dst      the destination MAC address
   * @param protocol EtherType / protocol number
   * @param sender   the sending UnderwaterLaserNetDevice
   */
  void TransmitStart (Ptr<Packet> packet,
                      Address src,
                      Address dst,
                      uint16_t protocol,
                      Ptr<UnderwaterLaserNetDevice> sender);

private:
  Ptr<UnderwaterLaserPropagationLossModel> m_lossModel;
  std::vector< Ptr<NetDevice> >            m_devices;

  // New: we allow a configurable Tx power in dBm (defaults to 0 dBm).
  double   m_txPowerDbm;

  // New: a user-settable link‐pruning threshold.
  DataRate m_minRate;
};

} // namespace ns3

#endif // UNDERWATER_LASER_CHANNEL_H
