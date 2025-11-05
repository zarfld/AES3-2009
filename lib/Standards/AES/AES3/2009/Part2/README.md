# AES3-2009 Part 2: Metadata and Subcode

**Location**: `lib/Standards/AES/AES3/2009/Part2/`  
**Namespace**: `AES::AES3::Part2::_2009`

## Overview

Part 2 implements metadata and subcode according to AES3-2-2009 (R2014):
- Channel status blocks (192 bits = 24 bytes)
- CRCC validation (CRC-8 polynomial)
- User data channel (1 bit per subframe)
- Auxiliary bits handling

## Requirements

Implements 9 requirements from `02-requirements/functional/aes3-part2-metadata-subcode-requirements.md`:

- **REQ-FUNC-META-001**: User Data Bit Transmission
- **REQ-FUNC-META-002**: Channel Status Bit Transmission
- **REQ-FUNC-META-003**: Channel Status Block Structure
- **REQ-FUNC-META-004**: Channel Status Byte 0 (Basic Parameters)
- **REQ-FUNC-META-005**: Channel Status Byte 2 (Word Length)
- **REQ-FUNC-META-006**: Channel Status Byte 23 (CRCC)
- **REQ-FUNC-META-007**: Auxiliary Bits Availability
- **REQ-PERF-META-001**: Channel Status Block Synchronization
- **REQ-QUAL-META-001**: AES3-2009 Part 2 Conformity Testing

## Module Structure

```
Part2/
├── channel_status/             # 192-bit channel status blocks
│   ├── channel_status_block.h  # 24-byte block management
│   ├── channel_status_block.cpp
│   ├── byte0_handler.h         # Basic audio parameters
│   ├── byte0_handler.cpp
│   ├── byte2_handler.h         # Word length indication
│   ├── byte2_handler.cpp
│   ├── crcc_validator.h        # CRC-8 validation (byte 23)
│   └── crcc_validator.cpp
├── user_data/                  # User data channel
│   ├── user_data_channel.h     # 1 bit/subframe handling
│   └── user_data_channel.cpp
├── auxiliary/                  # Auxiliary bits (LSBs 0-3)
│   ├── auxiliary_bits_handler.h
│   └── auxiliary_bits_handler.cpp
└── conformity/                 # Part 2 conformance testing
    └── conformity_tests.cpp
```

## Key Constants

```cpp
// Channel Status Block Structure
#define CHANNEL_STATUS_BLOCK_FRAMES  192  // Frames per block
#define CHANNEL_STATUS_BLOCK_BYTES   24   // Bytes per block (0-23)
#define CHANNEL_STATUS_BLOCK_BITS    192  // Bits per block

// Channel Status Byte 0 (Basic Parameters)
#define CS_BYTE0_PROFESSIONAL        0x01  // Bit 0: Professional use
#define CS_BYTE0_LINEAR_PCM          0x00  // Bit 1: Linear PCM (0) / Non-PCM (1)
#define CS_BYTE0_NON_PCM             0x02  // Bit 1: Non-PCM content

// Pre-emphasis (Byte 0 Bits 2-4)
#define CS_BYTE0_EMPHASIS_NONE       0x04  // 001: No emphasis
#define CS_BYTE0_EMPHASIS_50_15_US   0x0C  // 011: 50µs+15µs
#define CS_BYTE0_EMPHASIS_J17        0x1C  // 111: J.17

// Lock indication (Byte 0 Bit 5)
#define CS_BYTE0_LOCKED              0x00  // Bit 5: 0 = locked
#define CS_BYTE0_UNLOCKED            0x20  // Bit 5: 1 = unlocked

// Sampling Frequency (Byte 0 Bits 6-7)
#define CS_BYTE0_FS_48KHZ            0x40  // 01: 48 kHz
#define CS_BYTE0_FS_44_1KHZ          0x80  // 10: 44.1 kHz
#define CS_BYTE0_FS_32KHZ            0xC0  // 11: 32 kHz
#define CS_BYTE0_FS_NOT_INDICATED    0x00  // 00: Not indicated

// Channel Status Byte 2 (Word Length)
#define CS_BYTE2_AUX_MAX_20_UNDEF    0x00  // 000: Max 20 bits, aux undefined
#define CS_BYTE2_AUX_MAX_24_AUDIO    0x04  // 100: Max 24 bits, aux for audio
#define CS_BYTE2_AUX_MAX_20_COORD    0x02  // 010: Max 20 bits, aux for coord

// CRCC Polynomial (Byte 23)
#define CRCC_POLYNOMIAL              0x1D  // G(x) = x^8 + x^4 + x^3 + x^2 + 1
#define CRCC_INITIAL_VALUE           0xFF  // Initial condition: all ones
```

## Example Usage

```cpp
#include "AES/AES3/2009/Part2/channel_status/channel_status_block.h"
#include "AES/AES3/2009/Part2/channel_status/crcc_validator.h"

using namespace AES::AES3::Part2::_2009;

// Create channel status block
channel_status::ChannelStatusBlock cs_block;

// Configure byte 0 (basic parameters)
uint8_t byte0 = CS_BYTE0_PROFESSIONAL |  // Professional use
                CS_BYTE0_LINEAR_PCM |    // Linear PCM content
                CS_BYTE0_EMPHASIS_NONE | // No pre-emphasis
                CS_BYTE0_LOCKED |        // Clock locked
                CS_BYTE0_FS_48KHZ;       // 48 kHz sampling
cs_block.set_byte(0, byte0);

// Configure byte 2 (word length)
uint8_t byte2 = CS_BYTE2_AUX_MAX_24_AUDIO | 0x28;  // 24-bit audio (bits 3-5 = 101)
cs_block.set_byte(2, byte2);

// Compute and set CRCC (byte 23)
channel_status::CRCCValidator crcc;
uint8_t crcc_value = crcc.compute(cs_block.get_bytes(), 23);  // Bytes 0-22
cs_block.set_byte(23, crcc_value);

// Validate CRCC
bool valid = crcc.validate(cs_block.get_bytes(), 24);  // All 24 bytes
```

## Dependencies

- **Part 3 Integration**: Time slots 29 (user data), 30 (channel status)
- **Part 1 Integration**: Byte 0 (PCM/sampling freq), Byte 2 (word length)
- **HAL Interface**: `hal_get_clock_lock_status()` for byte 0 bit 5

## Testing

Unit tests: `tests/unit/part2/`
- Channel status block encoding/decoding
- CRCC computation with AES3 Annex C test vectors
- Byte 0/2 configuration correctness
- User data channel transmission

## Standards References

- **[AES3-2]** AES3-2-2009 (R2014) Digital Audio Interface Part 2: Metadata and Subcode
- **[ISO646]** ISO 646 7-bit Coded Character Set (channel origin/destination)
- **[IEC60958]** IEC 60958-1/-3/-4 Digital Audio Interface (compatibility)

---

**Status**: Structure created (Phase 02, November 2025)  
**Next**: Implement channel status block and CRCC validator
