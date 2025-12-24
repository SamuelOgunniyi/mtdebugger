/**
 * Basic test program for C++ tracer
 * 
 * This test verifies:
 * 1. Tracer can be initialized
 * 2. Thread IDs can be retrieved
 * 3. Macros compile without errors
 */

#include <ucdbg/ucdbg.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

// Mutex to synchronize output (prevent race conditions)
static std::mutex cout_mutex;

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



