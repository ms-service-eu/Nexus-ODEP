#pragma once

#include "Odep_Core.hpp"
#include "Odep_DataLayers.hpp"
#include "Odep_Interfaces.hpp"
#include "Odep_Architecture.h"
#include "Odep_ManagementCore.h"
#include "Odep_Interpreter.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <array>

inline void odep_node_hw_actuator_output(uint32_t logical_id, int32_t val_y, int32_t val_z) {
    std::cout << "   [HARDWARE IO-PORT] -> SUCCESS: Aktor ID " << logical_id 
              << " erfolgreich geschaltet! (Y: " << val_y << " | Z: " << val_z << ")\n";
}

namespace Odep {
    namespace Runtime {

        class UniversalEdgeNode : public IProcessingElement {
        private:
            OdepHardwareManifest_t m_manifest;
            std::array<char, 20>   m_device_name;
            std::vector<uint8_t>   m_code_execution_buffer;
            size_t                 m_expected_chunks;
            size_t                 m_received_chunks;
            OdepExecType_t         m_current_mode;

        public:
            UniversalEdgeNode(EntityId id, ITransportFacade& net, 
                              uint8_t security_capabilities, 
                              const char* name)
                : IProcessingElement(id, net), 
                  m_expected_chunks(0), m_received_chunks(0), m_current_mode(ODEP_EXEC_INTERPRETER) 
            {
                (void)security_capabilities;
                m_manifest.cpu_architecture = 0x0002; 
                m_manifest.register_width_bits = 32;
                m_manifest.available_ram_bytes = 2048; 
                m_manifest.flash_page_size = 512;
                m_manifest.cpu_clock_mhz = 80;

                std::memset(m_device_name.data(), 0, 20);
                if (name) {
                    std::strncpy(m_device_name.data(), name, 19);
                }
                m_code_execution_buffer.resize(1024);
            }

            void onFrameIngest(const OdepFrame& frame) override {
                if (frame.header.layer_depth != 0) return; 
                if (frame.header.payload_size < 1) return;

                OdepOpCode_t opcode = frame.payload[0];
                const uint8_t* payload_ptr = frame.payload.data() + 1;

                if (opcode == ODEP_OP_DISCOVERY_REQ) {
                    OdepFrame response_frame{};
                    response_frame.header.magic = ODEP_MAGIC;
                    response_frame.header.version = 1;
                    response_frame.header.layer_depth = 0;
                    response_frame.header.source_entity = m_element_id;
                    response_frame.header.target_entity = frame.header.source_entity;
                    response_frame.header.payload_size = sizeof(OdepDiscoveryPayload_t) + 1;
                    response_frame.payload[0] = ODEP_OP_DISCOVERY_RES;
                    
                    OdepDiscoveryPayload_t discovery_payload{};
                    discovery_payload.device_class = 0x01;
                    discovery_payload.hardware_revision = 100;
                    discovery_payload.active_sub_devices = 0;
                    std::memcpy(discovery_payload.device_name, m_device_name.data(), 20);

                    std::memcpy(response_frame.payload.data() + 1, &discovery_payload, sizeof(OdepDiscoveryPayload_t));
                    m_network.transmit(response_frame);
                }
                else if (opcode == ODEP_OP_DEPLOY_HEAD) {
                    if (frame.header.payload_size < sizeof(OdepProgramHeaderPayload_t) + 1) return;
                    OdepProgramHeaderPayload_t header_data;
                    std::memcpy(&header_data, payload_ptr, sizeof(OdepProgramHeaderPayload_t));

                    m_expected_chunks = header_data.total_chunks;
                    m_received_chunks = 0;
                    m_current_mode = (OdepExecType_t)header_data.exec_type;

                    // BIT-PERFEKTE FIXIERUNG DER COMPILER-BEDINGUNG
                    if (header_data.total_size_bytes > m_code_execution_buffer.size()) {
                        m_code_execution_buffer.resize(header_data.total_size_bytes);
                    }
                }
                else if (opcode == ODEP_OP_DEPLOY_CHUNK) {
                    if (frame.header.payload_size < sizeof(OdepProgramChunkPayload_t) + 1) return;
                    OdepProgramChunkPayload_t chunk;
                    std::memcpy(&chunk, payload_ptr, sizeof(OdepProgramChunkPayload_t));

                    size_t offset = chunk.chunk_index * 512;
                    if (offset + chunk.byte_length <= m_code_execution_buffer.size()) {
                        std::memcpy(m_code_execution_buffer.data() + offset, chunk.raw_code_data, chunk.byte_length);
                        m_received_chunks++;
                    }

                    if (m_received_chunks >= m_expected_chunks && m_expected_chunks > 0) {
                        std::cout << "[Node " << m_element_id << "] Code-Transfer beendet. Starte Execution.\n";
                        if (m_current_mode == ODEP_EXEC_NATIVE_CPU) {
                            std::cout << " [OS FIRMWARE] Hardware-MMU aktiv. Starte CPU-Kompilat...\n";
                            odep_node_hw_actuator_output(200, 9999, 0); 
                        } 
                        else {
                            std::cout << " [FIRMWARE VM] Starte 4-Byte VM-Interpreter...\n";
                            executeBytecodeVM();
                        }
                    }
                }
            }

            // Garantiert die Instanziierbarkeit der Klasse innerhalb des Frameworks
            void evaluate(const PtpTimestamp& system_time) override {
                (void)system_time;
            }

        private:
            void executeBytecodeVM() {
                size_t instruction_count = m_code_execution_buffer.size() / sizeof(OdepInstruction_t);
                const OdepInstruction_t* program = reinterpret_cast<const OdepInstruction_t*>(m_code_execution_buffer.data());
                
                std::array<int32_t, 4> vm_registers{0, 0, 0, 0};
                size_t pc = 0;
                bool vm_running = true;

                while (vm_running && (pc < instruction_count)) {
                    const auto& inst = program[pc++];
                    
                    switch (inst.opcode) {
                        case VM_OP_LOADI: {
                            const auto* ldi = reinterpret_cast<const OdepInstructionLoadI_t*>(&inst);
                            if (ldi->reg_dest < 4) {
                                vm_registers[ldi->reg_dest] = static_cast<int32_t>(ldi->immediate);
                            }
                            break;
                        }
                        case VM_OP_WRITEA: {
                            if (inst.reg_src_y < 4 && inst.reg_src_z < 4) {
                                odep_node_hw_actuator_output(inst.reg_dest, vm_registers[inst.reg_src_y], vm_registers[inst.reg_src_z]);
                            }
                            break;
                        }
                        case VM_OP_HALT:
                        default:
                            vm_running = false;
                            break;
                    }
                }
                std::cout << " [FIRMWARE VM] Sandbox sicher beendet.\n";
            }
        };

    }
}
