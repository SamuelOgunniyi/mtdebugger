# ucdbg

Unified Concurrency Debugger - A header-only C++ library for tracing concurrency events in multi-threaded applications.

## Overview

`ucdbg` is a lightweight, header-only C++ library for tracing concurrency events in multi-threaded applications. It was built to make concurrent systems easier to understand by providing low-overhead instrumentation for threads, locks and runtime events.

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

### Current focus

The tracing primitives are in place. Current work is focused on connecting the event pipeline by:

- integrating the lock-free event queue
- implementing asynchronous event transport
- adding serialization and visualization support

### 📋 Future Work

- Background worker thread for event consumption
- Transport layer (Unix domain socket/file output)
- Queue integration with TracerImpl
- Event serialization and transmission

### Project goals

The long-term goal is to provide a lightweight toolkit for understanding the runtime behaviour of concurrent systems without requiring heavyweight tracing frameworks.

While the initial motivation came from debugging robotics software, the library is designed to be useful for any modern C++ application where understanding thread interactions is important.

### Status

The project is still evolving. The core tracing infrastructure is in place, with the transport layer, visualization and developer tooling left as future work.

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

- **FastTimestamp**: Thread-local cached timestamps to reduce timestamp overhead on hot paths.
- **Thread-local caching**: Reduces system call overhead
- **Lock-free queue**: Zero-copy event transport (when implemented)

## Building

```bash
mkdir build && cd build
cmake ..
make
./test_basic
```

## License

See LICENSE file for details.
