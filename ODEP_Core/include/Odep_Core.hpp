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
