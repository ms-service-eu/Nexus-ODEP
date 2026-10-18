#pragma once
#include <cstdint>
#include <array>

namespace Odep {
    namespace Data {

        #pragma pack(push, 1)

        // =========================================================================
        // LAYER 4: MACRO / INTENT LAYER (Exakt 12 Bytes)
        // =========================================================================
        enum class MacroIntent : uint16_t {
            SetAmbience = 0x0001,
            TransitPath = 0x0002,
            Emergency   = 0xFFFF
        };

        struct Layer4Payload {
            uint32_t    zone_or_domain_id;    // 4 Bytes: Eindeutiger topologischer Raum
            MacroIntent intent;               // 2 Bytes: Semantische Absicht
            uint16_t    transition_time_ms;   // 2 Bytes: Zeitfenster für den Übergang
            uint32_t    intent_parameter_id;  // 4 Bytes: Verweis auf Preset (z.B. Farb-ID, Route-ID)
        };

        // =========================================================================
        // LAYER 3: DOMAIN / GROUP LAYER (Exakt 32 Bytes)
        // =========================================================================
        /*
         * Alle metrischen Koordinaten (X, Y, Z) werden im Q16.16 Fixed-Point-Format 
         * übertragen (1 Unit = 1 Meter, Auflösung bis zu 1/65536 Meter = ~0.015 mm).
         */
        struct FixedVector3D {
            int32_t x_fixed;                  // 4 Bytes: Q16.16 Koordinate
            int32_t y_fixed;                  // 4 Bytes: Q16.16 Koordinate
            int32_t z_fixed;                  // 4 Bytes: Q16.16 Koordinate
        };

        struct Layer3Command {
            uint32_t      group_or_path_id;   // 4 Bytes: Zielgruppe im Raum
            FixedVector3D target_metric;      // 12 Bytes: Räumliches Ziel/Vektor
            uint32_t      state_vector_alpha; // 4 Bytes: Universeller Parameter 1 (z.B. CIE-L-Wert / Soll-Geschwindigkeit)
            uint32_t      state_vector_beta;  // 4 Bytes: Universeller Parameter 2 (z.B. CIE-a-Wert / Weichen-ID)
            uint32_t      state_vector_gamma; // 4 Bytes: Universeller Parameter 3 (z.B. CIE-b-Wert / Beschleunigung)
            uint16_t      intensity_q16;      // 2 Bytes: Skalierungsfaktor (0 bis 65535 = 0% bis 100%)
            uint16_t      _reserved3;         // 2 Bytes: 32-Bit Alignment Ausgleich
        };

        // =========================================================================
        // LAYER 2: DEVICE / KINEMATICS LAYER (Exakt 24 Bytes)
        // =========================================================================
        enum class EngineType : uint8_t { 
            AgnosticRaw = 0x00, 
            ColorCMY    = 0x01, 
            ColorRGBW   = 0x02,
            Mechanical  = 0x03 
        };

        struct Layer2Payload {
            /* 
             * Kinematische Freiheitsgrade (Winkel, Schub) im Q16.16 Format. 
             * Ermöglicht eine Winkelpräzision von Bruchteilen eines Grades ohne floats.
             */
            int32_t    primary_axis_fixed;   // 4 Bytes: Z.B. Pan-Winkel, Lok-Geschwindigkeit
            int32_t    secondary_axis_fixed; // 4 Bytes: Z.B. Tilt-Winkel, Weichen-Servo-Stellwert
            uint32_t   output_intensity_q16; // 4 Bytes: Berechneter Ziel-Output-Wert (Skaliert)
            EngineType engine_type;          /* 1 Byte : Typ des Verarbeitungs-Algorithmus */
            uint8_t    _reserved2_1;         /* 1 Byte : Padding */
            uint16_t   _reserved2_2;         /* 2 Bytes: Padding */
            
            // 4 diskrete, bit-präzise Kanäle (0-65535) für direkte Treiber-Feinansteuerung (z.B. C,M,Y,W)
            std::array<uint16_t, 4> hardware_channels; // 8 Bytes
        };

        // =========================================================================
        // LAYER 1: HARDWARE / BUS LAYER (Exakt 264 Bytes - Buffer-Ahead optimiert)
        // =========================================================================
        enum class HardwareProtocol : uint8_t { 
            DMX512 = 0x01, 
            ArtNet = 0x02, 
            sACN   = 0x03, 
            DCC    = 0x04,
            CANBus = 0x05
        };

        struct Layer1Payload {
            HardwareProtocol protocol_id;     /* 1 Byte : Ziel-Bussystem */
            uint8_t          _reserved1_1;    /* 1 Byte : Padding */
            uint16_t         bus_universe;    // 2 Bytes: Universum, Sub-Netz oder CAN-ID
            uint32_t         base_address;    // 4 Bytes: Startadresse auf dem physischen Bus
            uint32_t         data_length;     // 4 Bytes: Anzahl der gültigen Bytes im Puffer
            
            // Fester, statischer RAM-Block für die Buffer-Ahead Austaktung der Hardware-Bytes
            std::array<uint8_t, 252> raw_bus_bytes; // 252 Bytes (Ergibt glatte 264 Bytes Gesamtstruktur)
        };

        #pragma pack(pop)
    }
}
