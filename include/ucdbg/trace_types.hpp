#pragma once

#include <cstdint>
#include <string>
#include <cstring>

namespace ucdbg {

// Binary format version (increment when format changes)
constexpr uint8_t TRACE_FORMAT_VERSION = 1;

// Fixed-size type aliases for ABI independence
using timestamp_t = uint64_t;      // Nanoseconds since epoch
using thread_id_t = uint64_t;      // Thread identifier
using lock_id_t = uint64_t;        // Lock identifier
using string_id_t = uint32_t;      // String table index (for log messages)

// Event kind discriminator (explicit uint8_t for binary format)
enum class EventKind : uint8_t {
    Concurrency = 0,
    Log = 1
};

// Event type (explicit uint8_t for binary format)
enum class EventType : uint8_t {
    ThreadStart = 0,
    ThreadEnd = 1,
    LockAcquire = 2,
    LockRelease = 3
    // Add new types here - old readers will skip unknown types
};

// Log level (explicit uint8_t for binary format)
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

/**
 * Binary-serializable trace event structure.
 * 
 * Design principles:
 * - ABI-independent: Fixed-size types only
 * - Endian-defined: Little-endian (standard for x86/x86-64)
 * - Forward compatible: Version field + type discriminator
 * - Hot path efficient: Simple memcpy serialization
 * - Extensible: New event types can be added without breaking old readers
 * 
 * Binary layout (little-endian, 24 bytes):
 *   Offset  Size  Field
 *   0       8     timestamp_ns
 *   8       8     thread_id
 *   16      1     format_version
 *   17      1     kind (EventKind)
 *   18      1     reserved (for future use)
 *   19      1     reserved (for future use)
 *   20      4     payload (union - see below)
 * 
 * Payload layout by kind:
 *   Concurrency (EventKind::Concurrency):
 *     20      1     type (EventType)
 *     21      1     reserved
 *     22      2     reserved
 *     24      8     lock_id
 *     Total: 32 bytes
 * 
 *   Log (EventKind::Log):
 *     20      1     level (LogLevel)
 *     21      1     reserved
 *     22      2     reserved
 *     24      4     message_string_id (string_id_t)
 *     Total: 28 bytes (rounded to 32 for alignment)
 */
#pragma pack(push, 1)  // No padding - critical for binary format
struct TraceEvent {
    // ============================================================================
    // Data Members (Binary Layout - 32 bytes total)
    // ============================================================================
    
    // Common fields (16 bytes)
    timestamp_t timestamp_ns;      // 0-7: Timestamp in nanoseconds
    thread_id_t thread_id;          // 8-15: Thread ID
    
    // Header (4 bytes)
    uint8_t format_version;         // 16: Format version (for forward compatibility)
    EventKind kind;                 // 17: Event kind discriminator
    uint8_t reserved[2];            // 18-19: Reserved for future use
    
    // Payload union (12 bytes)
    union {
        struct {
            EventType type;         // 20: Event type
            uint8_t reserved[3];    // 21-23: Reserved
            lock_id_t lock_id;      // 24-31: Lock ID
        } concurrency;
        
        struct {
            LogLevel level;         // 20: Log level
            uint8_t reserved[3];    // 21-23: Reserved
            string_id_t message_string_id;  // 24-27: String table index
            uint32_t reserved2;     // 28-31: Reserved
        } log;
    };
    
    // ============================================================================
    // Methods
    // ============================================================================
    
    // Serialization: Convert struct to binary format
    // Usage: event.serialize_to(buffer); write_to_file(buffer, 32);
    void serialize_to(void* buffer) const {
        // Simple memcpy - structure is packed and fixed-size
        std::memcpy(buffer, this, sizeof(TraceEvent));
        // Note: Endianness conversion would happen here if needed
        // For now, assuming little-endian (standard)
    }
    
    // Deserialization: Convert binary format to struct
    // Usage: read_from_file(buffer, 32); event.deserialize_from(buffer);
    void deserialize_from(const void* buffer) {
        std::memcpy(this, buffer, sizeof(TraceEvent));
        // Note: Endianness conversion would happen here if needed
    }
    
    // Validation: Check if event format is supported
    // Usage: if (!event.is_valid()) { skip event; }
    bool is_valid() const {
        return format_version <= TRACE_FORMAT_VERSION;
    }
};
#pragma pack(pop)  // Restore default packing

// Static size check (after struct definition)
static_assert(sizeof(TraceEvent) == 32, "TraceEvent must be exactly 32 bytes for binary format");

struct ThreadInfo {
    thread_id_t thread_id;
    std::string thread_name;
    timestamp_t start_time;
    timestamp_t end_time;
};

inline std::string event_type_to_string(EventType event_type) {
    switch (event_type) {
        case EventType::ThreadStart: return "ThreadStart";
        case EventType::ThreadEnd: return "ThreadEnd";
        case EventType::LockAcquire: return "LockAcquire";
        case EventType::LockRelease: return "LockRelease";
        default: return "Unknown";
    }
}

} // namespace ucdbg
