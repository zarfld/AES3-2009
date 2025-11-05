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
    - StR-FUNC-002
    - StR-QUAL-001
description: "Detailed system requirements for AES3-2009 Part 2: Metadata and Subcode"
---

# AES3-2009 Part 2: Metadata and Subcode - System Requirements

**Elicitation Session**: 2025-11-05  
**Source**: AES3-2-2009 (R2014) Specification  
**Stakeholder Requirement**: StR-FUNC-002 - Complete Part 2 (Metadata/Subcode) implementation  
**Standards Compliance**: ISO/IEC/IEEE 29148:2018

---

## Requirements Elicitation Summary

**Source Material**: AES3-2-2009 (R2014) - Digital Audio Interface Part 2: Metadata and Subcode

**8-Dimension Analysis Applied**:

1. ✅ Functional Behavior - Channel status blocks, user data, auxiliary bits
2. ✅ Boundary Values - 192-bit blocks, 24 bytes, CRCC polynomial
3. ✅ Failure Handling - CRCC errors, block sync loss, invalid states
4. ✅ Performance - 1-bit per subframe transmission rate
5. ✅ Security - Channel status validation, CRCC integrity
6. ✅ Regulatory - ISO 646 alphanumeric encoding, IEC 60958 compatibility
7. ✅ Integration - Part 1 (validity bit), Part 3 (block structure)
8. ✅ Prioritization - Critical for professional audio metadata

---

## Functional Requirements: User Data Channel

### REQ-FUNC-META-001: User Data Bit Transmission

**Priority**: High (P1)  
**AES3 Reference**: Part 2, Clause 4

**Description**:
The system SHALL transmit 1 bit of user data in time slot 29 of each subframe, providing a user data channel with capacity equal to the sampling frequency in kbit/s for each audio channel.

**Rationale**:
AES3-2009 Part 2 Clause 4 provides a user-defined data channel for custom applications (timecode, control data, etc.). Independent user data per channel enables flexible system design.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)

**Acceptance Criteria**:

```gherkin
Scenario: User data capacity at 48 kHz
  Given sampling frequency is 48 kHz
  When transmitting AES3 frames
  Then user data capacity SHALL be 48 kbit/s per channel
  And 1 bit SHALL be transmitted per subframe in time slot 29

Scenario: Independent user data per channel
  Given channel 1 user data is timecode stream
  And channel 2 user data is control commands
  When transmitting AES3 frames
  Then each subframe SHALL carry independent user data
  And channels SHALL NOT interfere with each other

Scenario: User data default value
  Given no user-defined data to transmit
  When encoding AES3 subframe
  Then user data bit SHALL default to logic 0
```

**Dependencies**:
- REQ-FUNC-TRANS-002 (Subframe time slot 29 allocation)

**Constraints**:
- User data format is not standardized by AES3 (application-specific)
- Capacity scales linearly with sampling frequency

**Test Strategy**:
- Unit tests: User data bit encoding/decoding
- Integration tests: Verify independent channel user data
- Performance tests: Sustained user data transmission at 192 kHz

---

## Functional Requirements: Channel Status Format

### REQ-FUNC-META-002: Channel Status Bit Transmission

**Priority**: Critical (P0)  
**AES3 Reference**: Part 2, Clause 5.1

**Description**:
The system SHALL transmit 1 bit of channel status data in time slot 30 of each subframe, with capacity equal to the sampling frequency in kbit/s for each audio channel.

**Rationale**:
AES3-2009 Part 2 Clause 5.1 defines channel status as the primary metadata mechanism, carrying audio parameters, sampling frequency, word length, and other critical information.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)

**Acceptance Criteria**:

```gherkin
Scenario: Channel status capacity at 48 kHz
  Given sampling frequency is 48 kHz
  When transmitting AES3 frames
  Then channel status capacity SHALL be 48 kbit/s per channel
  And 1 bit SHALL be transmitted per subframe in time slot 30

Scenario: Independent channel status per channel
  Given channel 1 is configured for 24-bit audio
  And channel 2 is configured for 20-bit audio
  When transmitting AES3 frames
  Then each subframe SHALL carry independent channel status
  And receivers SHALL decode each channel independently
```

**Dependencies**:
- REQ-FUNC-TRANS-002 (Subframe time slot 30 allocation)

**Test Strategy**:
- Unit tests: Channel status bit position verification
- Integration tests: Bi-directional channel status communication

---

### REQ-FUNC-META-003: Channel Status Block Structure

**Priority**: Critical (P0)  
**AES3 Reference**: Part 2, Clause 5.2

**Description**:
The system SHALL organize channel status information in 192-bit blocks (24 bytes numbered 0-23), where:

- Each block corresponds to 192 consecutive frames
- First frame in block is marked by preamble Z (Part 3)
- Bits are numbered 0-7 within each byte (bit 0 transmitted first)
- Multi-bit values use bit 0 as LSB (least significant bit)

**Rationale**:
AES3-2009 Part 2 Clause 5.2 defines the 192-bit block structure to efficiently encode all metadata parameters while maintaining synchronization with audio frames.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)

**Acceptance Criteria**:

```gherkin
Scenario: 192-bit block structure
  Given 192 consecutive AES3 frames
  When transmitting channel status
  Then exactly 192 bits SHALL be transmitted (24 bytes)
  And byte numbering SHALL be 0 to 23
  And bit numbering within each byte SHALL be 0 to 7

Scenario: Block start synchronization
  Given a channel status block about to start
  When transmitting frame 0 of the block
  Then preamble Z SHALL mark the first subframe
  And bit 0 of byte 0 SHALL be in that frame

Scenario: Bit ordering within bytes
  Given channel status byte 5 with value 0x2A (binary 00101010)
  When transmitting the byte
  Then bit 0 (LSB = 0) SHALL be transmitted first
  And bit 7 (MSB = 0) SHALL be transmitted last

Scenario: Multi-bit numeric encoding
  Given sampling frequency 48 kHz encoded in byte 0 bits 6-7
  When encoding value 0b10 (decimal 2) meaning "48 kHz"
  Then bit 6 SHALL be 0 (LSB)
  And bit 7 SHALL be 1 (MSB)
  And transmitted bit order SHALL be bit 6 then bit 7
```

**Dependencies**:
- REQ-FUNC-TRANS-005 (Frame structure with preamble Z)
- REQ-FUNC-TRANS-006 (Block structure definition)

**Constraints**:
- Block length fixed at 192 frames (not configurable)
- Byte/bit ordering must match AES3-2009 exactly for interoperability

**Test Strategy**:
- Unit tests: Block structure encoding/decoding
- Boundary tests: Verify bit/byte ordering matches specification
- Synchronization tests: Preamble Z correlation with byte 0 bit 0

---

### REQ-FUNC-META-004: Channel Status Byte 0 - Basic Audio Parameters

**Priority**: Critical (P0)  
**AES3 Reference**: Part 2, Clause 5.5.0

**Description**:
The system SHALL encode basic audio parameters in channel status byte 0:

**Bit 0**: Use of channel status block (0=Consumer, 1=Professional)
**Bit 1**: Linear PCM identification (0=Linear PCM, 1=Non-PCM)
**Bits 2-4**: Audio signal pre-emphasis (000=not indicated, 001=none, 011=50µs+15µs, 111=J.17)
**Bit 5**: Lock indication (0=default, 1=source unlocked)
**Bits 6-7**: Sampling frequency (00=not indicated, 01=48kHz, 10=44.1kHz, 11=32kHz)

**Rationale**:
AES3-2009 Part 2 Clause 5.5.0 defines byte 0 as carrying the most critical audio parameters for receiver configuration. These parameters must be correctly decoded for proper audio processing.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)
- StR-FUNC-001 (Integration with Part 1 audio content)

**Acceptance Criteria**:

```gherkin
Scenario: Professional use indication (bit 0)
  Given system configured for professional audio interface
  When encoding channel status byte 0
  Then bit 0 SHALL be logic 1
  And receivers SHALL interpret remaining bits per professional format

Scenario: Linear PCM content (bits 0-1)
  Given audio content is linear PCM samples
  When encoding channel status byte 0
  Then bit 0 SHALL be logic 1 (professional)
  And bit 1 SHALL be logic 0 (linear PCM)
  And validity bit SHALL be logic 0 (suitable for conversion)

Scenario: Non-PCM content (bits 0-1)
  Given audio content is compressed (e.g., Dolby Digital)
  When encoding channel status byte 0
  Then bit 0 SHALL be logic 1 (professional)
  And bit 1 SHALL be logic 1 (non-PCM)
  And validity bit SHALL be logic 1 (not suitable for conversion)

Scenario: Sampling frequency indication (bits 6-7)
  Given sampling frequency is 48 kHz
  When encoding channel status byte 0
  Then bits 6-7 SHALL be 0b01 (value 1 = 48 kHz)
  And receivers SHALL configure for 48 kHz operation

Scenario: Source unlock detection (bit 5)
  Given source sampling clock loses lock
  When encoding channel status byte 0
  Then bit 5 SHALL be set to logic 1
  And receivers SHALL be notified of unlock condition
```

**Dependencies**:
- REQ-FUNC-AUDIO-001 (PCM encoding for bit 1 correlation)
- REQ-FUNC-AUDIO-010 (Pre-emphasis for bits 2-4 correlation)

**Constraints**:
- Bit 0 = 1 required for professional use (AES3 scope)
- Consumer use (bit 0 = 0) follows IEC 60958-3 (out of scope)

**Test Strategy**:
- Unit tests: Encode/decode all byte 0 bit combinations
- Integration tests: Verify correlation with Part 1 (PCM, validity, pre-emphasis)
- Interoperability tests: Cross-check with commercial AES3 equipment

---

### REQ-FUNC-META-005: Channel Status Byte 2 - Word Length and Auxiliary Bits

**Priority**: Critical (P0)  
**AES3 Reference**: Part 2, Clause 5.5.2

**Description**:
The system SHALL encode audio sample word length and auxiliary bit usage in channel status byte 2:

**Bits 0-2**: Use of auxiliary bits
- 000: Max 20 bits, aux bits undefined (default)
- 100: Max 24 bits, aux bits for main audio
- 010: Max 20 bits, aux bits for coordination signal

**Bits 3-5**: Encoded audio sample word length (actual bit count)

**Bits 6-7**: Alignment level indication (00=not indicated, 10=SMPTE RP155, 01=EBU R68)

**Rationale**:
AES3-2009 Part 2 Clause 5.5.2 allows receivers to determine exact audio word length and whether auxiliary bits (time slots 4-7) contain audio or other data.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)
- StR-FUNC-001 (Part 1 word length correlation)

**Acceptance Criteria**:

```gherkin
Scenario: 24-bit audio with auxiliary bits for main audio
  Given audio samples are 24-bit resolution
  When encoding channel status byte 2
  Then bits 0-2 SHALL be 0b100 (max 24 bits, aux for audio)
  And bits 3-5 SHALL be 0b101 (24 bits actual)
  And all 24 time slots (4-27) SHALL contain audio data

Scenario: 20-bit audio with auxiliary bits for coordination
  Given audio samples are 20-bit resolution
  And auxiliary bits carry coordination signal
  When encoding channel status byte 2
  Then bits 0-2 SHALL be 0b010 (max 20 bits, aux for coordination)
  And bits 3-5 SHALL be 0b001 (20 bits actual)
  And time slots 4-7 SHALL carry coordination signal

Scenario: Word length indication for 16-bit audio in 20-bit range
  Given audio samples are 16-bit resolution
  When encoding channel status byte 2
  Then bits 0-2 SHALL be 0b000 (max 20 bits)
  And bits 3-5 SHALL be 0b001 (16 bits actual)
  And bits 4-19 of subframe SHALL contain 16-bit audio (MSB justified)
  And bits 0-3 SHALL be logic 0 (unused LSBs)
```

**Dependencies**:
- REQ-FUNC-AUDIO-003 (Coding precision options)
- REQ-FUNC-AUDIO-004 (MSB justification)

**Constraints**:
- Byte 2 must accurately reflect actual audio word length
- Mismatched word length indication causes level errors

**Test Strategy**:
- Unit tests: Encode/decode all word length combinations (16-24 bits)
- Integration tests: Verify correlation with Part 1 audio sample word
- Interoperability tests: Test with receivers expecting specific word lengths

---

### REQ-FUNC-META-006: Channel Status Byte 23 - CRCC Validation

**Priority**: Critical (P0)  
**AES3 Reference**: Part 2, Clause 5.5.11, Annex C

**Description**:
The system SHALL compute and transmit an 8-bit Cyclic Redundancy Check Character (CRCC) in channel status byte 23 using the generating polynomial:

**G(x) = x^8 + x^4 + x^3 + x^2 + 1**

The CRCC SHALL cover bytes 0-22 inclusive (184 bits), with:
- Initial condition: all ones (0xFF)
- LSB transmitted first
- Purpose: Detect channel status block corruption

**Rationale**:
AES3-2009 Part 2 Clause 5.5.11 and Annex C define CRCC as the integrity check for channel status. Receivers must validate CRCC to detect transmission errors or editing artifacts.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)
- StR-QUAL-001 (100% AES3-2009 conformity)
- StR-SEC-002 (Input validation - detect corruption)

**Acceptance Criteria**:

```gherkin
Scenario: CRCC computation for valid channel status block
  Given channel status bytes 0-22 with known values
  When computing CRCC using G(x) = x^8 + x^4 + x^3 + x^2 + 1
  And initial condition is 0xFF
  Then byte 23 SHALL contain correct 8-bit CRCC
  And receiver SHALL validate and accept the block

Scenario: CRCC error detection on corrupted block
  Given channel status block with CRCC computed correctly
  When bit 42 (byte 5 bit 2) is flipped during transmission
  And receiver recomputes CRCC
  Then CRCC SHALL NOT match byte 23 value
  And receiver SHALL reject the channel status block
  And receiver SHALL log error "CRCC validation failed"

Scenario: CRCC test vector validation (AES3 Annex C Example 1)
  Given channel status byte 0 = 0x3C (bits 2,3,4,5 set)
  And byte 1 = 0x02 (bit 1 set)
  And byte 4 = 0x01 (bit 0 set)
  And all other bytes 0-22 = 0x00
  When computing CRCC
  Then byte 23 SHALL equal 0xCD (binary 11001101)

Scenario: Receiver behavior on CRCC error
  Given channel status block received with CRCC mismatch
  When receiver validates channel status
  Then receiver SHALL discard the block
  And receiver SHALL NOT alter audio output (no muting)
  And receiver SHALL use last valid channel status
  And receiver SHALL increment CRCC error counter
```

**Dependencies**:
- REQ-FUNC-META-003 (Channel status block structure)

**Constraints**:
- CRCC is mandatory (no default value)
- Receivers must not mute audio on CRCC errors (per AES3 Clause 5.3.3)

**Test Strategy**:
- Unit tests: CRCC computation with AES3 Annex C test vectors
- Error injection tests: Corrupt random bits and verify detection
- Performance tests: CRCC computation latency (<1 frame period)
- Conformity tests: AES3-2009 Part 2 Annex C validation

---

## Functional Requirements: Auxiliary Bits

### REQ-FUNC-META-007: Auxiliary Bits Availability

**Priority**: Medium (P2)  
**AES3 Reference**: Part 2, Clause 6.1

**Description**:
The system SHALL make the four least-significant bits (LSBs) of the 24-bit audio sample word (time slots 4-7) available for auxiliary purposes when audio word length does not exceed 20 bits.

**Rationale**:
AES3-2009 Part 2 Clause 6.1 provides auxiliary bits for voice-quality coordination channels or other applications when audio resolution permits.

**Trace to Stakeholder Requirements**:
- StR-FUNC-002 (Complete Part 2 implementation)

**Acceptance Criteria**:

```gherkin
Scenario: Auxiliary bits available for 20-bit audio
  Given audio sample word length is 20 bits
  When encoding to AES3 subframe
  Then time slots 8-27 SHALL contain 20-bit audio (MSB justified)
  And time slots 4-7 SHALL be available for auxiliary use
  And channel status byte 2 bits 0-2 SHALL indicate auxiliary usage

Scenario: Auxiliary bits unavailable for 24-bit audio
  Given audio sample word length is 24 bits
  When encoding to AES3 subframe
  Then time slots 4-27 SHALL all contain audio data
  And auxiliary bits are not available
  And channel status byte 2 bits 0-2 SHALL be 0b100 (max 24 bits)

Scenario: Auxiliary bits for coordination channel (Annex B)
  Given 20-bit audio with coordination signal enabled
  When encoding to AES3 subframe
  Then auxiliary bits SHALL carry voice-quality data
  And channel status byte 2 bits 0-2 SHALL be 0b010
  And auxiliary signal is sampled at Fs/3 (e.g., 16 kHz for 48 kHz audio)
  And auxiliary signal is 12-bit uniformly coded (transmitted 4 bits per frame)
```

**Dependencies**:
- REQ-FUNC-META-005 (Byte 2 auxiliary bit usage indication)
- REQ-FUNC-AUDIO-003 (Coding precision)

**Constraints**:
- Auxiliary bits only available when audio ≤ 20 bits
- Auxiliary data format not standardized (application-specific)

**Test Strategy**:
- Unit tests: Verify auxiliary bit availability based on word length
- Integration tests: Encode/decode coordination channel per Annex B
- Interoperability tests: Test auxiliary bit use with commercial equipment

---

## Non-Functional Requirements

### REQ-PERF-META-001: Channel Status Block Synchronization Latency

**Priority**: High (P1)  
**Category**: Performance

**Description**:
The system SHALL achieve channel status block synchronization within 192 frames (4 ms at 48 kHz, 1 ms at 192 kHz) of receiving preamble Z.

**Metric**: Time from preamble Z detection to valid channel status byte 0 decoding

**Targets**:
- 48 kHz: < 4 ms (192 frames)
- 96 kHz: < 2 ms (192 frames)
- 192 kHz: < 1 ms (192 frames)

**Rationale**:
Fast channel status synchronization is critical for receivers to configure audio parameters (word length, sampling frequency) without prolonged muting or misconfiguration.

**Trace to Stakeholder Requirements**:
- StR-PERF-003 (Deterministic latency)
- StR-FUNC-002 (Complete Part 2 implementation)

**Acceptance Criteria**:
- [ ] Preamble Z detected within 1 frame of occurrence
- [ ] Channel status buffering starts immediately after Z
- [ ] Byte 0 decoded and applied within 192 frames
- [ ] No audio muting required during synchronization

**Test Strategy**:
- Latency measurement: Time preamble Z to byte 0 application
- Edge case testing: Synchronization during channel status changes

---

### REQ-QUAL-META-001: AES3-2009 Part 2 Conformity Testing

**Priority**: Critical (P0)  
**Category**: Quality

**Description**:
The system SHALL pass 100% of test cases defined in the AES3-2009 Part 2 conformity test suite.

**Standard Compliance**: AES3-2-2009 (R2014) Clauses 4-6

**Test Coverage Requirements**:
- User data format (Clause 4): 5 test cases
- Channel status format (Clause 5): 30 test cases
  - Byte 0 (basic parameters): 8 test cases
  - Byte 2 (word length): 8 test cases
  - Byte 23 (CRCC): 10 test cases
  - Other bytes: 4 test cases
- Auxiliary bits (Clause 6): 6 test cases

**Total Test Cases**: 41 minimum

**Trace to Stakeholder Requirements**:
- StR-QUAL-001 (100% AES3-2009 conformity)
- StR-QUAL-002 (Unit test coverage ≥95%)

**Acceptance Criteria**:
- [ ] All 41 conformity test cases pass
- [ ] CRCC validation passes all AES3 Annex C test vectors
- [ ] Channel status decoding matches AES-2id guidelines
- [ ] Test suite integrated in CI/CD pipeline

---

## Traceability Matrix: Part 2 Requirements

| Stakeholder Req | System Req | AES3 Reference | Priority | Status |
|----------------|-----------|----------------|----------|--------|
| StR-FUNC-002 | REQ-FUNC-META-001 | Part 2 Clause 4 | P1 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-002 | Part 2 Clause 5.1 | P0 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-003 | Part 2 Clause 5.2 | P0 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-004 | Part 2 Clause 5.5.0 | P0 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-005 | Part 2 Clause 5.5.2 | P0 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-006 | Part 2 Clause 5.5.11 | P0 | Draft |
| StR-FUNC-002 | REQ-FUNC-META-007 | Part 2 Clause 6.1 | P2 | Draft |
| StR-PERF-003 | REQ-PERF-META-001 | Part 2 Performance | P1 | Draft |
| StR-QUAL-001 | REQ-QUAL-META-001 | Part 2 All Clauses | P0 | Draft |

**Summary**: 9 detailed requirements elicited from StR-FUNC-002 (AES3-2009 Part 2)

---

## Next Steps

1. ✅ **Part 1 Requirements Elicitation** - COMPLETE
2. ✅ **Part 2 Requirements Elicitation** - COMPLETE (this document)
3. ⏳ **Part 3 Requirements Elicitation** - Transport layer (next)
4. ⏳ **Part 4 Requirements Elicitation** - Physical/Electrical abstraction
5. ⏳ **Requirements Review** - Stakeholder validation
6. ⏳ **Test Case Generation** - From acceptance criteria

---

## References

- **[AES3-2]** AES3-2-2009 (R2014) Digital Audio Interface Part 2: Metadata and Subcode
- **[ISO646]** ISO 646 7-bit Coded Character Set (for alphanumeric channel origin/destination)
- **[IEC60958]** IEC 60958-1/-3/-4 Digital Audio Interface (Consumer/Professional)
- **[ISO29148]** ISO/IEC/IEEE 29148:2018 Requirements Engineering

---

**Document Status**: Draft - Awaiting stakeholder review  
**Next Review**: Phase 02 Week 6 (December 3-9, 2025)
