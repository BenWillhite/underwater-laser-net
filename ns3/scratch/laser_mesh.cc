/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <filesystem>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"      // <-- for InternetStackHelper, Ipv4AddressHelper
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-flow-classifier.h"

#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/underwater-laser-helper.h"

using namespace ns3;

int
main (int argc, char *argv[])
{
  std::cout << ">> [DEBUG] Entering LaserMeshExample::main()" << std::endl;

  // Default parameters
  double nodeSpacing      = 50.0;
  bool   attachErrorModel = true;
  double simTime          = 30.0;
  double txDbm            = 0.0; // default 0 dBm

  CommandLine cmd;
  cmd.AddValue("nodeSpacing",      "Distance between grid nodes (m)", nodeSpacing);
  cmd.AddValue("attachErrorModel", "Attach PER error model",          attachErrorModel);
  cmd.AddValue("simTime",          "Simulation time (s)",             simTime);
  cmd.AddValue("txDbm",            "Transmit power in dBm",           txDbm);
  cmd.Parse (argc, argv);

  // CSV file paths
  const std::string alphaCsv     = "../scratch/alpha_depth_time.csv";
  const std::string perSnrCsv    = "../scratch/per_snr_table.csv";
  const std::string rateTableCsv = "../scratch/rate_vs_distance.csv";

  // 1) Verify CSV presence
  std::cout << ">> [DEBUG] Verifying CSV file presence..." << std::endl;
  if (!std::filesystem::exists(alphaCsv)) {
    std::cerr << "ERROR: Missing alpha CSV: " << alphaCsv << std::endl;
    return 1;
  }
  if (attachErrorModel && !std::filesystem::exists(perSnrCsv)) {
    std::cerr << "ERROR: Missing PER/SNR CSV: " << perSnrCsv << std::endl;
    return 1;
  }
  if (!std::filesystem::exists(rateTableCsv)) {
    std::cerr << "ERROR: Missing rate table CSV: " << rateTableCsv << std::endl;
    return 1;
  }

  // 2) Create nodes
  NodeContainer nodes;
  nodes.Create (9);
  std::cout << ">> [STEP 1] Created " << nodes.GetN() << " nodes." << std::endl;

  // 3) Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX",      DoubleValue(0.0),
                                 "MinY",      DoubleValue(0.0),
                                 "DeltaX",    DoubleValue(nodeSpacing),
                                 "DeltaY",    DoubleValue(nodeSpacing),
                                 "GridWidth", UintegerValue(3),
                                 "LayoutType",StringValue("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // Debug: confirm each node has a MobilityModel
  for (uint32_t i=0; i<nodes.GetN(); ++i)
    {
      Ptr<MobilityModel> mm = nodes.Get(i)->GetObject<MobilityModel>();
      if (!mm)
        {
          std::cerr << "Node " << i << " has NO mobility?\n";
        }
      else
        {
          std::cout << "Node " << i << " => Mobility: "
                    << mm->GetInstanceTypeId().GetName() << std::endl;
        }
    }
  std::cout << ">> [STEP 2] Mobility installed." << std::endl;

  // --------------------------------------------------------------------
  // CREATE LASER DEVICES BEFORE installing IP stack + OLSR
  // --------------------------------------------------------------------

  // 4) UnderwaterLaserHelper
  Ptr<UnderwaterLaserHelper> laser = CreateObject<UnderwaterLaserHelper> ();
  laser->SetAttribute ("AlphaCsv",       StringValue(alphaCsv));
  laser->SetAttribute ("BeamDivergence", DoubleValue(0.001));

  if (attachErrorModel)
    {
      laser->SetPhyAttribute ("TraceFilename", StringValue(perSnrCsv));
      std::cout << ">> [DEBUG] PER/SNR CSV set: " << perSnrCsv << std::endl;
    }
  laser->SetRateTableCsv (rateTableCsv);
  std::cout << ">> [STEP 3] Laser helper configured; RateTable=" << rateTableCsv << std::endl;

  // Set channel TxPower
  laser->SetChannelAttribute ("TxPowerDbm", DoubleValue(txDbm));
  std::cout << ">> [INFO] Using TxPower=" << txDbm << " dBm" << std::endl;

  // 5) Install the laser devices
  NetDeviceContainer devices = laser->Install (nodes);
  std::cout << ">> [STEP 4] Installed " << devices.GetN() << " laser devices." << std::endl;

  // Add a debug loop to print each device pointer
  for (uint32_t i = 0; i < devices.GetN(); ++i)
    {
      Ptr<NetDevice> nd = devices.Get(i);
      std::cout << "   devices[" << i << "] = " << nd
                << ", nodeId=" << (nd ? nd->GetNode()->GetId() : 9999)
                << std::endl;
    }

  // 6) IP addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.0.0", "255.255.255.0");
  auto interfaces = address.Assign (devices);
  std::cout << ">> [STEP 5] Assigned IP addresses." << std::endl;

  // 7) Internet stack + OLSR
  OlsrHelper olsr;
  InternetStackHelper stack;
  stack.SetRoutingHelper (olsr);
  olsr.Set("HelloInterval", TimeValue(Seconds(1.0)));
  olsr.Set("TcInterval",    TimeValue(Seconds(2.0)));
  stack.Install (nodes);
  std::cout << ">> [STEP 6] Internet stack + OLSR installed." << std::endl;

  // 8) Dump routes at 6s
  Simulator::Schedule(Seconds(6.0), [&nodes](){
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
    std::cout << ">> [ROUTES @ 6s] ----------------------------" << std::endl;
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
      Ptr<Ipv4> ipv4 = nodes.Get(i)->GetObject<Ipv4>();
      if (ipv4)
        {
          ipv4->GetRoutingProtocol()->PrintRoutingTable(routingStream);
        }
      else
        {
          std::cout << "Node " << i << " has no Ipv4 object?\n";
        }
    }
    std::cout << ">> [ROUTES END] ----------------------------" << std::endl;
  });

  // 9) OnOff traffic
  uint16_t port = 9000;
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (interfaces.GetAddress(0), port));
  onoff.SetConstantRate (DataRate("5Mbps"));
  auto app = onoff.Install (nodes.Get ( nodes.GetN() - 1 ));
  app.Start (Seconds(12.0));
  app.Stop  (Seconds(simTime));
  std::cout << ">> [STEP 7] OnOff app scheduled." << std::endl;

  // 10) Packet sink
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), port));
  auto sinkApp = sink.Install (nodes.Get(0));
  sinkApp.Start (Seconds(0.0));
  sinkApp.Stop  (Seconds(simTime));
  std::cout << ">> [STEP 8] Packet sink installed." << std::endl;

  // 11) FlowMonitor
  FlowMonitorHelper flowmon;
  auto monitor = flowmon.InstallAll ();
  std::cout << ">> [STEP 9] FlowMonitor installed." << std::endl;

  // Print times every 5s
  for (double t = 0.0; t <= simTime; t += 5.0)
    {
      Simulator::Schedule(Seconds(t), [t](){
        std::cout << ">> [TIME] t=" << t << "s" << std::endl;
      });
    }

  // 12) Run
  Simulator::Stop (Seconds(simTime));
  Simulator::Run ();
  std::cout << ">> [STEP 10] Simulator finished at t="
            << Simulator::Now().GetSeconds() << "s" << std::endl;

  // 13) FlowMonitor stats
  monitor->CheckForLostPackets ();
  auto stats = monitor->GetFlowStats ();
  std::cout << ">> [STEP 11] Found " << stats.size() << " flows" << std::endl;

  Simulator::Destroy ();
  std::cout << ">> [STEP 12] Done." << std::endl;
  return 0;
}
