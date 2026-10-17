#pragma once

// === ODEP COMPONENT SELECTION ===
// Jedes Gerät benötigt den Core (Schicht 1 Basic).

#define ODEP_ENABLE_SCHEDULER       // Auskommentieren für extrem kleine MCUs (spart RAM)
#define ODEP_ENABLE_VIRTUALIZATION  // Auskommentieren, wenn das Gerät kein Proxy sein muss

// === LIMITS FOR RESOURCE CONSTRAINED SYSTEMS ===
#define ODEP_MAX_RESOURCES 8
#define ODEP_MAX_QUEUED_COMMANDS 5
#define ODEP_MAX_FUTURE_SLOTS 10
#define ODEP_MAX_VIRTUAL_DEVICES 4
