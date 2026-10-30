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


#ifndef ODEP_ARCHITECTURE_H
#define ODEP_ARCHITECTURE_H

#include <stdint.h>

#pragma pack(push, 1)

/* Eindeutige IDs für CPU-Architekturen */
typedef uint16_t OdepCpuArch_t;
#define ODEP_ARCH_X86_64       ((OdepCpuArch_t)0x0001)
#define ODEP_ARCH_ARM_CORTEX_M ((OdepCpuArch_t)0x0002) /* Embedded Bare-Metal */
#define ODEP_ARCH_ARM_CORTEX_A ((OdepCpuArch_t)0x0003) /* Raspberry Pi / Linux */
#define ODEP_ARCH_AVR_8BIT     ((OdepCpuArch_t)0x0004) /* ATmega / Arduino */
#define ODEP_ARCH_RISCV_32     ((OdepCpuArch_t)0x0005)

/* Eindeutige IDs für das Betriebssystem / Runtime-Umgebung */
typedef uint8_t OdepOsType_t;
#define ODEP_OS_BARE_METAL     ((OdepOsType_t)0x00)
#define ODEP_OS_LINUX          ((OdepOsType_t)0x01)
#define ODEP_OS_FREERTOS       ((OdepOsType_t)0x02)

/* 
 * Das Hardware-Manifest (Exakt 16 Bytes, perfekt aligned).
 * Dieses holt sich die Steuerungsebene ab, um den Compiler exakt zu füttern.
 */
typedef struct {
    OdepCpuArch_t cpu_architecture;    /* 2 Bytes: Ziel-CPU Typ */
    OdepOsType_t  operating_system;    /* 1 Byte : OS Typ */
    uint8_t       register_width_bits; /* 1 Byte : 8, 16, 32 oder 64 Bit */
    uint32_t      available_ram_bytes; /* 4 Bytes: RAM-Limit für das Programm */
    uint32_t      flash_page_size;     /* 4 Bytes: Sektorgröße für den Flash-Schreibprozess */
    uint16_t      cpu_clock_mhz;       /* 2 Bytes: Taktfrequenz */
    uint16_t      _reserved_manifest;  /* 2 Bytes: Alignment-Ausgleich */
} OdepHardwareManifest_t;

#pragma pack(pop)

#endif /* ODEP_ARCHITECTURE_H */
