#ifndef ML_LINK_METRIC_H
#define ML_LINK_METRIC_H

#include "ns3/object.h"
#include "ns3/node.h"

namespace ns3 {

class MlLinkMetric : public Object
{
public:
  static TypeId GetTypeId ();
  MlLinkMetric () = default;

  // Stub: we’ll replace this with your real turbulence–sediment predictor
  double PredictCost (Ptr<Node> a, Ptr<Node> b) const;
};

} // namespace ns3

#endif // ML_LINK_METRIC_H
