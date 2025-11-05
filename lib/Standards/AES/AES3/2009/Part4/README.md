# AES3-2009 Part 4: Hardware Abstraction Layer (HAL)

**Location**: `lib/Standards/AES/AES3/2009/Part4/`  
**Namespace**: `AES::AES3::Part4::_2009`

## Overview

Part 4 implements Hardware Abstraction Layer according to AES3-4-2009 (R2014):
- HAL interface definition (<10 functions)
- Jitter requirements (transmit < 0.025 UI, receive tolerance)
- Timing accuracy (UI precision, sampling frequency)
- Platform portability abstraction

## ⚠️ CRITICAL SCOPE

**IN SCOPE** (Standards Layer):
- HAL interface specification
- Jitter measurement and validation interfaces
- Timing requirements abstraction
- Platform-agnostic signal interfaces

**OUT OF SCOPE** (Platform Layer):
- Physical connector implementations (XLR, BNC)
- Voltage levels and impedance (hardware-specific)
- Platform-specific drivers (ASIO, ALSA, CoreAudio)
- Transmitter/receiver circuits

## Requirements

Implements 15 requirements from `02-requirements/functional/aes3-part4-hal-abstraction-requirements.md`:

**HAL Functions (8)**:
- **REQ-FUNC-HAL-001**: HAL Function Count Limit (<10)
- **REQ-FUNC-HAL-002**: Bit-Level Transmit Function
- **REQ-FUNC-HAL-003**: Bit-Level Receive Function
- **REQ-FUNC-HAL-004**: Sampling Clock Configuration
- **REQ-FUNC-HAL-005**: Clock Synchronization Query
- **REQ-FUNC-HAL-006**: Jitter Measurement Query
- **REQ-FUNC-HAL-007**: Signal Loss Detection
- **REQ-FUNC-HAL-008**: Buffer Management Functions

**Performance (4)**:
- **REQ-PERF-HAL-001**: Transmitter Jitter < 0.025 UI
- **REQ-PERF-HAL-002**: Receiver Jitter Tolerance
- **REQ-PERF-HAL-003**: UI Timing Accuracy ±10 ppm
- **REQ-PERF-HAL-004**: HAL Function Call Latency

**Quality (3)**:
- **REQ-QUAL-HAL-001**: Platform Portability
- **REQ-QUAL-HAL-002**: HAL Interface Stability
- **REQ-QUAL-HAL-003**: AES3-2009 Part 4 Conformity Testing

## Module Structure

```
Part4/
├── hal_interface/              # HAL interface definition
│   ├── hal.h                   # 8 HAL functions (see Common/interfaces/hal_interface.h)
│   └── hal_mock.cpp            # Mock implementation for testing
├── jitter/                     # Jitter measurement and validation
│   ├── jitter_analyzer.h       # Measure jitter (optional platform feature)
│   ├── jitter_analyzer.cpp
│   ├── jitter_validator.h      # Validate against AES3-4 limits
│   └── jitter_validator.cpp
└── conformity/                 # Part 4 conformance testing
    └── conformity_tests.cpp
```

## HAL Interface Summary

**Total Functions**: 8 (meets <10 requirement)

```cpp
// Defined in Common/interfaces/hal_interface.h

1. hal_transmit_bit()           // Biphase-mark output
2. hal_receive_bit()            // Biphase-mark input
3. hal_set_sampling_frequency() // Clock configuration
4. hal_get_clock_lock_status()  // Lock detection
5. hal_get_jitter_measurement() // Jitter monitoring (optional)
6. hal_get_signal_status()      // Signal presence detection
7. hal_allocate_buffer()        // Memory allocation
8. hal_free_buffer()            // Memory deallocation
```

## Key Constants

```cpp
// Jitter Requirements (AES3-4 Clause 6-8)
#define AES3_JITTER_TRANSMIT_MAX_UI    0.025f  // Transmit jitter < 0.025 UI
#define AES3_JITTER_RECEIVE_HF_UI      0.25f   // High-freq tolerance ≥ 0.25 UI
#define AES3_JITTER_RECEIVE_LF_UI      10.0f   // Low-freq tolerance ≥ 10 UI

// Timing Accuracy
#define AES3_TIMING_ACCURACY_PPM       10      // ±10 ppm frequency accuracy
#define AES3_TIMING_STABILITY_PPM      50      // ±50 ppm long-term stability

// HAL Latency Targets (nanoseconds)
#define HAL_TRANSMIT_LATENCY_NS        500     // hal_transmit_bit() < 500 ns
#define HAL_RECEIVE_LATENCY_MAX_UI     1       // hal_receive_bit() < 1 UI
#define HAL_CLOCK_CONFIG_MS            100     // hal_set_sampling_frequency() < 100 ms
#define HAL_CLOCK_LOCK_US              100     // hal_get_clock_lock_status() < 100 µs
#define HAL_JITTER_MEASURE_MS          10      // hal_get_jitter_measurement() < 10 ms
#define HAL_SIGNAL_STATUS_US           10      // hal_get_signal_status() < 10 µs

// UI Calculation Macros
#define UI_DURATION_NS(fs)  (1000000000 / (2 * (fs)))  // UI in nanoseconds
// Example: UI_DURATION_NS(48000) = 10417 ns @ 48 kHz
```

## Example Usage (Mock HAL)

```cpp
#include "Common/interfaces/hal_interface.h"
#include "AES/AES3/2009/Part4/jitter/jitter_validator.h"

// Configure sampling frequency
hal_set_sampling_frequency(48000);  // 48 kHz

// Transmit biphase-mark bit
uint32_t ui_ns = UI_DURATION_NS(48000);  // 10417 ns
hal_transmit_bit(true, ui_ns);

// Check clock lock status
bool is_locked;
hal_get_clock_lock_status(&is_locked);
if (!is_locked) {
    // Handle clock loss
}

// Validate jitter (if supported)
float jitter_ui;
if (hal_get_jitter_measurement(&jitter_ui, true) == HAL_SUCCESS) {
    AES::AES3::Part4::_2009::jitter::JitterValidator validator;
    bool compliant = validator.validate_transmit_jitter(jitter_ui);
    // jitter_ui must be < 0.025 UI for AES3-4 compliance
}
```

## Platform Integration

Platforms implement HAL in platform-specific layer:

```
Platform Layer (out of scope for Standards)
├── Windows/
│   └── hal_windows.cpp         // ASIO, WASAPI, or DirectSound
├── Linux/
│   └── hal_linux.cpp           // ALSA or JACK
├── macOS/
│   └── hal_macos.cpp           // CoreAudio
└── RTOS/
    └── hal_rtos.cpp            // Platform-specific audio driver
```

Standards layer links against chosen platform HAL implementation.

## Dependencies

- **Part 1 Integration**: `hal_set_sampling_frequency()` for audio clock
- **Part 2 Integration**: `hal_get_clock_lock_status()` for channel status
- **Part 3 Integration**: `hal_transmit_bit()`, `hal_receive_bit()` for biphase-mark I/O

## Testing

Unit tests: `tests/unit/part4/`
- Mock HAL implementation for Standards layer testing
- Jitter validation against AES3-4 requirements
- HAL function latency measurement
- Platform portability verification

**Platform-specific tests**: Not in Standards layer (platform responsibility)

## Standards References

- **[AES3-4]** AES3-4-2009 (R2014) Digital Audio Interface Part 4: Physical and Electrical
- **[AES3-1]** Part 1: Audio Content (sampling frequency integration)
- **[AES3-2]** Part 2: Metadata (lock indication integration)
- **[AES3-3]** Part 3: Transport (biphase-mark I/O integration)

---

**Status**: Structure created (Phase 02, November 2025)  
**Next**: Implement jitter analyzer and mock HAL for testing
