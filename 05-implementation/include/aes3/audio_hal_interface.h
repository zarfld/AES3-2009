/**
 * @file audio_hal_interface.h
 * @brief AES3-2009 Hardware Abstraction Layer Interface (Pure C)
 * @implements DES-I-001
 * @traceability REQ-INTERFACE-001, ARC-C-002, ADR-001
 * 
 * Pure C interface (exactly 10 functions per ADR-001) for platform-independent
 * audio hardware access. This interface enables Standards Layer to remain
 * completely hardware and vendor agnostic.
 * 
 * AES3-2009 References:
 * - Part 4: Physical and Electrical interfaces
 * - Part 1: Sampling frequencies (AES5-2018 compliance)
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef AES3_AUDIO_HAL_INTERFACE_H
#define AES3_AUDIO_HAL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* =============================================================================
 * Constants and Enumerations
 * ============================================================================= */

/** Audio HAL function return codes */
typedef enum {
    AUDIO_HAL_SUCCESS = 0,           /**< Operation successful */
    AUDIO_HAL_ERROR_INVALID_PARAM,   /**< Invalid parameter */
    AUDIO_HAL_ERROR_NOT_INITIALIZED, /**< HAL not initialized */
    AUDIO_HAL_ERROR_ALREADY_RUNNING, /**< Audio already started */
    AUDIO_HAL_ERROR_NOT_RUNNING,     /**< Audio not started */
    AUDIO_HAL_ERROR_HARDWARE,        /**< Hardware error */
    AUDIO_HAL_ERROR_TIMEOUT,         /**< Operation timeout */
    AUDIO_HAL_ERROR_BUFFER_OVERFLOW, /**< Buffer overflow */
    AUDIO_HAL_ERROR_BUFFER_UNDERRUN, /**< Buffer underrun */
    AUDIO_HAL_ERROR_UNSUPPORTED      /**< Feature not supported */
} audio_hal_result_t;

/** AES5-2018 preferred sampling frequencies (Hz) */
typedef enum {
    AUDIO_HAL_SAMPLE_RATE_32KHZ = 32000,    /**< 32 kHz */
    AUDIO_HAL_SAMPLE_RATE_44_1KHZ = 44100,  /**< 44.1 kHz (CD) */
    AUDIO_HAL_SAMPLE_RATE_48KHZ = 48000,    /**< 48 kHz (professional) */
    AUDIO_HAL_SAMPLE_RATE_88_2KHZ = 88200,  /**< 88.2 kHz (2x 44.1) */
    AUDIO_HAL_SAMPLE_RATE_96KHZ = 96000,    /**< 96 kHz (2x 48) */
    AUDIO_HAL_SAMPLE_RATE_176_4KHZ = 176400,/**< 176.4 kHz (4x 44.1) */
    AUDIO_HAL_SAMPLE_RATE_192KHZ = 192000   /**< 192 kHz (4x 48) */
} audio_hal_sample_rate_t;

/** Audio HAL capability flags (bitfield) */
typedef enum {
    AUDIO_HAL_CAP_16BIT_SAMPLES = (1 << 0),  /**< 16-bit PCM */
    AUDIO_HAL_CAP_20BIT_SAMPLES = (1 << 1),  /**< 20-bit PCM */
    AUDIO_HAL_CAP_24BIT_SAMPLES = (1 << 2),  /**< 24-bit PCM */
    AUDIO_HAL_CAP_32KHZ_SAMPLING = (1 << 3), /**< 32 kHz */
    AUDIO_HAL_CAP_44_1KHZ_SAMPLING = (1 << 4),/**< 44.1 kHz */
    AUDIO_HAL_CAP_48KHZ_SAMPLING = (1 << 5), /**< 48 kHz */
    AUDIO_HAL_CAP_88_2KHZ_SAMPLING = (1 << 6),/**< 88.2 kHz */
    AUDIO_HAL_CAP_96KHZ_SAMPLING = (1 << 7), /**< 96 kHz */
    AUDIO_HAL_CAP_176_4KHZ_SAMPLING = (1 << 8),/**< 176.4 kHz */
    AUDIO_HAL_CAP_192KHZ_SAMPLING = (1 << 9), /**< 192 kHz */
    AUDIO_HAL_CAP_BALANCED_TRANSMISSION = (1 << 10),/**< 110Ω XLR */
    AUDIO_HAL_CAP_COAXIAL_TRANSMISSION = (1 << 11) /**< 75Ω BNC */
} audio_hal_capabilities_t;

/* =============================================================================
 * Data Structures
 * ============================================================================= */

/**
 * @brief Audio HAL configuration structure
 * 
 * Configures sampling rate, frame size, and channel count.
 */
typedef struct {
    audio_hal_sample_rate_t sample_rate; /**< Sampling frequency */
    uint32_t frames_per_callback;        /**< Frames per callback (power of 2) */
    uint8_t num_channels;                /**< Number of audio channels (2 for stereo) */
    uint32_t capabilities;               /**< Hardware capability flags */
} audio_hal_config_t;

/**
 * @brief Audio HAL status information
 * 
 * Provides runtime status including frame counts and errors.
 */
typedef struct {
    uint64_t frames_transmitted;  /**< Total frames transmitted */
    uint64_t frames_received;     /**< Total frames received */
    uint32_t buffer_overflows;    /**< Count of buffer overflows */
    uint32_t buffer_underruns;    /**< Count of buffer underruns */
    uint64_t current_time_ns;     /**< Current sample clock time (nanoseconds) */
    bool is_running;              /**< Audio streaming active */
} audio_hal_status_info_t;

/**
 * @brief Audio frame callback function type
 * 
 * Called by HAL when audio frames are ready for processing.
 * 
 * @param[in] input_frames  Input audio frames (interleaved, NULL if output-only)
 * @param[out] output_frames Output audio frames (interleaved, NULL if input-only)
 * @param[in] frame_count   Number of frames in buffers
 * @param[in] user_data     User-provided context pointer
 * @return 0 to continue streaming, non-zero to stop
 */
typedef int (*audio_frame_callback_t)(const int32_t* input_frames,
                                      int32_t* output_frames,
                                      size_t frame_count,
                                      void* user_data);

/**
 * @brief Audio error callback function type
 * 
 * Called by HAL when errors occur during audio streaming.
 * 
 * @param[in] error       Error code
 * @param[in] error_msg   Human-readable error message
 * @param[in] user_data   User-provided context pointer
 */
typedef void (*audio_error_callback_t)(audio_hal_result_t error,
                                       const char* error_msg,
                                       void* user_data);

/* =============================================================================
 * Audio HAL Interface Structure (Function Pointer Table)
 * ============================================================================= */

/**
 * @brief Audio Hardware Abstraction Layer Interface
 * 
 * Function pointer table for platform-independent audio hardware access.
 * Implementations must provide all 10 functions (ADR-001 constraint).
 * 
 * @note Thread Safety:
 * - send_frames/receive_frames: Lock-free (real-time safe)
 * - Other functions: May use locks (not real-time critical)
 */
typedef struct {
    /**
     * @brief Initialize audio hardware
     * 
     * @param[in] config  Hardware configuration
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance Non-critical (<1ms acceptable)
     */
    audio_hal_result_t (*init)(const audio_hal_config_t* config);
    
    /**
     * @brief Send audio frames to hardware (output)
     * 
     * @param[in] frames       Audio frames (interleaved int32_t samples)
     * @param[in] frame_count  Number of frames to send
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <5µs (real-time critical)
     * @threadsafety Lock-free implementation required
     */
    audio_hal_result_t (*send_frames)(const int32_t* frames, size_t frame_count);
    
    /**
     * @brief Receive audio frames from hardware (input)
     * 
     * @param[out] frames      Buffer for audio frames (interleaved int32_t)
     * @param[in] frame_count  Number of frames to receive
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <5µs (real-time critical)
     * @threadsafety Lock-free implementation required
     */
    audio_hal_result_t (*receive_frames)(int32_t* frames, size_t frame_count);
    
    /**
     * @brief Set sampling rate (AES5-2018 frequencies)
     * 
     * @param[in] sample_rate  New sampling frequency
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <10ms (non-critical)
     */
    audio_hal_result_t (*set_sample_rate)(audio_hal_sample_rate_t sample_rate);
    
    /**
     * @brief Get current sample clock time
     * 
     * @return Sample clock time in nanoseconds
     * @performance <1µs (timing critical)
     * @threadsafety Lock-free implementation required
     */
    uint64_t (*get_sample_clock_ns)(void);
    
    /**
     * @brief Set audio frame and error callbacks
     * 
     * @param[in] frame_callback  Callback for audio frame processing
     * @param[in] error_callback  Callback for error notifications
     * @param[in] user_data       User context pointer
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <100µs (non-critical)
     */
    audio_hal_result_t (*set_callbacks)(audio_frame_callback_t frame_callback,
                                        audio_error_callback_t error_callback,
                                        void* user_data);
    
    /**
     * @brief Start audio streaming
     * 
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <10ms (non-critical)
     */
    audio_hal_result_t (*start)(void);
    
    /**
     * @brief Stop audio streaming
     * 
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <50ms (non-critical)
     */
    audio_hal_result_t (*stop)(void);
    
    /**
     * @brief Get hardware status information
     * 
     * @param[out] status  Status information structure
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <100µs (monitoring, non-critical)
     */
    audio_hal_result_t (*get_status)(audio_hal_status_info_t* status);
    
    /**
     * @brief Cleanup and release hardware resources
     * 
     * @return AUDIO_HAL_SUCCESS or error code
     * @performance <100ms (shutdown, non-critical)
     */
    audio_hal_result_t (*cleanup)(void);
    
} audio_hal_interface_t;

/* =============================================================================
 * Global Interface Instance (Platform-Specific Implementation)
 * ============================================================================= */

/**
 * @brief Get platform-specific audio HAL interface implementation
 * 
 * Implemented by platform-specific HAL (ASIO/ALSA/CoreAudio).
 * Standards Layer calls this to obtain hardware access.
 * 
 * @return Pointer to platform audio HAL interface
 * @note Implementation provided by Service Layer, NOT Standards Layer
 */
extern const audio_hal_interface_t* audio_hal_get_platform_interface(void);

#ifdef __cplusplus
}
#endif

#endif /* AES3_AUDIO_HAL_INTERFACE_H */
