#ifndef ODEP_L1_H
#define ODEP_L1_H

#include <stdint.h>
#include <string.h>

#define ODEP_MAGIC_NUMBER 0x4E4F4450  /* "NODP" in ASCII */
#define ODEP_L1_MAX_BYTES 16          /* Begrenzt auf 16 Bytes Payload für kleine MCUs */

/* Extrem kompakte PTP-Zeitstruktur (64-Bit Sekunden, 32-Bit Nanosekunden) */
typedef struct {
    uint64_t seconds;
    uint32_t nanoseconds;
} OdepPtpTime_t;

/* Minimaler Layer-1 Netzwerk-Header (Bit-Packed und aligned) */
#pragma pack(push, 1)
typedef struct {
    uint32_t      magic;
    uint8_t       version;
    uint8_t       layer_depth;  /* Wird für L1 immer 1 sein */
    uint8_t       priority;
    uint16_t      payload_size;
    OdepPtpTime_t timestamp;     /* Vorgesehene Ausführungszeit */
} OdepL1Header_t;
#pragma pack(pop)

/* Das finale, extrem schlanke L1-Paket im RAM */
typedef struct {
    OdepL1Header_t header;
    uint8_t        payload[ODEP_L1_MAX_BYTES];
} OdepL1Frame_t;

/* Funktionsprototypen für die Hardware-Knoten-Logik */
int8_t odep_l1_init(void);
void   odep_l1_process_frame(const OdepL1Frame_t* frame, const OdepPtpTime_t* current_system_time);

/* Von dir bereitzustellende Hardware-Treiberfunktion (z.B. in Assembler oder C-Treiber) */
extern void odep_hw_raw_output(const uint8_t* data, uint16_t size);

#endif /* ODEP_L1_H */
