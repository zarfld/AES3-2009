/**
 * @file test_mock_audio_hal.cpp
 * @brief Unit Tests for Mock Audio HAL
 * @traceability TEST-MOCK-001 through TEST-MOCK-020
 * 
 * Comprehensive tests for mock audio HAL implementation including:
 * - Basic initialization and cleanup
 * - Frame send/receive operations
 * - Call logging and verification
 * - Frame capture and playback
 * - Error injection
 * - Timing simulation
 * - Status reporting
 * 
 * @copyright Copyright (c) 2024
 */

#include <gtest/gtest.h>
#include "mock_audio_hal.h"

class MockAudioHALTest : public ::testing::Test {
protected:
    const audio_hal_interface_t* hal_interface;
    
    void SetUp() override {
        // Initialize mock before each test
        mock_audio_hal_init();
        hal_interface = mock_audio_hal_get_interface();
        ASSERT_NE(hal_interface, nullptr);
    }
    
    void TearDown() override {
        // Cleanup after each test
        if (hal_interface && hal_interface->cleanup) {
            hal_interface->cleanup();
        }
        mock_audio_hal_reset();
    }
};

// =============================================================================
// Initialization and Configuration Tests
// =============================================================================

/**
 * @test TEST-MOCK-001: Mock HAL initialization succeeds with valid config
 */
TEST_F(MockAudioHALTest, Init_ValidConfig_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    
    // Act
    audio_hal_result_t result = hal_interface->init(&config);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    EXPECT_TRUE(mock_audio_hal_verify_init_called(AUDIO_HAL_SAMPLE_RATE_48KHZ, 2));
}

/**
 * @test TEST-MOCK-002: Mock HAL init fails with NULL config
 */
TEST_F(MockAudioHALTest, Init_NullConfig_Failure) {
    // Act
    audio_hal_result_t result = hal_interface->init(nullptr);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_INVALID_PARAM);
}

/**
 * @test TEST-MOCK-003: Mock HAL cleanup succeeds after init
 */
TEST_F(MockAudioHALTest, Cleanup_AfterInit_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    // Act
    audio_hal_result_t result = hal_interface->cleanup();
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    
    mock_hal_statistics_t stats;
    mock_audio_hal_get_statistics(&stats);
    EXPECT_EQ(stats.total_cleanup_calls, 1);
}

// =============================================================================
// Frame Send/Receive Tests
// =============================================================================

/**
 * @test TEST-MOCK-004: Send frames succeeds with valid data
 */
TEST_F(MockAudioHALTest, SendFrames_ValidData_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    int32_t frames[512];  // 256 frames * 2 channels
    for (int i = 0; i < 512; i++) {
        frames[i] = i * 1000;
    }
    
    // Act
    audio_hal_result_t result = hal_interface->send_frames(frames, 256);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    EXPECT_TRUE(mock_audio_hal_verify_send_count(1));
    
    mock_hal_statistics_t stats;
    mock_audio_hal_get_statistics(&stats);
    EXPECT_EQ(stats.total_frames_sent, 256);
}

/**
 * @test TEST-MOCK-005: Send frames fails when not initialized
 */
TEST_F(MockAudioHALTest, SendFrames_NotInitialized_Failure) {
    // Arrange
    int32_t frames[2] = {1000, 2000};
    
    // Act
    audio_hal_result_t result = hal_interface->send_frames(frames, 1);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_NOT_INITIALIZED);
}

/**
 * @test TEST-MOCK-006: Receive frames succeeds with valid buffer
 */
TEST_F(MockAudioHALTest, ReceiveFrames_ValidBuffer_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    int32_t frames[512];  // 256 frames * 2 channels
    
    // Act
    audio_hal_result_t result = hal_interface->receive_frames(frames, 256);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    EXPECT_TRUE(mock_audio_hal_verify_receive_count(1));
    
    mock_hal_statistics_t stats;
    mock_audio_hal_get_statistics(&stats);
    EXPECT_EQ(stats.total_frames_received, 256);
}

/**
 * @test TEST-MOCK-007: Receive frames returns silence when no playback buffer loaded
 */
TEST_F(MockAudioHALTest, ReceiveFrames_NoPlaybackBuffer_ReturnsSilence) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    int32_t frames[4] = {999, 999, 999, 999};  // 2 frames * 2 channels
    
    // Act
    audio_hal_result_t result = hal_interface->receive_frames(frames, 2);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    // Should be all zeros (silence)
    EXPECT_EQ(frames[0], 0);
    EXPECT_EQ(frames[1], 0);
    EXPECT_EQ(frames[2], 0);
    EXPECT_EQ(frames[3], 0);
}

// =============================================================================
// Frame Capture Tests
// =============================================================================

/**
 * @test TEST-MOCK-008: Frame capture stores sent frames
 */
TEST_F(MockAudioHALTest, FrameCapture_SendFrames_StoresFrames) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    mock_audio_hal_enable_frame_capture(true, 10);
    
    int32_t frames[6] = {1000, 2000, 3000, 4000, 5000, 6000};  // 3 frames
    
    // Act
    hal_interface->send_frames(frames, 3);
    
    // Assert
    EXPECT_EQ(mock_audio_hal_get_captured_frame_count(), 3);
    
    int32_t captured_frame[2];
    EXPECT_TRUE(mock_audio_hal_get_captured_frame(0, captured_frame));
    EXPECT_EQ(captured_frame[0], 1000);
    EXPECT_EQ(captured_frame[1], 2000);
    
    EXPECT_TRUE(mock_audio_hal_get_captured_frame(1, captured_frame));
    EXPECT_EQ(captured_frame[0], 3000);
    EXPECT_EQ(captured_frame[1], 4000);
    
    EXPECT_TRUE(mock_audio_hal_get_captured_frame(2, captured_frame));
    EXPECT_EQ(captured_frame[0], 5000);
    EXPECT_EQ(captured_frame[1], 6000);
}

/**
 * @test TEST-MOCK-009: Frame capture respects max frame limit
 */
TEST_F(MockAudioHALTest, FrameCapture_MaxFrames_LimitsCapture) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    mock_audio_hal_enable_frame_capture(true, 2);  // Limit to 2 frames
    
    int32_t frames[6] = {1000, 2000, 3000, 4000, 5000, 6000};  // 3 frames
    
    // Act
    hal_interface->send_frames(frames, 3);
    
    // Assert
    EXPECT_EQ(mock_audio_hal_get_captured_frame_count(), 2);  // Only 2 captured
}

// =============================================================================
// Frame Playback Tests
// =============================================================================

/**
 * @test TEST-MOCK-010: Load receive frames provides data for receive_frames calls
 */
TEST_F(MockAudioHALTest, LoadReceiveFrames_ValidData_ProvidesFrames) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    int32_t playback_frames[4] = {100, 200, 300, 400};  // 2 frames
    mock_audio_hal_load_receive_frames(playback_frames, 2);
    
    int32_t received_frames[4];
    
    // Act
    audio_hal_result_t result = hal_interface->receive_frames(received_frames, 2);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    EXPECT_EQ(received_frames[0], 100);
    EXPECT_EQ(received_frames[1], 200);
    EXPECT_EQ(received_frames[2], 300);
    EXPECT_EQ(received_frames[3], 400);
}

/**
 * @test TEST-MOCK-011: Receive frames exhausts playback buffer and returns silence
 */
TEST_F(MockAudioHALTest, ReceiveFrames_ExhaustsBuffer_ReturnsSilence) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    int32_t playback_frames[2] = {100, 200};  // 1 frame
    mock_audio_hal_load_receive_frames(playback_frames, 1);
    
    int32_t received_frames[4];  // Request 2 frames
    
    // Act
    audio_hal_result_t result = hal_interface->receive_frames(received_frames, 2);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    // First frame from buffer
    EXPECT_EQ(received_frames[0], 100);
    EXPECT_EQ(received_frames[1], 200);
    // Second frame is silence (buffer exhausted)
    EXPECT_EQ(received_frames[2], 0);
    EXPECT_EQ(received_frames[3], 0);
}

// =============================================================================
// Start/Stop Tests
// =============================================================================

/**
 * @test TEST-MOCK-012: Start succeeds when initialized
 */
TEST_F(MockAudioHALTest, Start_AfterInit_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    // Act
    audio_hal_result_t result = hal_interface->start();
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    
    audio_hal_status_info_t status;
    hal_interface->get_status(&status);
    EXPECT_TRUE(status.is_running);
}

/**
 * @test TEST-MOCK-013: Start fails when already running
 */
TEST_F(MockAudioHALTest, Start_AlreadyRunning_Failure) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    hal_interface->start();
    
    // Act
    audio_hal_result_t result = hal_interface->start();
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_ALREADY_RUNNING);
}

/**
 * @test TEST-MOCK-014: Stop succeeds when running
 */
TEST_F(MockAudioHALTest, Stop_WhenRunning_Success) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    hal_interface->start();
    
    // Act
    audio_hal_result_t result = hal_interface->stop();
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_SUCCESS);
    
    audio_hal_status_info_t status;
    hal_interface->get_status(&status);
    EXPECT_FALSE(status.is_running);
}

// =============================================================================
// Error Injection Tests
// =============================================================================

/**
 * @test TEST-MOCK-015: Error injection causes init to fail
 */
TEST_F(MockAudioHALTest, ErrorInjection_Init_Fails) {
    // Arrange
    mock_error_config_t error_config = {0};
    error_config.inject_init_error = true;
    mock_audio_hal_set_error_config(&error_config);
    
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    
    // Act
    audio_hal_result_t result = hal_interface->init(&config);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_HARDWARE);
}

/**
 * @test TEST-MOCK-016: Error injection causes send_frames to fail
 */
TEST_F(MockAudioHALTest, ErrorInjection_SendFrames_Fails) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    mock_error_config_t error_config = {0};
    error_config.inject_send_error = true;
    mock_audio_hal_set_error_config(&error_config);
    
    int32_t frames[2] = {1000, 2000};
    
    // Act
    audio_hal_result_t result = hal_interface->send_frames(frames, 1);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_HARDWARE);
}

/**
 * @test TEST-MOCK-017: Error injection causes buffer overflow after N sends
 */
TEST_F(MockAudioHALTest, ErrorInjection_BufferOverflow_AfterNSends) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    mock_error_config_t error_config = {0};
    error_config.fail_after_n_sends = 2;  // Fail after 2 successful sends
    mock_audio_hal_set_error_config(&error_config);
    
    int32_t frames[2] = {1000, 2000};
    
    // Act - First two sends should succeed
    EXPECT_EQ(hal_interface->send_frames(frames, 1), AUDIO_HAL_SUCCESS);
    EXPECT_EQ(hal_interface->send_frames(frames, 1), AUDIO_HAL_SUCCESS);
    
    // Third send should fail
    audio_hal_result_t result = hal_interface->send_frames(frames, 1);
    
    // Assert
    EXPECT_EQ(result, AUDIO_HAL_ERROR_BUFFER_OVERFLOW);
    
    audio_hal_status_info_t status;
    hal_interface->get_status(&status);
    EXPECT_EQ(status.buffer_overflows, 1);
}

// =============================================================================
// Timing Simulation Tests
// =============================================================================

/**
 * @test TEST-MOCK-018: Clock advances on send_frames
 */
TEST_F(MockAudioHALTest, Timing_SendFrames_AdvancesClock) {
    // Arrange
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    hal_interface->init(&config);
    
    uint64_t time_before = hal_interface->get_sample_clock_ns();
    
    int32_t frames[512];  // 256 frames
    memset(frames, 0, sizeof(frames));
    
    // Act
    hal_interface->send_frames(frames, 256);
    uint64_t time_after = hal_interface->get_sample_clock_ns();
    
    // Assert
    // At 48kHz: 256 frames = 256 / 48000 seconds = 5,333,333 ns
    uint64_t expected_elapsed = (256ULL * 1000000000ULL) / 48000ULL;
    uint64_t actual_elapsed = time_after - time_before;
    
    // Allow 1% tolerance for integer division rounding
    EXPECT_NEAR(static_cast<double>(actual_elapsed), 
                static_cast<double>(expected_elapsed), 
                expected_elapsed * 0.01);
}

/**
 * @test TEST-MOCK-019: Manual clock advance works
 */
TEST_F(MockAudioHALTest, Timing_ManualAdvance_UpdatesClock) {
    // Arrange
    mock_audio_hal_set_simulated_sample_rate(AUDIO_HAL_SAMPLE_RATE_48KHZ);
    
    uint64_t time_before = hal_interface->get_sample_clock_ns();
    
    // Act
    mock_audio_hal_advance_clock(480);  // 10ms worth at 48kHz
    
    uint64_t time_after = hal_interface->get_sample_clock_ns();
    
    // Assert
    // 480 frames at 48kHz = 10,000,000 ns (10ms)
    uint64_t expected_elapsed = (480ULL * 1000000000ULL) / 48000ULL;
    uint64_t actual_elapsed = time_after - time_before;
    
    // Allow 1% tolerance for integer division rounding
    EXPECT_NEAR(static_cast<double>(actual_elapsed), 
                static_cast<double>(expected_elapsed), 
                expected_elapsed * 0.01);
}

// =============================================================================
// Call Logging Tests
// =============================================================================

/**
 * @test TEST-MOCK-020: Call logging records all HAL calls
 */
TEST_F(MockAudioHALTest, CallLogging_RecordsAllCalls) {
    // Arrange & Act
    audio_hal_config_t config = {
        /*sample_rate*/ AUDIO_HAL_SAMPLE_RATE_48KHZ,
        /*frames_per_callback*/ 256,
        /*num_channels*/ 2,
        /*capabilities*/ 0
    };
    
    hal_interface->init(&config);
    hal_interface->start();
    
    int32_t frames[2] = {1000, 2000};
    hal_interface->send_frames(frames, 1);
    
    hal_interface->stop();
    
    // Assert
    size_t call_count = mock_audio_hal_get_call_count();
    EXPECT_GE(call_count, 4);  // At least init, start, send, stop
    
    mock_call_log_entry_t entry;
    
    // Check init call
    EXPECT_TRUE(mock_audio_hal_get_call_log(0, &entry));
    EXPECT_EQ(entry.call_type, MOCK_CALL_INIT);
    EXPECT_EQ(entry.result, AUDIO_HAL_SUCCESS);
    
    // Check start call
    EXPECT_TRUE(mock_audio_hal_get_call_log(1, &entry));
    EXPECT_EQ(entry.call_type, MOCK_CALL_START);
    EXPECT_EQ(entry.result, AUDIO_HAL_SUCCESS);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
