---
specType: architecture-decision
title: ADR-002 Programming Language Selection
phase: 03-architecture
date: "2025-11-06"
status: accepted
decision-id: ADR-002
---

# ADR-002: Programming Language Selection (C++17 with Pure C HAL)

**Status**: Accepted  
**Date**: 2025-11-06  
**Authors**: Architecture Team  
**Stakeholders**: Development Team, Platform Integration Teams, Compiler Vendors

## Context

The AES3-2009 Digital Audio Interface implementation requires careful language selection to balance:

- **Real-Time Performance**: Strict timing requirements (jitter <0.025 UI = 260ns @ 48kHz, latency <20.8µs)
- **Platform Portability**: Support Linux PREEMPT_RT (ARM Cortex-M7), Windows (x86-64), and future platforms
- **Hardware Abstraction**: Pure C interface for maximum compatibility with bare-metal and RTOS environments
- **Modern Development**: Type safety, testability, and maintainability for complex protocol implementation
- **Memory Constraints**: Embedded targets require <64KB code + <16KB RAM (StR-PERF-003)

### Language Requirements

1. **Deterministic Execution**: Predictable timing for real-time audio processing
2. **Zero-Cost Abstractions**: Performance comparable to hand-written C
3. **Static Memory Management**: No dynamic allocation in real-time paths
4. **Compiler Availability**: GCC, Clang, MSVC, IAR, Keil support
5. **Embedded Compatibility**: ARM Cortex-M7, x86-64 support
6. **Testability**: Modern testing frameworks and tooling

## Decision

We will use a **mixed language approach**:

### Language Assignment by Layer

| Layer | Language | Rationale |
|-------|----------|-----------|
| **Standards Layer** | **C++17** | Type safety, templates, zero-cost abstractions, modern features |
| **Protocol Engine** | **C++17** | State machines benefit from classes, RAII for resource management |
| **API Layer** | **C++17** | Object-oriented interface, exception handling for non-real-time |
| **HAL Interface** | **Pure C** | Maximum portability, compatible with all platforms and RTOS |
| **Platform Bridge** | **C/C++** | Platform-specific (C++ for complex platforms, C for bare-metal) |

### C++17 Features Utilized

#### Standards Layer & Protocol Engine

**Allowed C++17 Features**:
- **Classes and Namespaces**: Organize AES3 components logically
- **Templates**: Generic algorithms with zero-cost abstraction
- **constexpr**: Compile-time constants and calculations
- **inline**: Zero-overhead function calls
- **RAII**: Automatic resource management (buffers, timers)
- **std::array**: Fixed-size arrays with bounds checking
- **std::optional**: Optional return values without exceptions
- **Structured Bindings**: Clean tuple/pair unpacking
- **if constexpr**: Compile-time conditional compilation

**Forbidden C++17 Features** (Real-Time Paths):
- ❌ **Exceptions**: Non-deterministic, use return codes instead
- ❌ **RTTI** (typeid, dynamic_cast): Runtime overhead, disabled via `-fno-rtti`
- ❌ **Virtual Functions**: Use for non-critical paths only
- ❌ **Dynamic Allocation** (new/delete): Use static allocation
- ❌ **Standard Containers** (std::vector, std::map): Dynamic allocation
- ❌ **Standard Streams** (std::cout, std::cerr): Too slow for real-time
- ❌ **Standard Threads** (std::thread): Use platform threading

**Example Standards Layer Component** (C++17):
```cpp
namespace AES3 {
namespace Part3 {
namespace _2009 {

// constexpr compile-time constants
constexpr uint8_t PREAMBLE_X = 0xE2;
constexpr uint8_t PREAMBLE_Y = 0xE4;
constexpr uint8_t PREAMBLE_Z = 0xE8;
constexpr size_t SUBFRAME_SLOTS = 32;

// RAII subframe builder with static allocation
class SubframeBuilder {
public:
    // No exceptions, return error codes
    [[nodiscard]] int add_preamble(uint8_t preamble_type) noexcept;
    [[nodiscard]] int add_audio_data(uint32_t audio_sample) noexcept;
    [[nodiscard]] int add_validity_bit(bool valid) noexcept;
    
    // std::optional for safe optional returns
    std::optional<uint32_t> get_encoded_subframe() const noexcept;

private:
    std::array<uint8_t, SUBFRAME_SLOTS> slots_{};  // Static allocation
    size_t slot_index_{0};
};

// Template for generic biphase-mark encoding
template<typename SlotType>
constexpr SlotType biphase_encode_slot(SlotType data) noexcept {
    // Compile-time encoding if data is constexpr
    return /* biphase-mark encoding logic */;
}

} // namespace _2009
} // namespace Part3
} // namespace AES3
```

#### API Layer

**Additional C++17 Features Allowed** (Non-Real-Time):
- **std::string**: Configuration and error messages
- **std::vector**: Dynamic parameter lists
- **Exceptions**: Error handling in initialization/configuration
- **std::shared_ptr**: Shared resource management
- **std::mutex**: Thread synchronization

**Example API Layer** (C++17):
```cpp
namespace AES3 {
namespace API {

class AES3Interface {
public:
    // Constructor can throw exceptions (initialization only)
    explicit AES3Interface(const Config& config);
    
    // Non-real-time configuration with exceptions
    void configure(const Parameters& params);
    
    // Real-time transmit/receive: no exceptions, return codes
    [[nodiscard]] int transmit_samples(const int32_t* samples, size_t count) noexcept;
    [[nodiscard]] int receive_samples(int32_t* buffer, size_t* count) noexcept;
    
    // Status queries with modern types
    std::optional<Statistics> get_statistics() const;
    std::string get_error_message(int error_code) const;

private:
    std::unique_ptr<ProtocolEngine> engine_;  // RAII ownership
    mutable std::mutex status_mutex_;  // Thread-safe status
};

} // namespace API
} // namespace AES3
```

### Pure C HAL Interface

**HAL Interface Requirements**:
- **ISO C11 Standard**: Maximum compiler compatibility
- **No C++**: Must be callable from pure C code
- **extern "C"**: Prevent C++ name mangling
- **Function Pointers**: Virtual dispatch without C++ virtual functions
- **Return Codes**: errno-style error handling (0 = success, negative = error)

**HAL Interface Definition** (Pure C):
```c
/* aes3_audio_hal.h - Pure C interface */
#ifndef AES3_AUDIO_HAL_H
#define AES3_AUDIO_HAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
#define AES3_HAL_SUCCESS           0
#define AES3_HAL_ERROR_INVALID    -1
#define AES3_HAL_ERROR_TIMEOUT    -2
#define AES3_HAL_ERROR_HARDWARE   -3

/* Audio configuration structure */
typedef struct {
    uint32_t sample_rate_hz;
    uint8_t word_length_bits;
    uint8_t channel_count;
} aes3_audio_config_t;

/* Audio callback function pointer */
typedef void (*aes3_audio_callback_t)(int32_t* samples, size_t count, void* user_data);

/* Audio HAL interface structure (≤10 functions) */
typedef struct {
    int (*init)(const aes3_audio_config_t* config);
    int (*send_samples)(const int32_t* samples, size_t count);
    int (*receive_samples)(int32_t* buffer, size_t* count);
    int (*set_sample_rate)(uint32_t sample_rate_hz);
    uint64_t (*get_sample_clock)(void);
    int (*set_callback)(aes3_audio_callback_t callback, void* user_data);
    int (*start)(void);
    int (*stop)(void);
    int (*get_status)(uint32_t* status_flags);
    void (*cleanup)(void);
} aes3_audio_hal_t;

/* HAL registration function */
int aes3_register_audio_hal(const aes3_audio_hal_t* hal);

#ifdef __cplusplus
}
#endif

#endif /* AES3_AUDIO_HAL_H */
```

### Platform Bridge Language Choice

Platform Bridge layer can use either C or C++ depending on platform:

- **Bare-Metal / Simple RTOS**: Pure C implementation
- **Linux / Windows**: C++ implementation for richer OS integration
- **Mixed**: C++ wrapper around C hardware drivers

## Consequences

### Positive Consequences

1. **Type Safety and Modern Features**
   - C++17 provides compile-time error detection
   - Templates enable generic, reusable AES3 components
   - constexpr enables compile-time validation

2. **Zero-Cost Abstractions**
   - C++ classes compile to same assembly as C structs
   - Templates instantiated at compile-time (no runtime cost)
   - inline functions eliminate call overhead

3. **Maximum Portability**
   - Pure C HAL interface callable from any language/platform
   - No C++ runtime dependency in HAL
   - Compatible with bare-metal, RTOS, and OS environments

4. **Real-Time Determinism**
   - No exceptions in real-time paths (noexcept)
   - Static allocation with std::array
   - Predictable execution time

5. **Modern Development Experience**
   - C++17 features improve code readability
   - Better testing with modern C++ frameworks (Google Test)
   - Superior tooling support (clang-tidy, cppcheck)

### Negative Consequences

1. **Mixed Language Complexity**
   - Developers must understand C/C++ boundary
   - Build system more complex (C and C++ compilation)
   - Care needed with name mangling (extern "C")

2. **Compiler Version Requirements**
   - C++17 requires GCC 7+, Clang 5+, MSVC 2017+
   - Older embedded toolchains may lack C++17 support
   - IAR and Keil C++17 support varies by version

3. **Learning Curve**
   - Team must understand modern C++ best practices
   - Distinction between real-time safe and unsafe features
   - Template error messages can be cryptic

4. **Binary Size**
   - C++ templates may increase code size vs. C macros
   - Standard library inclusion must be controlled
   - **Mitigation**: Measure and optimize, use -Os for size

### Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| C++17 not available on embedded toolchain | High | Test toolchain compatibility early, fallback to C++14 if needed |
| Accidental dynamic allocation in real-time | High | Static analysis (clang-tidy), code review, custom allocator that asserts |
| Exception thrown in noexcept function | Critical | Enable `-Werror=noexcept`, unit test all error paths |
| Name mangling breaks C/HAL interface | High | Enforce extern "C" via static analysis, integration tests |
| Template bloat exceeds memory budget | Medium | Measure binary size, use explicit instantiation, -Os optimization |

## Alternatives Considered

### Alternative 1: Pure C Implementation
**Description**: Implement entire Standards layer in ISO C11

**Pros**:
- Maximum portability to oldest toolchains
- No C++ runtime dependency
- Smaller binary size (no templates)

**Cons**:
- Lose type safety (void* everywhere)
- Manual resource management (no RAII)
- Less readable code (macros instead of templates)
- Harder to test (no modern C++ test frameworks)
- **Rejected**: Development velocity and maintainability suffer

### Alternative 2: Full C++20 Implementation
**Description**: Use latest C++20 features (concepts, ranges, coroutines)

**Pros**:
- Most modern features and abstractions
- Better compile-time error messages (concepts)
- Potentially cleaner code (ranges)

**Cons**:
- Limited embedded toolchain support (IAR/Keil lag)
- Coroutines not suitable for real-time
- Binary size increase
- **Rejected**: Embedded toolchain support insufficient

### Alternative 3: Rust Implementation
**Description**: Implement in Rust for memory safety

**Pros**:
- Memory safety without garbage collection
- Zero-cost abstractions
- Modern tooling

**Cons**:
- Limited embedded toolchain support
- Steep learning curve for C/C++ teams
- FFI complexity with C HAL
- Less mature for embedded real-time audio
- **Rejected**: Team expertise and toolchain maturity

## Compiler and Toolchain Requirements

### Required Compilers

| Platform | Compiler | Minimum Version | C++ Standard |
|----------|----------|-----------------|--------------|
| Linux (x86-64) | GCC | 7.0+ | C++17 |
| Linux (ARM Cortex-M7) | ARM GCC | 9.0+ | C++17 |
| Windows (x86-64) | MSVC | 2017 (19.10+) | C++17 |
| Embedded (ARM) | IAR | 8.40+ | C++17 |
| Embedded (ARM) | Keil | MDK 5.29+ | C++17 |
| Alternative | Clang/LLVM | 5.0+ | C++17 |

### Compiler Flags

**C++17 Standards Layer**:
```bash
# GCC/Clang
-std=c++17 -fno-exceptions -fno-rtti -Wall -Wextra -Werror -pedantic

# MSVC
/std:c++17 /EHsc- /GR- /W4 /WX
```

**Pure C HAL Interface**:
```bash
# GCC/Clang
-std=c11 -Wall -Wextra -Werror -pedantic

# MSVC
/std:c11 /W4 /WX
```

### Static Analysis

- **clang-tidy**: Enforce C++17 best practices, detect noexcept violations
- **cppcheck**: Detect undefined behavior, memory leaks
- **PVS-Studio**: Commercial static analyzer for critical code

## Related Decisions

- **ADR-001: Layered Architecture** - Defines layer boundaries and responsibilities
- **ADR-003: Build System (CMake)** - C/C++ mixed compilation support
- **ADR-004: Real-Time Performance** - noexcept, static allocation enforcement
- **ADR-005: Memory Management** - Static allocation strategy

## References

- **ISO/IEC 14882:2017**: C++17 Standard
- **ISO/IEC 9899:2011**: C11 Standard
- **C++ Core Guidelines**: Modern C++ best practices
- **Real-Time C++ (Pfisterer)**: C++ for embedded real-time systems
- **StR-PERF-003**: Memory footprint requirements (<64KB code, <16KB RAM)
- **StR-PORT-002**: Compiler support requirements

## Trace-To

- `StR-PORT-002`: Compiler support (GCC, Clang, MSVC, IAR, Keil)
- `StR-PERF-001`: Real-time performance (<20.8µs latency)
- `StR-PERF-003`: Memory footprint (<64KB code, <16KB RAM)
- `StR-BUS-002`: Hardware abstraction (Pure C HAL interface)