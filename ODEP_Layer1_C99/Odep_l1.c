#include "odep_l1.h"

/* Hilfsfunktion: Vergleicht zwei PTP-Zeitstempel (Rückgabe 1 wenn t1 >= t2, sonst 0) */
static uint8_t odep_time_reached(const OdepPtpTime_t* t1, const OdepPtpTime_t* t2) {
    if (t1->seconds > t2->seconds) return 1;
    if (t1->seconds == t2->seconds) {
        return (t1->nanoseconds >= t2->nanoseconds) ? 1 : 0;
    }
    return 0;
}

/* Initialisiert den L1-Knoten */
int8_t odep_l1_init(void) {
    /* Hier Hardware-Peripherie initialisieren (z.B. UART-Baudrate für DMX/DCC) */
    return 0; /* 0 = OK */
}

/* 
 * Verarbeitet ein eingehendes L1-Paket.
 * Diese Funktion wird idealerweise aus der Hauptschleife (Main-Loop) 
 * oder einem hochpräzisen Timer-Interrupt aufgerufen.
 */
void odep_l1_process_frame(const OdepL1Frame_t* frame, const OdepPtpTime_t* current_system_time) {
    /* 1. Validitätsprüfung mit minimalem Overhead */
    if (frame->header.magic != ODEP_MAGIC_NUMBER) {
        return; /* Falsches Protokoll, sofort verwerfen */
    }
    if (frame->header.layer_depth != 1) {
        return; /* Nicht für diese Hardware-Ebene bestimmt */
    }

    /* 
     * 2. Buffer-Ahead / Zeit-Check
     * Prüfe, ob die aktuelle PTP-Systemzeit die geforderte Ausführungszeit 
     * des Pakets bereits erreicht oder überschritten hat.
     */
    if (odep_time_reached(current_system_time, &(frame->header.timestamp))) {
        
        /* 
         * 3. Direkter Hardware-Output
         * Reicht die rohen Nutzdaten (DMX-Bytes, DCC-Bitstream oder Registerwerte)
         * direkt an die physische Treiberschicht weiter.
         */
        if (frame->header.payload_size <= ODEP_L1_MAX_BYTES) {
            odep_hw_raw_output(frame->payload, frame->header.payload_size);
        }
    }
}
