# ucdbg

Unified Concurrency Debugger - A header-only C++ library for tracing concurrency events in multi-threaded applications.

## Overview

`ucdbg` provides lightweight instrumentation for tracking thread lifecycle, lock operations, and other concurrency events with minimal overhead. Designed for performance-critical applications where tracing overhead must be kept to a minimum.

## Requirements

- CMake ≥ 3.16  
- A C++ compiler with C++20 support (GCC, Clang, or MSVC)

## Current Status

### ✅ Completed

**Core Infrastructure:**
- **ThreadGuard** (`thread_guard.hpp`) - RAII guard for automatic thread start/end tracking
- **LockGuard** (`lock_guard.hpp`) - RAII guard for lock acquire/release tracing with `Lockable` concept
- **FastTimestamp** (`fast_timestamp.hpp`) - Thread-local cached timestamps for hot paths (~1-2ns overhead vs ~20ns for std::chrono)
- **Event Helpers** (`event_helpers.hpp`) - Helper functions for creating `TraceEvent` objects
- **Trace Types** (`trace_types.hpp`) - Core event data structures (`TraceEvent`, `EventType`, `EventKind`)

**Architecture:**
- Clean dependency hierarchy (no circular dependencies)
- Forward declarations used to break dependency cycles
- Modular header structure

### 🚧 In Progress

**Event Transport:**
- **MPMC Queue** - moodycamel ConcurrentQueue integrated for lock-free event queuing
- **TraceEvent Producer** - Implementation of event enqueueing from guards (coming shortly)

### 📋 Planned

- Background worker thread for event consumption
- Transport layer (Unix domain socket/file output)
- Queue integration with TracerImpl
- Event serialization and transmission

## Project Structure

```
include/ucdbg/
├── ucdbg.hpp              # Main API header
├── trace_types.hpp        # Event data structures
├── fast_timestamp.hpp     # High-performance timestamping
├── event_helpers.hpp      # Event creation helpers
├── thread_guard.hpp       # Thread lifecycle tracking
├── lock_guard.hpp         # Lock operation tracking
└── concurrentqueue.h      # moodycamel lock-free queue (3rd party)
```

## Usage

### Basic Setup

```cpp
#include <ucdbg/ucdbg.hpp>

// Initialize the tracer
ucdbg::init("/tmp/ucdbg.sock");

// Set thread name
UCDBG_THREAD_NAME("worker_thread");

// Automatic thread tracking
UCDBG_THREAD_START();  // Emits ThreadStart event
```

### Lock Tracing

```cpp
#include <ucdbg/lock_guard.hpp>

std::mutex mtx;

{
    UCDBG_LOCK_GUARD(mtx);  // Automatically traces LockAcquire/LockRelease
    // ... critical section ...
}  // Lock release automatically traced
```

## Performance

- **FastTimestamp**: ~1-2ns overhead (10-20x faster than std::chrono)
- **Thread-local caching**: Reduces system call overhead
- **Lock-free queue**: Zero-copy event transport (when implemented)
- **RAII guards**: Zero overhead when not used

## Building

```bash
mkdir build && cd build
cmake ..
make
./test_basic
```

## License

See LICENSE file for details.
