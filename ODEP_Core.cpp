#include "ODP_Core.h"
#include <iostream>

bool ODPNode::registerResource(const std::string& name) {
    if (resource_count >= ODP_MAX_RESOURCES) return false;
    resources[resource_count++] = {name, ODPMode::FREE, "", 0};
    return true;
}

bool ODPNode::requestResource(const std::string& res_name, const std::string& controller, ODPMode desired_mode, int priority) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name) {
            if (resources[i].mode == ODPMode::FREE) {
                resources[i].mode = desired_mode;
                resources[i].current_owner = controller;
                resources[i].priority = priority;
                return true;
            }
            if (resources[i].mode == ODPMode::EXCLUSIVE && priority > resources[i].priority) {
                std::cout << "[ODP TRACE] Eviction! " << controller << " verdrängt " << resources[i].current_owner << "\n";
                resources[i].mode = desired_mode;
                resources[i].current_owner = controller;
                resources[i].priority = priority;
                return true; // NOTIFY_EVICTION müsste hier ins Netz gesendet werden
            }
            if (resources[i].mode == ODPMode::SHARED && desired_mode == ODPMode::SHARED) {
                return true; // Shared Zugriff erlaubt
            }
        }
    }
    return false; // 409 CONFLICT
}

void ODPNode::releaseResource(const std::string& res_name, const std::string& controller) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name && resources[i].current_owner == controller) {
            resources[i].mode = ODPMode::FREE;
            resources[i].current_owner = "";
            resources[i].priority = 0;
        }
    }
}

bool ODPNode::processIncomingFrame(const std::string& res_name, const std::string& controller, int priority, const std::string& opaque_payload) {
    for (int i = 0; i < resource_count; ++i) {
        if (resources[i].name == res_name) {
            if (resources[i].mode == ODPMode::EXCLUSIVE && resources[i].current_owner != controller) {
                return false; // Blockiert durch Exklusiv-Rechte
            }
            executePayload(res_name, opaque_payload);
            return true;
        }
    }
    return false;
}
