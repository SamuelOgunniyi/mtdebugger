/**
 * Basic test program for C++ tracer
 * 
 * This test verifies:
 * 1. Tracer can be initialized
 * 2. Thread IDs can be retrieved
 * 3. Macros compile without errors
 */

#define UCDBG_TESTING 1
#include <ucdbg/ucdbg.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cassert>
#include <cstring>
#include <string>

// Mutex to synchronize output (prevent race conditions)
static std::mutex cout_mutex;

static void test_thread_name_truncation() {
    std::string long_name(64, 'A');
    UCDBG_THREAD_NAME(long_name);

    const auto full_name = ucdbg::get_thread_name();
    assert(full_name == long_name);

    auto& tracer = ucdbg::internal::TracerImpl::instance();
    const auto& buf = tracer.thread_name_buf_for_test();
    const auto len = tracer.thread_name_len_for_test();
    assert(len <= buf.size() - 1);
    assert(buf[len] == '\0');
    assert(std::memcmp(buf.data(), long_name.data(), len) == 0);
}

void worker_thread(int id) {
    // Set thread name
    char name[32];
    snprintf(name, sizeof(name), "worker_%d", id);
    UCDBG_THREAD_NAME(name);
    
    // Mark thread start
    UCDBG_THREAD_START();
    
}

int main() {
    std::cout << "=== C++ Tracer Basic Test ===" << std::endl;
    
    // Initialize tracer
    if (!UCDBG_INIT("/tmp/ucdbg.sock")) {
        std::cerr << "Failed to initialize tracer" << std::endl;
        return 1;
    }
    
    std::cout << "Tracer initialized successfully" << std::endl;
    std::cout << "Main thread ID: " << ucdbg::get_thread_id() << std::endl;

    test_thread_name_truncation();
    
    // Create worker threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker_thread, i);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "All threads completed" << std::endl;
    
    // Shutdown tracer
    ucdbg::shutdown();
    std::cout << "Tracer shutdown complete" << std::endl;
    
    return 0;
}



