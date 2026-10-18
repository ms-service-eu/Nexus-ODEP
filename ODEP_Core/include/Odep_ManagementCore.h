#ifndef ODEP_MANAGEMENT_CORE_H
#define ODEP_MANAGEMENT_CORE_H

#include <stdint.h>

/* --- OPCODES (1 Byte) --- */
typedef uint8_t OdepOpCode_t;
#define ODEP_OP_DISCOVERY_REQ  ((OdepOpCode_t)0x10)
#define ODEP_OP_DISCOVERY_RES  ((OdepOpCode_t)0x11)
#define ODEP_OP_SUBDEV_REG     ((OdepOpCode_t)0x12)
#define ODEP_OP_SUBDEV_UNREG   ((OdepOpCode_t)0x13)
#define ODEP_OP_DEPLOY_HEAD    ((OdepOpCode_t)0x20)
#define ODEP_OP_DEPLOY_CHUNK   ((OdepOpCode_t)0x21)
#define ODEP_OP_DEPLOY_ACK     ((OdepOpCode_t)0x22)

/* --- RUNTIME SYSTEM TYPEN (1 Byte) --- */
typedef uint8_t OdepExecType_t;
#define ODEP_EXEC_INTERPRETER  ((OdepExecType_t)0x01)
#define ODEP_EXEC_NATIVE_CPU   ((OdepExecType_t)0x02)

#pragma pack(push, 1)

/* =========================================================================
 * P2P DISCOVERY PAYLOAD (Exakt 44 Bytes -> 4-Byte Aligned)
 * ========================================================================= */
typedef struct {
    uint32_t device_class;              /* 4 Bytes */
    uint32_t hardware_revision;         /* 4 Bytes */
    uint32_t active_sub_devices;        /* 4 Bytes */
    uint8_t  device_name[32];           /* 32 Bytes ASCII */
} OdepDiscoveryPayload_t;

/* =========================================================================
 * LOGICAL SUB-DEVICE REGISTRY PAYLOAD (Exakt 24 Bytes -> 4-Byte Aligned)
 * ========================================================================= */
typedef struct {
    uint32_t logical_id;                /* 4 Bytes */
    uint32_t interface_type;            /* 4 Bytes */
    int32_t  min_range_fixed;           /* 4 Bytes (Q16.16) */
    int32_t  max_range_fixed;           /* 4 Bytes (Q16.16) */
    uint32_t step_resolution;           /* 4 Bytes (Q16.16) */
    uint32_t operational_flags;         /* 4 Bytes Bitmaske */
} OdepSubDevicePayload_t;

/* =========================================================================
 * CODE DEPLOYMENT HEADER (Exakt 44 Bytes -> 4-Byte Aligned)
 * ========================================================================= */
typedef struct {
    uint32_t       program_id;          /* 4 Bytes */
    uint32_t       total_size_bytes;    /* 4 Bytes */
    uint16_t       total_chunks;        /* 2 Bytes */
    OdepExecType_t exec_type;           /* 1 Byte */
    uint8_t        _reserved_align;     /* 1 Byte -> Richtet Steuerblock auf 4 Bytes aus */
    uint8_t        sha256[32];          /* 32 Bytes */
} OdepProgramHeaderPayload_t;

/* =========================================================================
 * CODE DEPLOYMENT FRAGMENT CHUNK (Exakt 520 Bytes -> 4-Byte Aligned)
 * ========================================================================= */
typedef struct {
    uint32_t program_id;                /* 4 Bytes */
    uint16_t chunk_index;               /* 2 Bytes */
    uint16_t byte_length;               /* 2 Bytes */
    uint8_t  raw_code_data[512];        /* 512 Bytes Segment */
} OdepProgramChunkPayload_t;

#pragma pack(pop)

#endif /* ODEP_MANAGEMENT_CORE_H */
