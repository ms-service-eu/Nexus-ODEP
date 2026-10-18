#include "odep_l1.h"

void odep_hw_raw_output(const uint8_t* data, uint16_t size) {
    /* Unterdrückt die Unused-Parameter Warnung für den leeren Hardware-Mock */
    (void)data; 
    (void)size;

    /* 
     * Sobald du echten Hardware-Code einfügst, kannst du diese Schleife nutzen:
     *
     * for (uint16_t i = 0; i < size; i++) {
     *     // UDR0 = data[i]; // Beispiel für AVR UART
     * }
     */
}
