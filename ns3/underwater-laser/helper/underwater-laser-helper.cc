/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/underwater-laser-helper.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"
#include "ns3/mobility-model.h"
#include "ns3/underwater-laser-channel.h"
#include "ns3/underwater-laser-net-device.h"
#include "ns3/underwater-laser-rate-table.h"
#include "ns3/underwater-laser-propagation-loss-model.h"
#include "ns3/type-id.h"
#include "ns3/string.h"
#include "ns3/double.h"

#include <iostream> // for std::cout

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnderwaterLaserHelper");
NS_OBJECT_ENSURE_REGISTERED (UnderwaterLaserHelper);

TypeId
UnderwaterLaserHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnderwaterLaserHelper")
    .SetParent<Object> ()
    .SetGroupName ("UnderwaterLaser")
    .AddConstructor<UnderwaterLaserHelper> ()
    // new attribute: path to time+depth CSV
    .AddAttribute ("AlphaCsv",
                   "Path to the alpha_depth_time.csv file (time,z_mid,alpha_m^-1).",
                   StringValue ("scratch/alpha_depth_time.csv"), // default
                   MakeStringAccessor (&UnderwaterLaserHelper::m_alphaCsv),
                   MakeStringChecker ())
    // new attribute: beam divergence half-angle
    .AddAttribute ("BeamDivergence",
                   "Beam divergence half-angle in radians",
                   DoubleValue (0.001),
                   MakeDoubleAccessor (&UnderwaterLaserHelper::m_divergenceRad),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

UnderwaterLaserHelper::UnderwaterLaserHelper ()
  : m_channelCreated (false),
    m_rateTableCsv   ("modulation/rate_vs_distance.csv"),
    m_alphaCsv       ("scratch/alpha_depth_time.csv"),
    m_divergenceRad  (0.001)
{
  NS_LOG_FUNCTION (this);
}

UnderwaterLaserHelper::~UnderwaterLaserHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
UnderwaterLaserHelper::SetChannelAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  m_channelHelper.SetAttribute (name, value);
}

void
UnderwaterLaserHelper::SetPhyAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  m_phyHelper.SetAttribute (name, value);
}

void
UnderwaterLaserHelper::SetMacAttribute (std::string name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this << name);
  m_macHelper.SetAttribute (name, value);
}

void
UnderwaterLaserHelper::SetRateTableCsv (std::string csvPath)
{
  m_rateTableCsv = csvPath;
}

NetDeviceContainer
UnderwaterLaserHelper::Install (NodeContainer c)
{
  NS_LOG_FUNCTION (this);

  NetDeviceContainer devices;

  // 1) Create channel once
  if (!m_channelCreated)
    {
      std::cout << "[UnderwaterLaserHelper] Creating channel..." << std::endl;
      m_channel = m_channelHelper.Create ();

      // configure our custom propagation-loss model
      Ptr<UnderwaterLaserPropagationLossModel> loss =
        m_channel->GetPropagationLossModel ()
                 ->GetObject<UnderwaterLaserPropagationLossModel> ();

      if (!loss)
        {
          std::cerr << "[UnderwaterLaserHelper] ERROR: channel->GetPropagationLossModel() returned null!\n";
        }
      else
        {
          loss->SetAlphaCsv      (m_alphaCsv);
          loss->SetBeamDivergence(m_divergenceRad);
        }

      // Create a shared RateTable
      m_rateTable = CreateObject<UnderwaterLaserRateTable> ();
      m_rateTable->Load (m_rateTableCsv);

      m_channelCreated = true;
      std::cout << "[UnderwaterLaserHelper] Channel + LossModel + RateTable created.\n";
    }

  // 2) For each node, create an UnderwaterLaserNetDevice
  uint32_t nodeIndex = 0;
  for (auto it = c.Begin (); it != c.End (); ++it, ++nodeIndex)
    {
      Ptr<Node> node = *it;
      if (!node)
        {
          std::cerr << "[UnderwaterLaserHelper] ERROR: NodeContainer had a null node at index=" << nodeIndex << std::endl;
          continue;
        }

      // create device
      Ptr<UnderwaterLaserNetDevice> device = CreateObject<UnderwaterLaserNetDevice> ();
      std::cout << "[UnderwaterLaserHelper] Created device=" << device
                << " for node=" << node->GetId() << std::endl;

      // Add the device to the node
      node->AddDevice (device);

      // Make sure the device knows which Node it belongs to:
      device->SetNode (node);

      // Double-check #devices
      uint32_t nDev = node->GetNDevices ();
      std::cout << "  => Node=" << node->GetId()
                << " now has #devices=" << nDev
                << " (just added underwater-laser-net-device)\n";

      // set IfIndex
      device->SetIfIndex (nDev - 1);

      // attach channel, no error-model yet, reference the shared rate table
      if (!m_channel)
        {
          std::cerr << "[UnderwaterLaserHelper] ERROR: m_channel is null!\n";
        }
      else
        {
          auto lossModel = m_channel->GetPropagationLossModel ()
                                ->GetObject<UnderwaterLaserPropagationLossModel>();
          device->Attach (m_channel, lossModel, 0, m_rateTable);
        }

      // mobility => device
      Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
      if (!mob)
        {
          std::cout << "[DEBUG] Node=" << node->GetId()
                    << " has NO MobilityModel?\n";
        }
      else
        {
          std::cout << "[DEBUG] Node=" << node->GetId()
                    << " has MobilityModel: " << mob->GetInstanceTypeId().GetName() << "\n";
          device->SetMobility (mob);
        }

      // MAC & PHY
      std::cout << "[UnderwaterLaserHelper] Creating MAC+PHY for device\n";
      m_macHelper.CreateMac (device);
      m_phyHelper.CreatePhy (device);

      // allocate a MAC address
      Mac48Address addr = Mac48Address::Allocate ();
      device->SetAddress (addr);
      std::cout << "  => Assigned MAC=" << addr << " to device=" << device << std::endl;

      devices.Add (device);
    } // end loop over nodes

  std::cout << "[UnderwaterLaserHelper] Install() returning " << devices.GetN() << " devices total.\n";
  return devices;
}

} // namespace ns3
