#pragma once

#include <ucdbg/event_helpers.hpp>



namespace ucdbg {
namespace internal {

class ThreadGuard {
public:
    ThreadGuard() {
        auto event = make_concurrency_event(EventType::ThreadStart);
    }

    ~ThreadGuard() noexcept {
        auto event = make_concurrency_event(EventType::ThreadEnd);
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
    ThreadGuard(ThreadGuard&&) = delete;
    ThreadGuard& operator=(ThreadGuard&&) = delete;
};

}  // namespace internal 
} // namespace ucdbg