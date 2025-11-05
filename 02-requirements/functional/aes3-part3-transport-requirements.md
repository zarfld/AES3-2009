---
specType: requirements
standard: ISO/IEC/IEEE 29148:2018
phase: 02-requirements
version: 1.0.0
author: Requirements Elicitation Session
date: 2025-11-05
status: draft
traceability:
  stakeholderRequirements:
    - StR-FUNC-003
    - StR-PERF-001
    - StR-QUAL-001
description: "Detailed system requirements for AES3-2009 Part 3: Transport"
---

# AES3-2009 Part 3: Transport - System Requirements

**Elicitation Session**: 2025-11-05  
**Source**: AES3-3-2009 (R2014) Specification  
**Stakeholder Requirement**: StR-FUNC-003 - Complete Part 3 (Transport) implementation  
**Standards Compliance**: ISO/IEC/IEEE 29148:2018

---

## Requirements Elicitation Summary

**Source Material**: AES3-3-2009 (R2014) - Digital Audio Interface Part 3: Transport

**8-Dimension Analysis Applied**:

1. ✅ Functional Behavior - Subframe/frame/block hierarchy, preambles, biphase-mark coding
2. ✅ Boundary Values - 32 time slots, 2 UI per slot, 192 frames per block
3. ✅ Failure Handling - Preamble detection errors, synchronization loss, coding violations
4. ✅ Performance - Real-time encoding/decoding at sampling frequency
5. ✅ Security - Preamble validation, bit stuffing prevention
6. ✅ Regulatory - AES3-2009 Part 3 mandatory compliance
7. ✅ Integration - Part 1 (audio samples), Part 2 (metadata), Part 4 (physical layer)
8. ✅ Prioritization - Critical for all AES3 implementations

---

## Functional Requirements: Subframe Structure

### REQ-FUNC-TRANS-001: Subframe Time Slot Structure

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 4.1

**Description**:
The system SHALL encode each audio subframe as 32 time slots (numbered 0-31), where each time slot occupies 2 Unit Intervals (UI) with duration = 1/(2 × sampling frequency).

**Rationale**:
AES3-2009 Part 3 Clause 4.1 defines the fundamental transport structure. The 32 time slot format provides capacity for preamble (4 slots), auxiliary bits (4 slots), audio sample (24 slots), validity bit (1 slot), user data (1 slot), channel status (1 slot), and parity (1 slot).

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)

**Acceptance Criteria**:

```gherkin
Scenario: Subframe timing at 48 kHz
  Given sampling frequency is 48 kHz
  When encoding AES3 subframe
  Then total subframe duration SHALL be 64 UI
  And UI duration SHALL be 10.417 µs (1 / (2 × 48000))
  And time slot duration SHALL be 20.833 µs (2 UI)
  And complete subframe duration SHALL be 666.667 µs (32 time slots)

Scenario: Time slot numbering
  Given AES3 subframe structure
  When encoding subframe
  Then time slots SHALL be numbered 0 to 31
  And time slot 0 SHALL be first bit of preamble
  And time slot 31 SHALL be even parity bit

Scenario: Subframe timing at 192 kHz (high sampling rate)
  Given sampling frequency is 192 kHz
  When encoding AES3 subframe
  Then UI duration SHALL be 2.604 µs (1 / (2 × 192000))
  And time slot duration SHALL be 5.208 µs (2 UI)
  And complete subframe duration SHALL be 166.667 µs (32 time slots)
```

**Dependencies**:
- REQ-FUNC-AUDIO-008 (AES5 sampling frequencies)

**Constraints**:
- Time slot count fixed at 32 (not configurable)
- UI duration inversely proportional to sampling frequency

**Test Strategy**:
- Unit tests: Verify time slot structure encoding/decoding
- Timing tests: Measure subframe duration accuracy at all sampling frequencies
- Boundary tests: Test minimum (16 kHz) and maximum (384 kHz) sampling rates

---

### REQ-FUNC-TRANS-002: Subframe Time Slot Allocation

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 4.2

**Description**:
The system SHALL allocate subframe time slots according to AES3-2009 Part 3 Table 1:

**Time Slots 0-3**: Preamble (X, Y, or Z)  
**Time Slots 4-7**: Auxiliary audio sample bits (LSBs 0-3)  
**Time Slots 8-27**: Main audio sample word (bits 4-23)  
**Time Slot 28**: Validity bit (V)  
**Time Slot 29**: User data bit (U)  
**Time Slot 30**: Channel status bit (C)  
**Time Slot 31**: Even parity bit (P)

**Rationale**:
AES3-2009 Part 3 Clause 4.2 and Table 1 define the precise allocation of time slots to carry audio, metadata, and synchronization information. This allocation is invariant and mandatory for AES3 compliance.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-FUNC-001 (Part 1 audio integration)
- StR-FUNC-002 (Part 2 metadata integration)

**Acceptance Criteria**:

```gherkin
Scenario: Audio sample bit allocation (24-bit mode)
  Given 24-bit audio sample value 0x123456
  When encoding to AES3 subframe
  Then bits 0-3 (LSBs) SHALL occupy time slots 4-7 (value 0x6)
  And bits 4-23 (main word) SHALL occupy time slots 8-27 (value 0x12345)
  And bit 23 (MSB) SHALL always be in time slot 27

Scenario: Audio sample bit allocation (20-bit mode)
  Given 20-bit audio sample value 0x12345 (bits 4-23)
  When encoding to AES3 subframe
  Then time slots 4-7 SHALL be logic 0 (no auxiliary bits)
  And bits 4-23 SHALL occupy time slots 8-27 (value 0x12345)
  And time slot 27 SHALL contain bit 23 (MSB = 0)

Scenario: Metadata bit allocation
  Given validity bit = 0, user data bit = 1, channel status bit = 1
  When encoding to AES3 subframe
  Then time slot 28 SHALL contain validity bit (logic 0)
  And time slot 29 SHALL contain user data bit (logic 1)
  And time slot 30 SHALL contain channel status bit (logic 1)

Scenario: Even parity calculation
  Given time slots 4-30 contain 15 logic 1 bits (odd count)
  When encoding AES3 subframe
  Then time slot 31 SHALL be logic 1 (even parity)
  And total count of logic 1 bits in time slots 4-31 SHALL be even
```

**Dependencies**:
- REQ-FUNC-AUDIO-004 (MSB justification - bit 23 always in slot 27)
- REQ-FUNC-META-001 (User data in slot 29)
- REQ-FUNC-META-002 (Channel status in slot 30)

**Constraints**:
- Time slot allocation is fixed and cannot be modified
- MSB always in time slot 27 regardless of word length

**Test Strategy**:
- Unit tests: Verify bit allocation for all word lengths (16-24 bits)
- Integration tests: Cross-check with Part 1 (audio) and Part 2 (metadata)
- Conformity tests: AES3-2009 Part 3 Table 1 validation

---

## Functional Requirements: Preambles

### REQ-FUNC-TRANS-003: Preamble X, Y, Z Patterns

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 4.3

**Description**:
The system SHALL encode preambles in time slots 0-3 using unique patterns that violate biphase-mark coding rules:

**Preamble X** (First subframe in frame):  
Binary: 11100010 (transmitted LSB first)  
Biphase-mark violation at boundaries

**Preamble Y** (Second subframe in frame):  
Binary: 11100100 (transmitted LSB first)  
Biphase-mark violation at boundaries

**Preamble Z** (First subframe at block start):  
Binary: 11101000 (transmitted LSB first)  
Biphase-mark violation at boundaries

**Rationale**:
AES3-2009 Part 3 Clause 4.3 defines preambles as intentional violations of biphase-mark coding to provide unambiguous synchronization. Preambles are polarity-insensitive (can be inverted without loss of function).

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)

**Acceptance Criteria**:

```gherkin
Scenario: Preamble X encoding (first subframe)
  Given encoding first subframe of a frame (not block start)
  When generating preamble
  Then time slots 0-3 SHALL contain preamble X pattern
  And binary pattern SHALL be 11100010 (0xE2)
  And LSB SHALL be transmitted first
  And pattern SHALL violate biphase-mark rules

Scenario: Preamble Y encoding (second subframe)
  Given encoding second subframe of a frame
  When generating preamble
  Then time slots 0-3 SHALL contain preamble Y pattern
  And binary pattern SHALL be 11100100 (0xE4)
  And LSB SHALL be transmitted first

Scenario: Preamble Z encoding (block start)
  Given encoding first subframe of a channel status block
  When generating preamble
  Then time slots 0-3 SHALL contain preamble Z pattern
  And binary pattern SHALL be 11101000 (0xE8)
  And LSB SHALL be transmitted first
  And this marks channel status byte 0 bit 0

Scenario: Preamble polarity insensitivity
  Given preamble X pattern 11100010
  When signal is inverted (polarity reversed)
  Then inverted pattern 00011101 SHALL still be recognized as preamble X
  And receiver SHALL accept either polarity
```

**Dependencies**:
- REQ-FUNC-TRANS-005 (Frame structure - X/Y alternation)
- REQ-FUNC-TRANS-006 (Block structure - Z at block start)
- REQ-FUNC-TRANS-008 (Biphase-mark coding for slots 4-31)

**Constraints**:
- Preambles must violate biphase-mark to be distinguishable
- Preamble patterns are fixed by AES3 specification

**Test Strategy**:
- Unit tests: Verify exact preamble bit patterns
- Detection tests: Preamble recognition with noise
- Polarity tests: Test inverted preamble detection

---

### REQ-FUNC-TRANS-004: Preamble Detection and Synchronization

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 4.3

**Description**:
The system SHALL detect preambles X, Y, Z by identifying biphase-mark violations in time slots 0-3, and SHALL use preamble patterns to establish:

1. Subframe synchronization (X or Y or Z detection)
2. Frame synchronization (X/Y alternation detection)
3. Block synchronization (Z detection every 192 frames)

**Rationale**:
AES3-2009 Part 3 Clause 4.3 requires receivers to synchronize at multiple hierarchical levels using preamble patterns. Robust preamble detection is critical for error-free audio reception.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-PERF-001 (Deterministic latency)
- StR-PERF-003 (Real-time operation)

**Acceptance Criteria**:

```gherkin
Scenario: Initial subframe synchronization
  Given receiver starting from unknown state
  When detecting first preamble (X, Y, or Z)
  Then receiver SHALL establish subframe timing
  And receiver SHALL lock to 32 time slot boundaries
  And synchronization SHALL complete within 3 subframes maximum

Scenario: Frame synchronization via X/Y alternation
  Given receiver detected initial subframe
  When observing preamble sequence X-Y-X-Y-X-Y
  Then receiver SHALL establish frame synchronization
  And receiver SHALL identify channel 1 (subframe 1) and channel 2 (subframe 2)
  And frame rate SHALL equal sampling frequency

Scenario: Block synchronization via preamble Z
  Given receiver synchronized to frames
  When detecting preamble Z
  Then receiver SHALL mark start of 192-frame block
  And receiver SHALL align channel status byte 0 bit 0 with Z
  And receiver SHALL track block count (0-191 frames)

Scenario: Preamble detection error recovery
  Given receiver loses synchronization due to noise burst
  When valid preambles resume
  Then receiver SHALL re-synchronize within 3 subframes
  And receiver SHALL NOT produce audio glitches during re-sync
```

**Dependencies**:
- REQ-FUNC-TRANS-003 (Preamble patterns)
- REQ-FUNC-META-003 (Channel status block structure)

**Constraints**:
- Synchronization latency critical for professional audio
- Must handle inverted polarity and DC offset

**Test Strategy**:
- Synchronization tests: Measure lock time from unknown state
- Error injection tests: Noise bursts, preamble corruption
- Performance tests: Re-synchronization latency under adverse conditions

---

## Functional Requirements: Hierarchical Structure

### REQ-FUNC-TRANS-005: Frame Structure

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 5.1

**Description**:
The system SHALL organize audio transmission into frames, where:

- Each frame contains exactly 2 subframes (64 time slots total)
- Subframe 1 (channel 1) begins with preamble X or Z
- Subframe 2 (channel 2) begins with preamble Y
- Frame rate equals sampling frequency Fs (frames per second)

**Rationale**:
AES3-2009 Part 3 Clause 5.1 defines the frame as the fundamental unit for stereo audio transmission. The X/Y preamble alternation provides frame synchronization and channel identification.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-FUNC-001 (Two-channel audio from Part 1)

**Acceptance Criteria**:

```gherkin
Scenario: Frame composition at 48 kHz
  Given sampling frequency is 48 kHz
  When transmitting AES3 frames
  Then frame rate SHALL be 48000 frames per second
  And each frame SHALL contain 2 subframes (64 time slots)
  And frame duration SHALL be 20.833 µs (1 / 48000)
  And subframe 1 SHALL carry channel 1 audio
  And subframe 2 SHALL carry channel 2 audio

Scenario: Preamble alternation in frames
  Given transmitting consecutive frames
  When encoding frame sequence
  Then frame N subframe 1 SHALL have preamble X (or Z if block start)
  And frame N subframe 2 SHALL have preamble Y
  And frame N+1 subframe 1 SHALL have preamble X (or Z if block start)
  And frame N+1 subframe 2 SHALL have preamble Y

Scenario: Channel identification via preambles
  Given receiver detects preamble sequence X-Y-X-Y
  When decoding frames
  Then samples following X preambles SHALL be assigned to channel 1
  And samples following Y preambles SHALL be assigned to channel 2
```

**Dependencies**:
- REQ-FUNC-TRANS-003 (Preambles X, Y, Z)
- REQ-FUNC-TRANS-001 (Subframe structure)

**Constraints**:
- Frame always contains exactly 2 subframes (no single-channel mode)
- Frame rate fixed to sampling frequency

**Test Strategy**:
- Unit tests: Frame structure encoding/decoding
- Timing tests: Verify frame rate matches sampling frequency
- Channel tests: Verify audio channel assignment correctness

---

### REQ-FUNC-TRANS-006: Block Structure

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 5.2

**Description**:
The system SHALL organize frames into blocks, where:

- Each block contains exactly 192 consecutive frames
- Block start is marked by preamble Z in first subframe
- Block duration = 192 / Fs seconds
- Channel status data (192 bits) transmitted over one block

**Rationale**:
AES3-2009 Part 3 Clause 5.2 defines the block as the synchronization unit for channel status metadata (Part 2). The 192-frame structure provides 192 bits for complete channel status transmission.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-FUNC-002 (Part 2 metadata integration)

**Acceptance Criteria**:

```gherkin
Scenario: Block composition at 48 kHz
  Given sampling frequency is 48 kHz
  When transmitting AES3 blocks
  Then block rate SHALL be 250 blocks per second (48000 / 192)
  And block duration SHALL be 4 ms (192 / 48000)
  And each block SHALL contain 192 frames (384 subframes)

Scenario: Preamble Z marks block start
  Given transmitting consecutive blocks
  When block N starts
  Then frame 0 subframe 1 of block N SHALL have preamble Z
  And frame 0 subframe 2 of block N SHALL have preamble Y
  And frames 1-191 subframe 1 SHALL have preamble X

Scenario: Channel status alignment with blocks
  Given channel status block of 192 bits (24 bytes)
  When transmitting block
  Then channel status bit 0 (byte 0 bit 0) SHALL be in frame with preamble Z
  And channel status bits 0-191 SHALL be transmitted in consecutive frames
  And channel status bit 191 (byte 23 bit 7) SHALL be in frame 191
```

**Dependencies**:
- REQ-FUNC-TRANS-005 (Frame structure)
- REQ-FUNC-META-003 (Channel status block structure)

**Constraints**:
- Block length fixed at 192 frames (not configurable)
- Block synchronization critical for channel status reception

**Test Strategy**:
- Unit tests: Block structure encoding/decoding
- Synchronization tests: Preamble Z detection reliability
- Integration tests: Channel status alignment verification

---

## Functional Requirements: Biphase-Mark Coding

### REQ-FUNC-TRANS-007: Biphase-Mark Encoding Principle

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 6.1

**Description**:
The system SHALL encode time slots 4-31 (28 time slots) using biphase-mark channel coding according to:

**Logic 0**: Transition at start of UI only  
**Logic 1**: Transition at start of UI AND at mid-point of UI

Where UI = Unit Interval = 1 / (2 × Fs)

**Rationale**:
AES3-2009 Part 3 Clause 6.1 defines biphase-mark coding to achieve:
- DC-free transmission (no DC component)
- Embedded clock recovery (transitions every UI or half-UI)
- Polarity insensitivity (inversion does not affect decoding)

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)

**Acceptance Criteria**:

```gherkin
Scenario: Logic 0 biphase-mark encoding
  Given data bit value is logic 0
  When encoding to biphase-mark
  Then transition SHALL occur at start of UI (time 0)
  And NO transition SHALL occur at mid-point of UI (time 0.5 UI)
  And signal level SHALL remain constant during second half of UI

Scenario: Logic 1 biphase-mark encoding
  Given data bit value is logic 1
  When encoding to biphase-mark
  Then transition SHALL occur at start of UI (time 0)
  And transition SHALL also occur at mid-point of UI (time 0.5 UI)
  And signal changes twice within the UI

Scenario: Guaranteed clock content
  Given any sequence of data bits
  When encoded to biphase-mark
  Then at least one transition per UI SHALL be guaranteed
  And receiver can extract clock from transitions
  And maximum time between transitions is 1 UI (for logic 0)

Scenario: DC-free transmission
  Given long sequence of encoded biphase-mark data
  When measuring DC component
  Then average DC level SHALL be zero (±tolerance)
  And no baseline wander SHALL occur
```

**Dependencies**:
- REQ-FUNC-TRANS-002 (Time slot allocation - slots 4-31 coded)

**Constraints**:
- Preambles (slots 0-3) intentionally violate biphase-mark
- Biphase-mark mandatory for slots 4-31

**Test Strategy**:
- Unit tests: Verify encoding for all bit patterns
- Clock recovery tests: Extract clock from encoded signal
- DC analysis tests: Measure DC component over long sequences
- Conformity tests: AES3-2009 Part 3 Clause 6.1 validation

---

### REQ-FUNC-TRANS-008: Biphase-Mark Decoding and Clock Recovery

**Priority**: Critical (P0)  
**AES3 Reference**: Part 3, Clause 6.2

**Description**:
The system SHALL decode biphase-mark encoded time slots 4-31 by:

1. Extracting clock from signal transitions (at least 1 per UI)
2. Sampling at UI mid-point to determine data value:
   - No mid-point transition = logic 0
   - Mid-point transition = logic 1
3. Tracking polarity (handle inverted signals transparently)

**Rationale**:
AES3-2009 Part 3 Clause 6.2 requires receivers to recover clock and data from the self-clocking biphase-mark signal without external clock reference.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-PERF-001 (Deterministic latency)

**Acceptance Criteria**:

```gherkin
Scenario: Clock recovery from biphase-mark signal
  Given incoming biphase-mark encoded signal
  When analyzing transition pattern
  Then receiver SHALL extract clock with period = 2 × UI
  And clock jitter SHALL be < 0.025 UI (per Part 4 requirements)
  And clock SHALL remain locked during continuous transmission

Scenario: Data decoding at UI mid-point
  Given clock recovered from signal
  When sampling at UI mid-point
  Then presence of transition SHALL decode as logic 1
  And absence of transition SHALL decode as logic 0
  And decoding accuracy SHALL be > 99.9999%

Scenario: Polarity-insensitive decoding
  Given biphase-mark signal with normal polarity
  When signal is inverted (polarity reversed)
  Then decoder SHALL still extract correct data values
  And decoder SHALL still recover clock correctly
  And polarity inversion SHALL be transparent to system

Scenario: Clock loss detection
  Given receiver decoding biphase-mark signal
  When signal loss occurs (no transitions detected)
  Then receiver SHALL detect loss of lock within 2 UI
  And receiver SHALL mute audio output
  And receiver SHALL enter re-synchronization mode
```

**Dependencies**:
- REQ-FUNC-TRANS-007 (Biphase-mark encoding)
- REQ-FUNC-TRANS-004 (Preamble detection for sync)

**Constraints**:
- Clock recovery must be robust to jitter and noise
- Must maintain lock during valid AES3 transmission

**Test Strategy**:
- Clock recovery tests: Measure lock time and jitter tolerance
- Bit error rate tests: Verify decoding accuracy
- Polarity tests: Test with inverted signals
- Loss detection tests: Verify prompt detection of signal loss

---

### REQ-FUNC-TRANS-009: Even Parity Bit Generation and Validation

**Priority**: High (P1)  
**AES3 Reference**: Part 3, Clause 7

**Description**:
The system SHALL generate and validate even parity over time slots 4-31:

**Transmitter**: Compute parity over slots 4-30, set slot 31 to achieve even parity  
**Receiver**: Recompute parity over slots 4-31, detect errors if parity is odd

**Rationale**:
AES3-2009 Part 3 Clause 7 provides basic error detection for each subframe. Even parity detects single-bit errors but cannot correct them.

**Trace to Stakeholder Requirements**:
- StR-FUNC-003 (Complete Part 3 implementation)
- StR-QUAL-001 (Error detection)

**Acceptance Criteria**:

```gherkin
Scenario: Even parity generation
  Given time slots 4-30 contain 13 logic 1 bits (odd count)
  When generating parity bit
  Then time slot 31 SHALL be set to logic 1
  And total count of logic 1 in slots 4-31 SHALL be even (14)

Scenario: Parity validation (no errors)
  Given received subframe with correct even parity
  When receiver validates parity
  Then parity check SHALL pass
  And subframe SHALL be processed normally
  And no error indication SHALL be generated

Scenario: Parity error detection (single-bit error)
  Given transmitted subframe with even parity
  When single bit is corrupted during transmission
  And receiver recomputes parity
  Then parity check SHALL fail (odd parity detected)
  And receiver SHALL log parity error
  And receiver SHALL increment error counter
  And receiver SHALL NOT mute audio (per AES3 Clause 5.3.3)

Scenario: Parity limitation (two-bit error undetected)
  Given transmitted subframe with even parity
  When two bits are corrupted during transmission
  Then parity may still be even (error undetected)
  And system SHALL rely on higher-layer error detection (CRCC, CRC)
```

**Dependencies**:
- REQ-FUNC-TRANS-002 (Time slot allocation)
- REQ-FUNC-META-006 (CRCC for stronger error detection)

**Constraints**:
- Parity detects single-bit errors only
- Parity errors should not cause audio muting

**Test Strategy**:
- Unit tests: Parity generation for all bit patterns
- Error detection tests: Single-bit error injection
- Integration tests: Verify no audio muting on parity errors

---

## Non-Functional Requirements

### REQ-PERF-TRANS-001: Real-Time Biphase-Mark Encoding Performance

**Priority**: Critical (P0)  
**Category**: Performance

**Description**:
The system SHALL encode audio samples to biphase-mark coded subframes with latency less than 1 subframe period.

**Metric**: Time from audio sample input to biphase-mark encoded output

**Targets**:
- 48 kHz: < 20.833 µs (1 subframe period)
- 96 kHz: < 10.417 µs (1 subframe period)
- 192 kHz: < 5.208 µs (1 subframe period)

**Rationale**:
Professional audio requires deterministic low-latency encoding to maintain real-time performance for live monitoring and processing.

**Trace to Stakeholder Requirements**:
- StR-PERF-001 (Deterministic latency)
- StR-PERF-003 (Real-time operation)

**Acceptance Criteria**:
- [ ] 99th percentile encoding latency meets targets
- [ ] Worst-case latency < 1.5 subframe periods
- [ ] Zero buffer overruns during sustained operation
- [ ] CPU usage < 10% per channel at 192 kHz

**Test Strategy**:
- Latency measurement: High-precision timing instrumentation
- Load testing: Sustained operation at all sampling frequencies
- Profiling: Identify encoding bottlenecks

---

### REQ-PERF-TRANS-002: Real-Time Biphase-Mark Decoding Performance

**Priority**: Critical (P0)  
**Category**: Performance

**Description**:
The system SHALL decode biphase-mark coded subframes to audio samples with latency less than 1 subframe period.

**Metric**: Time from biphase-mark input to decoded audio sample output

**Targets**:
- 48 kHz: < 20.833 µs (1 subframe period)
- 96 kHz: < 10.417 µs (1 subframe period)
- 192 kHz: < 5.208 µs (1 subframe period)

**Rationale**:
Real-time decoding is critical for professional audio monitoring without perceivable delay.

**Trace to Stakeholder Requirements**:
- StR-PERF-001 (Deterministic latency)
- StR-PERF-003 (Real-time operation)

**Acceptance Criteria**:
- [ ] 99th percentile decoding latency meets targets
- [ ] Clock recovery within 3 subframes from unknown state
- [ ] Zero buffer underruns during sustained operation
- [ ] CPU usage < 10% per channel at 192 kHz

---

### REQ-PERF-TRANS-003: Preamble Detection Latency

**Priority**: High (P1)  
**Category**: Performance

**Description**:
The system SHALL detect preambles (X, Y, Z) and achieve synchronization within 3 subframes maximum from unknown state.

**Metric**: Time from signal input to synchronized state

**Targets**:
- Initial subframe sync: < 3 subframes
- Frame sync (X/Y alternation): < 6 subframes
- Block sync (Z detection): < 192 frames (1 block)

**Trace to Stakeholder Requirements**:
- StR-PERF-001 (Deterministic latency)

**Acceptance Criteria**:
- [ ] Subframe sync within 3 subframes (99% of cases)
- [ ] Frame sync within 6 subframes
- [ ] Block sync within 1 block period
- [ ] Re-synchronization after signal loss < 5 subframes

---

### REQ-QUAL-TRANS-001: AES3-2009 Part 3 Conformity Testing

**Priority**: Critical (P0)  
**Category**: Quality

**Description**:
The system SHALL pass 100% of test cases defined in the AES3-2009 Part 3 conformity test suite.

**Standard Compliance**: AES3-3-2009 (R2014) Clauses 4-7

**Test Coverage Requirements**:
- Subframe structure (Clause 4): 10 test cases
- Preambles (Clause 4.3): 12 test cases
- Frame/block structure (Clause 5): 10 test cases
- Biphase-mark coding (Clause 6): 15 test cases
- Parity (Clause 7): 5 test cases

**Total Test Cases**: 52 minimum

**Trace to Stakeholder Requirements**:
- StR-QUAL-001 (100% AES3-2009 conformity)
- StR-QUAL-002 (Unit test coverage ≥95%)

**Acceptance Criteria**:
- [ ] All 52 conformity test cases pass
- [ ] Biphase-mark encoding/decoding accuracy > 99.9999%
- [ ] Preamble detection reliability > 99.99%
- [ ] Test suite integrated in CI/CD pipeline

---

## Traceability Matrix: Part 3 Requirements

| Stakeholder Req | System Req | AES3 Reference | Priority | Status |
|----------------|-----------|----------------|----------|--------|
| StR-FUNC-003 | REQ-FUNC-TRANS-001 | Part 3 Clause 4.1 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-002 | Part 3 Clause 4.2, Table 1 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-003 | Part 3 Clause 4.3 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-004 | Part 3 Clause 4.3 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-005 | Part 3 Clause 5.1 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-006 | Part 3 Clause 5.2 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-007 | Part 3 Clause 6.1 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-008 | Part 3 Clause 6.2 | P0 | Draft |
| StR-FUNC-003 | REQ-FUNC-TRANS-009 | Part 3 Clause 7 | P1 | Draft |
| StR-PERF-001 | REQ-PERF-TRANS-001 | Part 3 Performance | P0 | Draft |
| StR-PERF-001 | REQ-PERF-TRANS-002 | Part 3 Performance | P0 | Draft |
| StR-PERF-001 | REQ-PERF-TRANS-003 | Part 3 Performance | P1 | Draft |
| StR-QUAL-001 | REQ-QUAL-TRANS-001 | Part 3 All Clauses | P0 | Draft |

**Summary**: 13 detailed requirements elicited from StR-FUNC-003 (AES3-2009 Part 3)

---

## Cross-Part Dependencies

**Part 3 → Part 1 Integration**:
- Time slots 4-27 carry 24-bit audio samples (REQ-FUNC-AUDIO-001 to -004)
- Time slot 28 carries validity bit (REQ-FUNC-AUDIO-009)

**Part 3 → Part 2 Integration**:
- Time slot 29 carries user data bit (REQ-FUNC-META-001)
- Time slot 30 carries channel status bit (REQ-FUNC-META-002)
- Preamble Z synchronizes channel status blocks (REQ-FUNC-META-003)

**Part 3 → Part 4 Integration**:
- Biphase-mark signal must meet jitter requirements (REQ-FUNC-PHYS-001)
- Physical layer must support UI timing accuracy (REQ-FUNC-PHYS-002)

---

## Next Steps

1. ✅ **Part 1 Requirements Elicitation** - COMPLETE (12 requirements)
2. ✅ **Part 2 Requirements Elicitation** - COMPLETE (9 requirements)
3. ✅ **Part 3 Requirements Elicitation** - COMPLETE (13 requirements, this document)
4. ⏳ **Part 4 Requirements Elicitation** - Physical/Electrical abstraction (next)
5. ⏳ **HAL Interface Requirements** - Define <10 function HAL
6. ⏳ **Requirements Review** - Stakeholder validation

---

## References

- **[AES3-3]** AES3-3-2009 (R2014) Digital Audio Interface Part 3: Transport
- **[AES3-1]** AES3-1-2009 (R2014) Part 1: Audio Content (time slot allocation)
- **[AES3-2]** AES3-2-2009 (R2014) Part 2: Metadata (time slot 29-30 content)
- **[AES3-4]** AES3-4-2009 (R2014) Part 4: Physical/Electrical (jitter, timing)
- **[ISO29148]** ISO/IEC/IEEE 29148:2018 Requirements Engineering

---

**Document Status**: Draft - Awaiting stakeholder review  
**Next Review**: Phase 02 Week 6 (December 3-9, 2025)
