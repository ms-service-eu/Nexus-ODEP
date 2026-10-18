#include "Odep_Core.hpp"
#include "Odep_DataLayers.hpp"
#include "Odep_Interfaces.hpp"

#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Odep {
    namespace Control {

        class ControlNetworkBridge : public ITransportFacade {
        private:
            std::vector<IStreamListener*> m_listeners;

        public:
            bool transmit(const OdepFrame& frame) override {
                std::cout << "   [NETWORK BUS] -> Transmit Frame (Layer " 
                          << static_cast<int>(frame.header.layer_depth) 
                          << ", Size: " << frame.header.payload_size << " Bytes)\n";
                for (auto* listener : m_listeners) {
                    if (listener) listener->onFrameIngest(frame);
                }
                return true;
            }
            void attachListener(IStreamListener* listener) override {
                if (listener) m_listeners.push_back(listener);
            }
        };

        class LiveShowController {
        private:
            ITransportFacade& m_network;
            EntityId          m_controller_id;
            SequenceId        m_sequence_counter;

        public:
            LiveShowController(EntityId id, ITransportFacade& net)
                : m_network(net), m_controller_id(id), m_sequence_counter(1000) {}

            // METHODE 1: AMBIENTE (ZONE & COLOR)
            void injectStageColorIntent(uint32_t target_zone_id, uint32_t color_preset_id, uint16_t fade_time_ms) {
                std::cout << "[Live Controller] Erzeuge Layer 4 Intent -> Zone: " << target_zone_id 
                          << " | Color Preset ID: " << color_preset_id 
                          << " | Fadetime: " << fade_time_ms << " ms\n";

                OdepFrame frame{};
                frame.header.magic = ODEP_MAGIC;
                frame.header.version = 1;
                frame.header.layer_depth = 4; 
                frame.header.priority = 10;   
                frame.header.access_mode = AccessMode::Shared;
                frame.header.sequence_id = m_sequence_counter++;
                frame.header.source_entity = m_controller_id;
                frame.header.target_entity = 0xFFFFFFFFFFFFFFFFULL; 

                Data::Layer4Payload payload{};
                payload.zone_or_domain_id = target_zone_id;
                payload.intent = Data::MacroIntent::SetAmbience;
                payload.transition_time_ms = fade_time_ms;
                payload.intent_parameter_id = color_preset_id;

                std::memcpy(frame.payload.data(), &payload, sizeof(Data::Layer4Payload));
                frame.header.payload_size = sizeof(Data::Layer4Payload);
                m_network.transmit(frame);
            }
            // METHODE 2: KINETIK (DOMAIN & ROUTE)
            void injectTrackTransitIntent(uint32_t target_domain_id, uint32_t route_preset_id) {
                std::cout << "[Live Controller] Erzeuge Layer 4 Intent -> Domain: " << target_domain_id 
                          << " | Route-ID: " << route_preset_id << "\n";

                OdepFrame frame{};
                frame.header.magic = ODEP_MAGIC;
                frame.header.version = 1;
                frame.header.layer_depth = 4;
                frame.header.priority = 20; 
                frame.header.access_mode = AccessMode::Exclusive;
                frame.header.sequence_id = m_sequence_counter++;
                frame.header.source_entity = m_controller_id;
                frame.header.target_entity = 0xFFFFFFFFFFFFFFFFULL;

                Data::Layer4Payload payload{};
                payload.zone_or_domain_id = target_domain_id;
                payload.intent = Data::MacroIntent::TransitPath;
                payload.transition_time_ms = 0; 
                payload.intent_parameter_id = route_preset_id;

                std::memcpy(frame.payload.data(), &payload, sizeof(Data::Layer4Payload));
                frame.header.payload_size = sizeof(Data::Layer4Payload);
                m_network.transmit(frame);
            }
        };
    }
}

int main() {
    std::cout << "===================================================\n";
    std::cout << "=== Nexus-ODEP Live Show Controller Application ===\n";
    std::cout << "===================================================\n\n";

    Odep::Control::ControlNetworkBridge network;
    Odep::Control::LiveShowController controller(0x0001, network);

    controller.injectStageColorIntent(5, 1, 1000);
    std::cout << "\n";
    controller.injectTrackTransitIntent(10, 12);

    std::cout << "\n===================================================\n";
    return 0;
}
