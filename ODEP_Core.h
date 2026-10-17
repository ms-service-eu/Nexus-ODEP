#pragma once
#include "ODP_Config.h"
#include <string>
#include <vector>

enum class ODPMode { FREE, EXCLUSIVE, SHARED };

struct Resource {
    std::string name;
    ODPMode mode = ODPMode::FREE;
    std::string current_owner = "";
    int priority = 0;
};

class ODPNode {
protected:
    Resource resources[ODP_MAX_RESOURCES];
    int resource_count = 0;

public:
    ODPNode() = default;
    
    bool registerResource(const std::string& name);
    
    // Verarbeitet die administrativen Rechte im ODP-Rahmen
    bool requestResource(const std::string& res_name, const std::string& controller, ODPMode desired_mode, int priority);
    void releaseResource(const std::string& res_name, const std::string& controller);
    
    // Nimmt den ODP-Rahmen entgegen und entscheidet über die Weitergabe der Payload
    bool processIncomingFrame(const std::string& res_name, const std::string& controller, int priority, const std::string& opaque_payload);
    
    virtual void executePayload(const std::string& res_name, const std::string& payload) {
        // Muss von der eigentlichen Hardware-Applikation überschrieben werden
    }
};
