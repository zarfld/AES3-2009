---
applyTo: '**'
---

# Standards Layer AI Coding Instructions - AES3-2009 Digital Audio Interface

## CRITICAL: Hardware-Agnostic Protocol Implementation

The Standards layer (`lib/Standards/`) is the **pure protocol implementation layer** that MUST remain completely hardware and vendor agnostic.

**PROJECT SCOPE**: This project implements the AES3-2009 standard for digital audio interfacing - a serial transmission format for two-channel linearly-represented digital audio data. The implementation follows the four-part AES3-2009 specification structure.

## Working Principles for Standards Layer

- **Understand protocols before implementing** - study AES specifications and state machines thoroughly
- **No Fake, No Stubs, no Simulations** - implement complete protocol logic, not simplified placeholders
- **No implementation-based assumptions** - use AES specification or analysis results only (ask if required)
- **No false advertising** - prove and ensure protocol compliance and correctness
- **Never break protocol APIs** in ways that violate AES standards - prefer fixing implementation instead
- **Prevent dead code or orphan files** - fix code rather than creating new versions, ensure ALL code compiles
- **Always reference ### Documentation Requirements with Copyright Compliance

### Function Documentation with AES Context
```cpp
/**
 * @brief Parse AES3-2009 Audio Subframe from network packet
 *
 * Validates and extracts audio subframe according to AES3-2009
 *
 */
int aes3_parse_audio_subframe(const uint8_t* packet_data, 
                               size_t packet_length,
                               aes3_audio_subframe_t* audio_subframe);

```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements entity descriptor parsing per AES3-2009 Section 7.2.1
#define ENTITY_DESCRIPTOR_TYPE 0x0000  // As specified in AES3-2009

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted IEEE content:
// "The entity_descriptor field shall contain the following sub-fields:
//  descriptor_type (16 bits): shall be set to ENTITY (0x0000)
//  descriptor_index (16 bits): shall be set to 0x0000..."
// This is direct reproduction of IEEE copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct entity_descriptor {
    uint16_t descriptor_type;    // AES3-2009 Table 7.1 
    uint16_t descriptor_index;   // AES3-2009 Table 7.1
    uint64_t entity_id;         // AES3-2009 Table 7.1
    uint64_t entity_model_id;   // AES3-2009 Table 7.1
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Core Principle: Standards-Only Implementation

- **ONLY AES3-2009 protocol logic** - no hardware-specific code
- **Hardware agnostic** - compilable without any vendor drivers, audio hardware, or OS-specific headers
- **Mockable/testable** - runnable without physical audio hardware present
- **Dependency injection pattern** - receives hardware abstraction via interfaces/function pointers

### FORBIDDEN in Standards Layer
```cpp
// ❌ NEVER include vendor-specific or OS-specific headers
#include "asio_audio_driver.h"           // NO - ASIO specific
#include "../../realtek_audio/include/*.h" // NO - Realtek specific  
#include "../../common/hal/audio_hal.h"  // NO - HAL abstraction
#include <alsa/asoundlib.h>              // NO - ALSA specific (Linux)
#include <windows.h>                      // NO - Windows specific
#include <CoreAudio/CoreAudio.h>         // NO - macOS specific
```

### REQUIRED in Standards Layer
```cpp
// ✅ Only AES3-2009 standard protocol headers
#include "aes3_subframe.h"               // AES3-2009 Part 3: Transport (Subframe)
#include "aes3_channel_status.h"         // AES3-2009 Part 2: Metadata/Channel Status
#include "aes3_audio_sample.h"           // AES3-2009 Part 1: Audio Content
#include "aes3_biphase_mark.h"           // AES3-2009 Part 3: Biphase-Mark Coding
```

## Architecture Compliance

### Standards Layer Responsibility
- **AES3 Protocol state machines** (Subframe assembly, Frame/Block synchronization, Preamble detection)
- **Audio sample format handling** (Linear PCM encoding, word length justification, 2's complement)
- **Channel status decoding/encoding** (192-bit blocks, CRCC validation)
- **Biphase-mark coding/decoding** (Channel coding with DC balance, clock recovery)
- **Standard-defined structures** and constants (Preambles X/Y/Z, time slots, channel status bytes)
- **Protocol compliance validation** (AES5 sampling frequencies, validity bits, user data)

### Interface Pattern for Hardware Access
```cpp
// ✅ CORRECT: Dependency injection interface for audio hardware abstraction
typedef struct {
    int (*send_audio_frame)(const void* frame_data, size_t length);
    int (*receive_audio_frame)(void* buffer, size_t* length);
    uint64_t (*get_sample_clock_ns)(void);
    int (*set_sample_timer)(uint32_t sample_rate_hz, timer_callback_t callback);
} audio_interface_t;

// Protocol implementation receives interface
int aes3_transmitter_init(const audio_interface_t* audio_interface);
```

### Hardware Bridge (Service Layer Responsibility)
```cpp
// This code belongs in SERVICE layer (e.g., audio daemon), NOT Standards
static int platform_send_frame(const void* frame_data, size_t length) {
    return platform_audio_output(frame_data, length);  // Platform-specific call
}

static audio_interface_t platform_interface = {
    .send_audio_frame = platform_send_frame,
    .receive_audio_frame = platform_receive_frame,
    // ... other platform-specific implementations
};

// Service layer bridges Standards to Platform Audio Hardware
aes3_transmitter_init(&platform_interface);
```

## AES Standards Reference

### CRITICAL: Use Authoritative Standards Documents
When implementing protocols, reference these authoritative documents via MCP-Server "markitdown":

#### AES Audio Engineering Standards:
- `AES 3-1-2009 (R2014)-en.pdf` - AES3-1-2009 Digital Audio Interface Part 1: Audio Content - CURRENT
- `AES 3-2-2009 (R2014)-en.pdf` - AES3-2-2009 Digital Audio Interface Part 2: Metadata and Subcode - CURRENT
- `AES 3-3-2009 (R2014)-en.pdf` - AES3-3-2009 Digital Audio Interface Part 3: Transport - CURRENT
- `AES 3-4-2009 (R2014)-en.pdf` - AES3-4-2009 Digital Audio Interface Part 4: Physical and Electrical - CURRENT
- `AES 5-2018-en.pdf` - AES5-2018 Preferred Sampling Frequencies
- `AES 11-2009 (R2014)-en.pdf` - AES11-2009 Digital Audio Reference Signals
- `AES 67-2018-en.pdf` - AES67-2018 Audio-over-IP interoperability (related)
- `AES-70-1-2018-en.pdf` - AES70-1-2018 OCA Framework (related)

#### Related Standards:
- ITU-R BS.450-3 - Pre-emphasis for FM sound broadcasting
- ITU-T J.17 - Pre-emphasis used on sound-program circuits
- IEC 60958-1 - Digital audio interface - Part 1: General
- IEC 60958-3 - Digital audio interface - Part 3: Consumer applications  
- IEC 60958-4 - Digital audio interface - Part 4: Professional applications
- ISO 646 - 7-bit coded character set (for channel origin/destination data)

**CRITICAL COPYRIGHT AND LICENSING RESTRICTIONS**:

⚠️ **ABSOLUTELY FORBIDDEN**:
- **DO NOT copy any text, tables, figures, or content** directly from IEEE, AES, or AVnu documents
- **DO NOT reproduce specification text** verbatim in source code comments
- **DO NOT include copyrighted tables or figures** in repository documentation
- **DO NOT paste specification sections** into code or documentation files
- **DO NOT create derivative works** that reproduce substantial portions of standards

✅ **PERMITTED USAGE**:
- **Reference document sections** by number only (e.g., "See IEEE 1722.1-2021, Section 7.2.1")
- **Implement protocol logic** based on understanding of specifications
- **Create original code** that achieves compliance with standard requirements
- **Use specification constants and values** in implementation (packet types, field sizes, etc.)
- **Reference via MCP-Server** for compliance verification during development only

**LICENSE NOTICE**: These documents are referenced for compliance verification and implementation guidance only. All content remains under copyright of respective standards organizations (IEEE, AES, AVnu Alliance). Any reproduction, distribution, or derivative works require proper licensing from copyright holders.

**Usage Pattern**: When implementing standards-related code, reference these documents via MCP-Server to ensure:
- Protocol message format compliance
- State machine behavior correctness  
- Timing requirement adherence
- AES3 specification conformance
- Audio sample format compliance
- Channel status structure adherence

### Protocol Compliance Requirements
```cpp
// ✅ Use authoritative constants from AES3 specifications
#define AES3_PREAMBLE_X 0xE2  // First subframe preamble (channel coding: 11100010)
#define AES3_PREAMBLE_Y 0xE4  // Second subframe preamble (channel coding: 11100100)
#define AES3_PREAMBLE_Z 0xE8  // Block start preamble (channel coding: 11101000)
#define AES3_SUBFRAME_SLOTS 32  // Time slots per subframe
#define AES3_BLOCK_FRAMES 192   // Frames per channel status block

// ❌ NEVER use hardcoded values without AES3 specification reference
uint16_t magic_value = 0x1234;  // NO - not standards compliant
```

## Testing Approach

### Unit Testing (Hardware-Independent)
```cpp
// ✅ Mock network interface for testing
static int mock_send_packet(const void* packet, size_t length) {
    // Record packet for verification
    return 0;
}

static network_interface_t mock_interface = {
    .send_packet = mock_send_packet,
    // ... other mock implementations
};

// Test protocol logic without hardware
void test_avdecc_entity_descriptor_response(void) {
    avdecc_entity_init(&mock_interface);
    // ... test protocol behavior
}
```

### Integration Testing (Service Layer Responsibility)
Integration with real hardware happens in the Service Layer, NOT in Standards.

## Common Violations to Avoid

### ❌ Direct Hardware Calls
```cpp
// WRONG - Standards calling platform audio HAL directly
platform_audio_result_t result = platform_send_audio_frame(frame_data, length);
```

### ❌ OS-Specific Code
```cpp
// WRONG - OS-specific audio in Standards
#ifdef _WIN32
    HWAVEOUT wave_out = waveOutOpen(...);
#else
    int alsa_pcm = snd_pcm_open(...);
#endif
```

### ❌ Hardware Assumptions
```cpp
// WRONG - Assuming specific audio hardware capabilities
if (realtek_audio_supports_192khz()) {  // Hardware-specific assumption
    enable_high_sample_rate();
}
```

### ✅ Correct Abstraction
```cpp
// CORRECT - Protocol logic with abstracted capabilities
if (audio_interface->capabilities & AUDIO_CAP_192KHZ_SAMPLING) {
    enable_high_sample_rate();  // Protocol behavior, not hardware call
}
```

## Protocol Implementation Guidelines

### AES3-2009 Part 1: Audio Content
- Use linear PCM with 2's complement coding (16-24 bits)
- Implement word length justification to MSB
- Support AES5-2008 sampling frequencies (32kHz, 44.1kHz, 48kHz and multiples)
- Handle validity bit per sample according to Part 1 clause 6
- Support pre-emphasis indication (none, 50µs, J.17)
- Minimize DC content in coded audio

### AES3-2009 Part 2: Metadata and Subcode  
- Implement 192-bit channel status blocks (24 bytes)
- Support channel status byte 0: basic audio parameters (emphasis, lock, sampling frequency)
- Support channel status byte 1: channel modes, user bits management
- Support channel status byte 2: auxiliary bits, word length alignment
- Support channel status byte 23: CRCC validation (CRC-8 polynomial: x^8 + x^4 + x^3 + x^2 + 1)
- Handle user data channel (1 bit per subframe)
- Support alphanumeric channel origin/destination (ISO 646 IRV)

### AES3-2009 Part 3: Transport
- Implement subframe structure (32 time slots, each 2 UI)
- Support preambles X, Y, Z for synchronization (4 time slots)
- Implement biphase-mark channel coding for time slots 4-31
- Handle frame structure (2 subframes per frame)
- Support block structure (192 frames per block)
- Ensure even parity bit in time slot 31
- Implement proper preamble detection (violation of biphase-mark rules)

### AES3-2009 Part 4: Physical and Electrical
- Support jitter requirements (intrinsic jitter < 0.025 UI)
- Implement balanced transmission (110Ω XLR-3 connectors)
- Support coaxial transmission (75Ω BNC connectors) as alternative
- Handle signal amplitude requirements (2-7V peak-to-peak for balanced)
- Support rise/fall times (0.03 UI to 0.18 UI)
- Implement receiver sensitivity (Vmin 200mV, Tmin 0.5 UI for balanced)
- Support cable lengths up to 100m without equalization (balanced)

## Clean Submit Rules for Standards

- **Each commit compiles and passes AES3 compliance checks**
- **Small, single-purpose, reviewable diffs** (no WIP noise)
- **No dead or commented-out code** - remove unused protocol implementations
- **Run formatter and static analysis** before commit
- **Update documentation and reference AES3 spec sections** in commit messages
- **Use feature flags for incremental protocol changes** when they risk breaking AES3 compliance
- **Reference exact AES3 section numbers** (e.g., "Implements AES3-2009 Part 3 Section 4.1")

## Documentation Requirements with Copyright Compliance

### Function Documentation with AES Context
```cpp
/**
 * @brief Parse AES3-2009 Audio Subframe from network packet
 *
 * Validates and extracts audio subframe according to AES3-2009
 *
 */
int aes3_parse_audio_subframe(const uint8_t* packet_data, 
                               size_t packet_length,
                               aes3_audio_subframe_t* audio_subframe);

```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements entity descriptor parsing per AES3-2009 Section 7.2.1
#define ENTITY_DESCRIPTOR_TYPE 0x0000  // As specified in AES3-2009

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted IEEE content:
// "The entity_descriptor field shall contain the following sub-fields:
//  descriptor_type (16 bits): shall be set to ENTITY (0x0000)
//  descriptor_index (16 bits): shall be set to 0x0000..."
// This is direct reproduction of IEEE copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct entity_descriptor {
    uint16_t descriptor_type;    // AES3-2009 Table 7.1 
    uint16_t descriptor_index;   // AES3-2009 Table 7.1
    uint64_t entity_id;         // AES3-2009 Table 7.1
    uint64_t entity_model_id;   // AES3-2009 Table 7.1
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Build System Integration

### CMake Configuration for Standards
The Standards layer should compile independently:
```cmake
# Standards layer has NO hardware dependencies
add_library(aes3_standards STATIC
    src/aes3_audio_content.c      # Part 1: Audio Content
    src/aes3_channel_status.c     # Part 2: Metadata/Channel Status
    src/aes3_transport.c           # Part 3: Transport
    src/aes3_biphase_mark.c        # Part 3: Biphase-Mark Coding
)

# Only protocol headers, no hardware/HAL
target_include_directories(aes3_standards PUBLIC
    include/
)

# NO hardware libraries linked to Standards
# target_link_libraries(aes3_standards platform_audio_hal)  # ❌ WRONG
```

### Testing Framework Integration
Use unified testing framework for protocol validation:
```cmake
# Protocol compliance tests (hardware-independent)
add_executable(standards_protocol_tests
    tests/test_aes3_part1_compliance.c    # Audio content tests
    tests/test_aes3_part2_compliance.c    # Channel status tests
    tests/test_aes3_part3_compliance.c    # Transport tests
    tests/test_biphase_mark_coding.c      # Biphase-mark coding tests
)

target_link_libraries(standards_protocol_tests 
    aes3_standards 
    CppUTest
)
```

## Performance and Correctness

### Memory Management
- **Use static allocation** where possible for real-time protocol processing
- **Validate buffer bounds** against IEEE maximum packet sizes
- **Initialize all protocol structures** to prevent undefined behavior

### Timing Considerations
- **Nanosecond precision** for gPTP timing calculations
- **Microsecond accuracy** for AVTP presentation time
- **Avoid blocking operations** in protocol state machines

### Error Handling
- **Return IEEE-compliant error codes** where specifications define them
- **Log protocol violations** with specific IEEE section references
- **Graceful degradation** for optional protocol features

## Mandatory Namespace and Folder Structure

### Generic Structure Definition

**FUNDAMENTAL PATTERN**: All Standards implementations must follow this generic hierarchy:

```
<Organization>/<Standard>/<Subpart>/<Version>/
```

**Components Definition:**
- **Organization**: Standards body (IEEE, AVnu, AES, ITU, IETF, etc.)
- **Standard**: Standard family or main specification number  
- **Subpart**: Sub-specifications, working groups, or functional areas
- **Version**: Year of publication or version number

**Translation Rules:**
- **Folder Structure**: Preserve dots and organization naming (`IEEE/802.1/AS/2021/`)
- **Namespace Structure**: Convert dots to underscores, preserve organization (`IEEE::_802_1::AS::_2021`)
- **File Structure**: Follow folder pattern with appropriate extensions

### Concrete Examples by Organization

#### IEEE Standards Structure
```
IEEE/<Standard>/<Subpart>/<Year>/
├── IEEE/802.1/AS/2021/          # IEEE 802.1AS-2021 (gPTP)
├── IEEE/802.1/AS/2020/          # IEEE 802.1AS-2020 (previous version)
├── IEEE/802.1/Q/2018/           # IEEE 802.1Q-2018 (VLAN/QoS)
├── IEEE/1722/2016/              # IEEE 1722-2016 (AVTP)
├── IEEE/1722/2011/              # IEEE 1722-2011 (legacy AVTP)
├── IEEE/1722.1/2021/            # IEEE 1722.1-2021 (AVDECC)
├── IEEE/1722.1/2013/            # IEEE 1722.1-2013 (legacy AVDECC)
└── IEEE/1588/2019/              # IEEE 1588-2019 (PTPv2)
```

#### AVnu Alliance Structure  
```
AVnu/<Standard>/<Subpart>/<Version>/
├── AVnu/Milan/v1.2/             # Milan v1.2 professional audio
├── AVnu/Milan/v1.1/             # Milan v1.1 (legacy)
├── AVnu/Profiles/AVB/2.0/       # AVB interoperability profiles
└── AVnu/CertificationSuite/3.0/ # Certification test suites
```

#### Audio Engineering Society Structure
```
AES/<Standard>/<Subpart>/<Version>/
├── AES/AES3/2009/               # AES3-2009 digital audio interface (THIS PROJECT)
│   ├── Part1/                   # Audio Content
│   ├── Part2/                   # Metadata and Subcode
│   ├── Part3/                   # Transport
│   └── Part4/                   # Physical and Electrical
├── AES/AES5/2018/               # AES5-2018 preferred sampling frequencies
├── AES/AES11/2009/              # AES11-2009 digital audio reference signals
├── AES/AES67/2018/              # AES67-2018 audio-over-IP (related)
└── AES/AES70/2021/              # AES70-2021 device control OCA (related)
```

#### ITU-T Structure (if needed)
```
ITU/<Series>/<Standard>/<Version>/
├── ITU/G/G.8275.1/2016/         # ITU-T G.8275.1-2016 telecom PTP
└── ITU/G/G.8275.2/2017/         # ITU-T G.8275.2-2017 telecom PTP
```

### Required Directory Structure
```
lib/Standards/
├── AES/                        # Audio Engineering Society
│   └── AES3/                   # AES3 Digital Audio Interface
│       └── 2009/               # AES3-2009 specification (CURRENT)
│           ├── Part1/          # Audio Content
│           │   ├── audio_coding/     # PCM coding, word length
│           │   ├── sampling/         # Sampling frequency handling
│           │   ├── validity/         # Validity bit logic
│           │   └── conformity/       # Part 1 conformance testing
│           ├── Part2/          # Metadata and Subcode
│           │   ├── channel_status/   # 192-bit channel status blocks
│           │   ├── user_data/        # User data channel
│           │   ├── auxiliary/        # Auxiliary bits handling
│           │   └── conformity/       # Part 2 conformance testing
│           ├── Part3/          # Transport
│           │   ├── subframe/         # Subframe structure (32 time slots)
│           │   ├── frame/            # Frame structure (2 subframes)
│           │   ├── block/            # Block structure (192 frames)
│           │   ├── preambles/        # Preambles X, Y, Z
│           │   ├── biphase_mark/     # Biphase-mark coding
│           │   └── conformity/       # Part 3 conformance testing
│           └── Part4/          # Physical and Electrical
│               ├── balanced/         # Balanced transmission (110Ω)
│               ├── coaxial/          # Coaxial transmission (75Ω)
│               ├── optical/          # Optical transmission (future)
│               ├── jitter/           # Jitter requirements
│               └── conformity/       # Part 4 conformance testing
├── Related/                    # Related standards (optional)
│   ├── AES5/2018/             # Preferred sampling frequencies
│   ├── AES11/2009/            # Digital audio reference signals
│   ├── IEC60958/              # Consumer digital audio interface
│   └── ISO646/                # Character set for channel origin/dest
├── Common/                    # Cross-standard utilities
│   ├── utils/                 # Shared utilities (CRC, bit manipulation)
│   ├── testing/               # Common testing frameworks
│   └── interfaces/            # Hardware abstraction interfaces
└── Documentation/             # Standards documentation
    ├── conformance/           # Conformance test specifications
    ├── interop/              # Interoperability guidelines
    └── examples/             # Protocol usage examples
```

### Required C++ Namespace Structure Following Generic Pattern

**NAMESPACE TRANSLATION RULE**: `<Organization>::<Standard>::<Part>::<Version_with_underscores>`

**Examples of Translation:**
- Folder: `AES/AES3/2009/Part1/` → Namespace: `AES::AES3::Part1::_2009`
- Folder: `AES/AES3/2009/Part2/` → Namespace: `AES::AES3::Part2::_2009`
- Folder: `AES/AES3/2009/Part3/` → Namespace: `AES::AES3::Part3::_2009`
- Folder: `AES/AES5/2018/` → Namespace: `AES::AES5::_2018`

```cpp
// ✅ CORRECT namespace hierarchy for AES3-2009
namespace AES {
    namespace AES3 {            // Standard: AES3 (Digital Audio Interface)
        namespace Part1 {       // Part 1: Audio Content
            namespace _2009 {   // Version: 2009 (year)
                // AES3-2009 Part 1 implementation
                namespace audio_coding {
                    class PCMEncoder;
                    class WordLengthHandler;
                }
                namespace sampling {
                    class SamplingFrequencyManager;
                }
                namespace validity {
                    class ValidityBitHandler;
                }
            }
        }
        
        namespace Part2 {       // Part 2: Metadata and Subcode
            namespace _2009 {   // Version: 2009 (year)
                // AES3-2009 Part 2 implementation
                namespace channel_status {
                    class ChannelStatusBlock;
                    class CRCCValidator;
                }
                namespace user_data {
                    class UserDataChannel;
                }
                namespace auxiliary {
                    class AuxiliaryBitsHandler;
                }
            }
        }
        
        namespace Part3 {       // Part 3: Transport
            namespace _2009 {   // Version: 2009 (year)
                // AES3-2009 Part 3 implementation
                namespace subframe {
                    class SubframeBuilder;
                    class TimeSlotManager;
                }
                namespace frame {
                    class FrameAssembler;
                }
                namespace block {
                    class BlockSynchronizer;
                }
                namespace preambles {
                    class PreambleDetector;
                    // Preamble X, Y, Z handlers
                }
                namespace biphase_mark {
                    class BiphaseMarkCoder;
                    class BiphaseMarkDecoder;
                }
            }
        }
        
        namespace Part4 {       // Part 4: Physical and Electrical
            namespace _2009 {   // Version: 2009 (year)
                // AES3-2009 Part 4 implementation
                namespace balanced {
                    class BalancedTransmitter;
                    class BalancedReceiver;
                }
                namespace coaxial {
                    class CoaxialTransmitter;
                    class CoaxialReceiver;
                }
                namespace jitter {
                    class JitterAnalyzer;
                    class JitterFilter;
                }
            }
        }
    }
    
    namespace AES5 {            // Related Standard: AES5 (Sampling Frequencies)
        namespace _2018 {       // Version: 2018 (year)
            class PreferredSamplingFrequencies;
        }
    }
    
    namespace AES11 {           // Related Standard: AES11 (Reference Signals)
        namespace _2009 {       // Version: 2009 (year)
            class DigitalAudioReferenceSignal;
        }
    }
} // namespace AES

namespace Common {              // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class AudioInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class CRCCalculator;
        class BitManipulator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```
                    struct FollowUpMessage;
                }
                namespace clock {
                    class BestMasterClockAlgorithm;
                }
                namespace Testing {
                    class ConformityTestFramework;
                }
            }
            namespace _2020 {    // Version: 2020 (previous year)
                // IEEE 802.1AS-2020 implementation
            }
            namespace _2011 {    // Version: 2011 (legacy year)
                // IEEE 802.1AS-2011 legacy implementation
            }
        }
        namespace Q {            // Subpart: Q (VLAN and QoS)
            namespace _2018 {    // Version: 2018
                // IEEE 802.1Q-2018 implementation
            }
        }
    }
    
    namespace _1722 {            // Standard: 1722 (AVTP)
        namespace _2016 {        // Version: 2016 (year)
            // IEEE 1722-2016 AVTP implementation
            namespace avtp {
                class AVTPPacket;
                class StreamDataHeader;
            }
            namespace formats {
                namespace aaf {
                    class AudioFormat;
                }
                namespace crf {
                    class ClockReferenceFormat;
                }
            }
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722-2011 legacy implementation
        }
    }
    
    namespace _1722_1 {          // Standard: 1722.1 (dots→underscores)
        namespace _2021 {        // Version: 2021 (year)
            // IEEE 1722.1-2021 AVDECC implementation
            namespace aem {
                class EntityModel;
                class ConfigurationDescriptor;
            }
            namespace aecp {
                class EntityControlProtocol;
                class AEMCommand;
            }
            namespace acmp {
                class ConnectionManagementProtocol;
                class ConnectTXCommand;
            }
            namespace adp {
                class DiscoveryProtocol;
                class EntityAvailable;
            }
            namespace descriptors {
                class EntityDescriptor;
                class StreamInputDescriptor;
                class StreamOutputDescriptor;
            }
        }
        namespace _2013 {        // Version: 2013 (legacy year)
            // IEEE 1722.1-2013 legacy implementation
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722.1-2011 legacy implementation
        }
    }
    
    namespace _1588 {            // Standard: 1588 (PTPv2)
        namespace _2019 {        // Version: 2019 (year)
            // IEEE 1588-2019 PTPv2 implementation
        }
    }
} // namespace IEEE

namespace AVnu {                 // Organization: AVnu Alliance
    namespace Milan {            // Standard: Milan professional audio
        namespace v1_2 {         // Version: v1.2 (version number)
            // Milan v1.2 extensions
            namespace discovery {
                class MilanDiscoveryExtensions;
            }
            namespace connection {
                class RedundantStreamPairs;
            }
        }
        namespace v1_1 {         // Version: v1.1 (legacy version)
            // Milan v1.1 legacy implementation
        }
    }
    namespace Profiles {         // Standard: Profiles
        namespace AVB {          // Subpart: AVB
            namespace _2_0 {     // Version: 2.0 (dots→underscores)
                // AVB Profile 2.0 implementation
            }
        }
    }
} // namespace AVnu

namespace AES {                  // Organization: Audio Engineering Society
    namespace AES67 {            // Standard: AES67 (audio-over-IP)
        namespace _2018 {        // Version: 2018 (year)
            class AudioOverIP;
        }
    }
    namespace AES70 {            // Standard: AES70 (device control)
        namespace _2021 {        // Version: 2021 (year)
            class DeviceControl;
        }
    }
} // namespace AES

namespace ITU {                  // Organization: ITU-T (if needed)
    namespace G {                // Standard: G-series recommendations
        namespace G8275_1 {      // Subpart: G.8275.1 (dots→underscores)
            namespace _2016 {    // Version: 2016 (year)
                // ITU-T G.8275.1-2016 telecom PTP
            }
        }
    }
} // namespace ITU

namespace Common {               // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class NetworkInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class PacketParser;
        class CRCCalculator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```

### File Naming Conventions Following Generic Pattern

**PATTERN**: `<Organization>/<Standard>/<Part>/<Version>/<functional_area>/<file_name>.<ext>`

```cpp
// ✅ CORRECT file naming following generic pattern

// AES3 Examples:
lib/Standards/AES/AES3/2009/Part1/audio_coding/pcm_encoder.hpp          // AES::AES3::Part1::_2009::audio_coding
lib/Standards/AES/AES3/2009/Part1/audio_coding/pcm_encoder.cpp
lib/Standards/AES/AES3/2009/Part1/sampling/sampling_freq_manager.hpp    // AES::AES3::Part1::_2009::sampling
lib/Standards/AES/AES3/2009/Part1/validity/validity_bit_handler.hpp     // AES::AES3::Part1::_2009::validity

lib/Standards/AES/AES3/2009/Part2/channel_status/channel_status_block.hpp  // AES::AES3::Part2::_2009::channel_status
lib/Standards/AES/AES3/2009/Part2/channel_status/crcc_validator.hpp     // AES::AES3::Part2::_2009::channel_status
lib/Standards/AES/AES3/2009/Part2/user_data/user_data_channel.hpp       // AES::AES3::Part2::_2009::user_data

lib/Standards/AES/AES3/2009/Part3/subframe/subframe_builder.hpp         // AES::AES3::Part3::_2009::subframe
lib/Standards/AES/AES3/2009/Part3/frame/frame_assembler.hpp             // AES::AES3::Part3::_2009::frame
lib/Standards/AES/AES3/2009/Part3/block/block_synchronizer.hpp          // AES::AES3::Part3::_2009::block
lib/Standards/AES/AES3/2009/Part3/preambles/preamble_detector.hpp       // AES::AES3::Part3::_2009::preambles
lib/Standards/AES/AES3/2009/Part3/biphase_mark/biphase_coder.hpp        // AES::AES3::Part3::_2009::biphase_mark

lib/Standards/AES/AES3/2009/Part4/balanced/balanced_transmitter.hpp     // AES::AES3::Part4::_2009::balanced
lib/Standards/AES/AES3/2009/Part4/coaxial/coaxial_transmitter.hpp       // AES::AES3::Part4::_2009::coaxial
lib/Standards/AES/AES3/2009/Part4/jitter/jitter_analyzer.hpp            // AES::AES3::Part4::_2009::jitter

// Related Standards:
lib/Standards/AES/AES5/2018/sampling/preferred_sampling_freqs.hpp       // AES::AES5::_2018::sampling
lib/Standards/AES/AES11/2009/reference/digital_audio_ref_signal.hpp     // AES::AES11::_2009::reference

// Common utilities (organization-agnostic):
lib/Standards/Common/interfaces/audio_interface.hpp                      // Common::interfaces
lib/Standards/Common/utils/crc_calculator.hpp                            // Common::utils
lib/Standards/Common/testing/test_framework_base.hpp                     // Common::testing
```

### Header Guard and Include Conventions Following Actual Pattern
```cpp
// ✅ CORRECT header guards following AES namespace structure
#ifndef AES_AES3_2009_PART1_AUDIO_CODING_PCM_ENCODER_H
#define AES_AES3_2009_PART1_AUDIO_CODING_PCM_ENCODER_H

// ✅ CORRECT include structure - relative paths from AES namespace
#include "../sampling/sampling_freq_manager.h"
#include "../../../../Common/interfaces/audio_interface.h"

namespace AES {
namespace AES3 {
namespace Part1 {
namespace _2009 {
namespace audio_coding {

class PCMEncoder {
    // AES3-2009 Part 1 compliant implementation
};

} // namespace audio_coding
} // namespace _2009
} // namespace Part1
} // namespace AES3
} // namespace AES

#endif // AES_AES3_2009_PART1_AUDIO_CODING_PCM_ENCODER_H
```

### Correct Include Patterns for Cross-Standard Dependencies
```cpp
// ✅ CORRECT - AES standards can reference each other
#include "../../AES5/2018/sampling/preferred_sampling_freqs.h"  // AES3 using AES5
#include "../../AES11/2009/reference/digital_audio_ref_signal.h" // AES3 using AES11

// ✅ CORRECT - Common utilities accessible to all standards
#include "../../../Common/interfaces/audio_interface.h"
#include "../../../Common/utils/crc_calculator.h"

// ❌ WRONG - No hardware-specific includes in AES namespace
// #include "../../../../../realtek_audio/include/realtek_hal.h"  // NO!
// #include "../../../../../common/hal/audio_hal.h"               // NO!
```

### Cross-Standard Reuse and Dependencies

**MANDATORY RULE**: When an IEEE standard references or builds upon another IEEE standard, **ALWAYS reuse the existing implementation** rather than creating redundant code.

#### Examples of Required Cross-Standard Reuse:

**IEEE 1722.1 (AVDECC) Dependencies:**
```cpp
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aecp {

// ✅ CORRECT - Reuse IEEE 1722 AVTP implementation
#include "../../../1722/2016/avtp/avtp_packet.h"
using IEEE::_1722::_2016::avtp::AVTPPacket;

// ✅ CORRECT - Reuse IEEE 802.1AS time synchronization
#include "../../../802.1/AS/2021/core/time_sync.h"
using IEEE::_802_1::AS::_2021::core::TimeSynchronization;

class AEMCommand {
    // AVDECC commands are transported over AVTP
    IEEE::_1722::_2016::avtp::AVTPPacket create_avtp_packet() {
        // Reuse AVTP implementation, don't reimplement
        return IEEE::_1722::_2016::avtp::AVTPPacket::create_aecp_packet();
    }
    
    // AVDECC requires synchronized time from gPTP
    uint64_t get_synchronized_time() {
        // Reuse gPTP time, don't reimplement time sync
        return IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
    }
};

} // namespace aecp
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

**IEEE 1722 (AVTP) Dependencies:**
```cpp
namespace IEEE {
namespace _1722 {
namespace _2016 {
namespace avtp {

// ✅ CORRECT - Reuse IEEE 802.1AS timing for presentation time
#include "../../../802.1/AS/2021/core/time_sync.h"

class StreamDataHeader {
    uint64_t calculate_presentation_time(uint32_t delay_ns) {
        // Reuse gPTP synchronized time, don't reimplement
        auto current_time = IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
        return current_time + delay_ns;
    }
};

} // namespace avtp
} // namespace _2016
} // namespace _1722
} // namespace IEEE
```

**Milan Extensions Dependencies:**
```cpp
namespace AVnu {
namespace Milan {
namespace v1_2 {
namespace discovery {

// ✅ CORRECT - Milan builds on IEEE 1722.1, reuse implementation
#include "../../../../IEEE/1722.1/2021/adp/discovery_protocol.h"
#include "../../../../IEEE/1722.1/2021/aem/entity_model.h"

class MilanDiscoveryExtensions : public IEEE::_1722_1::_2021::adp::DiscoveryProtocol {
    // Milan extends IEEE 1722.1 AVDECC, inherit don't reimplement
public:
    // Milan-specific discovery features
    void discover_milan_devices() {
        // Use base IEEE 1722.1 discovery, add Milan extensions
        DiscoveryProtocol::discover_devices();
        apply_milan_filtering();
    }
    
private:
    void apply_milan_filtering() {
        // Milan-specific logic only
    }
};

} // namespace discovery
} // namespace v1_2
} // namespace Milan
} // namespace AVnu
```

#### Forbidden Redundant Implementations:
```cpp
// ❌ WRONG - Reimplementing existing IEEE standards
namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// DON'T DO THIS - AVTP already exists in IEEE::_1722
class AVDECCTransportPacket {  // NO - use IEEE::_1722::_2016::avtp::AVTPPacket
    // ... redundant AVTP implementation
};

// DON'T DO THIS - gPTP already exists in IEEE::_802_1::AS
class AVDECCTimeSync {  // NO - use IEEE::_802_1::AS::_2021::core::TimeSynchronization
    // ... redundant time sync implementation
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

#### Cross-Standard Dependency Rules:
1. **IEEE Layering Hierarchy** (higher layers depend on lower layers):
   - **Application Layer**: IEEE 1722.1 (AVDECC)
   - **Transport Layer**: IEEE 1722 (AVTP) 
   - **Timing Layer**: IEEE 802.1AS (gPTP)
   - **Network Layer**: IEEE 802.1Q (VLAN/QoS)

2. **Dependency Direction**: 
   - ✅ **IEEE 1722.1 CAN depend on IEEE 1722 and IEEE 802.1AS**
   - ✅ **IEEE 1722 CAN depend on IEEE 802.1AS**
   - ❌ **IEEE 802.1AS CANNOT depend on IEEE 1722 or IEEE 1722.1**

3. **Extension Standards**:
   - ✅ **Milan CAN depend on any IEEE standard it extends**
   - ✅ **AES67 CAN depend on IEEE standards it references**
   - ✅ **Multiple standard versions CAN coexist** (2021, 2016, 2013)

#### CMake Dependencies for Cross-Standard Reuse:
```cmake
# ✅ CORRECT - Respect IEEE layering in CMake dependencies
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)

target_link_libraries(ieee_1722_2016
    ieee_802_1_as_2021          # AVTP depends on gPTP for timing
    standards_common
)

target_link_libraries(avnu_milan_v12
    ieee_1722_1_2021            # Milan extends IEEE 1722.1
    ieee_1722_2016              # Milan may use AVTP directly
    ieee_802_1_as_2021          # Milan requires precise timing
    standards_common
)

# ❌ WRONG - Violates layering hierarchy
# target_link_libraries(ieee_802_1_as_2021
#     ieee_1722_2016            # NO - gPTP cannot depend on AVTP
# )
```

This approach ensures:
- **No code duplication** across IEEE standards
- **Proper architectural layering** following IEEE specifications  
- **Consistent behavior** when standards reference each other
- **Maintainable codebase** with single source of truth for each protocol feature

### Forbidden Namespace Violations - Corrected
```cpp
// ❌ WRONG - mixing standards with hardware (corrected understanding)
namespace IEEE {
namespace intel {              // NO - hardware vendor in IEEE namespace
    class IntelAVBInterface;
}
}

// ❌ WRONG - OS-specific namespaces in IEEE standards
namespace IEEE {
namespace windows {            // NO - OS specific in IEEE namespace
    class WinSockInterface;
}
}

// ❌ WRONG - implementation details in IEEE namespace  
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
    class WindowsSocketImpl;  // NO - implementation detail, not protocol
}
}
}

// ✅ CORRECT - IEEE standards are pure protocol implementations
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {
    class EntityModel;        // YES - pure IEEE 1722.1-2021 protocol
}
}
}
}
```

### CMake Integration with Correct Structure
```cmake
# ✅ CORRECT CMake structure following actual IEEE hierarchy
add_library(ieee_802_1_as_2021 STATIC
    IEEE/802.1/AS/2021/core/gptp_state_machine.cpp
    IEEE/802.1/AS/2021/messages/sync_message.cpp
    IEEE/802.1/AS/2021/clock/best_master_clock.cpp
    IEEE/802.1/AS/2021/core/conformity_test_framework.cpp
)

add_library(ieee_1722_1_2021 STATIC
    IEEE/1722.1/2021/aem/entity_model.cpp
    IEEE/1722.1/2021/aecp/aem_command.cpp
    IEEE/1722.1/2021/descriptors/entity_descriptor.cpp
)

add_library(ieee_1722_2016 STATIC
    IEEE/1722/2016/avtp/avtp_packet.cpp
    IEEE/1722/2016/formats/aaf/audio_format.cpp
)

add_library(avnu_milan_v12 STATIC
    AVnu/Milan/v1.2/discovery/milan_discovery.cpp
    AVnu/Milan/v1.2/connection/redundant_streams.cpp
)

add_library(standards_common STATIC
    Common/interfaces/network_interface.cpp
    Common/utils/packet_parser.cpp
    Common/testing/test_framework_base.cpp
)

# IEEE standards libraries can depend on each other following IEEE layering
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)
```

### Documentation Structure Requirements - Corrected
```cpp
/**
 * @file conformity_test_framework.h
 * @brief IEEE 802.1AS-2021 Conformity Testing Framework
 * @namespace IEEE::_802_1::AS::_2021::Testing
 * 
 * Implements conformity testing according to IEEE 802.1AS-2021 specification.
 * This namespace contains all testing functionality for validating IEEE 802.1AS-2021
 * compliance including state machine behavior, timing requirements, and interoperability.
 * 
 * @see IEEE 802.1AS-2021, Clauses 11.2-11.5 "Conformance requirements"
 * @see IEEE 802.1AS-2021, Annex A "Implementation conformance statement (ICS)"
 */
```

### Enforcement Rules - Corrected with Copyright Compliance
1. **IEEE namespaces are top-level** - not wrapped in `openavnu::standards`
2. **Namespace must match folder structure** exactly (`IEEE::_802_1::AS::_2021` = `IEEE/802.1/AS/2021/`)
3. **Version numbers use underscores** in namespaces (`_2021`, `_2016`) to avoid conflicts
4. **Dots become underscores** in namespaces (`802.1` becomes `_802_1`, `1722.1` becomes `_1722_1`)
5. **No hardware vendors** in IEEE namespace hierarchy
6. **No OS-specific namespaces** in IEEE standards
7. **IEEE layering respected** - higher layer standards can depend on lower layers
8. **Cross-standard utilities** only in Common namespace
9. **Conformance testing** isolated in Testing sub-namespaces
10. **COPYRIGHT COMPLIANCE MANDATORY**:
    - **NEVER reproduce copyrighted specification content** in source code or documentation
    - **Reference specifications by section number only** (e.g., "IEEE 1722.1-2021, Section 7.2.1")
    - **Implement based on understanding**, not by copying specification text
    - **Use MCP-Server for compliance verification only**, not content reproduction
    - **Respect all copyright holders**: IEEE, AES, AVnu Alliance, ITU, etc.
    - **Include copyright disclaimer** in implementations referencing multiple standards
    - **Document original implementation** that achieves compliance through understanding

### Repository Copyright Policy
```cpp
// ✅ REQUIRED copyright notice for standards-based implementations
/*
 * This file implements protocol functionality based on understanding of:
 * - IEEE 1722.1-2021 (AVDECC) - Copyright IEEE
 * - IEEE 1722-2016 (AVTP) - Copyright IEEE  
 * - IEEE 802.1AS-2021 (gPTP) - Copyright IEEE
 * - Milan v1.2 - Copyright AVnu Alliance
 * 
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through
 * understanding of specification requirements.
 * 
 * For authoritative requirements, refer to original specifications
 * available from respective standards organizations.
 */
```

This structure ensures clear separation of IEEE standards versions, prevents architectural violations, and maintains the hardware-agnostic principle while following the actual implementation pattern used in the codebase.

This architecture ensures the Standards layer remains pure, testable, reusable across different hardware platforms, and maintains strict IEEE compliance while following OpenAvnu's core development principles.

## ⚠️ MANDATORY: YAML Front Matter Schema Compliance

**CRITICAL ENFORCEMENT**: All specification files MUST use EXACT YAML front matter format per authoritative schemas.

**Authoritative Schema Sources** (SINGLE SOURCE OF TRUTH):
- Requirements: `spec-kit-templates/schemas/requirements-spec.schema.json`  
- Architecture: `spec-kit-templates/schemas/architecture-spec.schema.json`
- Design: `spec-kit-templates/schemas/ieee-design-spec.schema.json`
- Phase Gates: `spec-kit-templates/schemas/phase-gate-validation.schema.json`

**ZERO TOLERANCE POLICY**: 
- ❌ DO NOT modify schemas to fit incorrect front matter
- ❌ DO NOT use alternative YAML formats  
- ❌ DO NOT use full standard names where schemas expect short patterns
- ✅ ALWAYS reference authoritative schema files for format
- ✅ ALWAYS validate against schemas before submitting
- ✅ ALWAYS use phase-specific copilot-instructions for examples

**CI ENFORCEMENT**: Validation failures will block CI pipeline and prevent merges. There are no exceptions to schema compliance.