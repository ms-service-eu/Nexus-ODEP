#include "Odep_Interpreter_core.h"
#include "Odep_l1.h" /* Für Hardware-Manifest-Typen falls nötig */

/* Extern bereitgestellte, sichere Hardware-Interfaces des Devices */
extern int32_t odep_api_read_sensor(uint32_t logical_id, uint8_t channel_index);
extern void    odep_api_write_actuator(uint32_t logical_id, int32_t primary_fixed, int32_t secondary_fixed);

#define VM_NUM_REGISTERS 4

/* Der vollständige VM-Zustand belegt lediglich 20 Bytes im RAM! */
typedef struct {
    int32_t  registers[VM_NUM_REGISTERS];
    uint16_t pc;        /* Program Counter (Instruktions-Index) */
    uint8_t  running;   /* Status-Flag */
} OdepVirtualMachine_t;

/* 
 * Der Kern des Ultra-Lean Interpreters.
 * Arbeitet ein geladenes Programm komplett isoliert im RAM ab.
 */
void odep_vm_execute(const OdepInstruction_t* program, uint16_t instruction_count) {
    OdepVirtualMachine_t vm;
    vm.pc = 0;
    vm.running = 1;
    for (uint8_t i = 0; i < VM_NUM_REGISTERS; i++) {
        vm.registers[i] = 0;
    }

    /* Der kritische Ausführungs-Loop */
    while (vm.running && (vm.pc < instruction_count)) {
        const OdepInstruction_t* inst = &program[vm.pc];
        vm.pc++; /* Inkrementiere Program Counter vorab */

        switch (inst->opcode) {
            case VM_OP_NOP:
                break;

            case VM_OP_LOADI: {
                /* Cast auf das Immediate-Layout, um die 16-Bit sicher zu extrahieren */
                const OdepInstructionLoadI_t* ldi = (const OdepInstructionLoadI_t*)inst;
                if (ldi->reg_dest < VM_NUM_REGISTERS) {
                    vm.registers[ldi->reg_dest] = (int32_t)ldi->immediate;
                }
                break;
            }

            case VM_OP_ADD:
                if (inst->reg_dest < VM_NUM_REGISTERS && inst->reg_src_y < VM_NUM_REGISTERS && inst->reg_src_z < VM_NUM_REGISTERS) {
                    vm.registers[inst->reg_dest] = vm.registers[inst->reg_src_y] + vm.registers[inst->reg_src_z];
                }
                break;

            case VM_OP_SUB:
                if (inst->reg_dest < VM_NUM_REGISTERS && inst->reg_src_y < VM_NUM_REGISTERS && inst->reg_src_z < VM_NUM_REGISTERS) {
                    vm.registers[inst->reg_dest] = vm.registers[inst->reg_src_y] - vm.registers[inst->reg_src_z];
                }
                break;

            case VM_OP_READS: {
                /* Nutzt reg_dest als Speicher und reg_src_z als vordefinierte Logical-ID des Sensors */
                uint32_t logical_sensor_id = inst->reg_src_z; 
                uint8_t  channel = inst->reg_src_y;
                if (inst->reg_dest < VM_NUM_REGISTERS) {
                    vm.registers[inst->reg_dest] = odep_api_read_sensor(logical_sensor_id, channel);
                }
                break;
            }

            case VM_OP_WRITEA: {
                /* Schreibt den im Register Y und Z liegenden Q16.16 Wert direkt auf den Zielaktor */
                uint32_t logical_actuator_id = inst->reg_dest; /* Zweckentfremdung als ID-Speicher */
                if (inst->reg_src_y < VM_NUM_REGISTERS && inst->reg_src_z < VM_NUM_REGISTERS) {
                    odep_api_write_actuator(logical_actuator_id, vm.registers[inst->reg_src_y], vm.registers[inst->reg_src_z]);
                }
                break;
            }

            case VM_OP_JMP:
                /* Setzt den PC direkt auf das Ziel-Offset (reg_dest hält den Ziel-Index) */
                vm.pc = inst->reg_dest;
                break;

            case VM_OP_JZ:
                if (inst->reg_src_y < VM_NUM_REGISTERS && vm.registers[inst->reg_src_y] == 0) {
                    vm.pc = inst->reg_dest;
                }
                break;

            case VM_OP_HALT:
            default:
                vm.running = 0;
                break;
        }
    }
}
