# AES3-2009 Part 3: Transport

**Location**: `lib/Standards/AES/AES3/2009/Part3/`  
**Namespace**: `AES::AES3::Part3::_2009`

## Overview

Part 3 implements transport layer according to AES3-3-2009 (R2014):
- Subframe structure (32 time slots)
- Preambles X, Y, Z (biphase-mark violations)
- Frame and block hierarchy
- Biphase-mark channel coding
- Even parity error detection

## Requirements

Implements 13 requirements from `02-requirements/functional/aes3-part3-transport-requirements.md`:

- **REQ-FUNC-TRANS-001**: Subframe Time Slot Structure
- **REQ-FUNC-TRANS-002**: Subframe Time Slot Allocation
- **REQ-FUNC-TRANS-003**: Preamble X, Y, Z Patterns
- **REQ-FUNC-TRANS-004**: Preamble Detection and Synchronization
- **REQ-FUNC-TRANS-005**: Frame Structure
- **REQ-FUNC-TRANS-006**: Block Structure
- **REQ-FUNC-TRANS-007**: Biphase-Mark Encoding Principle
- **REQ-FUNC-TRANS-008**: Biphase-Mark Decoding and Clock Recovery
- **REQ-FUNC-TRANS-009**: Even Parity Generation and Validation
- **REQ-PERF-TRANS-001**: Real-Time Biphase-Mark Encoding
- **REQ-PERF-TRANS-002**: Real-Time Biphase-Mark Decoding
- **REQ-PERF-TRANS-003**: Preamble Detection Latency
- **REQ-QUAL-TRANS-001**: AES3-2009 Part 3 Conformity Testing

## Module Structure

```
Part3/
├── subframe/                   # 32 time slot structure
│   ├── subframe_builder.h      # Assemble subframe from components
│   ├── subframe_builder.cpp
│   ├── time_slot_manager.h     # Time slot allocation (0-31)
│   └── time_slot_manager.cpp
├── frame/                      # 2 subframes = 1 frame
│   ├── frame_assembler.h       # Frame construction (X-Y alternation)
│   └── frame_assembler.cpp
├── block/                      # 192 frames = 1 block
│   ├── block_synchronizer.h    # Block sync with preamble Z
│   └── block_synchronizer.cpp
├── preambles/                  # Preambles X, Y, Z
│   ├── preamble_detector.h     # Detect biphase-mark violations
│   ├── preamble_detector.cpp
│   ├── preamble_generator.h    # Generate X, Y, Z patterns
│   └── preamble_generator.cpp
├── biphase_mark/               # Biphase-mark coding
│   ├── biphase_coder.h         # Encode to biphase-mark
│   ├── biphase_coder.cpp
│   ├── biphase_decoder.h       # Decode from biphase-mark
│   ├── biphase_decoder.cpp
│   ├── clock_recovery.h        # Extract clock from transitions
│   └── clock_recovery.cpp
└── conformity/                 # Part 3 conformance testing
    └── conformity_tests.cpp
```

## Key Constants

```cpp
// Subframe Structure
#define SUBFRAME_TIME_SLOTS      32   // Time slots per subframe (0-31)
#define TIME_SLOT_UI             2    // Unit Intervals per time slot
#define SUBFRAME_TOTAL_UI        64   // Total UI per subframe (32 × 2)

// Time Slot Allocation (AES3-3 Table 1)
#define TIMESLOT_PREAMBLE_START  0    // Preamble occupies slots 0-3
#define TIMESLOT_PREAMBLE_END    3
#define TIMESLOT_AUX_START       4    // Auxiliary bits (LSBs 0-3)
#define TIMESLOT_AUX_END         7
#define TIMESLOT_AUDIO_START     8    // Main audio word (bits 4-23)
#define TIMESLOT_AUDIO_END       27
#define TIMESLOT_VALIDITY        28   // Validity bit (V)
#define TIMESLOT_USER_DATA       29   // User data bit (U)
#define TIMESLOT_CHANNEL_STATUS  30   // Channel status bit (C)
#define TIMESLOT_PARITY          31   // Even parity bit (P)

// Preamble Patterns (biphase-mark violations)
#define PREAMBLE_X               0xE2  // 11100010 - First subframe
#define PREAMBLE_Y               0xE4  // 11100100 - Second subframe
#define PREAMBLE_Z               0xE8  // 11101000 - Block start

// Frame and Block Structure
#define SUBFRAMES_PER_FRAME      2     // 2 subframes = 1 frame
#define FRAMES_PER_BLOCK         192   // 192 frames = 1 block
#define SUBFRAMES_PER_BLOCK      384   // 384 subframes = 1 block

// Biphase-Mark Coding
#define BIPHASE_LOGIC_0_TRANSITIONS  1  // Logic 0: transition at start only
#define BIPHASE_LOGIC_1_TRANSITIONS  2  // Logic 1: transitions at start + mid

// UI Calculation
// UI (nanoseconds) = 1000000000 / (2 × sampling_frequency_hz)
// Example: 48 kHz → UI = 10417 ns
```

## Example Usage

```cpp
#include "AES/AES3/2009/Part3/subframe/subframe_builder.h"
#include "AES/AES3/2009/Part3/preambles/preamble_generator.h"
#include "AES/AES3/2009/Part3/biphase_mark/biphase_coder.h"

using namespace AES::AES3::Part3::_2009;

// Build subframe
subframe::SubframeBuilder builder;

// Add preamble X (first subframe)
preambles::PreambleGenerator preamble_gen;
builder.set_preamble(preamble_gen.generate_x());

// Add 24-bit audio sample (time slots 4-27)
uint32_t audio_sample = 0x123456;  // 24-bit audio
builder.set_audio_sample(audio_sample);

// Add metadata bits (time slots 28-30)
builder.set_validity_bit(false);      // Logic 0 = PCM
builder.set_user_data_bit(true);      // User data
builder.set_channel_status_bit(true); // Channel status

// Compute and add parity (time slot 31)
builder.compute_and_set_parity();

// Encode to biphase-mark
biphase_mark::BiphaseMarkCoder coder;
std::vector<bool> biphase_signal = coder.encode(builder.get_subframe());

// Transmit via HAL
for (bool bit : biphase_signal) {
    hal_transmit_bit(bit, ui_duration_ns);
}
```

## Dependencies

- **Part 1 Integration**: Audio samples in time slots 4-27
- **Part 2 Integration**: User data (slot 29), channel status (slot 30)
- **HAL Interface**: `hal_transmit_bit()`, `hal_receive_bit()` for I/O

## Testing

Unit tests: `tests/unit/part3/`
- Subframe structure encoding/decoding
- Preamble X/Y/Z detection accuracy
- Biphase-mark coding/decoding correctness
- Frame/block synchronization
- Even parity generation/validation

## Standards References

- **[AES3-3]** AES3-3-2009 (R2014) Digital Audio Interface Part 3: Transport
- **[AES3-1]** Part 1: Audio Content (time slot audio data)
- **[AES3-2]** Part 2: Metadata (time slot metadata bits)

---

**Status**: Structure created (Phase 02, November 2025)  
**Next**: Implement subframe builder and biphase-mark coder
