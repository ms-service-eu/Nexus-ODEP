#include "Odep_Core.hpp"
#include "Odep_DataLayers.hpp"
#include "Odep_Architecture.h"
#include "Odep_ManagementCore.h"
#include "Odep_DeviceNode.hpp"
#include "Odep_Interfaces.hpp"

#include <iostream>
#include <vector>
#include <cstring>
#include <array>

// Lokale Loopback-Netzwerkbrücke für die Standalone-Test-Simulation
class NodeNetworkBridge : public Odep::ITransportFacade {
private:
    std::vector<Odep::IStreamListener*> m_listeners;

public:
    bool transmit(const Odep::OdepFrame& frame) override {
        for (auto* listener : m_listeners) {
            if (listener) {
                listener->onFrameIngest(frame);
            }
        }
        return true;
    }

    void attachListener(Odep::IStreamListener* listener) override { 
        if (listener) {
            m_listeners.push_back(listener); 
        }
    }
};

int main() {
    std::cout << "===================================================\n";
    std::cout << "=== Nexus-ODEP Secure Adaptive Edge Node System ===\n";
    std::cout << "===================================================\n\n";

    NodeNetworkBridge network;

    // Erzeuge zwei Test-Instanzen direkt im Workspace unter Nutzung der ODEP Sicherheits-Modi:
    
    // Instanz 1: Ein mächtiges Linux-basiertes Embedded Gateway mit aktiver CPU-MMU
    // Nutzt das ODEP_SEC_HAS_HARDWARE_MMU Flag (Simuliert über einen dedizierten Verarbeitungs-Zweig)
    Odep::Runtime::UniversalEdgeNode gateway_mmu(
        0xAAAA, 
        network, 
        0x01, // Aktiviert intern erweiterte Security-Validierung
        "Gateway_MMU_Node"
    );
    network.attachListener(&gateway_mmu);

    // Instanz 2: Ein ressourcenarmer 8-Bit Weichendecoder/Relais ohne MMU-Einheit
    Odep::Runtime::UniversalEdgeNode legacy_8bit(
        0xBBBB, 
        network, 
        0x00, // Standard-Sicherheitsstufe (Zwingt Code durch den VM-Interpreter)
        "Legacy_8Bit_Node"
    );
    network.attachListener(&legacy_8bit);

    std::cout << "[System] Beide Node-Runtimes initialisiert und empfangsbereit.\n";
    std::cout << "===================================================\n";
    
    return 0;
}
