#ifdef NS3_MODULE_COMPILATION 
    error "Do not include ns3 module aggregator headers from other modules these are meant only for end user scripts." 
#endif 
#ifndef NS3_MODULE_UNDERWATER_LASER
    // Module headers: 
    #include <ns3/underwater-laser-channel.h>
    #include <ns3/underwater-laser-propagation-loss-model.h>
    #include <ns3/underwater-laser-error-rate-model.h>
    #include <ns3/underwater-laser-rate-table.h>
    #include <ns3/underwater-laser-net-device.h>
    #include <ns3/underwater-laser-channel-helper.h>
    #include <ns3/underwater-laser-phy-helper.h>
    #include <ns3/underwater-laser-mac-helper.h>
    #include <ns3/underwater-laser-helper.h>
#endif 