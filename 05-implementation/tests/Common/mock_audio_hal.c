/**
 * @file mock_audio_hal.c
 * @brief Mock Audio HAL Implementation for Testing
 * @implements mock_audio_hal.h
 * 
 * Thread-safe mock implementation with:
 * - Call logging (up to 1000 entries)
 * - Frame capture (up to 10000 frames)
 * - Timing simulation (nanosecond precision)
 * - Error injection
 * 
 * @copyright Copyright (c) 2024
 */

#include "mock_audio_hal.h"
#include <string.h>
#include <stdlib.h>

/* =============================================================================
 * Internal Constants
 * ============================================================================= */

#define MAX_CALL_LOG_ENTRIES 1000
#define MAX_CAPTURED_FRAMES 10000
#define MAX_RECEIVE_FRAMES 10000

/* =============================================================================
 * Internal State
 * ============================================================================= */

typedef struct {
    // Configuration
    audio_hal_config_t config;
    bool is_initialized;
    bool is_running;
    
    // Callbacks
    audio_frame_callback_t frame_callback;
    audio_error_callback_t error_callback;
    void* user_data;
    
    // Timing simulation
    uint64_t current_time_ns;
    audio_hal_sample_rate_t simulated_sample_rate;
    
    // Call logging
    mock_call_log_entry_t call_log[MAX_CALL_LOG_ENTRIES];
    size_t call_log_count;
    
    // Statistics
    mock_hal_statistics_t stats;
    
    // Error injection
    mock_error_config_t error_config;
    uint32_t send_call_counter;
    uint32_t receive_call_counter;
    
    // Frame capture
    bool capture_enabled;
    size_t max_capture_frames;
    int32_t* captured_frames;  // Interleaved stereo
    size_t captured_frame_count;
    
    // Frame playback (for receive)
    int32_t* receive_frames_buffer;  // Interleaved stereo
    size_t receive_frames_count;
    size_t receive_frames_position;
    
    // Status tracking
    uint64_t frames_transmitted;
    uint64_t frames_received;
    uint32_t buffer_overflows;
    uint32_t buffer_underruns;
    
} mock_hal_state_t;

static mock_hal_state_t g_mock_state = {0};

/* =============================================================================
 * Internal Helper Functions
 * ============================================================================= */

/**
 * @brief Log a HAL call for verification
 */
static void log_call(mock_call_type_t call_type, audio_hal_result_t result, 
                     size_t frame_count, audio_hal_sample_rate_t sample_rate) {
    if (g_mock_state.call_log_count < MAX_CALL_LOG_ENTRIES) {
        mock_call_log_entry_t* entry = &g_mock_state.call_log[g_mock_state.call_log_count++];
        entry->call_type = call_type;
        entry->timestamp_ns = g_mock_state.current_time_ns;
        entry->result = result;
        entry->frame_count = frame_count;
        entry->sample_rate = sample_rate;
    }
}

/**
 * @brief Calculate nanoseconds per frame for current sample rate
 */
static uint64_t get_ns_per_frame(void) {
    if (g_mock_state.simulated_sample_rate == 0) {
        g_mock_state.simulated_sample_rate = AUDIO_HAL_SAMPLE_RATE_48KHZ;
    }
    
    // ns_per_frame = 1,000,000,000 / sample_rate
    return 1000000000ULL / (uint64_t)g_mock_state.simulated_sample_rate;
}

/* =============================================================================
 * Mock HAL Function Implementations
 * ============================================================================= */

static audio_hal_result_t mock_init(const audio_hal_config_t* config) {
    g_mock_state.stats.total_init_calls++;
    
    // Check error injection
    if (g_mock_state.error_config.inject_init_error) {
        log_call(MOCK_CALL_INIT, AUDIO_HAL_ERROR_HARDWARE, 0, 0);
        return AUDIO_HAL_ERROR_HARDWARE;
    }
    
    // Validate parameters
    if (!config) {
        log_call(MOCK_CALL_INIT, AUDIO_HAL_ERROR_INVALID_PARAM, 0, 0);
        return AUDIO_HAL_ERROR_INVALID_PARAM;
    }
    
    // Store configuration
    memcpy(&g_mock_state.config, config, sizeof(audio_hal_config_t));
    g_mock_state.is_initialized = true;
    g_mock_state.simulated_sample_rate = config->sample_rate;
    
    log_call(MOCK_CALL_INIT, AUDIO_HAL_SUCCESS, 0, config->sample_rate);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_send_frames(const int32_t* frames, size_t frame_count) {
    g_mock_state.stats.total_send_calls++;
    g_mock_state.send_call_counter++;
    
    // Check error injection
    if (g_mock_state.error_config.inject_send_error) {
        log_call(MOCK_CALL_SEND_FRAMES, AUDIO_HAL_ERROR_HARDWARE, frame_count, 0);
        return AUDIO_HAL_ERROR_HARDWARE;
    }
    
    if (g_mock_state.error_config.fail_after_n_sends > 0 &&
        g_mock_state.send_call_counter > g_mock_state.error_config.fail_after_n_sends) {
        log_call(MOCK_CALL_SEND_FRAMES, AUDIO_HAL_ERROR_BUFFER_OVERFLOW, frame_count, 0);
        g_mock_state.buffer_overflows++;
        return AUDIO_HAL_ERROR_BUFFER_OVERFLOW;
    }
    
    // Validate state
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_SEND_FRAMES, AUDIO_HAL_ERROR_NOT_INITIALIZED, frame_count, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    if (!frames || frame_count == 0) {
        log_call(MOCK_CALL_SEND_FRAMES, AUDIO_HAL_ERROR_INVALID_PARAM, frame_count, 0);
        return AUDIO_HAL_ERROR_INVALID_PARAM;
    }
    
    // Capture frames if enabled
    if (g_mock_state.capture_enabled) {
        size_t frames_to_capture = frame_count;
        
        // Limit to max capture size
        if (g_mock_state.max_capture_frames > 0) {
            size_t remaining = g_mock_state.max_capture_frames - g_mock_state.captured_frame_count;
            if (frames_to_capture > remaining) {
                frames_to_capture = remaining;
            }
        }
        
        if (frames_to_capture > 0 && 
            g_mock_state.captured_frame_count + frames_to_capture <= MAX_CAPTURED_FRAMES) {
            
            // Allocate capture buffer on first use
            if (!g_mock_state.captured_frames) {
                g_mock_state.captured_frames = (int32_t*)malloc(MAX_CAPTURED_FRAMES * 2 * sizeof(int32_t));
            }
            
            if (g_mock_state.captured_frames) {
                // Copy frames (2 channels interleaved)
                size_t samples_to_copy = frames_to_capture * 2;
                int32_t* dest = &g_mock_state.captured_frames[g_mock_state.captured_frame_count * 2];
                memcpy(dest, frames, samples_to_copy * sizeof(int32_t));
                g_mock_state.captured_frame_count += frames_to_capture;
            }
        }
    }
    
    // Update statistics
    g_mock_state.stats.total_frames_sent += (uint32_t)frame_count;
    g_mock_state.frames_transmitted += frame_count;
    
    // Advance simulated clock
    uint64_t elapsed_ns = get_ns_per_frame() * frame_count;
    g_mock_state.current_time_ns += elapsed_ns;
    
    log_call(MOCK_CALL_SEND_FRAMES, AUDIO_HAL_SUCCESS, frame_count, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_receive_frames(int32_t* frames, size_t frame_count) {
    g_mock_state.stats.total_receive_calls++;
    g_mock_state.receive_call_counter++;
    
    // Check error injection
    if (g_mock_state.error_config.inject_receive_error) {
        log_call(MOCK_CALL_RECEIVE_FRAMES, AUDIO_HAL_ERROR_HARDWARE, frame_count, 0);
        return AUDIO_HAL_ERROR_HARDWARE;
    }
    
    if (g_mock_state.error_config.fail_after_n_receives > 0 &&
        g_mock_state.receive_call_counter > g_mock_state.error_config.fail_after_n_receives) {
        log_call(MOCK_CALL_RECEIVE_FRAMES, AUDIO_HAL_ERROR_BUFFER_UNDERRUN, frame_count, 0);
        g_mock_state.buffer_underruns++;
        return AUDIO_HAL_ERROR_BUFFER_UNDERRUN;
    }
    
    // Validate state
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_RECEIVE_FRAMES, AUDIO_HAL_ERROR_NOT_INITIALIZED, frame_count, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    if (!frames || frame_count == 0) {
        log_call(MOCK_CALL_RECEIVE_FRAMES, AUDIO_HAL_ERROR_INVALID_PARAM, frame_count, 0);
        return AUDIO_HAL_ERROR_INVALID_PARAM;
    }
    
    // Copy frames from playback buffer if available
    if (g_mock_state.receive_frames_buffer && g_mock_state.receive_frames_count > 0) {
        size_t frames_to_copy = frame_count;
        size_t remaining = g_mock_state.receive_frames_count - g_mock_state.receive_frames_position;
        
        if (frames_to_copy > remaining) {
            frames_to_copy = remaining;
            
            // Fill remainder with silence
            if (frames_to_copy < frame_count) {
                size_t silence_frames = frame_count - frames_to_copy;
                memset(&frames[frames_to_copy * 2], 0, silence_frames * 2 * sizeof(int32_t));
            }
        }
        
        if (frames_to_copy > 0) {
            int32_t* src = &g_mock_state.receive_frames_buffer[g_mock_state.receive_frames_position * 2];
            memcpy(frames, src, frames_to_copy * 2 * sizeof(int32_t));
            g_mock_state.receive_frames_position += frames_to_copy;
        }
    } else {
        // No playback buffer - return silence
        memset(frames, 0, frame_count * 2 * sizeof(int32_t));
    }
    
    // Update statistics
    g_mock_state.stats.total_frames_received += (uint32_t)frame_count;
    g_mock_state.frames_received += frame_count;
    
    // Advance simulated clock
    uint64_t elapsed_ns = get_ns_per_frame() * frame_count;
    g_mock_state.current_time_ns += elapsed_ns;
    
    log_call(MOCK_CALL_RECEIVE_FRAMES, AUDIO_HAL_SUCCESS, frame_count, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_set_sample_rate(audio_hal_sample_rate_t sample_rate) {
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_SET_SAMPLE_RATE, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, sample_rate);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    g_mock_state.config.sample_rate = sample_rate;
    g_mock_state.simulated_sample_rate = sample_rate;
    
    log_call(MOCK_CALL_SET_SAMPLE_RATE, AUDIO_HAL_SUCCESS, 0, sample_rate);
    return AUDIO_HAL_SUCCESS;
}

static uint64_t mock_get_sample_clock_ns(void) {
    log_call(MOCK_CALL_GET_SAMPLE_CLOCK_NS, AUDIO_HAL_SUCCESS, 0, 0);
    return g_mock_state.current_time_ns;
}

static audio_hal_result_t mock_set_callbacks(audio_frame_callback_t frame_callback,
                                              audio_error_callback_t error_callback,
                                              void* user_data) {
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_SET_CALLBACKS, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    g_mock_state.frame_callback = frame_callback;
    g_mock_state.error_callback = error_callback;
    g_mock_state.user_data = user_data;
    
    log_call(MOCK_CALL_SET_CALLBACKS, AUDIO_HAL_SUCCESS, 0, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_start(void) {
    g_mock_state.stats.total_start_calls++;
    
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_START, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    if (g_mock_state.is_running) {
        log_call(MOCK_CALL_START, AUDIO_HAL_ERROR_ALREADY_RUNNING, 0, 0);
        return AUDIO_HAL_ERROR_ALREADY_RUNNING;
    }
    
    g_mock_state.is_running = true;
    log_call(MOCK_CALL_START, AUDIO_HAL_SUCCESS, 0, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_stop(void) {
    g_mock_state.stats.total_stop_calls++;
    
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_STOP, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    if (!g_mock_state.is_running) {
        log_call(MOCK_CALL_STOP, AUDIO_HAL_ERROR_NOT_RUNNING, 0, 0);
        return AUDIO_HAL_ERROR_NOT_RUNNING;
    }
    
    g_mock_state.is_running = false;
    log_call(MOCK_CALL_STOP, AUDIO_HAL_SUCCESS, 0, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_get_status(audio_hal_status_info_t* status) {
    if (!status) {
        log_call(MOCK_CALL_GET_STATUS, AUDIO_HAL_ERROR_INVALID_PARAM, 0, 0);
        return AUDIO_HAL_ERROR_INVALID_PARAM;
    }
    
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_GET_STATUS, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    status->frames_transmitted = g_mock_state.frames_transmitted;
    status->frames_received = g_mock_state.frames_received;
    status->buffer_overflows = g_mock_state.buffer_overflows;
    status->buffer_underruns = g_mock_state.buffer_underruns;
    status->current_time_ns = g_mock_state.current_time_ns;
    status->is_running = g_mock_state.is_running;
    
    log_call(MOCK_CALL_GET_STATUS, AUDIO_HAL_SUCCESS, 0, 0);
    return AUDIO_HAL_SUCCESS;
}

static audio_hal_result_t mock_cleanup(void) {
    g_mock_state.stats.total_cleanup_calls++;
    
    if (!g_mock_state.is_initialized) {
        log_call(MOCK_CALL_CLEANUP, AUDIO_HAL_ERROR_NOT_INITIALIZED, 0, 0);
        return AUDIO_HAL_ERROR_NOT_INITIALIZED;
    }
    
    // Free allocated buffers
    if (g_mock_state.captured_frames) {
        free(g_mock_state.captured_frames);
        g_mock_state.captured_frames = NULL;
    }
    
    if (g_mock_state.receive_frames_buffer) {
        free(g_mock_state.receive_frames_buffer);
        g_mock_state.receive_frames_buffer = NULL;
    }
    
    g_mock_state.is_initialized = false;
    g_mock_state.is_running = false;
    
    log_call(MOCK_CALL_CLEANUP, AUDIO_HAL_SUCCESS, 0, 0);
    return AUDIO_HAL_SUCCESS;
}

/* =============================================================================
 * Mock HAL Interface Table
 * ============================================================================= */

static const audio_hal_interface_t g_mock_interface = {
    .init = mock_init,
    .send_frames = mock_send_frames,
    .receive_frames = mock_receive_frames,
    .set_sample_rate = mock_set_sample_rate,
    .get_sample_clock_ns = mock_get_sample_clock_ns,
    .set_callbacks = mock_set_callbacks,
    .start = mock_start,
    .stop = mock_stop,
    .get_status = mock_get_status,
    .cleanup = mock_cleanup
};

/* =============================================================================
 * Public Mock Control Functions
 * ============================================================================= */

void mock_audio_hal_init(void) {
    memset(&g_mock_state, 0, sizeof(mock_hal_state_t));
    g_mock_state.simulated_sample_rate = AUDIO_HAL_SAMPLE_RATE_48KHZ;
}

void mock_audio_hal_reset(void) {
    // Preserve error config
    mock_error_config_t saved_error_config = g_mock_state.error_config;
    
    // Free buffers
    if (g_mock_state.captured_frames) {
        free(g_mock_state.captured_frames);
    }
    if (g_mock_state.receive_frames_buffer) {
        free(g_mock_state.receive_frames_buffer);
    }
    
    // Reset state
    memset(&g_mock_state, 0, sizeof(mock_hal_state_t));
    g_mock_state.simulated_sample_rate = AUDIO_HAL_SAMPLE_RATE_48KHZ;
    
    // Restore error config
    g_mock_state.error_config = saved_error_config;
}

const audio_hal_interface_t* mock_audio_hal_get_interface(void) {
    return &g_mock_interface;
}

void mock_audio_hal_set_error_config(const mock_error_config_t* error_config) {
    if (error_config) {
        memcpy(&g_mock_state.error_config, error_config, sizeof(mock_error_config_t));
    }
}

void mock_audio_hal_set_simulated_sample_rate(audio_hal_sample_rate_t sample_rate) {
    g_mock_state.simulated_sample_rate = sample_rate;
}

void mock_audio_hal_advance_clock(uint32_t frame_count) {
    uint64_t elapsed_ns = get_ns_per_frame() * frame_count;
    g_mock_state.current_time_ns += elapsed_ns;
}

size_t mock_audio_hal_get_call_count(void) {
    return g_mock_state.call_log_count;
}

bool mock_audio_hal_get_call_log(size_t index, mock_call_log_entry_t* entry) {
    if (index >= g_mock_state.call_log_count || !entry) {
        return false;
    }
    
    memcpy(entry, &g_mock_state.call_log[index], sizeof(mock_call_log_entry_t));
    return true;
}

void mock_audio_hal_get_statistics(mock_hal_statistics_t* stats) {
    if (stats) {
        memcpy(stats, &g_mock_state.stats, sizeof(mock_hal_statistics_t));
    }
}

bool mock_audio_hal_verify_init_called(audio_hal_sample_rate_t expected_sample_rate,
                                        uint8_t expected_channels) {
    if (!g_mock_state.is_initialized) {
        return false;
    }
    
    return (g_mock_state.config.sample_rate == expected_sample_rate &&
            g_mock_state.config.num_channels == expected_channels);
}

bool mock_audio_hal_verify_send_count(uint32_t expected_count) {
    return (g_mock_state.stats.total_send_calls == expected_count);
}

bool mock_audio_hal_verify_receive_count(uint32_t expected_count) {
    return (g_mock_state.stats.total_receive_calls == expected_count);
}

void mock_audio_hal_enable_frame_capture(bool enable, size_t max_frames) {
    g_mock_state.capture_enabled = enable;
    g_mock_state.max_capture_frames = max_frames;
    
    if (!enable && g_mock_state.captured_frames) {
        free(g_mock_state.captured_frames);
        g_mock_state.captured_frames = NULL;
        g_mock_state.captured_frame_count = 0;
    }
}

size_t mock_audio_hal_get_captured_frame_count(void) {
    return g_mock_state.captured_frame_count;
}

bool mock_audio_hal_get_captured_frame(size_t frame_index, int32_t frame_data[2]) {
    if (frame_index >= g_mock_state.captured_frame_count || !frame_data ||
        !g_mock_state.captured_frames) {
        return false;
    }
    
    int32_t* src = &g_mock_state.captured_frames[frame_index * 2];
    frame_data[0] = src[0];  // Left channel
    frame_data[1] = src[1];  // Right channel
    
    return true;
}

bool mock_audio_hal_load_receive_frames(const int32_t* frames, size_t frame_count) {
    if (!frames || frame_count == 0 || frame_count > MAX_RECEIVE_FRAMES) {
        return false;
    }
    
    // Allocate buffer if needed
    if (!g_mock_state.receive_frames_buffer) {
        g_mock_state.receive_frames_buffer = (int32_t*)malloc(MAX_RECEIVE_FRAMES * 2 * sizeof(int32_t));
        if (!g_mock_state.receive_frames_buffer) {
            return false;
        }
    }
    
    // Copy frames
    memcpy(g_mock_state.receive_frames_buffer, frames, frame_count * 2 * sizeof(int32_t));
    g_mock_state.receive_frames_count = frame_count;
    g_mock_state.receive_frames_position = 0;
    
    return true;
}

void mock_audio_hal_clear_frames(void) {
    g_mock_state.captured_frame_count = 0;
    g_mock_state.receive_frames_count = 0;
    g_mock_state.receive_frames_position = 0;
    
    if (g_mock_state.captured_frames) {
        memset(g_mock_state.captured_frames, 0, MAX_CAPTURED_FRAMES * 2 * sizeof(int32_t));
    }
}
