#include "ODEP_Core.h"
#include "ODEP_VirtualDevice.h"
#include <iostream>

#ifdef ODEP_ENABLE_VIRTUALIZATION
class IntelligentMovingHead : public ODEPVirtualDevice {
private:
    std::string sub_head_ips[3] = {"192.168.1.102", "192.168.1.103", "192.168.1.104"};

public:
    IntelligentMovingHead(const std::string& id) : ODEPVirtualDevice(id) {}

    // Überschreibt die Ausführung, um die anderen 3 Movingheads via IP-P2P zu steuern
    void executePayload(const std::string& res_name, const std::string& payload) override {
        if (res_name == "motion") {
            std::cout << "[MH 1 LOGIK] Ich berechne den Kreiseffekt für mich selbst...\n";
            // Interne Hardware-Motoren ansteuern...

            std::cout << "[MH 1 P2P] Kaskadiere Befehl autonom an alle Sub-Knoten im Mesh:\n";
            for (const std::string& ip : sub_head_ips) {
                // MH 1 schickt nun als Peer einen ODEP-Befehl direkt an die IPs von MH 2, 3 und 4.
                // Er berechnet dabei einen Phasenversatz für jeden Kopf, falls gewünscht.
                std::cout << "  -> Sende ODEP-Frame an Sub-Head [" << ip << "] mit Payload: " << payload << "\n";
            }
        }
    }
};
#endif

// Repräsentiert die Firmware, die auf Movinghead 1 läuft
void simulate_moving_head_1_firmware() {
#ifdef ODEP_ENABLE_VIRTUALIZATION
    // MH 1 deklariert das virtuelle Gruppen-Gerät im Netz
    IntelligentMovingHead mh1("VD_Cluster_Group");
    mh1.registerResource("motion");

    std::cout << "\n[MH 1] Bereit. Warte auf ODEP-Pakete vom Controller...\n\n";

    // Simulation: Der vom Controller gesendete EXECUTE_FRAME kommt über die IP-Buchse an:
    std::string empfangene_payload = "{\"effect_shape\": \"CIRCLE\", \"amplitude\": 45.0}";
    
    // MH 1 prüft die Rechte und führt die dezentrale P2P-Steuerung aus
    mh1.processIncomingFrame("motion", "Pult_Autopilot", 120, empfangene_payload);
#endif
}

int main() {
    // Zum Testen der Geräteseite:
    simulate_moving_head_1_firmware();
    return 0;
}
