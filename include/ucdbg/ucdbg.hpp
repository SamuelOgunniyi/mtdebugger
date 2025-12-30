/**
 * Unified Concurrency Debugger - C++ Tracer
 * 
 * Header-only library for tracing concurrency events in C++ applications.
 * 
 * Stage 1.1: Basic structure and macros
 */

#pragma once

#include <cstdint>
#include <unistd.h> 
#include <sys/syscall.h> 
#include <thread>
#include <atomic>
#include <concepts>
#include <type_traits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <ucdbg/fast_timestamp.hpp>
#include <ucdbg/thread_guard.hpp>
#include <ucdbg/lock_guard.hpp>


namespace ucdbg {

// Forward declarations
namespace internal {
    class TracerImpl;
}

/**
 * Initialize the tracer system
 * @param transport_path Path to Unix Domain Socket or file
 * @return true if initialization succeeded
 */
bool init(const char* transport_path = "/tmp/ucdbg.sock");

/**
 * Shutdown the tracer system
 */
void shutdown();

/**
 * Set a name for the current thread
 */
void set_thread_name(std::string_view name);

/**
 * Get the name for the current thread
 */
std::string get_thread_name();

/**
 * Get current thread ID (public API)
 */
uint64_t get_thread_id(); // Not implemented

} // namespace ucdbg

// ============================================================================
// Public Macros for Instrumentation
// ============================================================================

/**
 * Initialize the tracer
 * Usage: UCDBG_INIT("/tmp/ucdbg.sock")
 */
#define UCDBG_INIT(path) \
    ucdbg::init(path)

/**
 * Set thread name
 * Usage: UCDBG_THREAD_NAME("worker_thread")
 */
#define UCDBG_THREAD_NAME(name) \
    ucdbg::set_thread_name(name)

/**
 * Mark thread start (automatically called on first use)
 */
#define UCDBG_THREAD_START() \
    ucdbg::internal::ThreadGuard _ucdbg_thread_guard;

// ============================================================================
// Internal Implementation
// ============================================================================

namespace ucdbg {
namespace internal {

/**
 * Internal tracer implementation
 * This will be expanded in later stages
 */
class TracerImpl {
public:
    static TracerImpl& instance() {
        static TracerImpl inst;
        return inst;
    }

    bool initialize(const char* transport_path) {
        if (initialized_.load()) {
            return false;  // Already initialized
        }
        
        transport_path_ = transport_path ? transport_path : "/tmp/ucdbg.sock";

        // TODO: Open transport connection
        
        bool success = true; // replace with real check
        if (success) initialized_.store(true);

        return success;
    }

    void shutdown() {
        if (!initialized_.load()) {
            return;
        }
        
        // TODO: Close transport connection
        initialized_.store(false);
    }

    bool is_initialized() const {
        return initialized_.load();
    }

    friend void ucdbg::set_thread_name(std::string_view);
    friend std::string ucdbg::get_thread_name();


private:
    std::atomic<bool> initialized_{false};
    std::string transport_path_;
    inline static std::mutex thread_name_map_mutex_;
    inline static thread_local std::string thread_name_;  
    inline static std::unordered_map<uint64_t, std::string> thread_name_map_;

    std::string get_thread_name(){
        return thread_name_;
    }

    void register_thread_name(const std::string& name){
        thread_name_ = name;  
        std::lock_guard<std::mutex> lock(thread_name_map_mutex_);
        thread_name_map_[get_thread_id()] = thread_name_;
    }
};


} // namespace internal

// ============================================================================
// Public API Implementation
// ============================================================================

inline bool init(const char* transport_path) {
    auto& tracer_instance = internal::TracerImpl::instance();
    if(tracer_instance.is_initialized()) { return true; }
    return tracer_instance.initialize(transport_path);
}

inline void shutdown() {
    internal::TracerImpl::instance().shutdown();
}

inline void set_thread_name(std::string_view name) {    
    if (name.empty()) {
        throw std::invalid_argument("thread name cannot be empty");
    }
    internal::TracerImpl::instance().register_thread_name(std::string(name));
}

inline std::string get_thread_name() {
    return internal::TracerImpl::instance().get_thread_name();
}

inline uint64_t get_thread_id() {
    static thread_local uint64_t cached = static_cast<uint64_t>(syscall(SYS_gettid));
    return cached;
}

} // namespace ucdbg

