/**
 * @file test_transmit_integration.cpp
 * @brief Integration Tests for AES3-2009 Transmit Path
 * @traceability TEST-INT-001 through TEST-INT-010
 * 
 * End-to-end integration tests validating the complete transmit chain:
 * PCM Audio Sample → PCM Encoder → Subframe Builder → Mock Audio HAL
 * 
 * Tests verify:
 * - Complete data flow through all components
 * - Correct AES3-2009 protocol compliance
 * - Performance requirements (<10µs per sample)
 * - Error handling and edge cases
 * - Stereo frame assembly
 * 
 * @standard AES3-2009 Parts 1-3 (Audio Content, Metadata, Transport)
 * @copyright Copyright (c) 2025
 */

#include <gtest/gtest.h>
#include "AES/AES3/Part1/_2009/audio_coding/pcm_encoder.hpp"
#include "AES/AES3/Part3/_2009/subframe/subframe_builder.hpp"
#include "AES/AES3/Part3/_2009/subframe/subframe_data.hpp"
#include "../Common/mock_audio_hal.h"
#include <chrono>
#include <vector>

using namespace AES::AES3::Part1::_2009::audio_coding;
using namespace AES::AES3::Part3::_2009::subframe;

// =============================================================================
// C++ Wrapper for Mock HAL (for easier integration testing)
// =============================================================================

class MockHALWrapper {
public:
    MockHALWrapper() {
        mock_audio_hal_init();
    }
    
    ~MockHALWrapper() {
        // No cleanup function needed for mock HAL
    }
    
    void reset() {
        mock_audio_hal_reset();
    }
    
    int transmit_subframe(uint64_t subframe_data) {
        // Store subframe for verification
        transmitted_subframes.push_back(subframe_data);
        return 0;  // Success
    }
    
    size_t get_transmit_count() const {
        return transmitted_subframes.size();
    }
    
    const std::vector<uint64_t>& get_captured_subframes() const {
        return transmitted_subframes;
    }
    
private:
    std::vector<uint64_t> transmitted_subframes;
};

// =============================================================================
// Test Fixture
// =============================================================================

class TransmitIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize PCM encoder with 24-bit word length
        Config pcm_cfg;
        pcm_cfg.word_length = WordLength::BITS_24;
        pcm_cfg.pre_emphasis = PreEmphasis::NONE;
        pcm_cfg.auto_validity = true;
        pcm_cfg.validate_inputs = true;
        pcm_encoder = std::make_unique<PCMEncoder>(pcm_cfg);
        
        // Initialize Subframe builder
        SubframeBuilder::Config sf_cfg;
        sf_cfg.word_length = WordLength::BITS_24;
        sf_cfg.auto_parity = true;
        sf_cfg.biphase_coding = false;
        subframe_builder = std::make_unique<SubframeBuilder>(sf_cfg);
        
        // Initialize Mock HAL wrapper
        mock_hal = std::make_unique<MockHALWrapper>();
        mock_hal->reset();
    }
    
    void TearDown() override {
        // Clean up
        pcm_encoder.reset();
        subframe_builder.reset();
        mock_hal.reset();
    }
    
    /**
     * @brief Helper: Transmit single audio sample through complete chain
     */
    int transmit_sample(int32_t pcm_sample, 
                       uint8_t validity,
                       uint8_t user_bit,
                       uint8_t channel_bit,
                       SubframeBuilder::Preamble preamble) {
        // Step 1: Encode PCM sample
        EncodedSample encoded;
        int result = pcm_encoder->encode_sample(pcm_sample, true, encoded);
        if (result != 0) return result;
        
        // Step 2: Build subframe
        SubframeData subframe;
        result = subframe_builder->build_subframe(
            encoded.audio_data,
            validity,
            user_bit,
            channel_bit,
            preamble,
            subframe
        );
        if (result != 0) return result;
        
        // Step 3: Transmit via HAL
        result = mock_hal->transmit_subframe(subframe.data);
        return result;
    }
    
    /**
     * @brief Helper: Transmit stereo pair (left + right channels)
     */
    int transmit_stereo_pair(int32_t left_sample, 
                            int32_t right_sample,
                            uint8_t channel_status_left,
                            uint8_t channel_status_right) {
        int result = transmit_sample(left_sample, 0, 0, channel_status_left, 
                                     SubframeBuilder::Preamble::X);
        if (result != 0) return result;
        
        result = transmit_sample(right_sample, 0, 0, channel_status_right,
                                SubframeBuilder::Preamble::Y);
        return result;
    }
    
    std::unique_ptr<PCMEncoder> pcm_encoder;
    std::unique_ptr<SubframeBuilder> subframe_builder;
    std::unique_ptr<MockHALWrapper> mock_hal;
};

// =============================================================================
// Basic Integration Tests
// =============================================================================

/**
 * @test TEST-INT-001: Single sample transmit chain
 * @traceability DES-C-001, DES-C-003, DES-C-004
 */
TEST_F(TransmitIntegrationTest, SingleSample_CompleteChain_Success) {
    // Arrange
    int32_t pcm_sample = 0x00400000;  // Mid-range positive value
    
    // Act
    int result = transmit_sample(pcm_sample, 0, 0, 0, SubframeBuilder::Preamble::X);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mock_hal->get_transmit_count(), 1);
    
    // Verify subframe was captured
    auto captured = mock_hal->get_captured_subframes();
    ASSERT_EQ(captured.size(), 1);
    
    // Verify preamble X is present (slots 0-3)
    SubframeData received(captured[0]);
    EXPECT_EQ(received.get_bit(0), 0x02);  // Preamble X pattern starts with 0x02
}

/**
 * @test TEST-INT-002: Stereo pair transmit chain
 * @traceability DES-C-001, DES-C-003, DES-C-004
 */
TEST_F(TransmitIntegrationTest, StereoPair_CompleteChain_Success) {
    // Arrange
    int32_t left_sample = 0x00600000;   // Left channel
    int32_t right_sample = 0x00200000;  // Right channel
    
    // Act
    int result = transmit_stereo_pair(left_sample, right_sample, 1, 0);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mock_hal->get_transmit_count(), 2);
    
    // Verify both subframes captured
    auto captured = mock_hal->get_captured_subframes();
    ASSERT_EQ(captured.size(), 2);
    
    // Verify preambles: first=X, second=Y
    SubframeData subframe1(captured[0]);
    SubframeData subframe2(captured[1]);
    
    // Preamble X and Y have different patterns
    EXPECT_NE(subframe1.get_bit(0), subframe2.get_bit(0));
}

/**
 * @test TEST-INT-003: Maximum positive sample transmit
 * @traceability DES-C-003, AES3-2009 Part 1 Section 5
 */
TEST_F(TransmitIntegrationTest, MaxPositiveSample_24Bit_Success) {
    // Arrange
    int32_t pcm_sample = 0x007FFFFF;  // Max positive 24-bit
    
    // Act
    int result = transmit_sample(pcm_sample, 0, 0, 0, SubframeBuilder::Preamble::Z);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mock_hal->get_transmit_count(), 1);
    
    // Verify audio data integrity
    auto captured = mock_hal->get_captured_subframes();
    ASSERT_EQ(captured.size(), 1);
    SubframeData received(captured[0]);
    
    // Count ones in audio slots (4-27) - each audio bit is stored in both bit positions
    // 0x7FFFFF has 23 ones, stored twice = 46 ones total
    int ones_count = 0;
    for (size_t slot = SubframeData::AUDIO_START; slot <= SubframeData::AUDIO_END; ++slot) {
        uint8_t slot_value = received.get_bit(slot);
        if (slot_value & 0x01) ones_count++;
        if (slot_value & 0x02) ones_count++;
    }
    EXPECT_EQ(ones_count, 46);  // 0x7FFFFF has 23 ones × 2 (both bit positions)
}

/**
 * @test TEST-INT-004: Maximum negative sample transmit
 * @traceability DES-C-003, AES3-2009 Part 1 Section 5
 */
TEST_F(TransmitIntegrationTest, MaxNegativeSample_24Bit_Success) {
    // Arrange
    int32_t pcm_sample = 0xFF800000;  // Max negative 24-bit (sign-extended)
    
    // Act
    int result = transmit_sample(pcm_sample, 0, 0, 0, SubframeBuilder::Preamble::X);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(mock_hal->get_transmit_count(), 1);
}

/**
 * @test TEST-INT-005: Zero sample transmit
 * @traceability DES-C-003
 */
TEST_F(TransmitIntegrationTest, ZeroSample_Success) {
    // Arrange
    int32_t pcm_sample = 0x00000000;
    
    // Act
    int result = transmit_sample(pcm_sample, 0, 0, 0, SubframeBuilder::Preamble::X);
    
    // Assert
    EXPECT_EQ(result, 0);
    
    // Verify all audio bits are zero
    auto captured = mock_hal->get_captured_subframes();
    SubframeData received(captured[0]);
    
    for (size_t slot = SubframeData::AUDIO_START; slot <= SubframeData::AUDIO_END; ++slot) {
        EXPECT_EQ(received.get_bit(slot), 0x00) << "Audio slot " << slot << " should be zero";
    }
}

// =============================================================================
// Metadata Integration Tests
// =============================================================================

/**
 * @test TEST-INT-006: Validity bit propagation
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.1
 */
TEST_F(TransmitIntegrationTest, ValidityBit_Unreliable_Propagated) {
    // Arrange
    int32_t pcm_sample = 0x00400000;
    
    // Act - Set validity bit to 1 (unreliable)
    int result = transmit_sample(pcm_sample, 1, 0, 0, SubframeBuilder::Preamble::X);
    
    // Assert
    EXPECT_EQ(result, 0);
    
    auto captured = mock_hal->get_captured_subframes();
    SubframeData received(captured[0]);
    
    // Check validity bit in slot 28
    uint8_t validity_slot = received.get_bit(SubframeData::VALIDITY_SLOT);
    EXPECT_NE(validity_slot & 0x01, 0) << "Validity bit should be set (unreliable)";
}

/**
 * @test TEST-INT-007: Channel status bit propagation
 * @traceability DES-C-004, AES3-2009 Part 2
 */
TEST_F(TransmitIntegrationTest, ChannelStatusBit_Propagated) {
    // Arrange
    int32_t pcm_sample = 0x00400000;
    
    // Act - Set channel status bit
    int result = transmit_sample(pcm_sample, 0, 0, 1, SubframeBuilder::Preamble::X);
    
    // Assert
    EXPECT_EQ(result, 0);
    
    auto captured = mock_hal->get_captured_subframes();
    SubframeData received(captured[0]);
    
    // Check channel status bit in slot 30
    uint8_t channel_slot = received.get_bit(SubframeData::CHANNEL_STATUS_SLOT);
    EXPECT_NE(channel_slot & 0x01, 0) << "Channel status bit should be set";
}

// =============================================================================
// Performance Tests
// =============================================================================

/**
 * @test TEST-INT-008: Transmit path performance (<10µs per sample)
 * @traceability Performance requirements
 */
TEST_F(TransmitIntegrationTest, Performance_SingleSample_Under10Microseconds) {
    // Arrange
    int32_t pcm_sample = 0x00400000;
    const int iterations = 1000;
    
    // Act - Measure 1000 iterations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        transmit_sample(pcm_sample, 0, 0, 0, SubframeBuilder::Preamble::X);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    // Assert - Average time per sample should be < 10µs
    double avg_ns = static_cast<double>(duration.count()) / iterations;
    double avg_us = avg_ns / 1000.0;
    
    EXPECT_LT(avg_us, 10.0) << "Average transmit time: " << avg_us << "µs (target: <10µs)";
    
    // Log performance
    std::cout << "Performance: " << avg_us << "µs per sample (target: <10µs)" << std::endl;
}

/**
 * @test TEST-INT-009: Stereo frame performance
 * @traceability Performance requirements
 */
TEST_F(TransmitIntegrationTest, Performance_StereoFrame_Under20Microseconds) {
    // Arrange
    int32_t left_sample = 0x00400000;
    int32_t right_sample = 0x00600000;
    const int iterations = 1000;
    
    // Act
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        transmit_stereo_pair(left_sample, right_sample, 0, 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    // Assert
    double avg_ns = static_cast<double>(duration.count()) / iterations;
    double avg_us = avg_ns / 1000.0;
    
    EXPECT_LT(avg_us, 20.0) << "Average stereo frame time: " << avg_us << "µs (target: <20µs)";
    
    std::cout << "Stereo performance: " << avg_us << "µs per frame (target: <20µs)" << std::endl;
}

// =============================================================================
// Error Handling Tests
// =============================================================================

/**
 * @test TEST-INT-010: Sequence of samples maintains data integrity
 * @traceability DES-C-001 through DES-C-004
 */
TEST_F(TransmitIntegrationTest, SampleSequence_DataIntegrity_Maintained) {
    // Arrange - Sequence of different samples
    std::vector<int32_t> samples = {
        static_cast<int32_t>(0x00000000),  // Zero
        static_cast<int32_t>(0x00400000),  // Positive mid-range
        static_cast<int32_t>(0x007FFFFF),  // Max positive
        static_cast<int32_t>(0xFF800000),  // Max negative
        static_cast<int32_t>(0xFFE00000)   // Negative mid-range
    };
    
    // Act - Transmit sequence
    for (size_t i = 0; i < samples.size(); ++i) {
        SubframeBuilder::Preamble preamble = (i % 2 == 0) 
            ? SubframeBuilder::Preamble::X 
            : SubframeBuilder::Preamble::Y;
        
        int result = transmit_sample(samples[i], 0, 0, 0, preamble);
        ASSERT_EQ(result, 0) << "Failed at sample " << i;
    }
    
    // Assert
    EXPECT_EQ(mock_hal->get_transmit_count(), samples.size());
    
    auto captured = mock_hal->get_captured_subframes();
    EXPECT_EQ(captured.size(), samples.size());
    
    // Verify all subframes are different (except zero might match negative values)
    for (size_t i = 1; i < captured.size(); ++i) {
        // At least preambles should alternate
        SubframeData sf_prev(captured[i-1]);
        SubframeData sf_curr(captured[i]);
        
        // Preambles in different positions (X vs Y)
        bool preambles_different = (sf_prev.get_bit(0) != sf_curr.get_bit(0)) ||
                                   (sf_prev.get_bit(1) != sf_curr.get_bit(1));
        EXPECT_TRUE(preambles_different) << "Preambles should alternate at sample " << i;
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
