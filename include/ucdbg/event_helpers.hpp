#pragma once

#include <ucdbg/trace_types.hpp>
#include <ucdbg/fast_timestamp.hpp>
#include <cstring>

namespace ucdbg {
    uint64_t get_thread_id();
}

namespace ucdbg {
namespace internal {

    inline TraceEvent make_concurrency_event(EventType type, lock_id_t lock_id = 0) noexcept {
        TraceEvent event{};
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.format_version = TRACE_FORMAT_VERSION;
        event.kind = EventKind::Concurrency;
        event.concurrency = {
            .type    = type,
            .reserved = {0, 0, 0},
            .lock_id = lock_id
        };
        return event;
    }

    inline TraceEvent make_log_event(LogLevel level, string_id_t message_string_id) noexcept {
        TraceEvent event{};
        event.timestamp_ns = FastTimestamp::now_ns();
        event.thread_id = get_thread_id();
        event.format_version = TRACE_FORMAT_VERSION;
        event.kind = EventKind::Log;
        event.log = {
            .level      = level,
            .reserved   = {0, 0, 0},
            .message_string_id = message_string_id,
            .reserved2  = 0
        };
        return event;
    }

} // namespace internal
} // namespace ucdbg