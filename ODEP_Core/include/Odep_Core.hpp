#pragma once

#include <stdint.h>
#include <array>

namespace Odep {

    // Bit-perfekte Typdefinitionen nach Nexus-ODEP Spezifikation
    using EntityId   = uint64_t;
    using SequenceId = uint32_t;

    constexpr uint32_t ODEP_MAGIC = 0x4E584F44; // "NXOD"

    enum class AccessMode : uint8_t {
        Shared    = 0x00,
        Exclusive = 0x01
    };

    #pragma pack(push, 1)
    struct PtpTimestamp {
        uint64_t seconds;       // 8-Byte Grenze
        uint32_t nanoseconds;   // 4-Byte Grenze
        uint32_t _reserved;     // Natürlicher 4-Byte-Lückenfüller für perfektes Alignment
    };

    struct OdepHeader {
        uint32_t     magic;
        uint8_t      version;
        uint8_t      layer_depth;
        uint8_t      priority;
        AccessMode   access_mode;
        SequenceId   sequence_id;
        SequenceId   parent_sequence_id;
        EntityId     source_entity;
        EntityId     target_entity;
        uint64_t     seconds;
        uint32_t     nanoseconds;
        uint32_t     payload_size;
        uint32_t     _structure_padding; // Natürlicher Lückenfüller auf 8-Byte Grenze
    };

    struct OdepFrame {
        OdepHeader header;
        std::array<uint8_t, 1400> payload; // Transportagnostischer MTU-Sicherer Puffer
    };
    #pragma pack(pop)
}
