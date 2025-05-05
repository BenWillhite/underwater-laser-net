#include "throughput-tracer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/application.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ThroughputTracer");

ThroughputTracer::ThroughputTracer ()
  : m_bytesSent     (0),
    m_bytesReceived (0)
{
}

void
ThroughputTracer::InstallOn (NodeContainer nodes)
{
  for (uint32_t i = 0; i < nodes.GetN (); ++i)
    {
      Ptr<Node> node = nodes.Get (i);
      NS_LOG_INFO ("ThroughputTracer: inspecting Node " << node->GetId ());
      uint32_t nApps = node->GetNApplications ();
      NS_LOG_INFO (" Node " << node->GetId()
                  << " has " << nApps << " applications");

      for (uint32_t j = 0; j < nApps; ++j)
        {
          Ptr<Application> app = node->GetApplication (j);
          std::string typeName = app->GetInstanceTypeId ().GetName ();
          NS_LOG_INFO ("  App[" << j << "] type: " << typeName);

          if (typeName == "ns3::UdpEchoClient")
            {
              std::ostringstream txPath;
              txPath << "/NodeList/" << node->GetId()
                     << "/ApplicationList/" << j
                     << "/$ns3::UdpEchoClient/Tx";
              NS_LOG_INFO ("   Hooking Tx callback at " << txPath.str ());
              Config::ConnectWithoutContext (txPath.str (),
                                             MakeCallback (&ThroughputTracer::TxCallback, this));
            }
          else if (typeName == "ns3::UdpEchoServer")
            {
              std::ostringstream rxPath;
              rxPath << "/NodeList/" << node->GetId()
                     << "/ApplicationList/" << j
                     << "/$ns3::UdpEchoServer/Rx";
              NS_LOG_INFO ("   Hooking Rx callback at " << rxPath.str ());
              Config::ConnectWithoutContext (rxPath.str (),
                                             MakeCallback (&ThroughputTracer::RxCallback, this));
            }
          else
            {
              NS_LOG_DEBUG ("   Skipping App[" << j << "] (" << typeName << ")");
            }
        }
    }
}

void
ThroughputTracer::TxCallback (Ptr<const Packet> p)
{
  m_bytesSent += p->GetSize ();
  NS_LOG_INFO ("[Tracer] total Tx bytes = " << m_bytesSent);
}

void
ThroughputTracer::RxCallback (Ptr<const Packet> p)
{
  m_bytesReceived += p->GetSize ();
  NS_LOG_INFO ("[Tracer] total Rx bytes = " << m_bytesReceived);
}

} // namespace ns3
