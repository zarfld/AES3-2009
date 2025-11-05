---
specId: REQ-ERR
specVersion: "1.0"
specType: error-handling-specification
specStatus: draft
specCategory: reliability
dateCreated: 2025-11-05
dateModified: 2025-11-05
author: System Architect
reviewer: Reliability Team
approver: TBD
applicableStandards:
  - id: ISO-29148
    version: "2018"
    title: "Systems and software engineering — Life cycle processes — Requirements engineering"
    section: "6.4"
  - id: IEEE-1012
    version: "2016"
    title: "IEEE Standard for System, Software, and Hardware Verification and Validation"
  - id: AES3-2009
    version: "2009"
    title: "AES3-2009 Digital Audio Interface (Parts 1-4)"
relatedSpecs:
  - id: STR-REL-001
    type: stakeholder-requirement
    relationship: satisfies
    description: "System reliability and fault tolerance"
  - id: REQ-SEC-001
    type: security-requirement
    relationship: extends
    description: "Input validation error handling"
  - id: REQ-SEC-002
    type: security-requirement
    relationship: extends
    description: "Buffer overflow error prevention"
  - id: REQ-SEC-003
    type: security-requirement
    relationship: extends
    description: "Parity and CRCC error detection"
changeLog:
  - version: "1.0"
    date: 2025-11-05
    author: System Architect
    changes: "Initial error handling specification - addresses error handling gap identified in completeness audit (6.8/10 score)"
---

# Error Handling Specification - AES3-2009 Digital Audio Interface

## 1. Introduction

### 1.1 Purpose

This document specifies comprehensive error handling requirements for the AES3-2009 Digital Audio Interface implementation. It addresses the critical error handling gap identified during Phase 02 completeness audit (6.8/10 score, with 33% of requirements missing error scenarios).

### 1.2 Scope

This specification defines:
- **Error detection mechanisms** for all AES3-2009 protocol violations
- **Error recovery procedures** ensuring graceful degradation
- **Error logging and reporting** for diagnostics and monitoring
- **Timeout specifications** for all time-dependent operations
- **Fallback behaviors** when primary functionality fails
- **Error propagation rules** determining when to set validity bits or reject data

### 1.3 Error Handling Philosophy

**Guiding Principles** (from AES3-2-2009 §5.3.3):
1. **Audio continuity prioritized**: "Receivers shall NOT interpret any errors in a channel status block such as CRCC or block length errors as a reason to mute or alter the audio content"
2. **Graceful degradation**: Continue with reduced functionality rather than complete failure
3. **Error isolation**: Errors in metadata shall not affect audio processing
4. **Fail-safe operation**: Invalid data never causes undefined behavior or crashes
5. **Visibility**: All errors logged with context for diagnostics

### 1.4 AES3-2009 Error Detection Mechanisms

The AES3-2009 specification defines the following built-in error detection:

| Mechanism | Location | Purpose | AES3 Reference |
|-----------|----------|---------|----------------|
| Validity Bit (V) | Subframe Time Slot 28 | Indicates unsuitable audio sample | Part 1 §6.1 |
| Parity Bit (P) | Subframe Time Slot 31 | Even parity over time slots 4-30 | Part 3 §4.9 |
| CRCC Checksum | Channel Status Byte 23 | CRC-8 validation of channel status block | Part 2 §5.5.11 |
| Preambles X/Y/Z | Subframe Time Slots 0-3 | Synchronization via biphase-mark violations | Part 3 §7 |
| Lock Indication | Channel Status Byte 0 Bit 5 | Source sampling frequency lock status | Part 2 §5.5.0 |

---

## 2. Error Classification

### 2.1 Error Severity Levels

| Level | Name | Description | Response Time | Example |
|-------|------|-------------|---------------|---------|
| **CRITICAL** | Fatal | System cannot continue operation | Immediate (0µs) | HAL function failure, NULL pointer |
| **ERROR** | Serious | Functionality degraded, recovery attempted | <1 sample period | CRCC validation failure, preamble detection failure |
| **WARNING** | Recoverable | Temporary condition, self-correcting | <1 block period | Parity error, jitter excess, signal degradation |
| **INFO** | Informational | Non-error event worthy of logging | Asynchronous | Sampling frequency change, lock status change |

### 2.2 Error Categories by AES3-2009 Part

#### Part 1: Audio Content Errors
- **PCM Encoding Errors**: Invalid bit depth, sample overflow, DC offset excess
- **Sampling Frequency Errors**: Non-standard frequency, frequency mismatch between channels
- **Validity Bit Errors**: Sample marked invalid by transmitter
- **Pre-emphasis Errors**: Undefined pre-emphasis state, mismatch with channel status

#### Part 2: Metadata/Subcode Errors
- **Channel Status Errors**: Invalid byte combinations, reserved bit usage, inconsistent state
- **CRCC Errors**: Checksum mismatch indicating corrupted channel status block
- **Block Synchronization Errors**: Lost block boundary, incorrect block length
- **User Data Errors**: Format violation (if using standardized formats like AES18)

#### Part 3: Transport Errors
- **Preamble Errors**: No valid preamble detected, corrupted preamble pattern
- **Parity Errors**: Parity bit mismatch indicating transmission corruption
- **Biphase-Mark Errors**: Coding rule violation, transition timing error
- **Frame Synchronization Errors**: Lost frame boundaries, frame counter wrap error
- **Time Slot Errors**: Invalid time slot access, boundary violation

#### Part 4: HAL Abstraction Errors
- **Signal Errors**: Signal loss, amplitude below Vmin, eye diagram degradation
- **Jitter Errors**: Jitter exceeds specification (<0.025 UI TX, >0.25 UI RX tolerance)
- **Clock Errors**: Clock unlock, frequency drift beyond tolerance
- **Interface Errors**: HAL function timeout, buffer allocation failure, hardware fault

---

## 3. Error Detection Matrix

### 3.1 Part 1: Audio Content Error Detection

| Error Condition | Detection Method | Error Code | Severity | Timing |
|----------------|------------------|------------|----------|--------|
| Invalid PCM bit depth | Range check: [16..24] | ERR_INVALID_BIT_DEPTH | ERROR | Pre-processing |
| Sample overflow (2's complement) | Range validation | ERR_SAMPLE_OVERFLOW | WARNING | Per-sample |
| DC offset excess | Running average >noise level | ERR_DC_OFFSET_EXCESS | WARNING | Per-block |
| Non-standard sampling frequency | AES5-2018 lookup | ERR_INVALID_SAMPLE_RATE | ERROR | Configuration |
| Channel frequency mismatch | Compare Ch1 vs Ch2 | ERR_FREQ_MISMATCH | ERROR | Configuration |
| Validity bit set (V=1) | Check time slot 28 | INFO_VALIDITY_BIT_SET | INFO | Per-sample |
| NaN/Inf in float source | IEEE 754 check | ERR_INVALID_FLOAT | ERROR | Pre-conversion |

### 3.2 Part 2: Metadata/Subcode Error Detection

| Error Condition | Detection Method | Error Code | Severity | Timing |
|----------------|------------------|------------|----------|--------|
| CRCC mismatch | CRC-8 validation | ERR_CRCC_VALIDATION_FAILED | ERROR | Per-block (192 frames) |
| Invalid byte 0 combination | Table lookup (Part 2 §5.5.0) | ERR_INVALID_CHANNEL_STATUS_BYTE0 | ERROR | Per-block |
| Reserved bit used | Bit mask check | WARN_RESERVED_BIT_USED | WARNING | Per-block |
| Block sync lost | Preamble Z not at frame 0 | ERR_BLOCK_SYNC_LOST | ERROR | Per-block |
| Block length error | Frame counter ≠192 at Z preamble | ERR_INVALID_BLOCK_LENGTH | ERROR | Per-block |
| Invalid alphanumeric char | ISO 646 IRV validation | WARN_INVALID_CHAR | WARNING | Per-block |
| User data format violation | AES18 format check (if used) | WARN_USER_DATA_FORMAT | WARNING | Per-block |

### 3.3 Part 3: Transport Error Detection

| Error Condition | Detection Method | Error Code | Severity | Timing |
|----------------|------------------|------------|----------|--------|
| Preamble not detected | Pattern match X/Y/Z | ERR_PREAMBLE_NOT_FOUND | CRITICAL | Per-subframe (within 1 UI) |
| Corrupted preamble | Invalid bit pattern | ERR_PREAMBLE_CORRUPTED | ERROR | Per-subframe |
| Parity mismatch | Even parity check time slots 4-31 | WARN_PARITY_ERROR | WARNING | Per-subframe |
| Biphase-mark violation | Transition timing check | ERR_BIPHASE_VIOLATION | ERROR | Per-time-slot |
| Frame sync lost | X/Y preamble pattern broken | ERR_FRAME_SYNC_LOST | ERROR | Per-frame |
| Time slot out of bounds | Index check: [0..31] | ERR_TIME_SLOT_OUT_OF_BOUNDS | CRITICAL | Immediate |
| Frame counter overflow | Check wrap at 192 | ERR_FRAME_COUNTER_OVERFLOW | ERROR | Per-frame |

### 3.4 Part 4: HAL Abstraction Error Detection

| Error Condition | Detection Method | Error Code | Severity | Timing |
|----------------|------------------|------------|----------|--------|
| Signal loss | Amplitude <Vmin (200mV balanced) | ERR_SIGNAL_LOSS | CRITICAL | <1ms |
| Signal degraded | Eye diagram Tmin <0.5 UI | WARN_SIGNAL_DEGRADED | WARNING | Continuous |
| Jitter excess (TX) | Jitter >0.025 UI | ERR_JITTER_TX_EXCESS | ERROR | Per-measurement |
| Jitter excess (RX) | Jitter >tolerance (0.25-10 UI) | WARN_JITTER_RX_EXCESS | WARNING | Per-measurement |
| Clock unlock | Lock bit set in channel status | WARN_CLOCK_UNLOCK | WARNING | Per-block |
| Frequency drift | Deviation >±10 ppm | WARN_FREQ_DRIFT | WARNING | Continuous |
| HAL function timeout | Timeout exceeds threshold | ERR_HAL_TIMEOUT | CRITICAL | Per-call |
| Buffer allocation failure | malloc/new returns NULL | ERR_OUT_OF_MEMORY | CRITICAL | Immediate |
| NULL pointer dereference | Pointer validation | ERR_NULL_POINTER | CRITICAL | Immediate |

---

## 4. Error Recovery Procedures

### 4.1 Part 1: Audio Content Error Recovery

#### ERR_INVALID_BIT_DEPTH
**Detection**: PCM sample bit depth not in [16, 17, 18, 19, 20, 21, 22, 23, 24]  
**Recovery**:
1. Reject sample (do not encode)
2. Set validity bit V=1 in subframe
3. Log error with sample metadata (timestamp, channel, requested bit depth)
4. Return error code to caller
5. Caller may retry with valid bit depth or switch to default (24-bit)

**Fallback**: Use previous valid sample if real-time continuity required (zero-order hold)

#### ERR_SAMPLE_OVERFLOW
**Detection**: PCM sample value exceeds 2's complement range for bit depth  
**Recovery**:
1. Clamp sample to valid range (min/max for bit depth)
2. Set validity bit V=1 to indicate clipped sample
3. Log warning with sample value and valid range
4. Continue processing with clamped value

**Fallback**: Apply soft clipping or limiting if available

#### ERR_DC_OFFSET_EXCESS
**Detection**: DC component >noise level (Part 1 §4.6)  
**Recovery**:
1. Apply DC blocking high-pass filter (fc <10 Hz)
2. Log warning with measured DC offset
3. Continue processing with filtered audio
4. If filter unavailable, log error and continue (DC offset violation is non-critical)

**Fallback**: Document DC offset in channel status origin field if space permits

#### ERR_INVALID_SAMPLE_RATE
**Detection**: Sampling frequency not in AES5-2018 list  
**Recovery**:
1. Reject sampling frequency change request
2. Return error code ERR_INVALID_SAMPLE_RATE
3. Retain previous valid sampling frequency
4. Log error with requested vs current frequency
5. Notify application layer of rejection

**Fallback**: If no previous valid frequency, default to 48 kHz

#### ERR_FREQ_MISMATCH
**Detection**: Channel 1 and Channel 2 sampling frequencies differ  
**Recovery**:
1. CRITICAL ERROR - cannot transmit with mismatched frequencies (AES3-2009 Part 1 §5.1)
2. Stop transmission immediately
3. Log critical error with both frequencies
4. Return error code to application
5. Require manual intervention to resolve

**Fallback**: Use Channel 1 frequency for both channels (may cause audio artifacts)

### 4.2 Part 2: Metadata/Subcode Error Recovery

#### ERR_CRCC_VALIDATION_FAILED
**Detection**: Computed CRC-8 ≠ Channel Status Byte 23  
**Recovery** (per AES3-2-2009 §5.3.3):
1. Reject entire channel status block (bytes 0-23)
2. **Retain previous valid channel status block** (key requirement!)
3. Do NOT mute audio or alter audio processing
4. Set CRCC error flag in status structure
5. Log error with block number and computed vs received CRCC
6. Increment crcc_error_count metric
7. Wait for next valid block (192 frames later)

**Fallback**: If no previous valid block exists (e.g., first block), use default channel status values:
- Byte 0: Professional use (0b00000001), no emphasis, 48kHz
- Byte 1: Two-channel mode (0b00001000)
- Byte 2: 24-bit audio sample word (0b00000100)
- All other bytes: 0x00

#### ERR_INVALID_CHANNEL_STATUS_BYTE0
**Detection**: Byte 0 bit combination not in AES3-2-2009 Part 2 Table (§5.5.0)  
**Recovery**:
1. Log error with received byte value
2. Use default channel status byte 0: 0x01 (professional use, 48kHz, no emphasis)
3. Retain rest of channel status block if CRCC valid
4. Set channel status warning flag
5. Continue processing

**Fallback**: Mark entire block as invalid if multiple bytes have reserved bits set

#### ERR_BLOCK_SYNC_LOST
**Detection**: Preamble Z not detected at expected frame 0 of block  
**Recovery**:
1. Enter resynchronization mode
2. Search for next preamble Z in incoming stream (max 192 frames ahead)
3. Reset frame counter to 0 when Z found
4. Discard partial channel status block data
5. Begin new block with found Z preamble
6. Log sync loss event with duration (frames searched)
7. Increment block_sync_lost_count metric

**Timeout**: If no Z preamble found within 384 frames (2 blocks), signal critical error

**Fallback**: If persistent sync loss, signal ERR_SIGNAL_LOSS and stop decoding

### 4.3 Part 3: Transport Error Recovery

#### ERR_PREAMBLE_NOT_FOUND
**Detection**: No valid X/Y/Z preamble pattern detected at subframe start  
**Recovery**:
1. **CRITICAL ERROR** - cannot decode subframe without preamble
2. Enter synchronization search mode
3. Scan bit stream for valid preamble patterns (check all 3 types)
4. When preamble found:
   - Determine type (X=first subframe, Y=second subframe, Z=block start)
   - Reset subframe/frame/block counters appropriately
   - Resume normal decoding
5. Log sync loss event with recovery time
6. Discard all data until sync reacquired

**Timeout**: If no preamble found within 64 time slots (2 subframes), signal ERR_SIGNAL_LOSS

**Fallback**: Mute audio output during sync loss (set validity bit V=1)

#### WARN_PARITY_ERROR
**Detection**: Parity bit (time slot 31) does not produce even parity over time slots 4-31  
**Recovery** (per AES3-2009 approach):
1. Set validity bit V=1 in subframe to mark audio sample as unreliable
2. Log parity error with frame number and channel
3. Increment parity_error_count metric
4. **Continue processing** - do not discard audio sample
5. Application layer decides handling:
   - Option A: Mute sample (replace with silence)
   - Option B: Interpolate from surrounding samples
   - Option C: Pass through with V=1 flag for monitoring

**Error Rate Monitoring**: If parity error rate >1%, signal WARN_SIGNAL_DEGRADED

**Fallback**: If parity errors become excessive (>10%), enter ERR_SIGNAL_LOSS state

#### ERR_BIPHASE_VIOLATION
**Detection**: Biphase-mark coding rules violated (invalid transition timing)  
**Recovery**:
1. Log error with time slot number and transition timing
2. Attempt bit recovery using closest valid transition point
3. If recovery fails, mark bit as invalid
4. Set validity bit V=1 for affected subframe
5. Increment biphase_error_count metric
6. Continue to next time slot

**Fallback**: If >50% of time slots have violations, signal ERR_FRAME_SYNC_LOST

#### ERR_FRAME_COUNTER_OVERFLOW
**Detection**: Frame counter value ≥192 (should wrap at 192)  
**Recovery**:
1. Force frame counter modulo operation: frame_counter = frame_counter % 192
2. Log error with overflow value
3. Check for wraparound missed: if frame_counter was 191 last frame, this is expected
4. If unexpected overflow (not from 191), signal ERR_BLOCK_SYNC_LOST
5. Continue processing with corrected counter

**Prevention**: Always use modulo arithmetic for frame counter increment

### 4.4 Part 4: HAL Abstraction Error Recovery

#### ERR_SIGNAL_LOSS
**Detection**: Signal amplitude <Vmin (200mV balanced, 320mV coaxial) for >1ms  
**Recovery**:
1. **CRITICAL ERROR** - stop all decoding immediately
2. Mute audio output (set all samples to 0x000000)
3. Set validity bit V=1 for all subframes
4. Log signal loss event with timestamp
5. Enter signal detection mode:
   - Poll signal amplitude every 10ms
   - When amplitude ≥Vmin for >100ms, attempt resynchronization
6. After resync, resume normal operation
7. Increment signal_loss_count metric

**Timeout**: If signal not restored within 10 seconds, notify application layer

**Fallback**: Display "NO SIGNAL" status to user interface

#### ERR_JITTER_TX_EXCESS
**Detection**: Transmitter jitter >0.025 UI (AES3-4-2009 §5.1.2)  
**Recovery**:
1. Log error with measured jitter value
2. Check clock source stability
3. If clock unstable:
   - Switch to backup clock source if available
   - Reduce output sample rate if jitter persists
4. Set channel status byte 0 bit 5 (unlocked indication)
5. Continue transmission with warning flag
6. Increment jitter_excess_count metric

**Threshold**: If jitter >0.1 UI (4x spec), stop transmission (out of spec)

**Fallback**: External clock synchronization or crystal oscillator replacement may be required

#### ERR_HAL_TIMEOUT
**Detection**: HAL function does not return within specified timeout  
**Recovery**:
1. **CRITICAL ERROR** - HAL function hang detected
2. Abort HAL function call (if possible via OS/RTOS primitives)
3. Log critical error with function name and timeout duration
4. Attempt to reset HAL interface:
   - Call hal_reset() function if available
   - Re-initialize HAL state machine
5. If reset fails, signal system-level failure to application
6. Require manual intervention or system restart

**Timeout Values** (by function type):
- Bit I/O (hal_transmit_bit, hal_receive_bit): 1 UI (163 ns @ 48kHz)
- Configuration (hal_set_sampling_frequency): 100ms
- Status query (hal_get_clock_lock_status): 10ms
- Jitter measurement (hal_get_jitter_measurement): 1s
- Buffer operations (hal_allocate_buffer, hal_free_buffer): 1ms

**Fallback**: Fail gracefully - stop audio processing, return to idle state

#### ERR_OUT_OF_MEMORY
**Detection**: Buffer allocation returns NULL pointer  
**Recovery**:
1. **CRITICAL ERROR** - cannot allocate required memory
2. Log error with requested allocation size and current heap usage
3. Attempt memory recovery:
   - Free non-critical buffers (logs, statistics, debug info)
   - Run garbage collection if applicable (e.g., managed languages)
   - Retry allocation once after recovery
4. If still fails:
   - Enter degraded mode: reduce buffer sizes, disable optional features
   - Notify application layer of memory pressure
5. If degraded mode fails, stop processing and return error to application

**Prevention**: Pre-allocate critical buffers at initialization, avoid runtime allocation

**Fallback**: Use static pre-allocated emergency buffer pool for critical operations

---

## 5. Error Logging and Reporting

### 5.1 Log Entry Format

Every error shall be logged with the following information:

```c
typedef struct {
    // Timestamp
    uint64_t timestamp_ns;        // Nanosecond precision timestamp
    
    // Error identification
    error_code_t error_code;      // Standardized error code (see §5.2)
    error_severity_t severity;    // CRITICAL, ERROR, WARNING, INFO
    
    // Context
    aes3_part_t part;             // Part 1, 2, 3, 4, or Common
    const char* function_name;    // Function where error occurred
    uint32_t line_number;         // Source code line number
    
    // AES3 context
    uint32_t frame_number;        // Frame counter (0-191)
    uint8_t subframe_number;      // Subframe 1 or 2
    uint8_t channel_number;       // Audio channel (1 or 2)
    uint32_t sample_number;       // Cumulative sample counter
    
    // Error-specific data
    union {
        struct {
            uint8_t expected;
            uint8_t actual;
        } validation_error;       // For CRCC, parity, etc.
        
        struct {
            int32_t value;
            int32_t min;
            int32_t max;
        } range_error;            // For overflow, out-of-bounds
        
        struct {
            void* pointer;
            size_t size;
        } memory_error;           // For allocation failures
        
        uint8_t raw_data[32];     // Generic error data
    } data;
    
    // Human-readable message
    char message[128];            // Error description
} error_log_entry_t;
```

### 5.2 Error Code Enumeration

```c
typedef enum {
    // Part 1: Audio Content Errors (0x0100-0x01FF)
    ERR_INVALID_BIT_DEPTH = 0x0101,
    ERR_SAMPLE_OVERFLOW = 0x0102,
    ERR_DC_OFFSET_EXCESS = 0x0103,
    ERR_INVALID_SAMPLE_RATE = 0x0104,
    ERR_FREQ_MISMATCH = 0x0105,
    INFO_VALIDITY_BIT_SET = 0x0106,
    ERR_INVALID_FLOAT = 0x0107,
    
    // Part 2: Metadata/Subcode Errors (0x0200-0x02FF)
    ERR_CRCC_VALIDATION_FAILED = 0x0201,
    ERR_INVALID_CHANNEL_STATUS_BYTE0 = 0x0202,
    WARN_RESERVED_BIT_USED = 0x0203,
    ERR_BLOCK_SYNC_LOST = 0x0204,
    ERR_INVALID_BLOCK_LENGTH = 0x0205,
    WARN_INVALID_CHAR = 0x0206,
    WARN_USER_DATA_FORMAT = 0x0207,
    
    // Part 3: Transport Errors (0x0300-0x03FF)
    ERR_PREAMBLE_NOT_FOUND = 0x0301,
    ERR_PREAMBLE_CORRUPTED = 0x0302,
    WARN_PARITY_ERROR = 0x0303,
    ERR_BIPHASE_VIOLATION = 0x0304,
    ERR_FRAME_SYNC_LOST = 0x0305,
    ERR_TIME_SLOT_OUT_OF_BOUNDS = 0x0306,
    ERR_FRAME_COUNTER_OVERFLOW = 0x0307,
    
    // Part 4: HAL Abstraction Errors (0x0400-0x04FF)
    ERR_SIGNAL_LOSS = 0x0401,
    WARN_SIGNAL_DEGRADED = 0x0402,
    ERR_JITTER_TX_EXCESS = 0x0403,
    WARN_JITTER_RX_EXCESS = 0x0404,
    WARN_CLOCK_UNLOCK = 0x0405,
    WARN_FREQ_DRIFT = 0x0406,
    ERR_HAL_TIMEOUT = 0x0407,
    ERR_OUT_OF_MEMORY = 0x0408,
    ERR_NULL_POINTER = 0x0409,
    
    // Common Errors (0x0F00-0x0FFF)
    ERR_NOT_INITIALIZED = 0x0F01,
    ERR_INVALID_ARGUMENT = 0x0F02,
    ERR_OPERATION_FAILED = 0x0F03,
    ERR_TIMEOUT = 0x0F04,
} error_code_t;
```

### 5.3 Logging Levels and Rate Limiting

**Log Levels** (configurable at runtime):
- **CRITICAL**: Always logged, never rate-limited
- **ERROR**: Always logged, rate-limited to 100/second per error code
- **WARNING**: Logged if enabled, rate-limited to 10/second per error code
- **INFO**: Logged if verbose mode enabled, rate-limited to 1/second per error code

**Rate Limiting Algorithm** (Token Bucket):
```c
typedef struct {
    error_code_t error_code;
    uint32_t token_count;         // Current tokens available
    uint32_t max_tokens;          // Maximum tokens (burst size)
    uint32_t refill_rate;         // Tokens per second
    uint64_t last_refill_ns;      // Last refill timestamp
} rate_limiter_t;

// Refill tokens based on elapsed time
void rate_limiter_refill(rate_limiter_t* limiter, uint64_t current_time_ns) {
    uint64_t elapsed_ns = current_time_ns - limiter->last_refill_ns;
    uint32_t tokens_to_add = (elapsed_ns * limiter->refill_rate) / 1000000000;
    
    if (tokens_to_add > 0) {
        limiter->token_count = MIN(limiter->token_count + tokens_to_add, limiter->max_tokens);
        limiter->last_refill_ns = current_time_ns;
    }
}

// Check if logging allowed
bool rate_limiter_allow(rate_limiter_t* limiter) {
    if (limiter->token_count > 0) {
        limiter->token_count--;
        return true;
    }
    return false;  // Rate limit exceeded
}
```

### 5.4 Error Metrics and Monitoring

**Required Metrics** (exposed via read-only interface):
```c
typedef struct {
    // Error counters (cumulative since initialization)
    uint64_t crcc_error_count;
    uint64_t parity_error_count;
    uint64_t preamble_error_count;
    uint64_t signal_loss_count;
    uint64_t jitter_excess_count;
    uint64_t block_sync_lost_count;
    uint64_t frame_sync_lost_count;
    
    // Error rates (per second, sliding window)
    float crcc_error_rate;
    float parity_error_rate;
    float preamble_error_rate;
    
    // Operational status
    bool is_synchronized;         // Frame/block sync established
    bool is_signal_present;       // Signal amplitude >Vmin
    bool is_clock_locked;         // Clock lock indication
    
    // Performance metrics
    float jitter_current_ui;      // Current jitter measurement (UI)
    float signal_amplitude_v;     // Current signal amplitude (volts)
    uint32_t consecutive_errors;  // Consecutive error count (for threshold detection)
} error_metrics_t;
```

---

## 6. Timeout Specifications

### 6.1 Protocol Timeouts

| Operation | Timeout Value | Justification | Recovery Action |
|-----------|---------------|---------------|-----------------|
| Preamble detection | 2 subframes (64 UI) | Max time to find sync pattern | Enter synchronization mode |
| Frame synchronization | 4 frames (512 UI) | Max time to establish frame pattern | Signal ERR_FRAME_SYNC_LOST |
| Block synchronization | 2 blocks (384 frames) | Max time to find preamble Z | Signal ERR_BLOCK_SYNC_LOST |
| CRCC validation | 1 block (192 frames) | Channel status block period | Use previous valid block |
| Signal presence | 1ms | Physical layer detection time | Signal ERR_SIGNAL_LOSS |

**Timeout Calculation Example** (48kHz sampling):
- 1 UI = 163 ns (at 48kHz)
- 1 subframe = 32 time slots × 2 UI/slot = 64 UI = 10.4 µs
- 1 frame = 2 subframes = 128 UI = 20.8 µs
- 1 block = 192 frames = 24576 UI = 4.0 ms

### 6.2 HAL Function Timeouts

| HAL Function | Timeout | Error Code | Recovery |
|--------------|---------|------------|----------|
| `hal_transmit_bit()` | 1 UI (163 ns @ 48kHz) | ERR_HAL_TIMEOUT | Retry once, then signal hardware fault |
| `hal_receive_bit()` | 1 UI (163 ns @ 48kHz) | ERR_HAL_TIMEOUT | Retry once, then signal ERR_SIGNAL_LOSS |
| `hal_set_sampling_frequency()` | 100ms | ERR_HAL_TIMEOUT | Revert to previous frequency, log error |
| `hal_get_clock_lock_status()` | 10ms | ERR_HAL_TIMEOUT | Assume unlocked, set warning flag |
| `hal_get_jitter_measurement()` | 1s | ERR_HAL_TIMEOUT | Return last valid measurement, log warning |
| `hal_get_signal_status()` | 10ms | ERR_HAL_TIMEOUT | Assume signal lost, enter detection mode |
| `hal_allocate_buffer()` | 1ms | ERR_HAL_TIMEOUT | Signal ERR_OUT_OF_MEMORY |
| `hal_free_buffer()` | 1ms | ERR_HAL_TIMEOUT | Log error, continue (may leak memory) |

### 6.3 Watchdog Timers

**Real-Time Processing Watchdog**:
- **Purpose**: Detect stuck processing loops or infinite waits
- **Timeout**: 2× sample period (e.g., 42 µs @ 48kHz)
- **Action**: Trigger system reset or failsafe mode
- **Implementation**: Hardware watchdog timer or RTOS task watchdog

**Synchronization Watchdog**:
- **Purpose**: Detect persistent loss of synchronization
- **Timeout**: 10 blocks (40ms @ 48kHz)
- **Action**: Signal ERR_SIGNAL_LOSS, enter signal detection mode
- **Implementation**: Frame/block counter monitoring

---

## 7. Error Propagation Rules

### 7.1 Validity Bit Propagation (Part 1 §6.1)

**Set Validity Bit V=1 when**:
1. Transmitter: Audio sample is NOT suitable for direct conversion to analog
2. Parity error detected in received subframe
3. Sample value clamped due to overflow
4. Signal loss or degradation detected
5. Channel status indicates non-PCM audio (Part 2 Byte 0 Bit 1 = 1)

**Clear Validity Bit V=0 when**:
1. Audio sample is valid linear PCM
2. No parity errors detected
3. No signal quality issues
4. Channel status indicates linear PCM audio

### 7.2 CRCC Error Propagation (Part 2 §5.5.11)

**CRCC Error Handling** (per AES3-2-2009 §5.3.3):
- **Reject channel status block** with CRCC mismatch
- **Retain previous valid channel status block**
- **DO NOT mute or alter audio content**
- **DO NOT set validity bit** based on CRCC error
- **Log error** for diagnostics

**CRCC Error Escalation**:
- If CRCC errors >10% of blocks over 10 seconds:
  - Signal WARN_SIGNAL_DEGRADED
  - Check physical layer (cable, connectors, signal amplitude)
- If CRCC errors >50% of blocks over 1 second:
  - Signal ERR_SIGNAL_LOSS
  - Stop decoding channel status, use defaults

### 7.3 Error Escalation Thresholds

| Error Type | Threshold | Duration | Action |
|-----------|-----------|----------|--------|
| Parity errors | >1% of samples | 1 second | WARN_SIGNAL_DEGRADED |
| Parity errors | >10% of samples | 1 second | ERR_SIGNAL_LOSS |
| CRCC errors | >10% of blocks | 10 seconds | WARN_SIGNAL_DEGRADED |
| CRCC errors | >50% of blocks | 1 second | ERR_SIGNAL_LOSS |
| Preamble errors | 3 consecutive | Immediate | ERR_FRAME_SYNC_LOST |
| Preamble errors | >5 in 10 frames | 10 frames | ERR_SIGNAL_LOSS |
| Jitter excess | >2× spec | 100ms | ERR_JITTER_TX_EXCESS |
| Signal amplitude | <Vmin | 1ms | ERR_SIGNAL_LOSS |

---

## 8. Testing Strategy

### 8.1 Error Injection Testing

**Unit Tests**: Inject errors at each detection point
- Invalid input values (bit depth, sampling frequency, time slot index)
- Corrupted data (parity errors, CRCC errors, preamble errors)
- Resource failures (NULL allocations, timeouts)

**Integration Tests**: Test error propagation across layers
- Part 1 error → validity bit set → Part 3 encodes correctly
- Part 2 CRCC error → block rejected → previous block retained
- Part 4 signal loss → Part 3 sync lost → Part 1 mutes output

**Stress Tests**: High error rate scenarios
- 50% parity error rate (should trigger ERR_SIGNAL_LOSS)
- 100% CRCC error rate (should use defaults, continue audio)
- Rapid sync loss/recovery cycles

### 8.2 Recovery Time Testing

**Test Requirements**:
- Measure time from error detection to recovery completion
- Verify recovery time <1 sample period for ERROR severity
- Verify recovery time <1 block period for WARNING severity
- Verify CRITICAL errors stop processing immediately (<1 µs)

**Test Cases**:
1. Preamble sync lost → time to reacquire sync (target: <10 frames)
2. CRCC error → time to load previous block (target: immediate)
3. Signal loss → time to resume after signal restored (target: <100ms)

### 8.3 Error Logging Testing

**Test Requirements**:
- Verify all errors logged with complete context
- Verify rate limiting prevents log flooding
- Verify log entries contain correct timestamps, error codes, AES3 context

**Test Cases**:
1. Generate 1000 parity errors/second → verify only 100 logged (rate limit)
2. Verify CRITICAL errors always logged regardless of rate limit
3. Verify suppression count logged when rate limit exceeded

---

## 9. Compliance Checklist

### 9.1 AES3-2009 Compliance

- [ ] Validity bit handling per Part 1 §6.1 (set to 1 for unsuitable audio)
- [ ] CRCC validation per Part 2 §5.5.11 (CRC-8 polynomial)
- [ ] CRCC error handling per Part 2 §5.3.3 (reject block, retain previous, don't mute audio)
- [ ] Parity checking per Part 3 §4.9 (even parity over time slots 4-31)
- [ ] Preamble detection per Part 3 §7 (X/Y/Z patterns with biphase-mark violations)
- [ ] Block synchronization per Part 3 §8 (preamble Z marks frame 0 of 192-frame block)
- [ ] Signal detection per Part 4 Annex C/D (Vmin thresholds)
- [ ] Jitter tolerance per Part 4 §5.2 (0.25 UI high freq, 10 UI low freq)

### 9.2 ISO/IEC/IEEE 29148:2018 Compliance

- [ ] Error handling requirements are complete (detection, recovery, logging)
- [ ] Error handling requirements are testable (specific thresholds, timeouts)
- [ ] Error handling requirements are traceable (links to AES3-2009 clauses)
- [ ] Error scenarios included in all 49 system requirements
- [ ] Acceptance criteria include error cases (Given-When-Then for errors)

### 9.3 IEEE 1012-2016 Verification Compliance

- [ ] Error detection verification tests defined
- [ ] Error recovery verification tests defined
- [ ] Error logging verification tests defined
- [ ] Test coverage >90% of error paths
- [ ] Error injection tests automated

---

## 10. References

### 10.1 Standards
- **AES3-1-2009** - Digital Audio Interface Part 1: Audio Content
- **AES3-2-2009** - Digital Audio Interface Part 2: Metadata and Subcode
- **AES3-3-2009** - Digital Audio Interface Part 3: Transport
- **AES3-4-2009** - Digital Audio Interface Part 4: Physical and Electrical
- **AES5-2018** - Preferred Sampling Frequencies
- **AES11-2009** - Digital Audio Reference Signals
- **ISO/IEC/IEEE 29148:2018** - Requirements Engineering
- **IEEE 1012-2016** - Verification and Validation

### 10.2 Related Requirements Documents
- `stakeholder-requirements-specification.md` (Phase 01)
- `security-requirements.md` (Security requirements including error handling security)
- `aes3-part1-audio-content-requirements.md` (Part 1 functional requirements)
- `aes3-part2-metadata-subcode-requirements.md` (Part 2 functional requirements)
- `aes3-part3-transport-requirements.md` (Part 3 functional requirements)
- `aes3-part4-hal-abstraction-requirements.md` (Part 4 functional requirements)

---

**Document Status**: Draft - Awaiting Reliability Team Review  
**Target Approval Date**: Week 6 End (December 9, 2025)  
**Next Review**: Phase 02 Stakeholder Review (Week 7, December 10-16, 2025)
