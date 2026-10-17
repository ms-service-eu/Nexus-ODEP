#pragma once
#include "ODEP_Config.h"
#include <string>
#include <vector>

enum class ODEPMode { FREE, EXCLUSIVE, SHARED };

struct Resource {
    std::string name;
    ODEPMode mode = ODEPMode::FREE;
    std::string current_owner = "";
    int priority = 0;
};

class ODEPNode {
protected:
    Resource resources[ODEP_MAX_RESOURCES];
    int resource_count = 0;

public:
    ODEPNode() = default;
    
    bool registerResource(const std::string& name);
    
    // Verarbeitet die administrativen Rechte im ODEP-Rahmen
    bool requestResource(const std::string& res_name, const std::string& controller, ODEPMode desired_mode, int priority);
    void releaseResource(const std::string& res_name, const std::string& controller);
    
    // Nimmt den ODEP-Rahmen entgegen und entscheidet über die Weitergabe der Payload
    bool processIncomingFrame(const std::string& res_name, const std::string& controller, int priority, const std::string& opaque_payload);
    
    virtual void executePayload(const std::string& res_name, const std::string& payload) {
        // Muss von der eigentlichen Hardware-Applikation überschrieben werden
    }
};
