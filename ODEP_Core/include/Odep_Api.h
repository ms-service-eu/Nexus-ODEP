#ifndef ODEP_API_H
#define ODEP_API_H

#include <stdint.h>

/* 
 * Vom Node exportierte, sichere API-Schnittstellen.
 * Der Compiler linkt das Anwenderprogramm gegen diese Stubs.
 * Der Code selbst sieht niemals die echten Hardware-Register!
 */

/* --- AKTOREN-API --- */
/* Steuert eine Achse, einen Servo oder eine Triebkraft (Übersetzt intern in L2/L1) */
void odep_api_write_actuator(uint32_t logical_id, int32_t primary_fixed, int32_t secondary_fixed);

/* Schaltet diskrete Hardware-Zustände (z.B. Relais, Shutter, Weichen-Verriegelung) */
void odep_api_set_state_flag(uint32_t logical_id, uint32_t bitmask);


/* --- SENSOREN-API --- */
/* Liefert den präzisen Q16.16 Festkomma-Wert eines Sensors (z.B. Position, Strom, Temperatur) */
int32_t odep_api_read_sensor(uint32_t logical_id, uint8_t channel_index);

/* Überprüft, ob ein Sub-Gerät betriebsbereit und verriegelt ist */
uint8_t odep_api_is_device_ready(uint32_t logical_id);

#endif /* ODEP_API_H */
