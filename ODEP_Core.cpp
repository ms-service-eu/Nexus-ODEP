#include "ODEP_Core.h"
#include <iostream>

bool ODEPNode::registerResource(const std::string& name) {
    if (resource_count >= ODEP_MAX_RESOURCES) return false;
    resources[resource_count++] = {name, ODEPMode::FREE, "", 0};
    return true;
}

bool ODEPNode::requestResource(const std::string& res_name, const std::string& controller, ODEPMode desired_mode, int priority) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name) {
            if (resources[i].mode == ODEPMode::FREE) {
                resources[i].mode = desired_mode;
                resources[i].current_owner = controller;
                resources[i].priority = priority;
                return true;
            }
            if (resources[i].mode == ODEPMode::EXCLUSIVE && priority > resources[i].priority) {
                std::cout << "[ODEP TRACE] Eviction! " << controller << " verdrängt " << resources[i].current_owner << "\n";
                resources[i].mode = desired_mode;
                resources[i].current_owner = controller;
                resources[i].priority = priority;
                return true; // NOTIFY_EVICTION müsste hier ins Netz gesendet werden
            }
            if (resources[i].mode == ODEPMode::SHARED && desired_mode == ODEPMode::SHARED) {
                return true; // Shared Zugriff erlaubt
            }
        }
    }
    return false; // 409 CONFLICT
}

void ODEPNode::releaseResource(const std::string& res_name, const std::string& controller) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name && resources[i].current_owner == controller) {
            resources[i].mode = ODEPMode::FREE;
            resources[i].current_owner = "";
            resources[i].priority = 0;
        }
    }
}

bool ODEPNode::processIncomingFrame(const std::string& res_name, const std::string& controller, int priority, const std::string& opaque_payload) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name) {
            if (resources[i].mode == ODEPMode::EXCLUSIVE && resources[i].current_owner != controller) {
                return false; // Blockiert durch Exklusiv-Rechte
            }
            executePayload(res_name, opaque_payload);
            return true;
        }
    }
    return false;
}
