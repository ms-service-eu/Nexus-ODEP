#include "ODEP_Core.h"
#include "ODEP_VirtualDevice.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef ODEP_ENABLE_VIRTUALIZATION

// Struktur zur Überwachung des Gesundheitszustands der Gruppenmitglieder
struct SubFixturePeer {
    std::string ip_address;
    bool is_online = true;
    int consecutive_failures = 0;
};

class ResilientMovingHead : public ODEPVirtualDevice {
private:
    std::vector<SubFixturePeer> cluster_peers;
    std::string controller_ip = "192.168.1.5"; // IP des Lichtpults für Telemetrie
    const int MAX_ALLOWED_FAILURES = 2;        // Nach 2 Fehlversuchen gilt das Gerät als OFFLINE

    // Simuliert das Senden eines ODEP-Frames über das Netzwerk mit Rückkanal-Prüfung
    bool network_send_tcp(const std::string& ip, const std::string& payload) {
        // HIER würde in der Praxis der echte TCP/Socket-Code stehen.
        // Wir simulieren einen Defekt bei MH 3 (192.168.1.103)
        if (ip == "192.168.1.103") {
            return false; // Connection Timeout / Paketverlust simuliert
        }
        return true; // Erfolgreich gesendet und quittiert
    }

    // Sendet eine administrative Status-Meldung zurück an das Lichtpult
    void reportErrorToController(const std::string& affected_ip, const std::string& error_type) {
        std::cout << "\n🚨 [ODEP TELEMETRIE -> " << controller_ip << "]\n"
                  << "{\n"
                  << "  \"cmd\": \"NOTIFY_NODE_ERROR\",\n"
                  << "  \"proxy_node\": \"MH_1_Master\",\n"
                  << "  \"failed_peer_ip\": \"" << affected_ip << "\",\n"
                  << "  \"error_class\": \"" << error_type << "\",\n"
                  << "  \"action_taken\": \"EVICTED_FROM_LIVE_STREAM_RUN\"\n"
                  << "}\n--------------------------------------------------\n";
    }

public:
    ResilientMovingHead(const std::string& id) : ODEPVirtualDevice(id) {
        // Initialisiere die IP-Adressen der angeschlossenen Gruppen-Geräte
        cluster_peers.push_back({"192.168.1.102", true, 0}); // MH 2
        cluster_peers.push_back({"192.168.1.103", true, 0}); // MH 3 (Defekt)
        cluster_peers.push_back({"192.168.1.104", true, 0}); // MH 4
    }

    // Die erweiterte P2P-Ausführungslogik mit Fehlerbehandlung
    void executePayload(const std::string& res_name, const std::string& payload) override {
        if (res_name == "motion") {
            std::cout << "[MH 1 LOGIK] Berechne eigenen Anteil der Kreisbewegung (Phase 0°).\n";
            // Lokale Motoren-Ansteuerung...

            std::cout << "[MH 1 P2P] Kaskadiere Effekte an Peer-Mesh...\n";
            
            for (auto& peer : cluster_peers) {
                if (!peer.is_online) {
                    // Überspringe bekannte tote Geräte sofort, um Zeit/Latenz zu sparen!
                    std::cout << "  ⚠️  [SKIPPED] Peer [" << peer.ip_address << "] ist OFFLINE. Überspringe Frame.\n";
                    continue;
                }

                // Sende den abstrakten ODEP-Befehl an das Gerät
                bool success = network_send_tcp(peer.ip_address, payload);

                if (!success) {
                    peer.consecutive_failures++;
                    std::cout << "  ❌ [TIMEOUT] Fehler beim Senden an Peer [" << peer.ip_address 
                              << "] (Fehlversuch " << peer.consecutive_failures << "/" << MAX_ALLOWED_FAILURES << ")\n";
                    
                    if (peer.consecutive_failures >= MAX_ALLOWED_FAILURES) {
                        peer.is_online = false; // Gerät wird deklassiert
                        reportErrorToController(peer.ip_address, "TCP_CONNECTION_TIMEOUT_IN_SHOW");
                    }
                } else {
                    // Setze Fehlerzähler bei erfolgreicher Antwort sofort wieder zurück
                    peer.consecutive_failures = 0;
                    std::cout << "  🟢 [SUCCESS] Frame erfolgreich an Peer [" << peer.ip_address << "] zugestellt.\n";
                }
            }
        }
    }
};
#endif

void simulate_live_show_sequence() {
#ifdef ODEP_ENABLE_VIRTUALIZATION
    ResilientMovingHead mh1("VD_Cluster_Group");
    mh1.registerResource("motion");

    std::string test_payload = "{\"effect_shape\": \"CIRCLE\", \"amplitude\": 45.0}";

    std::cout << "\n=== SHOW-DURCHLAUF 1 (MH 3 fängt an zu schwanken) ===\n";
    mh1.processIncomingFrame("motion", "Pult_Autopilot", 120, test_payload);

    std::cout << "\n=== SHOW-DURCHLAUF 2 (MH 3 überschreitet das Limit) ===\n";
    mh1.processIncomingFrame("motion", "Pult_Autopilot", 120, test_payload);

    std::cout << "\n=== SHOW-DURCHLAUF 3 (MH 3 ist isoliert, Show läuft flüssig weiter) ===\n";
    mh1.processIncomingFrame("motion", "Pult_Autopilot", 120, test_payload);
#endif
}

int main() {
    simulate_live_show_sequence();
    return 0;
}
