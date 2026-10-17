#include "ODP_Core.h"
#include "ODP_Scheduler.h"
#include "ODP_VirtualDevice.h"
#include <iostream>

int main() {
    std::cout << "--- ODP Edge Device Engine Node Booting ---\n";

#ifdef ODP_ENABLE_VIRTUALIZATION
    // Erstelle ein Virtual Device auf dem Minicomputer
    ODPVirtualDevice myProxyNode("VD_Matrix_Unit_1");
    
    // Mappe Ressourcen auf den physischen DMX-Ausgang der Karte
    myProxyNode.mapResourceToPhysicalOut("fluid_valve", "DMX_OUT_A", 1);
    myProxyNode.mapResourceToPhysicalOut("drive_motor", "DMX_OUT_A", 4);
    
    // 1. Controller reserviert exklusiv das Ventil
    myProxyNode.requestResource("fluid_valve", "Pult_Main", ODPMode::EXCLUSIVE, 100);
    
    // 2. Ein Angreifer oder zweites Pult versucht dazwischenzufunken -> Wird abgewiesen
    bool success = myProxyNode.processIncomingFrame("fluid_valve", "Fremd_Pult", 50, "VALVE_OPEN");
    std::cout << "Fremdzugriff erfolgreich? " << (success ? "Ja" : "Nein (409 Blocked)") << "\n";

    // 3. Autorisierter Controller sendet Payload -> Proxy übersetzt an DMX-Karte
    myProxyNode.processIncomingFrame("fluid_valve", "Pult_Main", 100, "VALVE_FLOW_50_PCT");

#else
    // Minimaler Core-Knoten ohne Overhead (für kleine MCUs)
    ODPNode basicNode;
    basicNode.registerResource("led_strip");
    basicNode.requestResource("led_strip", "Local_Switch", ODPMode::SHARED, 10);
#endif

    return 0;
}
