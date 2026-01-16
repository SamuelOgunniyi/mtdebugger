#pragma once

#include <ucdbg/event_helpers.hpp>
#include <ucdbg/runtime.hpp>

namespace ucdbg {
namespace internal {

class ThreadGuard {
public:
    ThreadGuard() noexcept {
        Runtime::instance().submit(
            make_concurrency_event(EventType::ThreadStart)
        );
    }

    ~ThreadGuard() noexcept {
        Runtime::instance().submit(
            make_concurrency_event(EventType::ThreadEnd)
        );
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
    ThreadGuard(ThreadGuard&&) = delete;
    ThreadGuard& operator=(ThreadGuard&&) = delete;

};

}  // namespace internal 
} // namespace ucdbg