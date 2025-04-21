/* laser_mesh.cc
 * Example: Create 10 nodes in a 2D grid, install pairwise “laser” links,
 * attach custom error‑model (optional), run OLSR, measure UDP echo throughput,
 * dump a FlowMonitor trace for post‑mortem analysis.
 */

 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/olsr-helper.h"
 #include "ns3/applications-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/flow-monitor-module.h"
 #include "underwater_laser_error_model.h"
 
 using namespace ns3;
 
 NS_LOG_COMPONENT_DEFINE ("LaserMeshExample");
 
 int
 main (int argc, char *argv[])
 {
   // Enable all of our debug components
   LogComponentEnable ("LaserMeshExample",        LOG_LEVEL_INFO);
   LogComponentEnable ("OlsrRoutingProtocol",     LOG_LEVEL_INFO);
   LogComponentEnable ("UdpEchoClientApplication",LOG_LEVEL_INFO);
   LogComponentEnable ("UdpEchoServerApplication",LOG_LEVEL_INFO);
 
   CommandLine cmd;
   cmd.Parse (argc, argv);
 
   // 1) Create 10 nodes
   NodeContainer nodes;
   nodes.Create (10);
   NS_LOG_INFO ("Starting LaserMeshExample with " << nodes.GetN () << " nodes");
 
   // 2) Position them in a 2D grid
   MobilityHelper mobility;
   mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX",      DoubleValue (0.0),
                                  "MinY",      DoubleValue (0.0),
                                  "DeltaX",    DoubleValue (50.0),
                                  "DeltaY",    DoubleValue (50.0),
                                  "GridWidth", UintegerValue (5),
                                  "LayoutType",StringValue ("RowFirst"));
   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (nodes);
 
   // 3) Prepare point-to-point “laser” link parameters
   PointToPointHelper p2p;
   p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
   p2p.SetChannelAttribute("Delay",    StringValue ("10ms"));
 
   // 4) Install a full mesh: link every pair of nodes
   NetDeviceContainer devices;
   for (uint32_t i = 0; i < nodes.GetN (); ++i)
     {
       for (uint32_t j = i + 1; j < nodes.GetN (); ++j)
         {
           NetDeviceContainer link = p2p.Install (nodes.Get (i), nodes.Get (j));
           devices.Add (link);
         }
     }
 
   // 5) (Optional) Attach custom error model to each device
   bool attachErrorModel = false;  // set to true to re‑enable your laser error model
   if (attachErrorModel)
     {
       for (uint32_t i = 0; i < devices.GetN (); ++i)
         {
           Ptr<UnderwaterLaserErrorModel> em = CreateObject<UnderwaterLaserErrorModel> ();
           em->LoadChannelTrace ("per_snr_table.csv");
           devices.Get (i)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
         }
       NS_LOG_INFO ("UnderwaterLaserErrorModel attached to all links");
     }
 
   // 6) Install IP stack + OLSR routing
   InternetStackHelper stack;
   OlsrHelper olsr;
   Ipv4ListRoutingHelper list;
   list.Add (olsr, 10);
   stack.SetRoutingHelper (list);
   stack.Install (nodes);
 
   // 7) Assign IP addresses to all devices
   Ipv4AddressHelper address;
   address.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer ifs = address.Assign (devices);
 
   // 8) Install a FlowMonitor on all nodes
   FlowMonitorHelper flowHelper;
   Ptr<FlowMonitor> monitor = flowHelper.InstallAll ();
 
   // 9) UDP Echo server on node 0
   uint16_t port = 9;
   UdpEchoServerHelper echoServer (port);
   ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
   serverApps.Start (Seconds (1.0));
   serverApps.Stop  (Seconds (30.0));
 
   // 10) UDP Echo client on node 9 → sends to node 0
   UdpEchoClientHelper echoClient (ifs.GetAddress (0), port);
   echoClient.SetAttribute ("MaxPackets", UintegerValue (1000));
   echoClient.SetAttribute ("Interval",   TimeValue     (Seconds (1.0)));
   echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
   ApplicationContainer clientApps = echoClient.Install (nodes.Get (9));
   clientApps.Start (Seconds (10.0));  // give OLSR 10s to converge
   clientApps.Stop  (Seconds (30.0));
 
   // 11) Run the simulation
   Simulator::Stop (Seconds (35.0));
   Simulator::Run ();
 
   // 12) Dump flow‑monitor results for post‑run analysis
   monitor->SerializeToXmlFile ("laser-mesh-flow.xml", true, true);
 
   Simulator::Destroy ();
   NS_LOG_INFO ("LaserMeshExample finished");
   return 0;
 }
 