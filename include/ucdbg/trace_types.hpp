#pragma once


#include <cstdint>
#include <string>
#include <chrono>
#include <ucdbg/logger.hpp>

namespace ucdbg {

enum class EventKind {
    Concurrency,
    Log
};    

enum class EventType {
    ThreadStart,
    ThreadEnd,
    LockAcquire,
    LockRelease
};

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

struct TraceEvent {
    uint64_t timestamp_ns;
    uint64_t thread_id;
    EventKind kind;

    union {
        struct {
            EventType type;
            uint64_t lock_id;
        } concurrency;

        struct {
            LogLevel level;
            const char* message;
        } log;
    };
};

struct ThreadInfo {
    uint64_t thread_id;
    std::string thread_name;
    uint64_t start_time;
    uint64_t end_time;
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

