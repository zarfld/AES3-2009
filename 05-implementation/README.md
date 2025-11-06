# Phase 05: Implementation - AES3-2009 Digital Audio Interface

## Overview

This directory contains the **Test-Driven Development (TDD) implementation** of AES3-2009 standards following ISO/IEC/IEEE 12207:2017 Implementation Process and Extreme Programming practices.

## Directory Structure

```
05-implementation/
├── CMakeLists.txt                 # Build system configuration
├── src/                           # Production source code
│   └── AES/                       # Audio Engineering Society namespace
│       └── AES3/                  # AES3 Digital Audio Interface
│           ├── Part1/             # Audio Content (PCM encoding)
│           │   └── _2009/
│           │       └── audio_coding/
│           │           └── pcm_encoder.cpp
│           └── Part3/             # Transport (Subframes, Frames, Blocks)
│               └── _2009/
│                   └── subframe/
│                       └── subframe_builder.cpp
├── include/                       # Public C interfaces
│   └── aes3/
│       └── audio_hal_interface.h  # Pure C HAL interface (10 functions)
├── tests/                         # Unit and integration tests (Google Test)
│   ├── AES/                       # Namespace-aligned test structure
│   │   └── AES3/
│   │       ├── Part1/_2009/audio_coding/
│   │       │   └── test_pcm_encoder.cpp
│   │       └── Part3/_2009/subframe/
│   │           ├── test_subframe_data.cpp
│   │           └── test_subframe_builder.cpp
│   └── integration/
│       └── test_transmit_path.cpp # End-to-end integration tests
├── mocks/                         # Mock implementations for testing
│   └── mock_audio_hal.cpp         # Mock Audio HAL with call verification
└── benchmarks/                    # Performance benchmarks
    ├── bench_pcm_encoder.cpp      # <100ns per sample requirement
    └── bench_subframe_builder.cpp # <5µs per subframe requirement
```

## Build System (CMake 3.20+)

### Quick Start

```powershell
# Configure build with testing and coverage
cmake -B build -S 05-implementation -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON

# Build all targets
cmake --build build --config Release

# Run tests
cd build
ctest --output-on-failure

# Generate coverage report (requires lcov on Linux/macOS)
cmake --build . --target coverage
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_TESTING` | ON | Build Google Test unit tests |
| `ENABLE_COVERAGE` | OFF | Enable code coverage analysis (lcov/gcov) |
| `ENABLE_SANITIZERS` | OFF | Enable AddressSanitizer and UBSanitizer |
| `ENABLE_BENCHMARKS` | ON | Build performance benchmarks |
| `BUILD_SHARED_LIBS` | OFF | Build shared libraries (default: static) |

### Targets

- **Libraries**:
  - `aes3_part1_audio_coding` - PCM Encoder (Part 1: Audio Content)
  - `aes3_part3_transport` - Subframe Builder (Part 3: Transport)
  - `aes3_mocks` - Mock implementations for testing

- **Tests**:
  - `test_subframe_data` - SubframeData structure tests
  - `test_pcm_encoder` - PCM Encoder unit tests
  - `test_subframe_builder` - Subframe Builder unit tests
  - `test_transmit_path_integration` - End-to-end integration tests

- **Benchmarks**:
  - `bench_pcm_encoder` - PCM encoding performance (<100ns target)
  - `bench_subframe_builder` - Subframe building performance (<5µs target)

- **Coverage**:
  - `coverage` - Generate HTML coverage report
  - `coverage_check` - Enforce ≥90% coverage threshold

## Test-Driven Development Workflow

Following **Red-Green-Refactor** cycle (see `04-design/TDD-approach.md`):

### Step 1: RED - Write Failing Test
```cpp
TEST_F(PCMEncoderTest, Encode16BitSample_ValidInput_ProducesCorrectEncoding) {
    // Arrange
    int16_t input_sample = 0x1234;
    
    // Act
    auto encoded = encoder.encode_sample(input_sample);
    
    // Assert
    EXPECT_EQ(encoded.audio_sample_word, 0x123400); // MSB-justified to 24-bit
}
```

### Step 2: GREEN - Minimal Implementation
```cpp
EncodedSample PCMEncoder::encode_sample(int16_t sample) {
    EncodedSample result;
    result.audio_sample_word = static_cast<uint32_t>(sample) << 8; // Left-shift by 8
    return result;
}
```

### Step 3: REFACTOR - Improve Design
```cpp
EncodedSample PCMEncoder::encode_sample(int16_t sample) {
    constexpr uint8_t SHIFT_16_TO_24 = 8; // MSB justification
    EncodedSample result;
    result.audio_sample_word = justify_to_msb(sample, SHIFT_16_TO_24);
    result.validity_bit = calculate_validity(sample);
    return result;
}
```

### Running Tests

```powershell
# Run all tests
ctest --output-on-failure

# Run specific test suite
ctest -R test_pcm_encoder --verbose

# Run with labels
ctest -L unit           # All unit tests
ctest -L integration    # Integration tests only
ctest -L part1          # Part 1 tests only
```

## Implementation Order (Bottom-Up Dependencies)

### Layer 1: Leaf Components (No Dependencies)
1. ✅ **SubframeData** - 64-bit data structure with bit manipulation
2. ✅ **PCM Encoder** - 2's complement encoding, MSB justification
3. ✅ **Audio HAL Interface Header** - Pure C function declarations

### Layer 2: Dependent Components
4. ⏳ **Subframe Builder** - Uses SubframeData, depends on PCM Encoder
5. ⏳ **Mock Audio HAL** - Implements Audio HAL interface for testing

### Layer 3: Integration
6. ⏳ **Transmit Path Integration** - End-to-end PCM → Subframe → Mock HAL

## Quality Gates (CI/CD Enforcement)

All commits must pass:

1. **Compilation**: Zero errors, zero warnings (`-Werror`)
2. **All Tests Pass**: 100% test success rate
3. **Code Coverage**: ≥90% line coverage (≥95% for Standards Layer)
4. **Static Analysis**: clang-tidy checks pass
5. **Performance**: Benchmarks meet requirements (<100ns PCM, <5µs Subframe)
6. **Cyclomatic Complexity**: ≤10 per function

## Traceability

Every implementation unit maintains traceability:

```cpp
/**
 * @file pcm_encoder.cpp
 * @implements DES-C-003
 * @traceability REQ-FUNC-001, REQ-FUNC-002, REQ-FUNC-003, REQ-FUNC-004
 * @tests TEST-PCM-001 through TEST-PCM-015
 */
```

Traceability chain:
```
REQ-FUNC-001 (Requirements) 
  → DES-C-003 (Design)
    → SRC-PCM-001 (Implementation)
      → TEST-PCM-001 (Test)
```

## Standards Compliance

### AES3-2009 References
- **Part 1: Audio Content** - Linear PCM encoding (§4.1, §4.2)
- **Part 2: Metadata and Subcode** - Channel status blocks (§5.1)
- **Part 3: Transport** - Subframe structure (§3.1-3.3), Biphase-mark coding (§3.4)
- **Part 4: Physical and Electrical** - Jitter requirements (§6.2)

### Related Standards
- **AES5-2018** - Preferred sampling frequencies (32/44.1/48/88.2/96/176.4/192 kHz)
- **IEC 60958-4** - Professional digital audio interface
- **ISO/IEC/IEEE 12207:2017** - Implementation Process (§6.4.5)

## Performance Requirements

| Component | Requirement | Budget |
|-----------|-------------|--------|
| PCM Encoder | <100ns per sample | 0.48% of frame (48kHz) |
| Subframe Builder | <5µs per subframe | 24% of frame (48kHz) |
| Total Transmit Path | <20µs per frame | 96% of frame (48kHz) |

## Memory Constraints

- **Code Size**: <64KB per component
- **RAM Usage**: <16KB total (real-time buffers)
- **Stack Depth**: <4KB per call chain

## XP Practices Applied

1. **Test-Driven Development (TDD)** - Tests written before implementation
2. **Simple Design** - YAGNI (You Aren't Gonna Need It)
3. **Continuous Integration** - Automated testing on every commit
4. **Refactoring** - Improve design while keeping tests green
5. **Pair Programming** - Encouraged for complex protocols

## Next Steps

See `04-design/TDD-approach.md` for complete TDD methodology.

Current Phase 05 task status tracked in project todo list.

---

**Phase 05 Status**: ⏳ In Progress  
**Started**: 2024  
**Target Completion**: TBD  
**Quality Standard**: ISO/IEC/IEEE 12207:2017 Implementation Process
