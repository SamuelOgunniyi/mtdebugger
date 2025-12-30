#pragma once

#include <cstdint>
#include <chrono>

namespace ucdbg {
namespace internal {

/**
 * Fast timestamp for hot paths (lock acquire/release, etc.)
 *
 */
class FastTimestamp {
public:

    static uint64_t now_ns() {
        static thread_local uint64_t cached = 0;
        static thread_local std::chrono::steady_clock::time_point last_update;
        static thread_local uint32_t micro_seq = 0;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now - last_update).count();
        
        // Update cache every 100μs (100,000 ns) or on first call
        if (elapsed > 100'000 || cached == 0) {
            cached = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()
            ).count();
            last_update = now;
            micro_seq = 0;
        }
        
        // Add micro-sequence for ordering within same cache period
        return cached + (++micro_seq & 0xFFFF);
    }
};

} // namespace internal
} // namespace ucdbg

