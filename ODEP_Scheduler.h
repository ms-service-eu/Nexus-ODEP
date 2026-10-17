#pragma once
#include "ODEP_Config.h"

#ifdef ODEP_ENABLE_SCHEDULER
#include <cstdint>
#include <string>

struct FutureSlot {
    uint64_t start_time_ptp;
    uint32_t duration_ms;
    std::string controller_id;
    std::string payload;
    bool active = false;
};

class ODEPScheduler {
private:
    FutureSlot timeline[ODEP_MAX_FUTURE_SLOTS];
public:
    ODEPScheduler() = default;
    bool scheduleFrame(uint64_t ptp_time, uint32_t duration, const std::string& controller, const std::string& payload);
    void tick(uint64_t current_ptp_time, class ODEPNode* node, const std::string& res_name);
};
#endif
