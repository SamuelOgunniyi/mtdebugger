#pragma once

#include <ucdbg/trace_types.hpp>

namespace ucdbg {
    uint64_t get_thread_id();
}

namespace ucdbg {
namespace internal {

    inline TraceEvent make_concurrency_event(EventType type, uint64_t lock_id = 0) {
        TraceEvent event;
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.kind = EventKind::Concurrency;
        event.concurrency.type = type;
        event.concurrency.lock_id = lock_id;
        return event;
    }

    inline TraceEvent make_log_event(LogLevel level, const char* message) {
        TraceEvent event;
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.kind = EventKind::Log;
        event.log.level = level;
        event.log.message = message;
        return event;
    }

} // namespace internal
} // namespace ucdbg