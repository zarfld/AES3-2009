# AES3-2009 Part 1: Audio Content

**Location**: `lib/Standards/AES/AES3/2009/Part1/`  
**Namespace**: `AES::AES3::Part1::_2009`

## Overview

Part 1 implements audio content coding according to AES3-1-2009 (R2014):
- Linear PCM encoding (2's complement, 16-24 bits)
- Sampling frequency management (AES5-2018 compliance)
- Validity bit handling
- Pre-emphasis indication

## Requirements

Implements 12 requirements from `02-requirements/functional/aes3-part1-audio-content-requirements.md`:

- **REQ-FUNC-AUDIO-001**: Linear PCM Encoding
- **REQ-FUNC-AUDIO-002**: PCM Polarity Convention
- **REQ-FUNC-AUDIO-003**: Coding Precision Options (16-24 bits)
- **REQ-FUNC-AUDIO-004**: MSB Justification
- **REQ-FUNC-AUDIO-005**: Non-Audio Content Protection
- **REQ-FUNC-AUDIO-006**: DC Content Minimization
- **REQ-FUNC-AUDIO-007**: Channel Sampling Frequency Interdependency
- **REQ-FUNC-AUDIO-008**: AES5-2018 Sampling Frequency Compliance
- **REQ-FUNC-AUDIO-009**: Validity Bit Implementation
- **REQ-FUNC-AUDIO-010**: Pre-emphasis Characteristic Support
- **REQ-PERF-AUDIO-001**: Real-Time PCM Encoding Performance
- **REQ-QUAL-AUDIO-001**: AES3-2009 Part 1 Conformity Testing

## Module Structure

```
Part1/
├── audio_coding/       # PCM encoding, word length handling
│   ├── pcm_encoder.h   # Linear PCM 2's complement encoder
│   ├── pcm_encoder.cpp
│   ├── word_length.h   # 16-24 bit precision handling
│   └── word_length.cpp
├── sampling/           # Sampling frequency management
│   ├── sampling_freq_manager.h  # AES5-2018 frequency support
│   └── sampling_freq_manager.cpp
├── validity/           # Validity bit logic
│   ├── validity_bit_handler.h   # PCM/non-PCM indication
│   └── validity_bit_handler.cpp
└── conformity/         # Part 1 conformance testing
    └── conformity_tests.cpp
```

## Key Constants

```cpp
// AES5-2018 Sampling Frequencies (supported)
#define AES5_FS_16KHZ    16000   // ×0.5 multiple (optional)
#define AES5_FS_22KHZ    22050   // ×0.5 multiple (optional)
#define AES5_FS_24KHZ    24000   // ×0.5 multiple (optional)
#define AES5_FS_32KHZ    32000   // Base frequency
#define AES5_FS_44_1KHZ  44100   // Base frequency
#define AES5_FS_48KHZ    48000   // Base frequency
#define AES5_FS_64KHZ    64000   // ×2 multiple (optional)
#define AES5_FS_88_2KHZ  88200   // ×2 multiple (optional)
#define AES5_FS_96KHZ    96000   // ×2 multiple
#define AES5_FS_176_4KHZ 176400  // ×4 multiple (optional)
#define AES5_FS_192KHZ   192000  // ×4 multiple
#define AES5_FS_352_8KHZ 352800  // ×8 multiple (optional)
#define AES5_FS_384KHZ   384000  // ×8 multiple (optional)

// Word Length Options
#define AES3_WORD_LENGTH_MIN  16  // Minimum bits
#define AES3_WORD_LENGTH_MAX  24  // Maximum bits

// Pre-emphasis Types (Part 1 Clause 7)
#define PRE_EMPHASIS_NONE         0x00  // No pre-emphasis
#define PRE_EMPHASIS_50_15_US     0x03  // 50µs+15µs (ITU-R BS.450-3)
#define PRE_EMPHASIS_J17          0x07  // J.17 (ITU-T)
#define PRE_EMPHASIS_NOT_INDICATED 0x00 // Not indicated
```

## Example Usage

```cpp
#include "AES/AES3/2009/Part1/audio_coding/pcm_encoder.h"
#include "AES/AES3/2009/Part1/sampling/sampling_freq_manager.h"

using namespace AES::AES3::Part1::_2009;

// Configure sampling frequency
audio_coding::SamplingFreqManager freq_mgr;
freq_mgr.set_frequency(AES5_FS_48KHZ);  // 48 kHz

// Encode 24-bit PCM sample
audio_coding::PCMEncoder encoder;
int32_t audio_sample = 0x123456;  // 24-bit audio
uint32_t encoded = encoder.encode(audio_sample, 24);  // MSB-justified

// Set validity bit (PCM content)
validity::ValidityBitHandler validity;
bool is_pcm = true;
validity.set_validity(is_pcm);  // Logic 0 = suitable for D/A
```

## Dependencies

- **Part 3 Integration**: Time slots 4-27 carry encoded audio
- **HAL Interface**: `hal_set_sampling_frequency()` for clock configuration

## Testing

Unit tests: `tests/unit/part1/`
- PCM encoding/decoding accuracy
- Word length justification verification
- Sampling frequency validation (AES5-2018)
- Validity bit correctness

## Standards References

- **[AES3-1]** AES3-1-2009 (R2014) Digital Audio Interface Part 1: Audio Content
- **[AES5]** AES5-2018 Preferred Sampling Frequencies
- **[ITU-R BS.450-3]** Pre-emphasis for FM sound broadcasting
- **[ITU-T J.17]** Pre-emphasis used on sound-program circuits

---

**Status**: Structure created (Phase 02, November 2025)  
**Next**: Implement PCM encoder and sampling frequency manager
