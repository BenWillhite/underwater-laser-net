#ifndef THROUGHPUT_TRACER_H
#define THROUGHPUT_TRACER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"

namespace ns3 {

/**
 * \brief Track bytes Tx/Rx on your UDP echo sockets and log instantaneous throughput.
 */
class ThroughputTracer
{
public:
  ThroughputTracer ();
  /** Hook trace sinks onto each node's UdpEchoClient/Server apps. */
  void InstallOn (NodeContainer nodes);

private:
  void TxCallback (Ptr<const Packet> p);
  void RxCallback (Ptr<const Packet> p);

  uint64_t m_bytesSent;
  uint64_t m_bytesReceived;
};

} // namespace ns3

#endif // THROUGHPUT_TRACER_H
