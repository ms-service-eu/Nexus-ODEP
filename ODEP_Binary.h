#pragma once
#include <cstdint>

// Aktiviert striktes Byte-Alignment, damit der Compiler keine künstlichen 
// Padding-Bytes zwischen die Variablen schiebt. Wichtig für Netzwerk-Streams!
#pragma pack(push, 1)

/**
 * @brief Der administrative ODP-Binär-Header (Exakt 12 Bytes)
 * Presst Befehl, Modus, Priorität und IDs in das absolute Minimum.
 */
struct ODP_BinaryHeader {
    uint8_t start_code;       // Immer 0x4F (ODP-Identifikator)
    
    // Bit-Packing für administrative Flags (Zusammen exakt 1 Byte)
    uint8_t cmd_id       : 4; // 16 administrative ODP-Befehle (0=EXECUTE, 1=RESERVE, etc.)
    uint8_t resource_mode: 2; // 0=FREE, 1=EXCLUSIVE, 2=SHARED, 3=RESERVED
    uint8_t reserved     : 2; // Reserviert für zukünftige Protokoll-Erweiterungen
    
    uint8_t priority;         // Priorität (0 - 255) zur hardwareseitigen Konfliktlösung
    uint16_t controller_id;   // Numerische ID des Senders (spart den String-Namen, 0-65535)
    uint16_t target_id;       // Numerische ID des Ziel-Knotens oder Virtual Devices
    uint8_t  resource_id;     // Lokaler Funktions-Index auf dem Gerät (0-255, z.B. 1=Motion, 2=Color)
    
    uint32_t duration_ms;     // Ablaufzeit der Reservierung (0 = Unendlich/Live-Frame)
};

/**
 * @brief Der optionale, hochpräzise Zeitstempel (8 Bytes)
 * Wird nur bei zeitkritischen Befehlen oder Buffer-Ahead angehängt.
 */
struct ODP_TimestampExtension {
    uint64_t sync_time_ptp;   // PTP-Zeitstempel in Mikrosekunden für absolut synchronen Start
};

/**
 * @brief Das vollständige ODP-DMX-Netzwerkpaket
 */
struct ODP_DMXPacket {
    ODP_BinaryHeader header;
    
    // Je nach cmd_id können optionale Erweiterungen folgen:
    // ODP_TimestampExtension time_ext; 
    
    uint16_t payload_length;  // Länge der darauffolgenden Anwendungsdaten
    uint8_t  application_payload[498]; // Die restlichen Bytes für die unkomplizierte Nutzdaten-Payload
};

#pragma pack(pop)
