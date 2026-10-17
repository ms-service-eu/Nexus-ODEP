#include "ODEP_Scheduler.h"
#include "ODEP_Core.h"

#ifdef ODEP_ENABLE_SCHEDULER
bool ODEPScheduler::scheduleFrame(uint64_t ptp_time, uint32_t duration, const std::string& controller, const std::string& payload) {
    // Einfache Kollisionsprüfung im Array
    for (int i = 0; i < ODEP_MAX_FUTURE_SLOTS; ++i) {
        if (!timeline[i].active) {
            timeline[i] = {ptp_time, duration, controller, payload, true};
            return true;
        }
    }
    return false;
}

void ODEPScheduler::tick(uint64_t current_ptp_time, ODEPNode* node, const std::string& res_name) {
    for (int i = 0; i < ODEP_MAX_FUTURE_SLOTS; ++i) {
        if (timeline[i].active && current_ptp_time >= timeline[i].start_time_ptp) {
            // Buffer-Ahead Zeit erreicht! Payload an Core übergeben
            node->processIncomingFrame(res_name, timeline[i].controller_id, 100, timeline[i].payload);
            timeline[i].active = false; // Slot verbraucht
        }
    }
}
#endif
