/* underwater_laser.cc
 * A simple test of the UnderwaterLaserErrorModel.
 */

 #include "ns3/core-module.h"
 #include "underwater_laser_error_model.h"
 
 using namespace ns3;
 
 NS_LOG_COMPONENT_DEFINE("UnderwaterLaserModelTest");
 
 int main(int argc, char *argv[])
 {
   LogComponentEnable("UnderwaterLaserModelTest", LOG_LEVEL_INFO);
 
   std::string traceFile = "per_snr_table.csv";
   CommandLine cmd;
   cmd.AddValue("traceFile", "Path to PER vs SNR CSV", traceFile);
   cmd.Parse(argc, argv);
 
   Ptr<UnderwaterLaserErrorModel> errModel = CreateObject<UnderwaterLaserErrorModel>();
   errModel->LoadChannelTrace(traceFile);
 
   NS_LOG_INFO("Loaded PER table from " << traceFile);
   return 0;
 }
 