---
specType: architecture-decision
title: ADR-001 Layered Architecture with Hardware Abstraction
phase: 03-architecture
date: "2025-11-06"
status: accepted
decision-id: ADR-001
---

# ADR-001: Layered Architecture with Hardware Abstraction

**Status**: Accepted  
**Date**: 2025-11-06  
**Authors**: Architecture Team  
**Stakeholders**: Development Team, Platform Integration Teams, Open Source Community

## Context

The AES3-2009 Digital Audio Interface implementation must support multiple deployment scenarios:

- **Embedded Real-Time Systems**: ARM Cortex-M7 microcontrollers with real-time operating systems (Linux PREEMPT_RT)
- **Desktop Systems**: Windows x86-64 for non-real-time testing and development
- **Platform Diversity**: The implementation must be platform-agnostic to enable integration across various audio hardware platforms
- **Hardware Abstraction**: Integration time must be reduced from weeks to days through minimal hardware abstraction (≤10 HAL functions per StR-BUS-002)

The architecture must support:
1. **100% AES3-2009 specification compliance** across all 4 parts
2. **Real-time performance** (<20.8µs latency @ 48kHz, <0.025 UI jitter)
3. **Platform portability** (no vendor lock-in)
4. **Testability** (hardware-independent unit testing)
5. **Maintainability** (clear separation of concerns)

## Decision

We will implement a **layered architecture** with strict hardware abstraction boundaries:

### Architecture Layers

```
┌─────────────────────────────────────────────┐
│         Application Layer                   │
│  (Audio applications using AES3 interface)  │
└─────────────────┬───────────────────────────┘
                  │ AES3 Public API (C++17)
┌─────────────────▼───────────────────────────┐
│           API Layer (C++17)                 │
│  • Initialization & Configuration           │
│  • Error Handling & Status Reporting        │
│  • Thread-Safe Operation Coordination       │
└─────────────────┬───────────────────────────┘
                  │ Protocol Engine Interface
┌─────────────────▼───────────────────────────┐
│       Protocol Engine (C++17)               │
│  • AES3 State Machines                      │
│  • Frame Assembly/Disassembly               │
│  • Real-Time Timing Coordination            │
└─────────────────┬───────────────────────────┘
                  │ Standards Interface
┌─────────────────▼───────────────────────────┐
│      Standards Layer (C++17)                │
│  ┌──────────────────────────────────────┐   │
│  │ Part 1: Audio Content                │   │
│  │ • PCM Encoding/Decoding              │   │
│  │ • Sample Formatting                  │   │
│  │ • Validity Bit Handling              │   │
│  └──────────────────────────────────────┘   │
│  ┌──────────────────────────────────────┐   │
│  │ Part 2: Metadata/Subcode             │   │
│  │ • Channel Status Management          │   │
│  │ • CRCC Validation                    │   │
│  │ • User Data Channel                  │   │
│  └──────────────────────────────────────┘   │
│  ┌──────────────────────────────────────┐   │
│  │ Part 3: Transport                    │   │
│  │ • Subframe/Frame/Block Structure     │   │
│  │ • Preamble Detection/Generation      │   │
│  │ • Biphase-Mark Coding                │   │
│  └──────────────────────────────────────┘   │
│  ┌──────────────────────────────────────┐   │
│  │ Part 4: Physical/Electrical          │   │
│  │ • Jitter Specifications              │   │
│  │ • Signal Validation                  │   │
│  │ • Electrical Parameters              │   │
│  └──────────────────────────────────────┘   │
└─────────────────┬───────────────────────────┘
                  │ HAL Interface (Pure C)
┌─────────────────▼───────────────────────────┐
│  Hardware Abstraction Layer (Pure C)        │
│  • Audio HAL (≤10 functions)                │
│  • Timing HAL (≤10 functions)               │
│  • I/O HAL (≤10 functions)                  │
└─────────────────┬───────────────────────────┘
                  │ Platform-Specific Implementation
┌─────────────────▼───────────────────────────┐
│       Platform Bridge Layer                 │
│  • Linux PREEMPT_RT Implementation          │
│  • Windows Implementation                   │
│  • Other Platform Implementations (future)  │
└─────────────────┬───────────────────────────┘
                  │ Hardware Drivers
┌─────────────────▼───────────────────────────┐
│         Hardware Layer                      │
│  • Audio Hardware (DACs/ADCs)               │
│  • Timing Hardware (Clocks/PLLs)            │
│  • Digital I/O Hardware                     │
└─────────────────────────────────────────────┘
```

### Key Architectural Principles

1. **Dependency Inversion**: High-level Standards layer depends on HAL abstraction, not concrete hardware
2. **Single Responsibility**: Each layer has one clear purpose and responsibility
3. **Interface Segregation**: HAL interfaces are minimal (≤10 functions each)
4. **Open/Closed Principle**: Standards layer is closed for modification, open for platform extension via HAL
5. **Separation of Concerns**: Protocol logic completely separated from hardware access

### Layer Responsibilities

#### API Layer (C++17)
- **Public Interface**: Application-facing API for AES3 operations
- **Configuration Management**: System initialization and parameter configuration
- **Error Handling**: Unified error reporting and status queries
- **Thread Safety**: Coordinate multi-threaded access to AES3 system

#### Protocol Engine (C++17)
- **State Machine Coordination**: Manage AES3 transmitter/receiver states
- **Real-Time Orchestration**: Coordinate timing-critical operations
- **Buffer Management**: Manage audio buffers for continuous streaming
- **Performance Monitoring**: Track jitter, latency, and throughput metrics

#### Standards Layer (C++17)
- **Pure Protocol Implementation**: AES3-2009 Parts 1-4 specification compliance
- **Hardware Agnostic**: No platform-specific code, no hardware dependencies
- **Testable**: Fully unit-testable without hardware present
- **Deterministic**: Predictable timing and memory behavior

#### Hardware Abstraction Layer (Pure C)
- **Minimal Interface**: ≤10 functions per HAL category
- **Platform Agnostic**: C function pointers for maximum portability
- **Zero Overhead**: Inline-able function calls, no virtual dispatch
- **Clear Contracts**: Well-defined function signatures and error codes

#### Platform Bridge Layer
- **Platform-Specific Implementation**: Implements HAL for specific platforms
- **Driver Integration**: Maps HAL to platform audio/timing drivers
- **OS Service Usage**: Utilizes platform threading, memory, and I/O services
- **Optimization**: Platform-specific performance tuning

### Hardware Abstraction Interface

**Audio HAL Interface** (≤10 functions):
```c
typedef struct {
    int (*init)(const audio_config_t* config);
    int (*send_samples)(const int32_t* samples, size_t count);
    int (*receive_samples)(int32_t* buffer, size_t* count);
    int (*set_sample_rate)(uint32_t sample_rate_hz);
    uint64_t (*get_sample_clock)(void);
    int (*set_callback)(audio_callback_t callback);
    int (*start)(void);
    int (*stop)(void);
    int (*get_status)(audio_status_t* status);
    void (*cleanup)(void);
} audio_hal_interface_t;
```

**Timing HAL Interface** (≤10 functions):
```c
typedef struct {
    int (*init)(const timing_config_t* config);
    uint64_t (*get_timestamp_ns)(void);
    int (*set_timer)(uint32_t interval_ns, timer_callback_t callback);
    int (*get_clock_source)(clock_source_t* source);
    int (*measure_jitter)(jitter_stats_t* stats);
    int (*synchronize_clock)(uint64_t reference_time);
    int (*start)(void);
    int (*stop)(void);
    int (*get_status)(timing_status_t* status);
    void (*cleanup)(void);
} timing_hal_interface_t;
```

## Consequences

### Positive Consequences

1. **Platform Portability**
   - Standards layer compiles without any platform dependencies
   - Single codebase supports Linux PREEMPT_RT, Windows, and future platforms
   - Minimal integration effort (≤10 HAL functions per platform)

2. **Testability**
   - Standards layer fully unit-testable with mock HAL implementations
   - Hardware-independent testing on development workstations
   - Conformance testing without physical audio hardware

3. **Maintainability**
   - Clear separation of concerns between layers
   - AES3 protocol changes isolated to Standards layer
   - Platform-specific changes isolated to Platform Bridge layer

4. **Performance**
   - Static allocation in Standards/Protocol layers (deterministic real-time)
   - Zero-overhead HAL abstraction (function pointers, inline-able)
   - Direct hardware access through thin Platform Bridge

5. **Compliance**
   - Standards layer focuses exclusively on AES3-2009 specification
   - No mixing of protocol logic with hardware concerns
   - Easier certification and conformance validation

### Negative Consequences

1. **Abstraction Overhead**
   - HAL function pointer indirection (mitigated by inlining)
   - Potential cache misses from layered calls (minimal impact <50ns)

2. **Complexity**
   - More interfaces to maintain and document
   - Learning curve for contributors understanding layer boundaries

3. **Initial Development Effort**
   - Requires careful HAL interface design upfront
   - More boilerplate code for abstraction layers

### Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| HAL interface insufficient for all platforms | High | Design HAL based on widest platform requirements, include extension points |
| Performance degradation from abstraction | Medium | Profile on target hardware, optimize hot paths, use inline functions |
| Standards layer accidentally depends on platform | High | Enforce through build system (link only HAL interface), code reviews |
| HAL interface changes break existing platforms | High | Version HAL interface, maintain backward compatibility |

## Alternatives Considered

### Alternative 1: Monolithic Platform-Specific Implementation
**Description**: Separate implementations for each platform (Linux, Windows, etc.)

**Pros**:
- Maximum performance (no abstraction overhead)
- Platform-specific optimizations easier

**Cons**:
- Duplicate AES3 protocol logic across platforms
- High maintenance burden (bugs fixed multiple times)
- Difficult to ensure consistent AES3 compliance
- **Rejected**: Violates DRY principle and portability requirements

### Alternative 2: Virtual Function Abstraction (C++ Virtual HAL)
**Description**: Use C++ virtual functions for HAL instead of C function pointers

**Pros**:
- Type-safe interfaces
- Easier C++ testing with mocks

**Cons**:
- Not usable from bare-metal C environments
- Virtual function call overhead (vtable dispatch)
- Exception handling complexity in real-time
- **Rejected**: Reduces platform portability, C function pointers more universal

### Alternative 3: Header-Only Template Library
**Description**: Implement Standards layer as C++ templates instantiated per platform

**Pros**:
- Zero abstraction overhead (compile-time polymorphism)
- Maximum performance

**Cons**:
- Long compilation times
- Binary bloat from template instantiation
- Difficult to debug template errors
- **Rejected**: Complexity outweighs benefits, harder to maintain

## Related Decisions

- **ADR-002: Programming Language Selection** - Defines C++17 for Standards/API, Pure C for HAL
- **ADR-003: Build System and Toolchain** - CMake build supporting multi-platform compilation
- **ADR-004: Real-Time Performance Strategy** - Static allocation and timing guarantees
- **ADR-005: Memory Management Strategy** - Static allocation in real-time paths

## References

- **StR-BUS-002**: Hardware abstraction requirements (≤10 HAL functions)
- **StR-FUNC-001-004**: AES3-2009 Parts 1-4 implementation requirements
- **StR-PORT-001**: Multi-platform support requirement
- **ISO/IEC/IEEE 42010:2011**: Architecture description standard
- **AES3-2009**: Digital audio interface standard (all 4 parts)
- **C4 Model**: Context, Containers, Components, Code architecture views

## Trace-To

- `StR-BUS-002`: Hardware abstraction requirements
- `StR-PORT-001`: Multi-platform support
- `StR-FUNC-001`: AES3-2009 Part 1 implementation
- `StR-FUNC-002`: AES3-2009 Part 2 implementation
- `StR-FUNC-003`: AES3-2009 Part 3 implementation
- `StR-FUNC-004`: AES3-2009 Part 4 implementation