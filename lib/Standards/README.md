# AES3-2009 Standards Layer

**Pure protocol implementation layer - hardware and vendor agnostic**

## Directory Structure

```
lib/Standards/
├── AES/                        # Audio Engineering Society Standards
│   └── AES3/                   # AES3 Digital Audio Interface
│       └── 2009/               # AES3-2009 (R2014) specification
│           ├── Part1/          # Audio Content (PCM, sampling, validity)
│           ├── Part2/          # Metadata and Subcode (channel status, CRCC, user data)
│           ├── Part3/          # Transport (subframes, preambles, biphase-mark)
│           └── Part4/          # HAL Abstraction (jitter, timing, interface)
└── Common/                     # Cross-standard utilities
    ├── interfaces/             # Hardware abstraction interfaces
    ├── utils/                  # Shared utilities (CRC, bit manipulation)
    └── testing/                # Common testing frameworks
```

## Namespace Structure

All code follows the namespace pattern: `AES::AES3::<Part>::_2009::<module>`

Examples:
- `AES::AES3::Part1::_2009::audio_coding::PCMEncoder`
- `AES::AES3::Part2::_2009::channel_status::ChannelStatusBlock`
- `AES::AES3::Part3::_2009::biphase_mark::BiphaseMarkCoder`
- `AES::AES3::Part4::_2009::hal_interface::HAL`

## Core Principles

### ✅ STANDARDS LAYER (IN SCOPE)
- **ONLY AES3-2009 protocol logic** - no hardware-specific code
- **Hardware agnostic** - compilable without any vendor drivers
- **Mockable/testable** - runnable without physical audio hardware
- **Dependency injection** - receives hardware abstraction via HAL interfaces

### ❌ PLATFORM LAYER (OUT OF SCOPE)
- Hardware-specific drivers (ASIO, ALSA, CoreAudio, etc.)
- Vendor-specific audio interfaces (Realtek, Intel, etc.)
- OS-specific implementations (Windows, Linux, macOS)
- Physical layer details (connectors, voltages, impedances)

## Hardware Abstraction Layer (HAL)

The Standards layer interfaces with hardware through a minimal HAL interface (<10 functions):

**Location**: `Common/interfaces/hal_interface.h`

**8 HAL Functions**:
1. `hal_transmit_bit()` - Biphase-mark output
2. `hal_receive_bit()` - Biphase-mark input
3. `hal_set_sampling_frequency()` - Clock configuration
4. `hal_get_clock_lock_status()` - Lock detection
5. `hal_get_jitter_measurement()` - Jitter monitoring (optional)
6. `hal_get_signal_status()` - Signal presence detection
7. `hal_allocate_buffer()` - Memory allocation
8. `hal_free_buffer()` - Memory deallocation

## Requirements Traceability

All code in this directory implements requirements from Phase 02:

- **Part 1**: `02-requirements/functional/aes3-part1-audio-content-requirements.md` (12 requirements)
- **Part 2**: `02-requirements/functional/aes3-part2-metadata-subcode-requirements.md` (9 requirements)
- **Part 3**: `02-requirements/functional/aes3-part3-transport-requirements.md` (13 requirements)
- **Part 4**: `02-requirements/functional/aes3-part4-hal-abstraction-requirements.md` (15 requirements)

**Total**: 49 requirements → Standards layer implementation

## Build System

CMake structure mirrors directory hierarchy:

```cmake
add_library(aes3_part1 STATIC ...)  # Part 1: Audio Content
add_library(aes3_part2 STATIC ...)  # Part 2: Metadata/Subcode
add_library(aes3_part3 STATIC ...)  # Part 3: Transport
add_library(aes3_part4 STATIC ...)  # Part 4: HAL Abstraction
add_library(standards_common STATIC ...)  # Common utilities
```

## Testing Approach

### Unit Testing (Hardware-Independent)
- Mock HAL interface for protocol testing
- Test AES3-2009 compliance without hardware
- Located in `tests/unit/`

### Integration Testing (Service Layer)
- Integration with real hardware happens in Service Layer
- NOT in Standards layer

## Copyright and Licensing

**CRITICAL**: This implementation is based on understanding of AES3-2009 specification requirements.

**No copyrighted content** from AES, IEEE, or AVnu documents is reproduced. All code is original work achieving compliance through understanding of specification requirements.

For authoritative requirements, refer to:
- AES3-1-2009 (R2014) - Part 1: Audio Content
- AES3-2-2009 (R2014) - Part 2: Metadata and Subcode
- AES3-3-2009 (R2014) - Part 3: Transport
- AES3-4-2009 (R2014) - Part 4: Physical and Electrical

Available from Audio Engineering Society: https://www.aes.org/

## Development Guidelines

See `.github/instructions/copilot-instructions.md` for:
- Standards layer architecture compliance
- Forbidden patterns (hardware-specific code)
- Required patterns (HAL abstraction)
- Testing requirements
- Documentation standards

---

**Status**: Initial structure created (Phase 02, November 2025)  
**Next**: Implement Part 1 Audio Content components
