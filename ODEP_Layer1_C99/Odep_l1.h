/*
Copyright (C) 2026 Michael Schultz, Bremen-Germany

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#ifndef ODEP_L1_H
#define ODEP_H

#include <stdint.h>

/* Synchronisiert mit dem C++ Core: "NXOD" */
#define ODEP_MAGIC_NUMBER 0x4E584F44  
#define ODEP_L1_MAX_BYTES 16          /* Strikte Begrenzung für ressourcenarme MCUs */

#pragma pack(push, 1)

/* 
 * Kompakte Echtzeit-PTP-Zeitstruktur (8 Bytes gesamt).
 * Reduziert auf 4 Bytes Sekunden und 4 Bytes Nanosekunden.
 * Reicht für den lokalen Ausführungs-Horizont im Buffer-Ahead vollkommen aus.
 */
typedef struct {
    uint32_t seconds;
    uint32_t nanoseconds;
} OdepL1MiniTime_t;

/* 
 * Hochkompakter Layer-1 Differenz-Header (Exakt 11 Bytes).
 * Enthält nur die absolut kritischen Daten für die lokale Austaktung.
 */
typedef struct {
    uint32_t         magic;         /* 4 Bytes */
    uint8_t          layer_depth;   /* 1 Byte  (Für L1 immer 0x01) */
    uint8_t          priority;      /* 1 Byte  */
    uint8_t          payload_size;  /* 1 Byte  (Maximal 16) */
    OdepL1MiniTime_t timestamp;     /* 4 Bytes Ausführungszeit (Delta-Deadline) */
} OdepL1MiniHeader_t;

/* Das finale, extrem schlanke Differenz-Paket im RAM (Exakt 27 Bytes) */
typedef struct {
    OdepL1MiniHeader_t header;
    uint8_t            payload[ODEP_L1_MAX_BYTES];
} OdepL1Frame_t;

typedef struct {
    uint8_t  device_class;         /* 1 Byte */
    uint8_t  hardware_revision;    /* 1 Byte */
    uint16_t active_sub_devices;   /* 2 Bytes */
    
    /* --- CRITICAL SECURITY CAPABILITY FLAGS --- */
    uint8_t  bus_capabilities;     /* Bit 0: 1 = Verträgt große Frames (1464B), 0 = NUR kleine Frames (27B) */
                                   /* Bits 1-7: Reserviert für zukünftige Bus-Härtung */
    
    char     device_name[20];      /* 20 Bytes */
    uint8_t  _reserved_align;      /* 1 Byte Pad für perfekte 4-Byte/8-Byte Ausrichtung */
} OdepDiscoveryPayload_t;

#pragma pack(pop)


/* Funktionsprototypen für die Hardware-Knoten-Logik */
int8_t odep_l1_init(void);
void   odep_l1_process_frame(const OdepL1Frame_t* frame, uint32_t current_sec, uint32_t current_nsec);

/* BSP-Hardware-Treiberfunktion */
extern void odep_hw_raw_output(const uint8_t* data, uint16_t size);

#endif /* ODEP_L1_H */
