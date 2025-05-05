#ifndef UNDERWATER_LASER_PROPAGATION_LOSS_MODEL_H
#define UNDERWATER_LASER_PROPAGATION_LOSS_MODEL_H

#include "ns3/propagation-loss-model.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"
#include "ns3/mobility-model.h"

#include <vector>
#include <string>
#include <utility>
#include <map>

/**
 * \file underwater-laser-propagation-loss-model.h
 * \ingroup underwater-laser
 *
 * A custom PropagationLossModel that reads alpha_depth_time.csv
 * to compute time- and depth-dependent Beer–Lambert attenuation plus
 * approximate beam divergence.
 */

namespace ns3 {

/**
 * \class UnderwaterLaserPropagationLossModel
 * \brief Implements a 2D (time+depth) Beer–Lambert attenuation with optional beam divergence.
 *
 * Expects alpha_depth_time.csv with columns:
 *   time_s, z_mid, alpha_m^-1
 *
 * We group rows by time, then within each time sort by z_mid.  At runtime:
 *   - Compute midpoint depth between Tx and Rx (zMid = (zA+zB)/2).
 *   - Interpolate alpha in time and depth slices.
 *   - Apply P_rx = P_tx * beamGain(distance) * exp(-alpha * distance).
 */
class UnderwaterLaserPropagationLossModel : public PropagationLossModel
{
public:
  static TypeId GetTypeId (void);

  UnderwaterLaserPropagationLossModel ();
  virtual ~UnderwaterLaserPropagationLossModel ();

  /**
   * \brief Specify the CSV file containing (time,z_mid,alpha).
   */
  void SetAlphaCsv (const std::string &alphaCsv);

  /**
   * \brief Set the beam divergence half-angle in radians.
   */
  void SetBeamDivergence (double divergenceRad);

protected:
  /**
   * \brief NS-3’s main interface to compute Rx power in dBm from Tx in dBm.
   */
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const override;

  /**
   * \brief We don't use random streams here, so just return 0.
   */
  virtual int64_t DoAssignStreams (int64_t stream) override;

private:

  struct DepthAlpha
  {
    double zMid;
    double alpha;
  };

  // For each time, store a vector of (zMid, alpha).
  std::vector<std::pair<double, std::vector<DepthAlpha>>> m_alphaTable2D;

  std::string m_alphaCsvFilename;
  double      m_divergenceRad;
  double      m_w0; ///< an initial beam waist or radius offset.

private:
  double GetAlphaAtTimeAndDepth (Time t, double z) const;
  double InterpDepth (const std::vector<DepthAlpha> &vec, double z) const;
  double GetBeamGain (double distance) const;
};

} // namespace ns3

#endif // UNDERWATER_LASER_PROPAGATION_LOSS_MODEL_H
