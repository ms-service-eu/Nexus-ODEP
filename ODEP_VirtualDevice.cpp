#include "ODEP_VirtualDevice.h"
#include <iostream>

#ifdef ODEP_ENABLE_VIRTUALIZATION
void ODEPVirtualDevice::mapResourceToPhysicalOut(const std::string& res_name, const std::string& proto, int addr) {
    hardware_matrix[res_name] = {proto, addr};
    registerResource(res_name);
}

void ODEPVirtualDevice::executePayload(const std::string& res_name, const std::string& payload) {
    auto mapping = hardware_matrix[res_name];
    
    // Hier schlägt die Proxy-Abstraktion zu:
    std::cout << "[ODEP PROXY -> " << mapping.protocol << " @ Addr " << mapping.address << "] "
              << "Translating abstract payload: " << payload << "\n";
              
    // HIER käme der Treiber-Code hin, um z.B. echte serielle DMX-Bytes zu senden
}
#endif
