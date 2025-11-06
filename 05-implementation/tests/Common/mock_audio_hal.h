/**
 * @file mock_audio_hal.h
 * @brief Mock Audio HAL for Testing AES3-2009 Implementation
 * @implements audio_hal_interface_t
 * @traceability TEST-MOCK-001
 * 
 * Mock implementation of audio HAL interface for unit and integration testing.
 * Features:
 * - Call logging and verification
 * - Frame capture and playback
 * - Timing simulation (nanosecond precision)
 * - Error injection for testing error paths
 * - Thread-safe operation
 * 
 * @note This is ONLY for testing - not for production use
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MOCK_AUDIO_HAL_H
#define MOCK_AUDIO_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aes3/audio_hal_interface.h"
#include <stdbool.h>

/* =============================================================================
 * Mock Configuration and Control
 * ============================================================================= */

/**
 * @brief Mock HAL call types for logging
 */
typedef enum {
    MOCK_CALL_INIT,
    MOCK_CALL_SEND_FRAMES,
    MOCK_CALL_RECEIVE_FRAMES,
    MOCK_CALL_SET_SAMPLE_RATE,
    MOCK_CALL_GET_SAMPLE_CLOCK_NS,
    MOCK_CALL_SET_CALLBACKS,
    MOCK_CALL_START,
    MOCK_CALL_STOP,
    MOCK_CALL_GET_STATUS,
    MOCK_CALL_CLEANUP
} mock_call_type_t;

/**
 * @brief Mock HAL call log entry
 */
typedef struct {
    mock_call_type_t call_type;   /**< Type of HAL call */
    uint64_t timestamp_ns;        /**< Call timestamp (simulated clock) */
    audio_hal_result_t result;    /**< Return value */
    size_t frame_count;           /**< Frame count (for send/receive) */
    audio_hal_sample_rate_t sample_rate; /**< Sample rate (for set_sample_rate) */
} mock_call_log_entry_t;

/**
 * @brief Mock HAL error injection configuration
 */
typedef struct {
    bool inject_init_error;           /**< Fail init call */
    bool inject_send_error;           /**< Fail send_frames call */
    bool inject_receive_error;        /**< Fail receive_frames call */
    bool inject_buffer_overflow;      /**< Simulate buffer overflow */
    bool inject_buffer_underrun;      /**< Simulate buffer underrun */
    uint32_t fail_after_n_sends;      /**< Fail after N send calls (0=disabled) */
    uint32_t fail_after_n_receives;   /**< Fail after N receive calls (0=disabled) */
} mock_error_config_t;

/**
 * @brief Mock HAL statistics
 */
typedef struct {
    uint32_t total_init_calls;
    uint32_t total_send_calls;
    uint32_t total_receive_calls;
    uint32_t total_frames_sent;
    uint32_t total_frames_received;
    uint32_t total_start_calls;
    uint32_t total_stop_calls;
    uint32_t total_cleanup_calls;
} mock_hal_statistics_t;

/* =============================================================================
 * Mock HAL Initialization and Configuration
 * ============================================================================= */

/**
 * @brief Initialize mock audio HAL for testing
 * 
 * Must be called before using the mock interface.
 * Resets all state, clears logs, and disables error injection.
 */
void mock_audio_hal_init(void);

/**
 * @brief Reset mock audio HAL state
 * 
 * Clears all logs, captured frames, and statistics.
 * Does not change error injection configuration.
 */
void mock_audio_hal_reset(void);

/**
 * @brief Get mock audio HAL interface
 * 
 * @return Pointer to mock HAL function table
 */
const audio_hal_interface_t* mock_audio_hal_get_interface(void);

/* =============================================================================
 * Mock HAL Configuration
 * ============================================================================= */

/**
 * @brief Set error injection configuration
 * 
 * @param[in] error_config  Error injection settings
 */
void mock_audio_hal_set_error_config(const mock_error_config_t* error_config);

/**
 * @brief Set simulated sample rate for timing calculations
 * 
 * @param[in] sample_rate  Simulated sampling frequency
 */
void mock_audio_hal_set_simulated_sample_rate(audio_hal_sample_rate_t sample_rate);

/**
 * @brief Advance simulated clock by number of frames
 * 
 * Used to simulate time passing during frame processing.
 * 
 * @param[in] frame_count  Number of frames elapsed
 */
void mock_audio_hal_advance_clock(uint32_t frame_count);

/* =============================================================================
 * Mock HAL Verification and Inspection
 * ============================================================================= */

/**
 * @brief Get number of logged calls
 * 
 * @return Number of HAL calls logged
 */
size_t mock_audio_hal_get_call_count(void);

/**
 * @brief Get specific call log entry
 * 
 * @param[in] index  Call log index (0 to call_count-1)
 * @param[out] entry  Pointer to receive log entry
 * @return true if entry retrieved, false if index out of range
 */
bool mock_audio_hal_get_call_log(size_t index, mock_call_log_entry_t* entry);

/**
 * @brief Get mock HAL statistics
 * 
 * @param[out] stats  Pointer to receive statistics
 */
void mock_audio_hal_get_statistics(mock_hal_statistics_t* stats);

/**
 * @brief Verify that init was called with expected configuration
 * 
 * @param[in] expected_sample_rate  Expected sample rate
 * @param[in] expected_channels     Expected channel count
 * @return true if init was called with matching config
 */
bool mock_audio_hal_verify_init_called(audio_hal_sample_rate_t expected_sample_rate,
                                        uint8_t expected_channels);

/**
 * @brief Verify that send_frames was called N times
 * 
 * @param[in] expected_count  Expected number of send_frames calls
 * @return true if count matches
 */
bool mock_audio_hal_verify_send_count(uint32_t expected_count);

/**
 * @brief Verify that receive_frames was called N times
 * 
 * @param[in] expected_count  Expected number of receive_frames calls
 * @return true if count matches
 */
bool mock_audio_hal_verify_receive_count(uint32_t expected_count);

/* =============================================================================
 * Frame Capture and Playback
 * ============================================================================= */

/**
 * @brief Enable frame capture for verification
 * 
 * When enabled, all sent frames are stored for later inspection.
 * 
 * @param[in] enable  true to enable capture, false to disable
 * @param[in] max_frames  Maximum number of frames to capture (0=unlimited)
 */
void mock_audio_hal_enable_frame_capture(bool enable, size_t max_frames);

/**
 * @brief Get number of captured frames
 * 
 * @return Number of frames captured via send_frames
 */
size_t mock_audio_hal_get_captured_frame_count(void);

/**
 * @brief Get captured frame data
 * 
 * @param[in] frame_index  Frame index (0 to captured_count-1)
 * @param[out] frame_data  Buffer to receive frame data (2 channels)
 * @return true if frame retrieved, false if index out of range
 */
bool mock_audio_hal_get_captured_frame(size_t frame_index, int32_t frame_data[2]);

/**
 * @brief Load frames for receive_frames playback
 * 
 * Pre-loads frames that will be returned by receive_frames calls.
 * 
 * @param[in] frames       Frame data (interleaved, 2 channels)
 * @param[in] frame_count  Number of frames to load
 * @return true if frames loaded successfully
 */
bool mock_audio_hal_load_receive_frames(const int32_t* frames, size_t frame_count);

/**
 * @brief Clear all captured and loaded frames
 */
void mock_audio_hal_clear_frames(void);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_AUDIO_HAL_H */
