#pragma once

// === ODP COMPONENT SELECTION ===
// Jedes Gerät benötigt den Core (Schicht 1 Basic).

#define ODP_ENABLE_SCHEDULER       // Auskommentieren für extrem kleine MCUs (spart RAM)
#define ODP_ENABLE_VIRTUALIZATION  // Auskommentieren, wenn das Gerät kein Proxy sein muss

// === LIMITS FOR RESOURCE CONSTRAINED SYSTEMS ===
#define ODP_MAX_RESOURCES 8
#define ODP_MAX_QUEUED_COMMANDS 5
#define ODP_MAX_FUTURE_SLOTS 10
#define ODP_MAX_VIRTUAL_DEVICES 4
