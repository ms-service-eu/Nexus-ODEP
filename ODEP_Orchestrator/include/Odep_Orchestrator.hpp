#pragma once
#include "OdepInterfaces.hpp"
#include "OdepSystemCore.hpp"
#include <iostream>
#include <cstring>
#include <vector>

namespace Odep {

    class OdepOrchestrator : public IStreamListener {
    private:
        ITransportFacade& m_network;
        EntityId m_orchestrator_id;

    public:
        OdepOrchestrator(EntityId id, ITransportFacade& net) 
            : m_network(net), m_orchestrator_id(id) {}

        void startNetworkDiscovery() {
            std::cout  code_payload;
            Management::OdepExecType selected_type;

            // STRIGTE ENTSCHEIDUNGSEBENE: Nur CPUs mit fester Hardware-MMU erhalten Compilate!
            if (manifest.security_capabilities & Management::ODEP_SEC_HAS_HARDWARE_MMU) {
                std::cout (code_payload.size());
            header.total_chunks = 1;
            header.exec_type = selected_type;

            std::memcpy(head_frame.payload.data() + 1, &header, sizeof(Management::OdepProgramHeaderPayload));
            head_frame.header.payload_size = sizeof(Management::OdepProgramHeaderPayload) + 1;
            m_network.transmit(head_frame);

            // Sende Code-Segment (Chunk 1)
            OdepFrame chunk_frame{};
            chunk_frame.header.magic = ODEP_MAGIC;
            chunk_frame.header.layer_depth = 0;
            chunk_frame.header.source_entity = m_orchestrator_id;
            chunk_frame.header.target_entity = target;
            chunk_frame.payload[0] = Management::ODEP_OP_DEPLOY_CHUNK;

            Management::OdepProgramChunkPayload chunk{};
            chunk.program_id = 777;
            chunk.chunk_index = 0;
            chunk.byte_length = static_cast<uint16_t>(code_payload.size());
            std::memcpy(chunk.raw_code_data.data(), code_payload.data(), code_payload.size());

            std::memcpy(chunk_frame.payload.data() + 1, &chunk, sizeof(Management::OdepProgramChunkPayload));
            chunk_frame.header.payload_size = sizeof(Management::OdepProgramChunkPayload) + 1;
            m_network.transmit(chunk_frame);
        }
    };
}
