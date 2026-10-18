#pragma once
#include "Odep_Interfaces.hpp"
#include "Odep_DataLayers.hpp"
#include <cstring>

namespace Odep {

    // =========================================================================
    // LAYER 4: MACRO TRANSFORMATION ENGINE (Intent -> Domain Space)
    // =========================================================================
    class MacroIntentEngine : public IProcessingElement {
    public:
        using IProcessingElement::IProcessingElement;

        void onFrameIngest(const OdepFrame& frame) override {
            if (frame.header.layer_depth != 4) return;

            Data::Layer4Payload macro_intent;
            std::memcpy(&macro_intent, frame.payload.data(), sizeof(Data::Layer4Payload));

            OdepFrame l3_frame;
            std::memset(&l3_frame, 0, sizeof(OdepFrame));
            
            l3_frame.header.magic = ODEP_MAGIC;
            l3_frame.header.version = 1;
            l3_frame.header.layer_depth = 3;
            l3_frame.header.priority = frame.header.priority;
            l3_frame.header.access_mode = frame.header.access_mode;
            l3_frame.header.sequence_id = frame.header.sequence_id + 1;
            l3_frame.header.parent_sequence_id = frame.header.sequence_id;
            l3_frame.header.source_entity = m_element_id;
            l3_frame.header.target_entity = frame.header.target_entity;
            l3_frame.header.timestamp = frame.header.timestamp;

            // Transformation der semantischen Absicht in metrische Koordinatenräume (L3)
            Data::Layer3Command domain_cmd;
            domain_cmd.group_or_path_id = macro_intent.zone_or_domain_id;
            
            // Beispiel: Festkomma-Raumkoordinate (X=10.5m, Y=2.0m, Z=0.0m) via fixed point zuweisen
            domain_cmd.target_metric.x_fixed = float_to_q16(10.5f);
            domain_cmd.target_metric.y_fixed = float_to_q16(2.0f);
            domain_cmd.target_metric.z_fixed = float_to_q16(0.0f);
            
            // Abstraktes State-Vector Mapping basierend auf Preset-ID
            domain_cmd.state_vector_alpha = macro_intent.intent_parameter_id; 
            domain_cmd.state_vector_beta = 0;
            domain_cmd.state_vector_gamma = 0;
            domain_cmd.intensity_q16 = 65535; // 100% Full Power
            domain_cmd._reserved3 = 0;

            std::memcpy(l3_frame.payload.data(), &domain_cmd, sizeof(Data::Layer3Command));
            l3_frame.header.payload_size = sizeof(Data::Layer3Command);
            m_network.transmit(l3_frame);
        }
        
        void evaluate(const PtpTimestamp&) override {}
    };

    // =========================================================================
    // LAYER 3: DOMAIN MULTIPLEXER (Domain Space -> Device Kinematics)
    // =========================================================================
    class DomainMultiplexer : public IProcessingElement {
    public:
        using IProcessingElement::IProcessingElement;

        void onFrameIngest(const OdepFrame& frame) override {
            if (frame.header.layer_depth != 3) return;

            Data::Layer3Command domain_cmd;
            std::memcpy(&domain_cmd, frame.payload.data(), sizeof(Data::Layer3Command));

            OdepFrame l2_frame;
            std::memset(&l2_frame, 0, sizeof(OdepFrame));
            
            l2_frame.header.magic = ODEP_MAGIC;
            l2_frame.header.version = 1;
            l2_frame.header.layer_depth = 2;
            l2_frame.header.priority = frame.header.priority;
            l2_frame.header.access_mode = frame.header.access_mode;
            l2_frame.header.sequence_id = frame.header.sequence_id + 1;
            l2_frame.header.parent_sequence_id = frame.header.sequence_id;
            l2_frame.header.source_entity = m_element_id;
            l2_frame.header.target_entity = frame.header.target_entity;
            l2_frame.header.timestamp = frame.header.timestamp;

            // Kinematische Berechnung im Festkommabereich (Keine float-Abweichung)
            Data::Layer2Payload device_cmd;
            
            // Simpel-Transformation: Multiplikation im Q16.16 Raum benötigt bit-shifting,
            // für lineare Transformationen reicht eine direkte Multiplikation mit Integern.
            device_cmd.primary_axis_fixed = domain_cmd.target_metric.x_fixed * 2; // Skalierungs-Abstraktion
            device_cmd.secondary_axis_fixed = domain_cmd.target_metric.y_fixed;
            device_cmd.output_intensity_q16 = static_cast<uint32_t>(domain_cmd.intensity_q16) * 1;
            device_cmd.engine_type = Data::EngineType::Mechanical;
            device_cmd._reserved2_1 = 0;
            device_cmd._reserved2_2 = 0;

            // Direkte Befüllung der Treiber-Kanäle
            device_cmd.hardware_channels[0] = 255; // Grob-Kanal
            device_cmd.hardware_channels[1] = 0;
            device_cmd.hardware_channels[2] = 0;
            device_cmd.hardware_channels[3] = 0;

            std::memcpy(l2_frame.payload.data(), &device_cmd, sizeof(Data::Layer2Payload));
            l2_frame.header.payload_size = sizeof(Data::Layer2Payload);
            m_network.transmit(l2_frame);
        }
        
        void evaluate(const PtpTimestamp&) override {}
    };

    // =========================================================================
    // LAYER 2 & 1: TIME-DECOUPLED EXECUTION ENGINE (Buffer-Ahead Engine)
    // =========================================================================
    class TimeDecoupledExecutionEngine : public IProcessingElement {
    private:
        struct QuantumSlot {
            PtpTimestamp execution_deadline;
            Data::Layer1Payload buffered_payload;
            bool pending;
        };

        std::array<QuantumSlot, 8> m_temporal_buffer;

    public:
        TimeDecoupledExecutionEngine(EntityId id, ITransportFacade& net) 
            : IProcessingElement(id, net) {
            for (auto& slot : m_temporal_buffer) {
                slot.pending = false;
                std::memset(&slot.buffered_payload, 0, sizeof(Data::Layer1Payload));
            }
        }

        void onFrameIngest(const OdepFrame& frame) override {
            if (frame.header.layer_depth != 2) return;

            Data::Layer2Payload device_cmd;
            std::memcpy(&device_cmd, frame.payload.data(), sizeof(Data::Layer2Payload));

            // Vorbereitung der nackten Hardware-Bus-Bytes (Layer 1)
            Data::Layer1Payload bus_cmd;
            bus_cmd.protocol_id = Data::HardwareProtocol::CANBus;
            bus_cmd._reserved1_1 = 0;
            bus_cmd.bus_universe = 1;
            bus_cmd.base_address = 10;
            bus_cmd.data_length = 4;
            bus_cmd.raw_bus_bytes.fill(0);

            // Mappe die präzisen Q16.16-Achswerte direkt auf den Hardware-Bitstream
            bus_cmd.raw_bus_bytes[0] = static_cast<uint8_t>((device_cmd.primary_axis_fixed >> 16) & 0xFF);
            bus_cmd.raw_bus_bytes[1] = static_cast<uint8_t>(device_cmd.primary_axis_fixed & 0xFF);
            bus_cmd.raw_bus_bytes[2] = static_cast<uint8_t>((device_cmd.secondary_axis_fixed >> 16) & 0xFF);
            bus_cmd.raw_bus_bytes[3] = static_cast<uint8_t>(device_cmd.secondary_axis_fixed & 0xFF);

            // Deterministischer Buffer-Ahead Jitter-Schutz (+100ms Austaktungs-Verzögerung)
            QuantumSlot slot;
            slot.execution_deadline = frame.header.timestamp;
            slot.execution_deadline.nanoseconds += 100000000;
            if (slot.execution_deadline.nanoseconds >= 1000000000) {
                slot.execution_deadline.seconds += 1;
                slot.execution_deadline.nanoseconds -= 1000000000;
            }
            slot.buffered_payload = bus_cmd;
            slot.pending = true;

            for (auto& buffer_slot : m_temporal_buffer) {
                if (!buffer_slot.pending) {
                    buffer_slot = slot;
                    break;
                }
            }
        }

        void evaluate(const PtpTimestamp& system_time) override {
            for (auto& slot : m_temporal_buffer) {
                if (slot.pending && (system_time >= slot.execution_deadline)) {
                    
                    OdepFrame l1_frame;
                    std::memset(&l1_frame, 0, sizeof(OdepFrame));
                    
                    l1_frame.header.magic = ODEP_MAGIC;
                    l1_frame.header.version = 1;
                    l1_frame.header.layer_depth = 1;
                    l1_frame.header.timestamp = system_time;
                    l1_frame.header.payload_size = sizeof(Data::Layer1Payload);
                    
                    std::memcpy(l1_frame.payload.data(), &slot.buffered_payload, sizeof(Data::Layer1Payload));

                    m_network.transmit(l1_frame);
                    slot.pending = false;
                }
            }
        }
    };
}
