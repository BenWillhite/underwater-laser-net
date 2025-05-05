#ifndef UNDERWATER_LASER_ERROR_RATE_MODEL_H
#define UNDERWATER_LASER_ERROR_RATE_MODEL_H

#include "ns3/object.h"
#include "ns3/type-id.h"
#include <vector>
#include <utility>
#include <string>

namespace ns3 {

/**
 * \class UnderwaterLaserErrorRateModel
 * \brief A simpler error model for underwater lasers, returning success probability based on SNR & nbits.
 */
class UnderwaterLaserErrorRateModel : public Object
{
public:
  static TypeId GetTypeId ();
  UnderwaterLaserErrorRateModel ();
  virtual ~UnderwaterLaserErrorRateModel ();

  /**
   * \brief Read SNR->(PER or BER) from CSV; set isBerData if this file is BER
   */
  void Load (const std::string &filename, bool isBerData = false);

  /**
   * \brief Return the success probability (1 - PER) for a chunk of nbits at SNR=snrDb
   */
  double DoGetChunkSuccessRate (double snrDb, uint64_t nbits);

  /**
   * \brief Setter for the TraceFilename attribute: load PER data from CSV
   */
  void SetTraceFilename (const std::string &filename);

  /**
   * \brief Getter for the TraceFilename attribute
   */
  std::string GetTraceFilename () const;

private:
  double InterpolateRawValue (double snrDb);
  double ComputeChunkPer      (double snrDb, uint64_t nbits);

  bool                m_isBerData;
  std::string         m_traceFilename;
  std::vector<std::pair<double,double>> m_table;
};

} // namespace ns3

#endif // UNDERWATER_LASER_ERROR_RATE_MODEL_H
