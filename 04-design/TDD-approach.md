---
id: TDD-APPROACH-001
title: Test-Driven Development Approach for AES3-2009
type: methodology
standard: XP Practices, IEEE 1012-2016
version: 1.0.0
status: approved
date: 2025-11-06
author: Design Team
applies_to:
  - Phase 05 (Implementation)
  - All Standards Layer components
  - All API Layer components
trace_to:
  - ADR-002  # Programming Language (TDD support)
  - docs/xp-practices.md  # XP core practices
---

# Test-Driven Development Approach for AES3-2009 Implementation

## 1. Introduction

### 1.1 Purpose

This document defines the **Test-Driven Development (TDD)** methodology for Phase 05 implementation of the AES3-2009 Digital Audio Interface project. TDD is a core XP (Extreme Programming) practice that ensures high code quality, comprehensive test coverage, and working software delivered incrementally.

### 1.2 Scope

**Applies to**:
- ✅ All Standards Layer components (C++17)
- ✅ All API Layer components (C++17)
- ✅ Component integration tests
- ✅ Performance tests and benchmarks

**Does NOT apply to**:
- ❌ Platform Bridge implementations (platform-specific testing)
- ❌ Documentation-only changes
- ❌ Configuration files

### 1.3 Standards Compliance

- **IEEE 1012-2016**: Software Verification and Validation
- **XP Practices**: Test-Driven Development, Continuous Integration
- **ISO/IEC/IEEE 12207:2017**: Software Implementation Process

## 2. TDD Fundamentals

### 2.1 Red-Green-Refactor Cycle

TDD follows a strict three-phase cycle:

```
┌─────────────────────────────────────────────────────────┐
│                   TDD CYCLE                             │
│                                                         │
│  ┌──────────┐      ┌──────────┐      ┌──────────┐    │
│  │   RED    │─────▶│  GREEN   │─────▶│ REFACTOR │    │
│  │          │      │          │      │          │    │
│  │ Write    │      │ Make it  │      │ Clean up │    │
│  │ failing  │      │ pass     │      │ Keep     │    │
│  │ test     │      │          │      │ tests    │    │
│  │          │      │          │      │ green    │    │
│  └──────────┘      └──────────┘      └──────────┘    │
│       │                                      │         │
│       └──────────────────────────────────────┘         │
│                    Repeat                              │
└─────────────────────────────────────────────────────────┘
```

#### Phase 1: RED (Write Failing Test)

**Objective**: Write a test that fails because the functionality doesn't exist yet.

**Activities**:
1. Identify next small piece of functionality to implement
2. Write test that exercises that functionality
3. Run test - verify it FAILS for the right reason
4. Commit failing test to version control (optional, with `[WIP]` tag)

**Example** (Subframe Builder):

```cpp
TEST_F(SubframeBuilderTest, BuildSubframe_ValidAudioData_AssemblesCorrectly) {
    // ARRANGE
    uint32_t audio_data = 0x00123456;
    Preamble preamble = Preamble::X;
    SubframeData subframe_out;
    
    // ACT
    int result = builder_->build_subframe(
        audio_data, preamble, 0, 0, 0, subframe_out
    );
    
    // ASSERT
    EXPECT_EQ(result, 0);  // ❌ FAILS - build_subframe not implemented yet
    EXPECT_EQ(subframe_out.get_bit(0), 1);  // ❌ FAILS - preamble not set
}
```

**Exit Criteria**:
- ✅ Test compiles (interface exists)
- ✅ Test fails with expected error message
- ✅ Test failure is clear and informative

#### Phase 2: GREEN (Make Test Pass)

**Objective**: Write minimal code to make the test pass.

**Activities**:
1. Implement ONLY enough code to pass the failing test
2. Take shortcuts if needed (refactor later)
3. Run test - verify it PASSES
4. Run ALL tests - verify no regressions

**Example** (Subframe Builder):

```cpp
// Minimal implementation to make test pass
int SubframeBuilder::build_subframe(
    uint32_t audio_data,
    Preamble preamble,
    uint8_t validity,
    uint8_t user_bit,
    uint8_t channel_status,
    SubframeData& subframe_out
) {
    subframe_out.clear();
    
    // Minimal preamble assembly
    if (preamble == Preamble::X) {
        subframe_out.set_bit(0, 1);  // Just enough to pass test
    }
    
    return 0;  // Success
}
```

**Exit Criteria**:
- ✅ New test passes
- ✅ All existing tests still pass
- ✅ Code compiles without warnings

#### Phase 3: REFACTOR (Clean Up)

**Objective**: Improve code quality while keeping tests green.

**Activities**:
1. Remove duplication
2. Improve naming and clarity
3. Apply design patterns where appropriate
4. Optimize performance (if needed)
5. Run ALL tests after each refactor

**Example** (Subframe Builder):

```cpp
// Refactored implementation with extracted method
int SubframeBuilder::build_subframe(/*...*/) {
    subframe_out.clear();
    assemble_preamble(preamble, subframe_out);  // Extracted method
    return 0;
}

void SubframeBuilder::assemble_preamble(
    Preamble preamble,
    SubframeData& subframe
) noexcept {
    // Clean implementation with all preamble types
    switch (preamble) {
        case Preamble::X:
            subframe.set_bit(0, 1);
            subframe.set_bit(1, 1);
            subframe.set_bit(2, 1);
            subframe.set_bit(3, 0);
            break;
        // ... other preambles
    }
}
```

**Exit Criteria**:
- ✅ All tests still pass
- ✅ Code is cleaner than before refactoring
- ✅ No duplication remains
- ✅ Cyclomatic complexity ≤10

### 2.2 TDD Rules (Uncle Bob's Three Laws)

**Law 1**: You are not allowed to write any production code unless it is to make a failing unit test pass.

**Law 2**: You are not allowed to write any more of a unit test than is sufficient to fail; and compilation failures are failures.

**Law 3**: You are not allowed to write any more production code than is sufficient to pass the one failing unit test.

**Practical Application**:
- Write tests FIRST, always
- Keep test-code cycles SHORT (minutes, not hours)
- Commit frequently with passing tests

## 3. Test Structure and Organization

### 3.1 Test File Structure

**Directory Structure**:

```
05-implementation/
├── src/
│   └── AES/
│       └── AES3/
│           ├── Part1/
│           │   └── _2009/
│           │       └── audio_coding/
│           │           └── pcm_encoder.cpp        # Implementation
│           └── Part3/
│               └── _2009/
│                   └── subframe/
│                       └── subframe_builder.cpp   # Implementation
└── tests/
    └── AES/
        └── AES3/
            ├── Part1/
            │   └── _2009/
            │       └── audio_coding/
            │           └── test_pcm_encoder.cpp   # Tests
            └── Part3/
                └── _2009/
                    └── subframe/
                        └── test_subframe_builder.cpp  # Tests
```

**Naming Convention**:
- Test files: `test_<component_name>.cpp`
- Test fixtures: `<ComponentName>TestFixture`
- Test cases: `TEST_F(<Fixture>, <Method>_<Scenario>_<ExpectedBehavior>)`

### 3.2 Test Fixture Pattern

**Purpose**: Reduce test setup duplication and provide consistent test environment.

**Template**:

```cpp
namespace AES::AES3::<Part>::_2009::<namespace>::test {

class <ComponentName>TestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for all tests
        config_ = ComponentConfig::Default();
        component_ = std::make_unique<Component>(config_);
    }
    
    void TearDown() override {
        // Common cleanup
        component_.reset();
    }
    
    // Helper functions for test assertions
    void VerifyCommonCondition(const Result& result) {
        ASSERT_EQ(result.error_code, 0);
        ASSERT_TRUE(result.is_valid());
    }
    
    // Test data generators
    InputData CreateValidInput() {
        return InputData{/* valid test data */};
    }
    
    // Shared test resources
    ComponentConfig config_;
    std::unique_ptr<Component> component_;
};

} // namespace test
```

**Example** (PCM Encoder):

```cpp
namespace AES::AES3::Part1::_2009::audio_coding::test {

class PCMEncoderTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = PCMEncoder::Config::Default();
        encoder_ = std::make_unique<PCMEncoder>(config_);
    }
    
    void TearDown() override {
        encoder_.reset();
    }
    
    // Helper: Verify MSB justification
    void VerifyMSBJustification(
        const PCMEncoder::EncodedSample& sample,
        PCMEncoder::WordLength word_length
    ) {
        uint32_t shift = 24 - static_cast<uint32_t>(word_length);
        uint32_t lsb_mask = (1u << shift) - 1;
        ASSERT_EQ(sample.audio_data & lsb_mask, 
                  static_cast<uint32_t>(sample.auxiliary_bits));
    }
    
    // Helper: Create test PCM sample
    int32_t CreateTestSample(int16_t value) {
        return static_cast<int32_t>(value);
    }
    
    PCMEncoder::Config config_;
    std::unique_ptr<PCMEncoder> encoder_;
};

} // namespace test
```

### 3.3 Test Naming Convention

**Format**: `TEST_F(Fixture, Method_Scenario_ExpectedBehavior)`

**Examples**:

```cpp
// Good test names - clear intent
TEST_F(SubframeBuilderTest, BuildSubframe_ValidAudio_ReturnsSuccess)
TEST_F(SubframeBuilderTest, BuildSubframe_NullOutput_ReturnsError)
TEST_F(SubframeBuilderTest, BuildSubframe_PreambleX_SetsCorrectBits)
TEST_F(PCMEncoderTest, EncodeSample16bit_PositiveValue_MSBJustified)
TEST_F(PCMEncoderTest, EncodeSample_NegativeValue_TwosComplementCorrect)
TEST_F(PCMEncoderTest, EncodeSample_OutOfRange_ValidityBitSet)

// Bad test names - unclear intent
TEST_F(SubframeBuilderTest, Test1)           // ❌ No information
TEST_F(SubframeBuilderTest, BuildTest)       // ❌ Too vague
TEST_F(SubframeBuilderTest, TestSubframe)    // ❌ What aspect?
```

## 4. Test Categories and Coverage

### 4.1 Unit Tests (Primary Focus)

**Definition**: Test individual components in isolation with mocked dependencies.

**Characteristics**:
- Fast execution (<1ms per test)
- No external dependencies (use mocks)
- 100% deterministic
- High coverage (>95% line coverage)

**Example** (Subframe Builder Unit Test):

```cpp
TEST_F(SubframeBuilderTest, BuildSubframe_AllPreambles_CorrectPatterns) {
    SubframeData subframe_x, subframe_y, subframe_z;
    uint32_t audio_data = 0x123456;
    
    // Test Preamble X
    builder_->build_subframe(audio_data, Preamble::X, 0, 0, 0, subframe_x);
    EXPECT_EQ(subframe_x.get_bit(0), 1);  // Preamble X pattern: 11100010
    EXPECT_EQ(subframe_x.get_bit(1), 1);
    EXPECT_EQ(subframe_x.get_bit(2), 1);
    EXPECT_EQ(subframe_x.get_bit(3), 0);
    
    // Test Preamble Y
    builder_->build_subframe(audio_data, Preamble::Y, 0, 0, 0, subframe_y);
    EXPECT_EQ(subframe_y.get_bit(0), 1);  // Preamble Y pattern: 11100100
    EXPECT_EQ(subframe_y.get_bit(1), 1);
    EXPECT_EQ(subframe_y.get_bit(2), 1);
    EXPECT_EQ(subframe_y.get_bit(3), 0);
    
    // Test Preamble Z
    builder_->build_subframe(audio_data, Preamble::Z, 0, 0, 0, subframe_z);
    EXPECT_EQ(subframe_z.get_bit(0), 1);  // Preamble Z pattern: 11101000
    EXPECT_EQ(subframe_z.get_bit(1), 1);
    EXPECT_EQ(subframe_z.get_bit(2), 1);
    EXPECT_EQ(subframe_z.get_bit(3), 0);
}
```

### 4.2 Integration Tests

**Definition**: Test multiple components working together.

**Characteristics**:
- Medium execution time (1-10ms per test)
- Real dependencies (no mocks)
- Tests component interfaces
- Validates integration points

**Example** (PCM Encoder + Subframe Builder Integration):

```cpp
TEST(IntegrationTest, PCMEncoder_SubframeBuilder_EndToEndTransmit) {
    // ARRANGE: Create components
    PCMEncoder encoder(PCMEncoder::Config::Default());
    SubframeBuilder builder(SubframeBuilder::Config::Default());
    
    int16_t pcm_audio = 0x4000;  // 16-bit PCM sample
    
    // ACT: Encode PCM sample
    PCMEncoder::EncodedSample encoded;
    int result = encoder.encode_sample_16bit(pcm_audio, true, encoded);
    ASSERT_EQ(result, 0);
    
    // ACT: Build subframe with encoded audio
    SubframeData subframe;
    result = builder.build_subframe(
        encoded.audio_data,
        Preamble::X,
        encoded.validity,
        0, 0,
        subframe
    );
    
    // ASSERT: Verify end-to-end correctness
    ASSERT_EQ(result, 0);
    EXPECT_EQ(subframe.get_bit(SubframeData::VALIDITY_SLOT), encoded.validity);
    EXPECT_EQ(encoded.audio_data, 0x400000u);  // MSB-justified 16-bit
}
```

### 4.3 Performance Tests

**Definition**: Verify performance budgets and timing requirements.

**Characteristics**:
- Execution time varies (benchmark-dependent)
- May require real hardware or simulators
- Reports metrics (latency, throughput, jitter)
- Validates real-time constraints

**Example** (PCM Encoder Performance Test):

```cpp
TEST_F(PCMEncoderTest, EncodeSample_Performance_MeetsBudget) {
    constexpr int ITERATIONS = 100000;
    int32_t pcm_sample = 0x123456;
    PCMEncoder::EncodedSample encoded;
    
    // Warmup
    for (int i = 0; i < 1000; i++) {
        encoder_->encode_sample(pcm_sample, true, encoded);
    }
    
    // Measure
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        encoder_->encode_sample(pcm_sample, true, encoded);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculate average latency
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end - start
    ).count();
    double avg_latency_ns = static_cast<double>(duration_ns) / ITERATIONS;
    
    // ASSERT: Meet performance budget (<100ns per sample)
    EXPECT_LT(avg_latency_ns, 100.0) 
        << "Performance budget exceeded: " << avg_latency_ns << "ns";
    
    // Report metrics
    std::cout << "PCM Encoder Performance:\n"
              << "  Average latency: " << avg_latency_ns << " ns\n"
              << "  Throughput: " << (1e9 / avg_latency_ns) << " samples/sec\n";
}
```

### 4.4 Property-Based Tests (Optional)

**Definition**: Test invariants across wide input ranges using generated test data.

**Example** (Subframe Parity Property):

```cpp
TEST_F(SubframeBuilderTest, BuildSubframe_AllInputs_EvenParityMaintained) {
    // Property: Even parity must hold for ALL valid inputs
    std::mt19937 rng(12345);  // Fixed seed for reproducibility
    std::uniform_int_distribution<uint32_t> audio_dist(0, 0x00FFFFFF);
    
    for (int trial = 0; trial < 10000; trial++) {
        // Generate random audio data
        uint32_t audio_data = audio_dist(rng);
        Preamble preamble = static_cast<Preamble>(rng() % 3);
        
        SubframeData subframe;
        builder_->build_subframe(audio_data, preamble, 0, 0, 0, subframe);
        
        // Verify property: even parity across time slots 4-31
        uint8_t parity = 0;
        for (size_t slot = 4; slot <= 31; slot++) {
            parity ^= subframe.get_bit(slot);
        }
        
        ASSERT_EQ(parity, 0) << "Even parity violated for audio_data=0x" 
                             << std::hex << audio_data;
    }
}
```

### 4.5 Coverage Requirements

| Component Type | Line Coverage | Branch Coverage | Function Coverage |
|----------------|---------------|-----------------|-------------------|
| **Standards Layer** | ≥95% | ≥90% | 100% |
| **API Layer** | ≥90% | ≥85% | 100% |
| **Integration** | ≥80% | ≥75% | N/A |
| **Overall Target** | ≥90% | ≥85% | 100% |

**Measurement Tools**:
- `gcov` (GCC coverage tool)
- `lcov` (Coverage visualization)
- `gcovr` (Coverage reports)

**Coverage Enforcement**:
```bash
# CMake configuration for coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
make coverage
# Fail CI if coverage < 90%
```

## 5. Mock Objects and Test Doubles

### 5.1 When to Use Mocks

**Use mocks for**:
- ✅ HAL interfaces (platform dependencies)
- ✅ Slow external dependencies
- ✅ Non-deterministic behavior (time, random)
- ✅ Testing error paths

**Do NOT mock**:
- ❌ Simple data structures
- ❌ Internal implementation details
- ❌ Pure functions with no side effects

### 5.2 Mock HAL Interface

**Purpose**: Enable unit testing of Standards Layer without hardware.

**Implementation**:

```cpp
namespace AES::AES3::test {

class MockAudioHAL {
public:
    // Mock state tracking
    struct CallLog {
        int init_calls = 0;
        int send_frames_calls = 0;
        int receive_frames_calls = 0;
        std::vector<std::vector<uint8_t>> sent_frames;
    };
    
    CallLog call_log;
    
    // Mock functions matching audio_hal_interface_t
    static audio_hal_error_t mock_init(const audio_hal_config_t* config) {
        instance_->call_log.init_calls++;
        instance_->config_ = *config;
        return AUDIO_HAL_SUCCESS;
    }
    
    static audio_hal_error_t mock_send_frames(
        const void* frame_data,
        uint32_t num_frames,
        uint32_t* frames_sent
    ) {
        instance_->call_log.send_frames_calls++;
        
        // Record sent frames for verification
        const uint8_t* data = static_cast<const uint8_t*>(frame_data);
        std::vector<uint8_t> frame_copy(data, data + num_frames * 8);
        instance_->call_log.sent_frames.push_back(frame_copy);
        
        *frames_sent = num_frames;
        return AUDIO_HAL_SUCCESS;
    }
    
    // ... other mock functions ...
    
    // Helper: Get interface structure
    static audio_hal_interface_t GetInterface() {
        return audio_hal_interface_t{
            .init = mock_init,
            .send_frames = mock_send_frames,
            .receive_frames = mock_receive_frames,
            .set_sample_rate = mock_set_sample_rate,
            .get_sample_clock = mock_get_sample_clock,
            .set_callbacks = mock_set_callbacks,
            .start = mock_start,
            .stop = mock_stop,
            .get_status = mock_get_status,
            .cleanup = mock_cleanup
        };
    }
    
    // Helper: Reset mock state
    void Reset() {
        call_log = CallLog{};
    }
    
private:
    static MockAudioHAL* instance_;
    audio_hal_config_t config_;
};

// Usage in tests
TEST(TransmitTest, FrameTransmitter_SendFrame_CallsHAL) {
    MockAudioHAL mock_hal;
    auto hal_interface = MockAudioHAL::GetInterface();
    
    // Inject mock HAL
    FrameTransmitter transmitter(&hal_interface);
    
    // ACT: Transmit frame
    SubframeData subframe = CreateTestSubframe();
    transmitter.transmit_subframes(&subframe, 1);
    
    // ASSERT: Verify HAL was called
    EXPECT_EQ(mock_hal.call_log.send_frames_calls, 1);
    ASSERT_EQ(mock_hal.call_log.sent_frames.size(), 1);
}

} // namespace test
```

## 6. Test Data Management

### 6.1 Test Data Organization

**Principles**:
- Keep test data CLOSE to tests (in test files)
- Use helper functions to generate test data
- Document WHY specific test values are chosen

**Example** (Test Data Helpers):

```cpp
namespace AES::AES3::Part1::_2009::audio_coding::test {

class TestDataGenerator {
public:
    // Generate full-scale positive sample
    static int32_t MaxPositiveSample(PCMEncoder::WordLength word_length) {
        uint32_t bits = static_cast<uint32_t>(word_length);
        return (1 << (bits - 1)) - 1;
    }
    
    // Generate full-scale negative sample
    static int32_t MaxNegativeSample(PCMEncoder::WordLength word_length) {
        uint32_t bits = static_cast<uint32_t>(word_length);
        return -(1 << (bits - 1));
    }
    
    // Generate digital silence
    static int32_t Silence() {
        return 0;
    }
    
    // Generate sine wave sample (for integration tests)
    static int32_t SineWaveSample(double frequency_hz, double sample_rate, int sample_index) {
        double phase = 2.0 * M_PI * frequency_hz * sample_index / sample_rate;
        return static_cast<int32_t>(0x7FFFFF * std::sin(phase));  // 24-bit amplitude
    }
};

// Usage in tests
TEST_F(PCMEncoderTest, EncodeSample_MaxPositive_NoClipping) {
    int32_t max_positive = TestDataGenerator::MaxPositiveSample(
        PCMEncoder::WordLength::BITS_24
    );
    
    PCMEncoder::EncodedSample encoded;
    encoder_->encode_sample(max_positive, true, encoded);
    
    EXPECT_EQ(encoded.validity, 0);  // Should be valid (no clipping)
}

} // namespace test
```

### 6.2 AES3 Test Vectors

**Source**: Use AES3-2009 specification test vectors where available.

**Custom Test Vectors**:

```cpp
namespace AES::AES3::test_vectors {

// Known-good subframe for validation
constexpr uint64_t REFERENCE_SUBFRAME_X = 0x/* hex value from spec */;

// Known-good channel status block
constexpr uint8_t REFERENCE_CHANNEL_STATUS[24] = {
    0x00, 0x00, 0x00, /* ... */ 0x00  // From AES3-2009 Part 2 examples
};

// Test audio samples with known characteristics
struct TestAudioSample {
    int32_t pcm_value;
    uint32_t expected_encoded;
    const char* description;
};

constexpr TestAudioSample AUDIO_TEST_VECTORS[] = {
    {0x000000, 0x000000, "Digital silence"},
    {0x7FFFFF, 0x7FFFFF, "Maximum positive (24-bit)"},
    {0x800000, 0x800000, "Maximum negative (24-bit)"},
    {0x400000, 0x400000, "50% positive full scale"},
    {0xC00000, 0xC00000, "50% negative full scale"}
};

} // namespace test_vectors
```

## 7. Continuous Integration and Testing

### 7.1 CI Pipeline Integration

**Pre-commit Checks** (local):
```bash
#!/bin/bash
# .git/hooks/pre-commit

# Run fast unit tests
echo "Running unit tests..."
cd 05-implementation/build
ctest --output-on-failure -R "^unit_"

if [ $? -ne 0 ]; then
    echo "❌ Unit tests failed. Commit aborted."
    exit 1
fi

# Check code coverage
echo "Checking code coverage..."
make coverage_check
if [ $? -ne 0 ]; then
    echo "⚠️  Code coverage below threshold"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "✅ Pre-commit checks passed"
```

**CI Pipeline** (GitHub Actions / Azure DevOps):

```yaml
# .github/workflows/ci.yml
name: CI - Test and Build

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install Dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake g++ googletest lcov
      
      - name: Configure CMake
        run: |
          cd 05-implementation
          mkdir -p build && cd build
          cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
      
      - name: Build
        run: |
          cd 05-implementation/build
          make -j$(nproc)
      
      - name: Run Unit Tests
        run: |
          cd 05-implementation/build
          ctest --output-on-failure -R "^unit_"
      
      - name: Run Integration Tests
        run: |
          cd 05-implementation/build
          ctest --output-on-failure -R "^integration_"
      
      - name: Generate Coverage Report
        run: |
          cd 05-implementation/build
          make coverage
      
      - name: Check Coverage Threshold
        run: |
          cd 05-implementation/build
          lcov --summary coverage.info | grep "lines" | awk '{print $2}' | sed 's/%//' | \
          awk '{if ($1 < 90) exit 1}'
      
      - name: Upload Coverage to Codecov
        uses: codecov/codecov-action@v2
        with:
          files: 05-implementation/build/coverage.info
          fail_ci_if_error: true
```

### 7.2 Test Execution Strategy

**Development Workflow**:
1. **Local development**: Run relevant unit tests continuously (watch mode)
2. **Pre-commit**: Run all unit tests (<30 seconds)
3. **CI/CD**: Run full test suite (unit + integration + performance)

**Test Categorization**:
```cmake
# CMakeLists.txt - Test categories

# Unit tests - fast, no dependencies
add_test(NAME unit_pcm_encoder COMMAND test_pcm_encoder)
set_tests_properties(unit_pcm_encoder PROPERTIES LABELS "unit")

# Integration tests - medium speed
add_test(NAME integration_transmit_path COMMAND test_transmit_path)
set_tests_properties(integration_transmit_path PROPERTIES LABELS "integration")

# Performance tests - slow
add_test(NAME perf_subframe_builder COMMAND bench_subframe_builder)
set_tests_properties(perf_subframe_builder PROPERTIES LABELS "performance")

# Run only unit tests:
# ctest -L unit

# Run only integration tests:
# ctest -L integration
```

## 8. Implementation Workflow Example

### 8.1 Implementing Subframe Builder (TDD Step-by-Step)

**Step 1: Write First Test (RED)**

```cpp
// test_subframe_builder.cpp
TEST_F(SubframeBuilderTest, BuildSubframe_ClearOutput_ReturnsSuccess) {
    SubframeData subframe;
    
    int result = builder_->build_subframe(
        0, Preamble::X, 0, 0, 0, subframe
    );
    
    EXPECT_EQ(result, 0);  // ❌ FAILS - method doesn't exist
}
```

**Compile**: ❌ Compilation error - `build_subframe` not declared

**Step 2: Create Interface (Still RED)**

```cpp
// subframe_builder.hpp
class SubframeBuilder {
public:
    int build_subframe(
        uint32_t audio_data,
        Preamble preamble,
        uint8_t validity,
        uint8_t user_bit,
        uint8_t channel_status,
        SubframeData& subframe_out
    ) noexcept;
};
```

**Compile**: ✅ Compiles  
**Test**: ❌ Linker error - method not implemented

**Step 3: Minimal Implementation (GREEN)**

```cpp
// subframe_builder.cpp
int SubframeBuilder::build_subframe(
    uint32_t audio_data,
    Preamble preamble,
    uint8_t validity,
    uint8_t user_bit,
    uint8_t channel_status,
    SubframeData& subframe_out
) noexcept {
    return 0;  // Minimal: just return success
}
```

**Test**: ✅ PASSES

**Step 4: Add Next Test (RED)**

```cpp
TEST_F(SubframeBuilderTest, BuildSubframe_PreambleX_FirstBitSet) {
    SubframeData subframe;
    builder_->build_subframe(0, Preamble::X, 0, 0, 0, subframe);
    
    EXPECT_EQ(subframe.get_bit(0), 1);  // ❌ FAILS - bit not set
}
```

**Step 5: Implement Preamble (GREEN)**

```cpp
int SubframeBuilder::build_subframe(/*...*/) {
    subframe_out.clear();
    
    // Set preamble X first bit
    if (preamble == Preamble::X) {
        subframe_out.set_bit(0, 1);
    }
    
    return 0;
}
```

**Test**: ✅ PASSES

**Step 6: Refactor (REFACTOR)**

```cpp
// Extract method
int SubframeBuilder::build_subframe(/*...*/) {
    subframe_out.clear();
    assemble_preamble(preamble, subframe_out);  // Extracted
    return 0;
}

void SubframeBuilder::assemble_preamble(
    Preamble preamble,
    SubframeData& subframe
) noexcept {
    // Implement all preambles cleanly
    switch (preamble) {
        case Preamble::X:
            subframe.set_bit(0, 1);
            break;
        // ... handle Y, Z
    }
}
```

**Test**: ✅ Still PASSES  
**Coverage**: Improved structure

**Repeat**: Continue adding tests for audio data, parity, etc.

## 9. Best Practices and Anti-Patterns

### 9.1 TDD Best Practices ✅

1. **Write smallest possible test** - test one thing at a time
2. **Test behavior, not implementation** - don't test private methods
3. **Make tests fast** - unit tests should be <1ms each
4. **Keep tests independent** - no shared state between tests
5. **Use descriptive test names** - `Method_Scenario_Expected`
6. **Commit with green tests** - never commit failing tests (except WIP)
7. **Refactor mercilessly** - clean code after green
8. **Test error paths** - don't just test happy path

### 9.2 TDD Anti-Patterns ❌

1. **❌ Writing tests after code** - defeats TDD purpose
2. **❌ Testing implementation details** - brittle tests
3. **❌ Slow tests** - discourages running tests frequently
4. **❌ Flaky tests** - non-deterministic failures
5. **❌ Testing private methods** - test public interface only
6. **❌ Over-mocking** - mocking everything makes tests brittle
7. **❌ Large test fixtures** - keep setup simple
8. **❌ Ignoring failing tests** - fix or delete, never ignore

## 10. Phase 05 Implementation Checklist

### 10.1 Before Starting Implementation

- [ ] Read and understand this TDD approach document
- [ ] Set up test framework (Google Test + CMake)
- [ ] Configure code coverage tools (lcov/gcov)
- [ ] Create test directory structure
- [ ] Set up CI pipeline with test automation
- [ ] Install pre-commit hooks

### 10.2 During Implementation (Per Component)

- [ ] Write failing test first (RED)
- [ ] Verify test fails for correct reason
- [ ] Write minimal code to pass test (GREEN)
- [ ] Verify all tests pass
- [ ] Refactor code (REFACTOR)
- [ ] Verify tests still pass after refactoring
- [ ] Check code coverage (target >95%)
- [ ] Commit with passing tests
- [ ] Repeat for next functionality

### 10.3 After Component Completion

- [ ] All unit tests passing
- [ ] Code coverage ≥95%
- [ ] Integration tests written and passing
- [ ] Performance tests run and meet budgets
- [ ] Code review completed
- [ ] Documentation updated
- [ ] CI pipeline green

## 11. Metrics and Reporting

### 11.1 Test Metrics Dashboard

**Track Weekly**:
- Total test count
- Test pass rate (target: 100%)
- Code coverage percentage (target: ≥90%)
- Average test execution time
- CI pipeline success rate

**Example Report**:

```
AES3-2009 Test Metrics - Week 45, 2025
========================================
Unit Tests:              247 (100% pass)
Integration Tests:        32 (100% pass)
Performance Tests:        12 (100% pass)
----------------------------------------
Total Tests:             291 (100% pass)
Code Coverage:         94.2% (target: ≥90%)
Average Test Time:      0.8ms per test
CI Success Rate:       98.5% (67/68 builds)
----------------------------------------
Status: ✅ ALL METRICS WITHIN TARGET
```

## 12. Conclusion

Test-Driven Development is **mandatory** for Phase 05 implementation of the AES3-2009 project. By following the Red-Green-Refactor cycle and maintaining >90% code coverage, we ensure:

- ✅ High code quality
- ✅ Comprehensive test coverage
- ✅ Working software at all times
- ✅ Regression prevention
- ✅ Confidence in refactoring
- ✅ Living documentation (tests as specs)

**Remember**: *Tests are not optional. They are the implementation specification.*

---

**Standards Compliance**:
- IEEE 1012-2016: Software Verification and Validation
- XP Practices: Test-Driven Development
- ISO/IEC/IEEE 12207:2017: Software Implementation Process

**Status**: Approved for Phase 05  
**Effective Date**: 2025-11-06
