# Binary Trace Format Specification

## Design Goals

This format is designed to be:
- **ABI-independent**: Fixed-size types only, no pointers
- **Endian-defined**: Little-endian (standard for x86/x86-64)
- **Forward & backward compatible**: Version field + type discriminator
- **Efficient to write from hot paths**: Simple memcpy serialization
- **Easy to extend**: New event types can be added without breaking old readers
- **Friendly to offline tooling**: Fixed-size records, easy to parse

## TraceEvent Structure

### Binary Layout (32 bytes, little-endian)

```
Offset  Size  Type          Field
─────────────────────────────────────────────────
0       8     uint64_t      timestamp_ns
8       8     uint64_t      thread_id
16      1     uint8_t       format_version
17      1     uint8_t       kind (EventKind)
18      1     uint8_t       reserved[0]
19      1     uint8_t       reserved[1]
20      12    (union)       payload
─────────────────────────────────────────────────
Total:  32 bytes
```

### Payload by Event Kind

#### Concurrency Events (EventKind::Concurrency = 0)

```
Offset  Size  Type          Field
─────────────────────────────────────────────────
20      1     uint8_t       type (EventType)
21      1     uint8_t       reserved
22      1     uint8_t       reserved
23      1     uint8_t       reserved
24      8     uint64_t      lock_id
─────────────────────────────────────────────────
```

#### Log Events (EventKind::Log = 1)

```
Offset  Size  Type          Field
─────────────────────────────────────────────────
20      1     uint8_t       level (LogLevel)
21      1     uint8_t       reserved
22      1     uint8_t       reserved
23      1     uint8_t       reserved
24      4     uint32_t      message_string_id
28      4     uint32_t      reserved
─────────────────────────────────────────────────
```

## Type Definitions

### Fixed-Size Type Aliases
```cpp
using timestamp_t = uint64_t;      // Nanoseconds since epoch
using thread_id_t = uint64_t;       // Thread identifier
using lock_id_t = uint64_t;         // Lock identifier
using string_id_t = uint32_t;       // String table index
```

### Enumerations (explicit uint8_t)
```cpp
enum class EventKind : uint8_t {
    Concurrency = 0,
    Log = 1
};

enum class EventType : uint8_t {
    ThreadStart = 0,
    ThreadName = 1,
    ThreadEnd = 2,
    LockAcquire = 3,
    LockRelease = 4
    // Future types can be added here
};

enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};
```

## Serialization

### Writing (Hot Path)
```cpp
TraceEvent event = make_concurrency_event(EventType::LockAcquire, lock_id);
event.serialize_to(buffer);  // Simple memcpy - ~1-2ns
```

### Reading (Offline Tooling)
```cpp
TraceEvent event;
event.deserialize_from(buffer);

if (event.format_version > TRACE_FORMAT_VERSION) {
    // Skip unknown version
    continue;
}

switch (event.kind) {
    case EventKind::Concurrency:
        // Process concurrency event
        break;
    case EventKind::Log:
        // Resolve message_string_id from string table
        break;
    default:
        // Skip unknown event types (forward compatibility)
        break;
}
```

## Forward Compatibility

### Adding New Event Types

1. **Add to enum** (increment value):
   ```cpp
   enum class EventType : uint8_t {
       // ... existing types ...
       LockTryAcquire = 4,  // New type
   };
   ```

2. **Old readers**: Will see unknown `type` value and skip the event
3. **New readers**: Can handle both old and new types

### Reassigning EventType Values

If you reorder or renumber existing `EventType` values, **bump `TRACE_FORMAT_VERSION`** and update any readers/writers and documentation that assume the old numeric values.

### Versioning

- `format_version` field allows format changes
- Old readers check version and skip if too new
- New readers can handle multiple versions

## String Table

Log messages use `string_id_t` instead of `const char*`:
- **Hot path**: Store string_id (fast, no string copying)
- **String table**: Separate structure mapping string_id → string
- **Serialization**: Write string table separately, events reference by ID

### String Table Format (Future)

```
[Header]
  uint32_t entry_count
  uint32_t total_size

[Entries]
  For each entry:
    uint32_t string_id
    uint32_t string_length
    char[]   string_data (UTF-8, null-terminated)
```

## Endianness

- **Format**: Little-endian (standard for x86/x86-64)
- **Conversion**: If needed for big-endian systems, add conversion in serialize/deserialize
- **Current**: Assumes little-endian (no conversion)

## Performance Characteristics

- **Size**: Fixed 32 bytes per event
- **Serialization**: Single memcpy (~1-2ns)
- **Alignment**: Packed structure (no padding)
- **Cache-friendly**: Small, fixed-size records

## Example Binary Dump

```
Event: LockAcquire
Hex dump (32 bytes, little-endian):
00 00 00 00 00 00 00 01  # timestamp_ns = 1
00 00 00 00 00 00 00 42  # thread_id = 66
02                       # format_version = 2
00                       # kind = Concurrency
00 00                    # reserved
03                       # type = LockAcquire
00 00 00                 # reserved
00 00 00 00 00 00 00 05  # lock_id = 5
00 00 00 00              # padding
```

## Validation

```cpp
bool TraceEvent::is_valid() const {
    return format_version <= TRACE_FORMAT_VERSION;
}
```

## Migration from Legacy Format

The `TraceEventLegacy` struct provides backward compatibility:
- Old code can use `TraceEventLegacy` (with `const char*`)
- Convert to binary format when enqueueing:
  ```cpp
  TraceEventLegacy legacy = ...;
  string_id_t msg_id = resolve_string(legacy.log.message);
  TraceEvent binary = legacy.to_binary_format(msg_id);
  ```



