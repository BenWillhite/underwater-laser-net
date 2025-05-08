#ifndef ML_LINK_METRIC_H
#define ML_LINK_METRIC_H

#include "ns3/object.h"
#include "ns3/node.h"

/*
Author's note:

These files are NOT utilized in the actual ML model. This serves
as a placeholder for those who want to experiment with using a native
CPP based integration, instead of a python-based integration as we tested.
*/

namespace ns3 {

class MlLinkMetric : public Object
{
public:
  static TypeId GetTypeId ();
  MlLinkMetric () = default;

  double PredictCost (Ptr<Node> a, Ptr<Node> b) const;
};

} // namespace ns3

#endif // ML_LINK_METRIC_H
