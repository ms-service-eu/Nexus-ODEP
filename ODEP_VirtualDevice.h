#pragma once
#include "ODP_Config.h"

#ifdef ODP_ENABLE_VIRTUALIZATION
#include "ODP_Core.h"
#include <map>

struct SubHardwareMapping {
    std::string protocol; // e.g. "DMX_OUT_A", "MODBUS"
    int address;
};

class ODPVirtualDevice : public ODPNode {
private:
    std::string vd_id;
    std::map<std::string, SubHardwareMapping> hardware_matrix;

public:
    ODPVirtualDevice(const std::string& id) : vd_id(id) {}
    
    void mapResourceToPhysicalOut(const std::string& res_name, const std::string& proto, int addr);
    
    // Überschreibt die Core-Execution, um Daten an die physikalischen Ports zu jagen
    void executePayload(const std::string& res_name, const std::string& payload) override;
};
#endif
