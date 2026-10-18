#include "Odep_l1.h"

int8_t odep_l1_init(void) {
    /* Hier Hardware-Peripherie initialisieren (z.B. UART-Baudrate für DMX/CAN) */
    return 0; 
}

void odep_l1_process_frame(const OdepL1Frame_t* frame, uint32_t current_sec, uint32_t current_nsec) {
    /* 1. Validitätsprüfung auf dem Microcontroller mit minimalen CPU-Zyklen */
    if (frame->header.magic != ODEP_MAGIC_NUMBER) {
        return; /* Falsches Protokoll, sofort verwerfen */
    }
    if (frame->header.layer_depth != 1) {
        return; /* Nicht für diese Hardware-Ebene bestimmt */
    }

    /* 
     * 2. Echtzeit Buffer-Ahead / Delta-Zeit-Check
     * Direkter Vergleich der entpackten PTP-Registerwerte zur Einsparung von Struct-Kopien.
     */
    if (current_sec > frame->header.timestamp.seconds || 
       (current_sec == frame->header.timestamp.seconds && current_nsec >= frame->header.timestamp.nanoseconds)) {
        
        /* 3. Direkter Hardware-Output des Differenz-Pakets */
        if (frame->header.payload_size <= ODEP_L1_MAX_BYTES) {
            odep_hw_raw_output(frame->payload, (uint16_t)frame->header.payload_size);
        }
    }
}
