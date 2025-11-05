/*
 * AES3-2009 Hardware Abstraction Layer (HAL) Interface
 * 
 * This file defines the minimal HAL interface (<10 functions) required for
 * Standards layer to remain hardware-agnostic while implementing AES3-2009.
 * 
 * Copyright Notice:
 * This interface is based on understanding of AES3-4-2009 (R2014) requirements.
 * No copyrighted content from AES specifications is reproduced.
 * 
 * Standards Compliance:
 * - AES3-4-2009 (R2014) Part 4: Physical and Electrical (abstraction)
 * - Implements REQ-FUNC-HAL-001 through REQ-FUNC-HAL-008
 * - Total: 8 functions (meets <10 requirement per StR-FUNC-004)
 * 
 * Namespace: Common::interfaces
 */

#ifndef COMMON_INTERFACES_HAL_INTERFACE_H
#define COMMON_INTERFACES_HAL_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error Codes (negative values indicate errors)
 */
#define HAL_SUCCESS           0      /* Operation successful */
#define HAL_ERROR_INVALID    -1      /* Invalid parameter */
#define HAL_ERROR_TIMEOUT    -2      /* Operation timed out */
#define HAL_ERROR_NOTSUP     -3      /* Operation not supported by platform */
#define HAL_ERROR_NOMEM      -4      /* Memory allocation failed */
#define HAL_ERROR_HW         -5      /* Hardware error */

/*
 * HAL Function 1: Bit-Level Transmit
 * 
 * Transmit a biphase-mark encoded bit with precise timing.
 * 
 * @param bit_value    Logic level to transmit (0 or 1, after biphase-mark encoding)
 * @param duration_ns  Bit duration in nanoseconds (1 UI for biphase-mark)
 * @return             0 on success, negative error code on failure
 * 
 * Requirements:
 * - REQ-FUNC-HAL-002: Bit-Level Transmit Function
 * - REQ-PERF-HAL-001: Timing accuracy ±0.025 UI (jitter < 0.025 UI)
 * - Latency: < 500 ns (per REQ-PERF-HAL-004)
 * 
 * Notes:
 * - Standards layer (Part 3) performs biphase-mark encoding
 * - HAL provides timing-accurate bit transmission only
 * - Platform must ensure jitter < 0.025 UI per AES3-4 Clause 6.1
 */
int hal_transmit_bit(bool bit_value, uint32_t duration_ns);

/*
 * HAL Function 2: Bit-Level Receive
 * 
 * Receive a biphase-mark encoded bit with transition detection.
 * 
 * @param bit_value    Pointer to store received logic level (0 or 1)
 * @param timeout_ns   Maximum time to wait for bit reception (nanoseconds)
 * @return             0 on success, HAL_ERROR_TIMEOUT on timeout, negative on other errors
 * 
 * Requirements:
 * - REQ-FUNC-HAL-003: Bit-Level Receive Function
 * - REQ-PERF-HAL-002: Jitter tolerance ≥ 0.1 UI
 * - Latency: < 1 UI worst case (per REQ-PERF-HAL-004)
 * 
 * Notes:
 * - Standards layer (Part 3) performs biphase-mark decoding
 * - HAL provides transition detection and timing only
 * - Platform must tolerate ≥ 0.25 UI high-freq jitter per AES3-4 Clause 8.2
 */
int hal_receive_bit(bool* bit_value, uint32_t timeout_ns);

/*
 * HAL Function 3: Sampling Clock Configuration
 * 
 * Configure the sampling clock frequency.
 * 
 * @param frequency_hz  Desired sampling frequency in Hz (per AES5-2018)
 * @return              0 on success, HAL_ERROR_INVALID if unsupported, negative on other errors
 * 
 * Requirements:
 * - REQ-FUNC-HAL-004: Sampling Clock Configuration
 * - REQ-PERF-HAL-003: Frequency accuracy ±10 ppm
 * - Latency: < 100 ms (initialization only, per REQ-PERF-HAL-004)
 * 
 * Supported Frequencies (AES5-2018):
 * - Base: 32000, 44100, 48000 Hz (mandatory)
 * - Extended: 16000, 22050, 24000, 64000, 88200, 96000, 176400, 192000, 352800, 384000 Hz (optional)
 * 
 * Notes:
 * - Platform must maintain ±10 ppm accuracy (REQ-PERF-HAL-003)
 * - Long-term stability: ±50 ppm
 * - UI = 1 / (2 × frequency_hz)
 */
int hal_set_sampling_frequency(uint32_t frequency_hz);

/*
 * HAL Function 4: Clock Synchronization Query
 * 
 * Query clock synchronization (lock) status.
 * 
 * @param is_locked  Pointer to store lock status (true = locked, false = unlocked)
 * @return           0 on success, negative error code on failure
 * 
 * Requirements:
 * - REQ-FUNC-HAL-005: Clock Synchronization Query
 * - Integration with REQ-FUNC-META-004 (Channel status byte 0 bit 5)
 * - Latency: < 100 µs (per REQ-PERF-HAL-004)
 * 
 * Notes:
 * - Standards layer reports lock status in channel status byte 0 bit 5
 * - Lock loss detection latency should be < 10 ms
 * - Used for professional audio source monitoring
 */
int hal_get_clock_lock_status(bool* is_locked);

/*
 * HAL Function 5: Jitter Measurement Query
 * 
 * Query measured jitter on transmitted or received signal.
 * 
 * @param jitter_ui     Pointer to store measured jitter in UI (Unit Intervals)
 * @param is_transmit   true = measure transmit jitter, false = measure receive jitter
 * @return              0 on success, HAL_ERROR_NOTSUP if not supported, negative on other errors
 * 
 * Requirements:
 * - REQ-FUNC-HAL-006: Jitter Measurement Query
 * - REQ-PERF-HAL-001: Transmit jitter < 0.025 UI (AES3-4 Clause 6.1)
 * - REQ-PERF-HAL-002: Receive tolerance ≥ 0.25 UI high-freq (AES3-4 Clause 8.2)
 * - Latency: < 10 ms (per REQ-PERF-HAL-004)
 * 
 * Notes:
 * - Jitter measurement is OPTIONAL (platform-dependent)
 * - Return HAL_ERROR_NOTSUP if not supported
 * - Measurement bandwidth per AES3-4 (700 Hz high-pass filter)
 * - Used for conformity testing and monitoring
 */
int hal_get_jitter_measurement(float* jitter_ui, bool is_transmit);

/*
 * HAL Function 6: Signal Loss Detection
 * 
 * Detect signal loss on receiver input.
 * 
 * @param signal_present  Pointer to store signal presence (true = present, false = lost)
 * @return                0 on success, negative error code on failure
 * 
 * Requirements:
 * - REQ-FUNC-HAL-007: Signal Loss Detection
 * - Integration with REQ-FUNC-TRANS-004 (Re-synchronization)
 * - Latency: < 10 µs for loss detection (per REQ-PERF-HAL-004)
 * 
 * Notes:
 * - Signal loss: no transitions detected for > 2 UI
 * - Standards layer mutes audio output on signal loss
 * - Restoration detection: < 3 subframes
 * - Used for error handling and receiver muting
 */
int hal_get_signal_status(bool* signal_present);

/*
 * HAL Function 7: Buffer Allocation
 * 
 * Allocate audio frame buffer suitable for real-time operation.
 * 
 * @param buffer  Pointer to buffer pointer (allocated by HAL)
 * @param frames  Number of AES3 frames to buffer
 * @return        0 on success, HAL_ERROR_NOMEM on allocation failure, negative on other errors
 * 
 * Requirements:
 * - REQ-FUNC-HAL-008: Buffer Management Functions
 * - Integration with REQ-FUNC-TRANS-006 (Block structure - 192 frames)
 * 
 * Notes:
 * - Buffer must be suitable for real-time DMA (if applicable)
 * - Buffer alignment is platform-specific (16-byte, 64-byte, page-aligned, etc.)
 * - Each frame contains 2 subframes (64 time slots total)
 * - Typical allocation: 192 frames = 1 block = 4 ms @ 48 kHz
 */
int hal_allocate_buffer(void** buffer, size_t frames);

/*
 * HAL Function 8: Buffer Deallocation
 * 
 * Deallocate audio frame buffer.
 * 
 * @param buffer  Buffer pointer (previously allocated by hal_allocate_buffer)
 * @return        0 on success, negative error code on failure
 * 
 * Requirements:
 * - REQ-FUNC-HAL-008: Buffer Management Functions
 * 
 * Notes:
 * - Must deallocate buffers allocated by hal_allocate_buffer()
 * - No memory leaks permitted
 * - Safe to call with NULL pointer (no-op)
 */
int hal_free_buffer(void* buffer);

/*
 * HAL Function Count Verification
 * 
 * Total HAL functions: 8
 * Requirement: < 10 functions (REQ-FUNC-HAL-001, StR-FUNC-004)
 * Status: ✅ COMPLIANT (8 < 10)
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INTERFACES_HAL_INTERFACE_H */
