#include "ODP_Binary.h"
#include <iostream>

// Simulierter DMX-Empfangspuffer der Hardware
uint8_t dmx_buffer[512];

void on_dmx_frame_received() {
    // Pointer-Cast: Interpretiere die rohen Bytes sofort als ODP-Struktur
    ODP_BinaryHeader* odp_frame = reinterpret_cast<ODP_BinaryHeader*>(dmx_buffer);
    
    // 1. Filter: Ist es überhaupt ein ODP-Paket?
    if (odp_frame->start_code != 0x4F) {
        // Fallback: Verarbeite als klassisches, "dummes" DMX
        return; 
    }
    
    // 2. Administrative Logik extrem schnell im Speicher prüfen
    if (odp_frame->cmd_id == 0) { // 0 = EXECUTE_FRAME
        
        std::cout << "[ODP BINARY] Paket erkannt!\n"
                  << "-> Controller-ID: " << odp_frame->controller_id << "\n"
                  << "-> Ziel-Ressource: " << (int)odp_frame->resource_id << "\n"
                  << "-> Modus:          " << (int)odp_frame->resource_mode << "\n"
                  << "-> Priorität:      " << (int)odp_frame->priority << "\n";
                  
        // Hier greift sofort deine Schicht-1-Rechteprüfung aus dem ODP-Core:
        // if (check_rights(odp_frame->resource_id, odp_frame->controller_id, odp_frame->priority)) {
        //     execute_payload(dmx_buffer + sizeof(ODP_BinaryHeader));
        // }
    }
}
