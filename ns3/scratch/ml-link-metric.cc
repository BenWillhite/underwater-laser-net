#include "ml-link-metric.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MlLinkMetric");

TypeId
MlLinkMetric::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MlLinkMetric")
    .SetParent<Object> ()
    .AddConstructor<MlLinkMetric> ();
  return tid;
}

double
MlLinkMetric::PredictCost (Ptr<Node> a, Ptr<Node> b) const
{
  return 1.0;
}

} // namespace ns3
