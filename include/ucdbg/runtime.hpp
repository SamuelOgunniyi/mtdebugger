#pragma once

#include <ucdbg/trace_types.hpp>
#include <ucdbg/concurrentqueue.h>

#include <atomic>
#include <array>
#include <chrono>
#include <thread>

#if defined(__x86_64__) || defined(__i386__)
  #include <immintrin.h>
#endif

namespace ucdbg {

class Runtime {
public:
    static inline Runtime& instance() {
        static Runtime r;
        return r;
    }

    inline void start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            return; // already running
        }
        consumer_ = std::thread(&Runtime::consumer_loop, this);
    }

    inline void stop() {
        if (!running_.exchange(false)) {
            return; // already stopped
        }

        if (consumer_.joinable()) {
            consumer_.join();
        }

        // Optional: drain remaining events after stopping
        // drain_queue_();
    }

    inline void submit(TraceEvent&& e) {
        event_queue_.enqueue(std::move(e));
    }

private:
    Runtime() = default;

    inline ~Runtime() {
        stop();
    }

    static inline void cpu_relax() noexcept {
        #if defined(__x86_64__) || defined(__i386__)
                _mm_pause();
        #elif defined(__aarch64__)
                asm volatile("yield");
        #else
                std::this_thread::yield();
        #endif
    }

    inline void consumer_loop() {
        using namespace std::chrono_literals;

        std::array<TraceEvent, 1024> buffer;
        warm_consumer_stack();
        uint32_t idle_loops{0};

        while (running_.load(std::memory_order_relaxed)) {
            const size_t n =
                event_queue_.try_dequeue_bulk(buffer.begin(), buffer.size());

            if (n == 0) {
                if (idle_loops < 1000) {
                    cpu_relax();
                } else if (idle_loops < 2000) {
                    std::this_thread::yield();
                } else {
                    std::this_thread::sleep_for(50us);
                }

                ++idle_loops;
                continue;
            }

            idle_loops = 0;

            for (size_t i = 0; i < n; ++i) {
                process_event(buffer[i]);
            }
        }
    }


    inline void warm_consumer_stack() {
        volatile std::byte scratch[64 * 1024]; // touch 64KB
        for (auto &b : scratch) b = std::byte{0};
    }

    inline void process_event(TraceEvent& event) {
        (void)event;
        // TODO: implement
    }



private:
    std::atomic<bool> running_{false};
    std::thread consumer_;
    moodycamel::ConcurrentQueue<TraceEvent> event_queue_;
};

} // namespace ucdbg
