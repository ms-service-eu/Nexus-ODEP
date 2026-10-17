#include "ODP_VirtualDevice.h"
#include <iostream>

#ifdef ODP_ENABLE_VIRTUALIZATION
void ODPVirtualDevice::mapResourceToPhysicalOut(const std::string& res_name, const std::string& proto, int addr) {
    hardware_matrix[res_name] = {proto, addr};
    registerResource(res_name);
}

void ODPVirtualDevice::executePayload(const std::string& res_name, const std::string& payload) {
    auto mapping = hardware_matrix[res_name];
    
    // Hier schlägt die Proxy-Abstraktion zu:
    std::cout << "[ODP PROXY -> " << mapping.protocol << " @ Addr " << mapping.address << "] "
              << "Translating abstract payload: " << payload << "\n";
              
    // HIER käme der Treiber-Code hin, um z.B. echte serielle DMX-Bytes zu senden
}
#endif
