#pragma once

#include <ucdbg/trace_types.hpp>
#include <ucdbg/fast_timestamp.hpp>
#include <cstring>

namespace ucdbg {
    uint64_t get_thread_id();
}

namespace ucdbg {
namespace internal {

    inline TraceEvent make_concurrency_event(EventType type, lock_id_t lock_id = 0) {
        TraceEvent event;
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.format_version = TRACE_FORMAT_VERSION;
        event.kind = EventKind::Concurrency;
        event.reserved[0] = 0;
        event.reserved[1] = 0;
        event.concurrency.type = type;
        event.concurrency.lock_id = lock_id;
        std::memset(event.concurrency.reserved, 0, 3);
        return event;
    }

    // For hot paths: create log event with string_id (from string table)
    inline TraceEvent make_log_event(LogLevel level, string_id_t message_string_id) {
        TraceEvent event;
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.format_version = TRACE_FORMAT_VERSION;
        event.kind = EventKind::Log;
        event.reserved[0] = 0;
        event.reserved[1] = 0;
        event.log.level = level;
        event.log.message_string_id = message_string_id;
        std::memset(event.log.reserved, 0, 3);
        event.log.reserved2 = 0;
        return event;
    }

} // namespace internal
} // namespace ucdbg