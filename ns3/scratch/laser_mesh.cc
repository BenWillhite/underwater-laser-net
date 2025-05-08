/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <filesystem>
#include <fstream>                           // log file redirection

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"             // InternetStackHelper, Ipv4AddressHelper
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/log.h"
#include "ns3/arp-cache.h"
#include "ns3/mac48-address.h"
#include "ns3/object-ptr-container.h"
#include "ns3/aodv-helper.h"                 // AODV on-demand routing
#include "ns3/ipv4-list-routing-helper.h"    // combine multiple routing protocols
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/underwater-laser-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LaserMeshExample");

int
main (int argc, char *argv[])
{
  // --- 0) Prepare log file
  const std::string logName = "laser_mesh.log";
  if (std::filesystem::exists (logName))
    {
      std::filesystem::remove (logName);
    }
  std::ofstream logFile (logName);
  logFile << std::unitbuf; // flush immediately
  auto oldCout = std::cout.rdbuf (logFile.rdbuf ());
  auto oldCerr = std::cerr.rdbuf (logFile.rdbuf ());

  // --- 1) Simulation parameters & logging
  NS_LOG_INFO ("Entering LaserMeshExample::main()");
  LogComponentEnable ("Ipv4L3Protocol",           LOG_LEVEL_ALL);
  LogComponentEnable ("ArpL3Protocol",            LOG_LEVEL_ALL);
  LogComponentEnable ("UnderwaterLaserRateTable", LOG_LEVEL_ALL);

  double nodeSpacing      = 20.0;
  bool   attachErrorModel = true;
  double simTime          = 30.0;
  double txDbm            = 20.0;

  CommandLine cmd;
  cmd.AddValue ("nodeSpacing",      "Grid spacing (m)",            nodeSpacing);
  cmd.AddValue ("attachErrorModel", "Enable PER error model",      attachErrorModel);
  cmd.AddValue ("simTime",          "Simulation time (s)",         simTime);
  cmd.AddValue ("txDbm",            "Transmit power (dBm)",        txDbm);
  cmd.Parse (argc, argv);

  // CSV data files
  const std::string alphaCsv     = "scratch/alpha_depth_time.csv";
  const std::string perSnrCsv    = "scratch/per_snr_table.csv";
  const std::string rateTableCsv = "scratch/cleaned_rate_vs_distance.csv";

  // --- 2) Check CSV presence
  NS_LOG_INFO ("Verifying CSV file presence...");
  if (!std::filesystem::exists (alphaCsv) ||
      (attachErrorModel && !std::filesystem::exists (perSnrCsv)) ||
      !std::filesystem::exists (rateTableCsv))
    {
      std::cerr << "ERROR: Missing CSV files\n"
                << "  " << alphaCsv << "\n"
                << "  " << perSnrCsv << "\n"
                << "  " << rateTableCsv << std::endl;
      return 1;
    }

  // --- 3) Create & position nodes in 3×3 grid
  NodeContainer nodes;
  nodes.Create (9);
  NS_LOG_INFO ("Created " << nodes.GetN() << " nodes");

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX",      DoubleValue (0.0),
                                 "MinY",      DoubleValue (0.0),
                                 "DeltaX",    DoubleValue (nodeSpacing),
                                 "DeltaY",    DoubleValue (nodeSpacing),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType",StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  // --- 4) Configure underwater‐laser PHY
  Ptr<UnderwaterLaserHelper> laser = CreateObject<UnderwaterLaserHelper> ();
  laser->SetAttribute ("AlphaCsv",       StringValue (alphaCsv));
  laser->SetAttribute ("BeamDivergence", DoubleValue (1e-5));
  if (attachErrorModel)
    {
      laser->SetPhyAttribute ("TraceFilename", StringValue (perSnrCsv));
    }
  laser->SetRateTableCsv (rateTableCsv);
  laser->SetChannelAttribute ("TxPowerDbm", DoubleValue (txDbm));
  laser->SetChannelAttribute ("MinRate", DataRateValue (DataRate ("100Mbps")));

  // --- 5) Install Internet stack with AODV routing
  AodvHelper            aodv;
  Ipv4ListRoutingHelper list;
  list.Add (aodv, 10);
  InternetStackHelper   internet;
  internet.SetRoutingHelper (list);
  internet.Install (nodes);

  // --- 6) Build /30 subnets and attach laser devices
  Ipv4AddressHelper ipv4;
  uint32_t subnetIndex = 0;
  Ipv4Address sinkAddress;
  for (uint32_t row = 0; row < 3; ++row)
    {
      for (uint32_t col = 0; col < 3; ++col)
        {
          uint32_t id = row*3 + col;
          // Horizontal link
          if (col < 2)
          {
            auto link = laser->InstallPair (nodes.Get(id), nodes.Get(id+1));
            {
              std::string net = "10.1." + std::to_string(++subnetIndex) + ".0";
              ipv4.SetBase (net.c_str(), "255.255.255.252");
            }
            auto ifs = ipv4.Assign (link);
            if (row == 0 && col == 0) sinkAddress = ifs.GetAddress(0);
          }

          // Vertical link
          if (row < 2)
          {
            auto link = laser->InstallPair (nodes.Get(id), nodes.Get(id+3));
            {
              std::string net = "10.1." + std::to_string(++subnetIndex) + ".0";
              ipv4.SetBase (net.c_str(), "255.255.255.252");
            }
            ipv4.Assign (link);
          }

        }
    }

  // --- 7) Schedule routing table dump at t=6s
  Simulator::Schedule (Seconds (6.0), [&nodes]()
    {
      Ptr<OutputStreamWrapper> osw = Create<OutputStreamWrapper> (&std::cout);
      std::cout << ">> [ROUTES @6s]\n";
      for (uint32_t i = 0; i < nodes.GetN(); ++i)
        {
          nodes.Get(i)->GetObject<Ipv4> ()->
            GetRoutingProtocol ()->PrintRoutingTable (osw);
        }
    });

  // --- 8) Set up traffic: node8 → node0
  uint16_t port = 9000;
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (sinkAddress, port));
  onoff.SetConstantRate (DataRate ("5Mbps"));
  ApplicationContainer apps = onoff.Install (nodes.Get(8));
  apps.Start (Seconds (20.0));
  apps.Stop  (Seconds (simTime));

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), port));
  apps = sink.Install (nodes.Get(0));
  apps.Start (Seconds (0.0));
  apps.Stop  (Seconds (simTime));

  // --- 9) FlowMonitor for stats
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // Print time markers every 5s
  for (double t = 0.0; t <= simTime; t += 5.0)
    {
      Simulator::Schedule (Seconds (t),
        [t]() { std::cout << ">> [TIME] t=" << t << "s\n"; });
    }

  // --- 10) Run & collect
  Simulator::Stop   (Seconds (simTime));
  Simulator::Run    ();
  monitor->CheckForLostPackets ();

  // Report FlowMonitor results
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (
                                          flowmon.GetClassifier ());
  auto stats = monitor->GetFlowStats ();
  std::cout << "=== FLOW MONITOR RESULTS ===\n";
  for (auto &kv : stats)
    {
      auto fs = kv.second;
      auto t  = classifier->FindFlow (kv.first);
      double duration = fs.timeLastRxPacket.GetSeconds()
                        - fs.timeFirstTxPacket.GetSeconds ();
      double throughput = (duration>0)
                          ? fs.rxBytes*8.0/duration
                          : 0.0;
      std::cout << t.sourceAddress << "->" << t.destinationAddress
                << "  Tx=" << fs.txPackets
                << ", Rx="   << fs.rxPackets
                << ", Lost=" << fs.lostPackets
                << ", Thrpt="<< (throughput/1e6) << " Mbps\n";
    }

  Simulator::Destroy ();
  std::cout.rdbuf (oldCout);
  std::cerr.rdbuf (oldCerr);
  return 0;
}
