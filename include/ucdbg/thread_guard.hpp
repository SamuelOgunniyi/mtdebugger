#pragma once

#include <ucdbg/event_helpers.hpp>
#include <ucdbg/concurrentqueue.h>


namespace ucdbg {
namespace internal {

class ThreadGuard {
public:
    ThreadGuard() {
        event_queue_.enqueue(make_concurrency_event(EventType::ThreadStart));

    }

    ~ThreadGuard() noexcept {
        event_queue_.enqueue(make_concurrency_event(EventType::ThreadEnd));
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
    ThreadGuard(ThreadGuard&&) = delete;
    ThreadGuard& operator=(ThreadGuard&&) = delete;


private:
    moodycamel::ConcurrentQueue<ucdbg::TraceEvent> event_queue_;
};

}  // namespace internal 
} // namespace ucdbg