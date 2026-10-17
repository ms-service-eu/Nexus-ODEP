#include "ODEP_Binary.h"
#include <iostream>

// Simulierter DMX-Empfangspuffer der Hardware
uint8_t dmx_buffer[512];

void on_dmx_frame_received() {
    // Pointer-Cast: Interpretiere die rohen Bytes sofort als ODEP-Struktur
    ODEP_BinaryHeader* odep_frame = reinterpret_cast<ODEP_BinaryHeader*>(dmx_buffer);
    
    // 1. Filter: Ist es überhaupt ein ODEP-Paket?
    if (odep_frame->start_code != 0x4F) {
        // Fallback: Verarbeite als klassisches, "dummes" DMX
        return; 
    }
    
    // 2. Administrative Logik extrem schnell im Speicher prüfen
    if (odep_frame->cmd_id == 0) { // 0 = EXECUTE_FRAME
        
        std::cout << "[ODEP BINARY] Paket erkannt!\n"
                  << "-> Controller-ID: " << odep_frame->controller_id << "\n"
                  << "-> Ziel-Ressource: " << (int)odep_frame->resource_id << "\n"
                  << "-> Modus:          " << (int)odep_frame->resource_mode << "\n"
                  << "-> Priorität:      " << (int)odep_frame->priority << "\n";
                  
        // Hier greift sofort deine Schicht-1-Rechteprüfung aus dem ODEP-Core:
        // if (check_rights(odep_frame->resource_id, odep_frame->controller_id, odep_frame->priority)) {
        //     execute_payload(dmx_buffer + sizeof(ODEP_BinaryHeader));
        // }
    }
}
