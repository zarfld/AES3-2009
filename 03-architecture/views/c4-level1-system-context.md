---
specType: architecture-view
title: C4 Level 1 System Context
phase: 03-architecture
date: "2025-11-05"
standard: ISO/IEC/IEEE 42010:2011
model: C4
view-level: context
---

# C4 Model - Level 1: System Context

**Date**: 2025-11-05  
**Standard**: ISO/IEC/IEEE 42010:2011  
**Model**: C4 (Context, Containers, Components, Code)

---

## Purpose

Shows the AES3-2009 Digital Audio Interface system within its environment, identifying external actors and systems that interact with it.

## System Context Diagram

```mermaid
graph TB
    %% External Actors
    AudioApp[Audio Application<br/>Transmits/receives<br/>digital audio streams]
    Developer[Embedded Developer<br/>Integrates AES3<br/>into products]
    TestEngineer[Test Engineer<br/>Validates AES3<br/>compliance]
    
    %% The System
    AES3System[AES3-2009 Implementation<br/>Hardware-agnostic digital<br/>audio interface protocol<br/>Parts 1-4 complete]
    
    %% External Systems
    AudioHW[Audio Hardware<br/>Platform-specific<br/>audio I/O drivers]
    TimingHW[Timing Hardware<br/>Sample clock, jitter<br/>measurement, PTP sync]
    TestEquip[Test Equipment<br/>Audio analyzers,<br/>oscilloscopes, protocol analyzers]
    AES3Spec[AES3-2009 Standard<br/>Official specification<br/>conformity requirements]
    PlatformOS[Platform OS/RTOS<br/>Linux, Windows,<br/>FreeRTOS, bare-metal]
    
    %% Relationships
    AudioApp -->|encode/decode audio| AES3System
    Developer -->|integrates| AES3System
    TestEngineer -->|validates| AES3System
    
    AES3System -->|abstracts via HAL| AudioHW
    AES3System -->|uses timing services| TimingHW
    AES3System -->|tested with| TestEquip
    AES3System -->|implements| AES3Spec
    AES3System -->|runs on| PlatformOS
    
    %% Styling
    classDef person fill:#08427b,stroke:#052e56,stroke-width:2px,color:#fff
    classDef system fill:#1168bd,stroke:#0b4884,stroke-width:4px,color:#fff
    classDef external fill:#999999,stroke:#6b6b6b,stroke-width:2px,color:#fff
    
    class AudioApp,Developer,TestEngineer person
    class AES3System system
    class AudioHW,TimingHW,TestEquip,AES3Spec,PlatformOS external
```

## Key Elements

### The System (Blue)

**AES3-2009 Implementation**
- **Purpose**: Hardware-agnostic reference implementation of AES3-2009 digital audio interface standard
- **Coverage**: All 4 parts (Audio Content, Metadata/Subcode, Transport, Physical/Electrical)
- **Key Feature**: Zero vendor-specific code in Standards layer
- **License**: Open-source (Apache-2.0 or equivalent)

### External Actors (Dark Blue)

1. **Audio Application**
   - Desktop/embedded audio software
   - Uses AES3 for professional audio I/O
   - Expects compliant PCM audio streams

2. **Embedded Developer**
   - Integrates AES3 into products
   - Implements <10 HAL functions
   - Targets: Consumer audio, pro audio, broadcast

3. **Test Engineer**
   - Validates AES3 conformity
   - Runs conformity test suite
   - Measures jitter, timing, signal quality

### External Systems (Gray)

1. **Audio Hardware**
   - Platform-specific audio I/O (ALSA, ASIO, CoreAudio, bare-metal)
   - Provides sample clock, bit transmission/reception
   - AES3 system abstracts via HAL interface

2. **Timing Hardware**
   - Sample clock generation (crystal oscillator, PLL)
   - Jitter measurement circuitry
   - Optional: PTP sync for multi-device systems

3. **Test Equipment**
   - Audio analyzers (Audio Precision, Prism Sound)
   - Oscilloscopes (jitter measurement)
   - Protocol analyzers (channel status decoding)

4. **AES3-2009 Standard**
   - Official specification (AES3-1/2/3/4-2009 R2014)
   - Defines conformity requirements
   - System must pass all conformity tests

5. **Platform OS/RTOS**
   - Linux (x86-64, ARM), Windows, FreeRTOS, bare-metal
   - Provides threading, memory management
   - System compiles/runs without modification

## Key Interfaces

| Interface | Type | Protocol | Description |
|-----------|------|----------|-------------|
| Application API | Software | C API | Encode/decode audio streams, configure parameters |
| HAL Interface | Software | C function pointers | <10 functions for platform adaptation |
| Audio Hardware | Hardware/Software | Platform-specific | Bit-level transmission/reception |
| Timing Hardware | Hardware | Clock signals | Sample clock, jitter measurement |

## Architectural Constraints

1. **Hardware Abstraction**: No vendor-specific code in Standards layer
2. **Standards Compliance**: 100% AES3-2009 Parts 1-4 coverage
3. **Platform Portability**: Compiles on Linux, Windows, RTOS, bare-metal
4. **Real-Time Performance**: <1 sample period latency (99th percentile)
5. **Open Source**: Permissive license for commercial use

## Quality Attributes

| Attribute | Target | Measurement |
|-----------|--------|-------------|
| **Conformity** | 100% AES3-2009 | Pass all conformity tests |
| **Portability** | 4+ platforms | Linux, Windows, RTOS, bare-metal |
| **Performance** | <20.8µs encoding @ 48kHz | 99th percentile latency |
| **Jitter** | <0.025 UI transmit | Per AES3-2009 Part 4 |
| **Integration Time** | <1 day | Developer survey |

## Stakeholder Mapping

| Stakeholder | Primary Concerns | Related Requirements |
|-------------|------------------|---------------------|
| Audio Equipment Manufacturers | Standards compliance, portability | StR-BUS-001, StR-FUNC-001-004 |
| Embedded Engineers | Simple integration, clear docs | StR-BUS-002, StR-FUNC-004 (HAL) |
| Test Engineers | Conformity validation, test suite | StR-QUAL-003 (conformity tests) |
| Academic Institutions | Educational value, code clarity | StR-BUS-003 (education) |
| Open-Source Community | License, contribution model | StR-BUS-001 (open-source) |

---

**Next Level**: [C4 Level 2 - Container View](c4-level2-container-view.md) - Shows high-level architecture with Standards layer, HAL layer, and platform bridges
