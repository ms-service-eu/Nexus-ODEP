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


#ifndef ODEP_VM_CORE_H
#define ODEP_VM_CORE_H

#include <stdint.h>

#pragma pack(push, 1)

/* 
 * Der Ultra-Lean Interpreter verfügt über:
 * - 4 universelle 32-Bit Register: R0, R1, R2, R3
 * - Zugriff auf die ODEP API (Sensoren lesen / Aktoren schreiben)
 */

/* --- OPCODES (5 Bits verwendet, max 32 Instruktionen für minimalen Decoder) --- */
typedef uint8_t OdepVmOp_t;
#define VM_OP_NOP    ((OdepVmOp_t)0x00)
#define VM_OP_LOADI  ((OdepVmOp_t)0x01) /* Lädt einen 16-Bit Immediate-Wert in ein Register */
#define VM_OP_ADD    ((OdepVmOp_t)0x02) /* RX = RY + RZ */
#define VM_OP_SUB    ((OdepVmOp_t)0x03) /* RX = RY - RZ */
#define VM_OP_READS  ((OdepVmOp_t)0x04) /* Liest Sensor -> Register: RX = read_sensor(LogicalID, Channel) */
#define VM_OP_WRITEA ((OdepVmOp_t)0x05) /* Schreibt Aktor aus Registern: write_actuator(LogicalID, RY, RZ) */
#define VM_OP_JMP    ((OdepVmOp_t)0x06) /* Unbedingter Sprung zu Instruktions-Index */
#define VM_OP_JZ     ((OdepVmOp_t)0x07) /* Sprung zu Index, wenn Register RX == 0 */
#define VM_OP_HALT   ((OdepVmOp_t)0x1F) /* Beendet die Programmausführung */

/* 
 * Das bit-präzise 32-Bit Instruktions-Layout.
 * Jede Anweisung im Speicher verbraucht exakt 4 Bytes.
 */
typedef struct {
    uint8_t  opcode;     /* 1 Byte : Befehlstyp (VM_OP_*) */
    uint8_t  reg_dest;   /* 1 Byte : Zielregister (Index 0 bis 3) */
    uint8_t  reg_src_y;  /* 1 Byte : Quellregister Y oder Sub-Funktion/Kanal */
    uint8_t  reg_src_z;  /* 1 Byte : Quellregister Z */
} OdepInstruction_t;

/* Für den LOADI-Befehl wird die Struktur umgedeutet (16-Bit Immediate) */
typedef struct {
    uint8_t  opcode;
    uint8_t  reg_dest;
    uint16_t immediate;  /* 2 Bytes: Direkt geladener Festkomma-Wert */
} OdepInstructionLoadI_t;

#pragma pack(pop)

#endif /* ODEP_VM_CORE_H */
