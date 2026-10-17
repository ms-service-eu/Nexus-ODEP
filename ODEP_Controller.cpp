#include <iostream>
#include <chrono>
#include <thread>
#include <string>

// Simuliert das Senden eines ODEP-JSON-Rahmens über TCP/IP
void sendOdepFrame(const std::string& target_ip, const std::string& json_payload) {
    std::cout << "[CONTROLLER -> TCP/IP to " << target_ip << "]\n" 
              << json_payload << "\n--------------------------------------------------\n";
}

int main() {
    std::cout << "=== ODEP Master Controller Script gestartet ===\n";
    std::string mh1_ip = "192.168.1.101"; // IP-Adresse des ersten, mächtigen Movingheads

    // Endlosschleife zur Simulation der Show
    while (true) {
        std::cout << "\n[STATUS] Warte 15 Sekunden bis zum nächsten Effekt-Slot...\n";
        std::this_thread::sleep_for(std::chrono::seconds(15));

        std::cout << "\n[EFFEKT] Erzeuge 5-Sekunden-Slot für Kreisbewegung...\n";
        
        // 1. SCHRITT: Exklusive Reservierung der Bewegungs-Ressourcen für die Zukunft (Sofortstart für 5 Sek.)
        // Wir adressieren das Virtual Device "VD_Cluster_Group", das MH 1 für alle 4 Heads verwaltet.
        std::string reserve_cmd = R"({
            "cmd": "RESERVE_RESOURCE",
            "controller_id": "Pult_Autopilot",
            "target_vd_id": "VD_Cluster_Group",
            "target_resources": ["motion"],
            "mode": "EXCLUSIVE",
            "priority": 120,
            "start_time_ptp": "NOW",
            "duration_ms": 5000
        })";
        sendOdepFrame(mh1_ip, reserve_cmd);

        // 2. SCHRITT: Den eigentlichen mathematischen Effekt-Trigger mit Application-Payload senden
        // Das Pult sagt nur: "Kreis". Wie die 4 Heads das physikalisch machen, weiß nur MH 1.
        std::string execute_cmd = R"({
            "cmd": "EXECUTE_FRAME",
            "target_id": "VD_Cluster_Group",
            "priority": 120,
            "sync_time_ptp": "NOW",
            "application_payload": {
                "effect_shape": "CIRCLE",
                "amplitude": 45.0,
                "speed": 2.5
            }
        })";
        sendOdepFrame(mh1_ip, execute_cmd);

        // 5 Sekunden warten, während der Effekt läuft
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "[EFFEKT] 5-Sekunden-Slot abgelaufen. Ressourcen automatisch wieder FREE.\n";
    }

    return 0;
}
