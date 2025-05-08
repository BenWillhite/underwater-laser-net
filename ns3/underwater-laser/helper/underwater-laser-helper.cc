/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/underwater-laser-helper.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"
#include "ns3/underwater-laser-net-device.h"
#include "ns3/underwater-laser-channel.h"
#include "ns3/underwater-laser-propagation-loss-model.h"
#include "ns3/underwater-laser-rate-table.h"
#include "ns3/mobility-model.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/type-id.h"
#include "ns3/string.h"
#include "ns3/double.h"

#include <iostream>

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
    .AddAttribute ("AlphaCsv",
                   "Path to the alpha_depth_time.csv file (time,z_mid,alpha_m^-1).",
                   StringValue ("scratch/alpha_depth_time.csv"),
                   MakeStringAccessor (&UnderwaterLaserHelper::m_alphaCsv),
                   MakeStringChecker ())
    .AddAttribute ("BeamDivergence",
                   "Beam divergence half-angle in radians",
                   DoubleValue (0.001),
                   MakeDoubleAccessor (&UnderwaterLaserHelper::m_divergenceRad),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("FluxPredictorScript",
                  "Path to the Python flux-predictor wrapper",
                  StringValue ("scratch/python/flux_predictor.py"), // default
                  MakeStringAccessor (&UnderwaterLaserHelper::m_fluxPredictorScript),
                  MakeStringChecker ());
  return tid;
}

UnderwaterLaserHelper::UnderwaterLaserHelper ()
  : m_channelCreated (false),
    m_rateTableCsv   ("scratch/cleaned_rate_distance.csv"),
    m_alphaCsv       ("scratch/alpha_depth_time.csv"),
    m_divergenceRad  (0.001),
    m_fluxPredictorScript("scratch/python/flux_predictor.py") // NEW
{
  NS_LOG_FUNCTION (this);
}

UnderwaterLaserHelper::~UnderwaterLaserHelper ()
{
  std::cout << "[Destructor] UnderwaterLaserHelper" << std::endl;
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

bool UnderwaterLaserHelper::DeviceReceiveCallback(
    Ptr<NetDevice> device, 
    Ptr<const Packet> packet, 
    uint16_t protocol, 
    const Address &source)
{
    Ptr<Node> node = device->GetNode();
    NS_ASSERT(node);
    
    std::cout << "[DeviceReceiveCallback] Node ID=" << node->GetId()
              << " Device=" << device
              << " Src MAC=" << Mac48Address::ConvertFrom(source)
              << " Protocol=0x" << std::hex << protocol << std::dec
              << " Packet size=" << packet->GetSize() << " bytes"
              << std::endl;

    if (protocol == 0x0806) { // ARP protocol number in hex
        std::cout << "[ARP PACKET RECEIVED] Node " << node->GetId() << std::endl;

        Ptr<ArpL3Protocol> arp = node->GetObject<ArpL3Protocol>();
        if (!arp) {
            std::cout << "[ERROR] Node " << node->GetId() << " ARP pointer is NULL!" << std::endl;
            return false;
        }

        arp->Receive(device, packet, protocol, source, device->GetAddress(), NetDevice::PACKET_HOST);
        std::cout << "[ARP HANDLER CALLED] Node " << node->GetId() << std::endl;
    }
    else if (protocol == 0x0800) { // IPv4 protocol number
        Ptr<Ipv4L3Protocol> ipv4 = node->GetObject<Ipv4L3Protocol>();
        if (!ipv4) {
            std::cout << "[ERROR] Node " << node->GetId() << " IPv4 pointer is NULL!" << std::endl;
            return false;
        }

        ipv4->Receive(device, packet, protocol, source, device->GetAddress(), NetDevice::PACKET_HOST);
        std::cout << "[IPv4 HANDLER CALLED] Node " << node->GetId() << std::endl;
    }
    else {
        std::cout << "[UNKNOWN PROTOCOL] Node " << node->GetId()
                  << " protocol=" << protocol << std::endl;
        return false;
    }

    return true; 
}




NetDeviceContainer
UnderwaterLaserHelper::InstallPair (Ptr<Node> nA, Ptr<Node> nB)
{
  // 1) Create a fresh channel + loss model
  Ptr<UnderwaterLaserChannel> channel = m_channelHelper.Create ();
  auto lossModel = channel->GetPropagationLossModel()
                      ->GetObject<UnderwaterLaserPropagationLossModel> ();
  lossModel->SetAlphaCsv      (m_alphaCsv);
  lossModel->SetBeamDivergence(m_divergenceRad);

  // 2a) Figure out distance
  double linkDistance = nA->GetObject<MobilityModel>()->GetDistanceFrom(nB->GetObject<MobilityModel>());

  // 2b) Call flux_predictor.py
  std::ostringstream outCsvPath;
  outCsvPath << "/tmp/flux_" << nA->GetId() << "_" << nB->GetId() << ".csv";

  std::ostringstream cmd;
  cmd << "python3 "
      << m_fluxPredictorScript
      << " --alpha_csv " << m_alphaCsv
      << " --distance "  << linkDistance
      << " --lambda_nm 532 "
      << " --out_csv "   << outCsvPath.str();

  int ret = std::system(cmd.str().c_str());
  if (ret != 0)
  {
    NS_FATAL_ERROR("Flux predictor script failed, ret=" << ret);
  }

  // 2c) Now load that new CSV
  Ptr<UnderwaterLaserRateTable> rateTable = CreateObject<UnderwaterLaserRateTable>();
  rateTable->Load(outCsvPath.str());

  // 2d) Prepare an error model
  Ptr<UnderwaterLaserErrorRateModel> errModel = CreateObject<UnderwaterLaserErrorRateModel>();
  errModel->SetTraceFilename("scratch/per_snr_table.csv");

  // 3) Create & wire device on node A, hooking in the new rateTable
  Ptr<UnderwaterLaserNetDevice> devA = CreateObject<UnderwaterLaserNetDevice>();
  nA->AddDevice(devA);
  devA->SetNode(nA);
  devA->SetIfIndex(nA->GetNDevices()-1);
  devA->SetChannelModels(channel,
                         lossModel,
                         errModel,
                         rateTable);
  channel->AddDevice(devA);
  devA->SetMobility(nA->GetObject<MobilityModel>());
  m_macHelper.CreateMac(devA);
  m_phyHelper.CreatePhy(devA);
  devA->SetAddress(Mac48Address::Allocate());
  devA->SetQueue(CreateObject<DropTailQueue<Packet>>());
  // Register IP & ARP on devA
  nA->RegisterProtocolHandler(MakeCallback(&Ipv4L3Protocol::Receive,
                                           nA->GetObject<Ipv4L3Protocol>()),
                              Ipv4L3Protocol::PROT_NUMBER, devA);
  nA->RegisterProtocolHandler(MakeCallback(&ArpL3Protocol::Receive,
                                           nA->GetObject<ArpL3Protocol>()),
                              ArpL3Protocol::PROT_NUMBER, devA);
  devA->SetReceiveCallback(MakeCallback(&UnderwaterLaserHelper::DeviceReceiveCallback, this));

  // 4) Repeat for node B (basically the same lines)...
  Ptr<UnderwaterLaserNetDevice> devB = CreateObject<UnderwaterLaserNetDevice>();
  nB->AddDevice(devB);
  devB->SetNode(nB);
  devB->SetIfIndex(nB->GetNDevices()-1);
  devB->SetChannelModels(channel,
                         lossModel,
                         errModel,
                         rateTable);
  channel->AddDevice(devB);
  devB->SetMobility(nB->GetObject<MobilityModel>());
  m_macHelper.CreateMac(devB);
  m_phyHelper.CreatePhy(devB);
  devB->SetAddress(Mac48Address::Allocate());
  devB->SetQueue(CreateObject<DropTailQueue<Packet>>());
  // Register IP & ARP on devB
  nB->RegisterProtocolHandler(MakeCallback(&Ipv4L3Protocol::Receive,
                                           nB->GetObject<Ipv4L3Protocol>()),
                              Ipv4L3Protocol::PROT_NUMBER, devB);
  nB->RegisterProtocolHandler(MakeCallback(&ArpL3Protocol::Receive,
                                           nB->GetObject<ArpL3Protocol>()),
                              ArpL3Protocol::PROT_NUMBER, devB);
  devB->SetReceiveCallback(MakeCallback(&UnderwaterLaserHelper::DeviceReceiveCallback, this));

  // 5) Return the two new devices
  NetDeviceContainer linkDevs;
  linkDevs.Add(devA);
  linkDevs.Add(devB);
  return linkDevs;
}


} // namespace ns3
