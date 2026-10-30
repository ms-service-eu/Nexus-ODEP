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

extern "C" {
    void odep_l1_hw_driver_tx(uint8_t protocol, uint16_t universe, uint32_t address, const uint8_t* data, uint32_t length) {
        (void)protocol; (void)universe; (void)address; (void)data; (void)length;
    }
}

namespace Odep {
    namespace Management {

        class CentralOrchestratorService : public IStreamListener {
        private:
            ITransportFacade& m_network;
            EntityId          m_orchestrator_id;

        public:
            CentralOrchestratorService(EntityId id, ITransportFacade& net) 
                : m_network(net), m_orchestrator_id(id) {}

            void discoverNetworkPeers() {
                std::cout << "[Orchestrator] Starte P2P-Netzwerk-Discovery...\n";
                
                OdepFrame discovery_frame{};
                discovery_frame.header.magic = ODEP_MAGIC;
                discovery_frame.header.version = 1;
                discovery_frame.header.layer_depth = 0; 
                discovery_frame.header.priority = 255;   
                discovery_frame.header.access_mode = AccessMode::Shared;
                discovery_frame.header.sequence_id = 1;
                discovery_frame.header.parent_sequence_id = 0;
                discovery_frame.header.source_entity = m_orchestrator_id;
                discovery_frame.header.target_entity = 0xFFFFFFFFFFFFFFFFULL; 

                // Zuweisung in den ERSTEN Slot des Arrays via Index 0
                discovery_frame.payload[0] = ODEP_OP_DISCOVERY_REQ;
                discovery_frame.header.payload_size = 1;

                m_network.transmit(discovery_frame);
            }

            void onFrameIngest(const OdepFrame& frame) override {
                if (frame.header.layer_depth != 0) return;
                if (frame.header.payload_size < 1) return;

                // Explizites Auslesen des ersten Byte-Slots via Index 0
                uint8_t opcode = frame.payload[0];
                const uint8_t* payload_ptr = frame.payload.data() + 1;

                if (opcode == ODEP_OP_DISCOVERY_RES) {
                    if (frame.header.payload_size < sizeof(OdepDiscoveryPayload_t) + 1) return;

                    OdepDiscoveryPayload_t response{};
                    std::memcpy(&response, payload_ptr, sizeof(OdepDiscoveryPayload_t));

                    // Sauberes, nullterminiertes 33-Byte Array fuer die Namens-Ausgabe
                    char safe_name[33];
                    std::memcpy(safe_name, response.device_name, 32);
                    safe_name[32] = '\0';

                    std::cout << "[Orchestrator] Edge-Knoten gefunden! ID: 0x" 
                              << std::hex << frame.header.source_entity << std::dec
                              << " | Name: " << safe_name 
                              << " | Rev: " << response.hardware_revision << "\n";

                    OdepHardwareManifest_t targeted_manifest{};
                    if (frame.header.source_entity == 0xAAAA) {
                        targeted_manifest.cpu_architecture = ODEP_ARCH_ARM_CORTEX_A;
                        targeted_manifest.register_width_bits = 64;
                        targeted_manifest.available_ram_bytes = 1024 * 1024; 
                    } else {
                        targeted_manifest.cpu_architecture = ODEP_ARCH_AVR_8BIT;
                        targeted_manifest.register_width_bits = 8;
                        targeted_manifest.available_ram_bytes = 512; 
                    }

                    deployCodeToTarget(frame.header.source_entity, targeted_manifest);
                }
            }
            void deployCodeToTarget(EntityId target_node, const OdepHardwareManifest_t& manifest) {
                std::vector<uint8_t> code_binary;
                uint8_t execution_mode = ODEP_EXEC_INTERPRETER;

                std::cout << "[Orchestrator] Analysiere Zielarchitektur-Sicherheitslevel fuer 0x" 
                          << std::hex << target_node << std::dec << "...\n";

                if (manifest.cpu_architecture == ODEP_ARCH_ARM_CORTEX_A || manifest.register_width_bits >= 32) {
                    std::cout << " -> Feste Hardware-MMU vorausgesetzt. Starte nativen Cross-Compiler-Lauf...\n";
                    execution_mode = ODEP_EXEC_NATIVE_CPU;
                    code_binary = {0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00}; 
                } 
                else {
                    std::cout << " -> Keine MMU vorhanden! Native Ausfuehrung verboten.\n";
                    std::cout << " -> Generiere ultra-lean bit-praezisen 4-Byte VM-Bytecode...\n";
                    
                    execution_mode = ODEP_EXEC_INTERPRETER;

                    OdepInstruction_t inst1{};
                    inst1.opcode = VM_OP_LOADI;
                    inst1.reg_dest = 1;
                    inst1.reg_src_y = 0x00;
                    inst1.reg_src_z = 0xFF;

                    OdepInstruction_t inst2{};
                    inst2.opcode = VM_OP_WRITEA;
                    inst2.reg_dest = 50; 
                    inst2.reg_src_y = 1;  
                    inst2.reg_src_z = 1;  

                    OdepInstruction_t inst3{};
                    inst3.opcode = VM_OP_HALT;

                    code_binary.resize(sizeof(OdepInstruction_t) * 3);
                    std::memcpy(code_binary.data() + 0, &inst1, sizeof(OdepInstruction_t));
                    std::memcpy(code_binary.data() + sizeof(OdepInstruction_t), &inst2, sizeof(OdepInstruction_t));
                    std::memcpy(code_binary.data() + (sizeof(OdepInstruction_t) * 2), &inst3, sizeof(OdepInstruction_t));
                }

                // --- 1. SENDER-PHASE: CODE DEPLOYMENT HEADER ---
                OdepFrame head_frame{};
                head_frame.header.magic = ODEP_MAGIC;
                head_frame.header.version = 1;
                head_frame.header.layer_depth = 0;
                head_frame.header.source_entity = m_orchestrator_id;
                head_frame.header.target_entity = target_node;
                
                // Setzt das Opcode-Byte via Pointer-Zugriff
                *(head_frame.payload.data()) = ODEP_OP_DEPLOY_HEAD;

                OdepProgramHeaderPayload_t header_payload{};
                header_payload.program_id = 42; 
                header_payload.total_size_bytes = static_cast<uint32_t>(code_binary.size());
                header_payload.total_chunks = 1;
                header_payload.exec_type = execution_mode;
                header_payload._reserved_align = 0;
                std::memset(header_payload.sha256, 0xAB, 32); 

                std::memcpy(head_frame.payload.data() + 1, &header_payload, sizeof(OdepProgramHeaderPayload_t));
                head_frame.header.payload_size = sizeof(OdepProgramHeaderPayload_t) + 1;
                
                std::cout << "[Orchestrator] Sende Deployment-Header...\n";
                m_network.transmit(head_frame);
                // --- 2. SENDER-PHASE: PAYLOAD-STREAMING (CHUNK 1) ---
                OdepFrame chunk_frame{};
                chunk_frame.header.magic = ODEP_MAGIC;
                chunk_frame.header.version = 1;
                chunk_frame.header.layer_depth = 0;
                chunk_frame.header.source_entity = m_orchestrator_id;
                chunk_frame.header.target_entity = target_node;
                
                // Setzt das Opcode-Byte via Pointer-Zugriff
                *(chunk_frame.payload.data()) = ODEP_OP_DEPLOY_CHUNK;

                OdepProgramChunkPayload_t chunk_payload{};
                chunk_payload.program_id = 42;
                chunk_payload.chunk_index = 0;
                chunk_payload.byte_length = static_cast<uint16_t>(code_binary.size());
                std::memcpy(chunk_payload.raw_code_data, code_binary.data(), code_binary.size());

                std::memcpy(chunk_frame.payload.data() + 1, &chunk_payload, sizeof(OdepProgramChunkPayload_t));
                chunk_frame.header.payload_size = sizeof(OdepProgramChunkPayload_t) + 1;

                std::cout << "[Orchestrator] Sende Programmsegment-Chunk...\n";
                m_network.transmit(chunk_frame);
            }
        };
    }
}

class OrchestratorNetworkBridge : public Odep::ITransportFacade {
private:
    Odep::IStreamListener* m_router = nullptr;
public:
    bool transmit(const Odep::OdepFrame& frame) override {
        if (m_router) m_router->onFrameIngest(frame);
        return true;
    }
    void attachListener(Odep::IStreamListener* listener) override { m_router = listener; }
};

int main() {
    std::cout << "===================================================\n";
    std::cout << "=== Nexus-ODEP Central Orchestrator Core System ===\n";
    std::cout << "===================================================\n\n";
    
    OrchestratorNetworkBridge network;
    Odep::Management::CentralOrchestratorService orchestrator(0x1000, network);
    network.attachListener(&orchestrator);

    orchestrator.discoverNetworkPeers();

    std::cout << "\n===================================================\n";
    return 0;
}
