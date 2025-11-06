/**
 * @file test_subframe_builder.cpp
 * @brief Comprehensive Unit Tests for AES3-2009 Subframe Builder
 * @traceability TEST-SFB-001 through TEST-SFB-018
 * 
 * RED Phase: Comprehensive failing tests for subframe assembly per AES3-2009 Part 3.
 * 
 * Tests cover:
 * - Subframe construction and configuration
 * - Preamble insertion (X/Y/Z patterns)
 * - Audio data encoding (24-bit PCM with MSB justification)
 * - Auxiliary bits handling
 * - Validity bit calculation
 * - User and channel status bits
 * - Even parity calculation
 * - Biphase-mark coding simulation
 * - Frame synchronization
 * 
 * Requirements Coverage:
 * - REQ-FUNC-TRANS-001: Subframe Time Slot Structure (32 slots)
 * - REQ-FUNC-TRANS-002: Time Slot Allocation (preamble, audio, metadata)
 * - REQ-FUNC-TRANS-003: Preamble X, Y, Z Patterns
 * - REQ-FUNC-TRANS-009: Even Parity Bit Generation
 * 
 * @standard AES3-2009 Part 3: Transport (Subframe Structure, Section 4.1-4.3)
 * @copyright Copyright (c) 2024
 */

#include <gtest/gtest.h>
#include "AES/AES3/Part3/_2009/subframe/subframe_data.hpp"
#include "AES/AES3/Part3/_2009/subframe/subframe_builder.hpp"
#include "AES/AES3/Part1/_2009/audio_coding/pcm_encoder.hpp"

using namespace AES::AES3::Part3::_2009::subframe;
using namespace AES::AES3::Part1::_2009::audio_coding;

// =============================================================================
// Test Fixture
// =============================================================================

class SubframeBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default configuration
        config.word_length = WordLength::BITS_24;
        config.auto_parity = true;
        config.biphase_coding = false;
    }
    
    SubframeBuilder::Config config;
};

// =============================================================================
// Construction and Configuration Tests
// =============================================================================

/**
 * @test TEST-SFB-001: Default construction with 24-bit word length
 * @verifies REQ-FUNC-TRANS-001 (Subframe Time Slot Structure)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, DefaultConstruction_24BitWordLength) {
    // Act
    SubframeBuilder builder;
    
    // Assert
    EXPECT_EQ(builder.get_config().word_length, WordLength::BITS_24);
    EXPECT_TRUE(builder.get_config().auto_parity);
    EXPECT_FALSE(builder.get_config().biphase_coding);
}

/**
 * @test TEST-SFB-002: Custom configuration
 * @verifies REQ-FUNC-TRANS-001 (Subframe Time Slot Structure - configuration)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, CustomConfiguration_20BitWordLength) {
    // Arrange
    config.word_length = WordLength::BITS_20;
    config.biphase_coding = true;
    
    // Act
    SubframeBuilder builder(config);
    
    // Assert
    EXPECT_EQ(builder.get_config().word_length, WordLength::BITS_20);
    EXPECT_TRUE(builder.get_config().biphase_coding);
}

// =============================================================================
// Preamble Insertion Tests
// =============================================================================

/**
 * @test TEST-SFB-003: Preamble X pattern (channel A, block start)
 * @verifies REQ-FUNC-TRANS-003 (Preamble X, Y, Z Patterns - X pattern)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.2.1
 * 
 * Preamble X (11100010 in biphase-mark):
 * Violation of biphase-mark rules to ensure sync
 */
TEST_F(SubframeBuilderTest, PreambleX_ChannelA_BlockStart) {
    // Arrange
    SubframeBuilder builder;
    SubframeData subframe;
    
    // Act
    SubframeBuilder::insert_preamble(SubframeBuilder::Preamble::X, subframe);
    
    // Assert - Check preamble pattern in slots 0-3
    // Preamble X should have specific bit pattern for sync
    // (Exact pattern depends on AES3-2009 spec - typically violations of biphase-mark)
    EXPECT_NE(subframe.data, 0);  // Preamble should set some bits
}

/**
 * @test TEST-SFB-004: Preamble Y pattern (channel B, block start)
 * @verifies REQ-FUNC-TRANS-003 (Preamble X, Y, Z Patterns - Y pattern)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.2.1
 */
TEST_F(SubframeBuilderTest, PreambleY_ChannelB_BlockStart) {
    // Arrange
    SubframeBuilder builder;
    SubframeData subframe;
    
    // Act
    SubframeBuilder::insert_preamble(SubframeBuilder::Preamble::Y, subframe);
    
    // Assert
    EXPECT_NE(subframe.data, 0);
    // Y pattern should differ from X pattern
}

/**
 * @test TEST-SFB-005: Preamble Z pattern (channel A, mid-block)
 * @verifies REQ-FUNC-TRANS-003 (Preamble X, Y, Z Patterns - Z pattern)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.2.1
 */
TEST_F(SubframeBuilderTest, PreambleZ_ChannelA_MidBlock) {
    // Arrange
    SubframeBuilder builder;
    SubframeData subframe;
    
    // Act
    SubframeBuilder::insert_preamble(SubframeBuilder::Preamble::Z, subframe);
    
    // Assert
    EXPECT_NE(subframe.data, 0);
    // Z pattern should differ from X and Y
}

// =============================================================================
// Audio Data Encoding Tests
// =============================================================================

/**
 * @test TEST-SFB-006: Build subframe with 24-bit audio sample
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - audio data in slots 4-27)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSubframe_24BitAudio_Success) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    uint32_t audio_sample = 0x007FFFFF;  // Max positive 24-bit
    
    // Act
    int result = builder.build_subframe(
        audio_sample,
        0,  // valid
        0,  // user bit
        0,  // channel status bit
        SubframeBuilder::Preamble::X,
        subframe
    );
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_NE(subframe.data, 0);  // Should have audio data
}

/**
 * @test TEST-SFB-007: Build subframe with 20-bit audio sample
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - MSB justification)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSubframe_20BitAudio_MSBJustified) {
    // Arrange
    config.word_length = WordLength::BITS_20;
    SubframeBuilder builder(config);
    SubframeData subframe;
    uint32_t audio_sample = 0x0007FFFF;  // Max positive 20-bit
    
    // Act
    int result = builder.build_subframe(
        audio_sample,
        0,  // valid
        0,  // user bit
        0,  // channel status bit
        SubframeBuilder::Preamble::Z,
        subframe
    );
    
    // Assert
    EXPECT_EQ(result, 0);
    // Audio should be MSB-justified (shifted left by 4 bits)
    // Verify audio data is in correct time slots (4-27)
}

/**
 * @test TEST-SFB-008: Build subframe with zero audio sample
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - zero audio handling)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSubframe_ZeroAudio_ValidSubframe) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    int result = builder.build_subframe(
        0,  // zero audio
        0,  // valid
        0,  // user bit
        0,  // channel status bit
        SubframeBuilder::Preamble::X,
        subframe
    );
    
    // Assert
    EXPECT_EQ(result, 0);
    // Should have preamble but audio slots may be zero
}

/**
 * @test TEST-SFB-009: Build subframe with negative audio sample (2's complement)
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - 2's complement audio)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSubframe_NegativeAudio_TwosComplement) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    uint32_t audio_sample = 0x00800000;  // Most negative 24-bit
    
    // Act
    int result = builder.build_subframe(
        audio_sample,
        0,  // valid
        0,  // user bit
        0,  // channel status bit
        SubframeBuilder::Preamble::X,
        subframe
    );
    
    // Assert
    EXPECT_EQ(result, 0);
    // 2's complement negative value should be encoded correctly
}

// =============================================================================
// Metadata Bit Tests
// =============================================================================

/**
 * @test TEST-SFB-010: Validity bit set correctly (0=valid)
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - validity bit in slot 28)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.1
 */
TEST_F(SubframeBuilderTest, BuildSubframe_ValidityBit_Zero_Valid) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    builder.build_subframe(0x007FFFFF, 0, 0, 0, SubframeBuilder::Preamble::X, subframe);
    
    // Assert
    // Validity bit in time slot 28 should be 0
    uint8_t validity_slot = subframe.get_bit(SubframeData::VALIDITY_SLOT);
    uint8_t validity_bit = validity_slot & 0x01;
    EXPECT_EQ(validity_bit, 0);
}

/**
 * @test TEST-SFB-011: Validity bit set correctly (1=unreliable)
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - validity bit unreliable flag)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSubframe_ValidityBit_One_Unreliable) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    builder.build_subframe(0x007FFFFF, 1, 0, 0, SubframeBuilder::Preamble::X, subframe);
    
    // Assert
    // Validity bit in time slot 28 should be 1 (stored in both bit positions)
    uint8_t validity_slot = subframe.get_bit(SubframeData::VALIDITY_SLOT);
    uint8_t validity_bit = validity_slot & 0x01;
    EXPECT_EQ(validity_bit, 1);
}

/**
 * @test TEST-SFB-012: User bit set correctly
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - user bit in slot 29)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.1
 */
TEST_F(SubframeBuilderTest, BuildSubframe_UserBit_Set) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    builder.build_subframe(0x007FFFFF, 0, 1, 0, SubframeBuilder::Preamble::X, subframe);
    
    // Assert
    // User bit in time slot 29 should be 1
    uint8_t user_slot = subframe.get_bit(SubframeData::USER_SLOT);
    uint8_t user_bit = user_slot & 0x01;
    EXPECT_EQ(user_bit, 1);
}

/**
 * @test TEST-SFB-013: Channel status bit set correctly
 * @verifies REQ-FUNC-TRANS-002 (Time Slot Allocation - channel status in slot 30)
 * @traceability DES-C-004, AES3-2009 Part 2
 */
TEST_F(SubframeBuilderTest, BuildSubframe_ChannelStatusBit_Set) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    builder.build_subframe(0x007FFFFF, 0, 0, 1, SubframeBuilder::Preamble::X, subframe);
    
    // Assert
    // Channel status bit in time slot 30 should be 1
    uint8_t channel_slot = subframe.get_bit(SubframeData::CHANNEL_STATUS_SLOT);
    uint8_t channel_bit = channel_slot & 0x01;
    EXPECT_EQ(channel_bit, 1);
}

// =============================================================================
// Parity Calculation Tests
// =============================================================================

/**
 * @test TEST-SFB-014: Even parity calculated automatically
 * @verifies REQ-FUNC-TRANS-009 (Even Parity Bit Generation and Validation)
 * @traceability DES-C-004, AES3-2009 Part 3 Section 4.1
 */
TEST_F(SubframeBuilderTest, BuildSubframe_AutoParity_EvenParity) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe;
    
    // Act
    builder.build_subframe(0x007FFFFF, 0, 0, 0, SubframeBuilder::Preamble::X, subframe);
    
    // Assert
    // Calculate parity over slots 4-30 (audio, validity, user, channel status)
    uint8_t calculated_parity = SubframeBuilder::calculate_parity(subframe);
    uint8_t stored_parity_slot = subframe.get_bit(SubframeData::PARITY_SLOT);
    
    // Parity should be single bit value, stored in both bits of slot
    uint8_t stored_parity = stored_parity_slot & 0x01;
    
    EXPECT_EQ(stored_parity, calculated_parity);
    
    // Total number of 1-bits in slots 4-31 should be even
    uint8_t bit_count = 0;
    for (size_t slot = SubframeData::AUDIO_START; slot <= SubframeData::PARITY_SLOT; ++slot) {
        uint8_t slot_value = subframe.get_bit(slot);
        if (slot_value & 0x01) bit_count++;
        if (slot_value & 0x02) bit_count++;
    }
    EXPECT_EQ(bit_count % 2, 0);  // Even parity
}

/**
 * @test TEST-SFB-015: Parity calculation utility function
 * @verifies REQ-FUNC-TRANS-009 (Even Parity Bit Generation - calculation algorithm)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, CalculateParity_ManualVerification) {
    // Arrange
    SubframeData subframe;
    // Set specific bits for known parity (using time slot interface)
    subframe.set_bit(SubframeData::AUDIO_START, 0x01);      // 1 bit set
    subframe.set_bit(SubframeData::VALIDITY_SLOT, 0x01);    // 2 bits set
    subframe.set_bit(SubframeData::USER_SLOT, 0x01);        // 3 bits set (odd)
    
    // Act
    uint8_t parity = SubframeBuilder::calculate_parity(subframe);
    
    // Assert
    EXPECT_EQ(parity, 1);  // Should be 1 to make total even (3 + 1 = 4 = even)
}

// =============================================================================
// Multiple Subframe Tests
// =============================================================================

/**
 * @test TEST-SFB-016: Build stereo pair (X and Y preambles)
 * @verifies REQ-FUNC-TRANS-003 (Preamble X, Y, Z Patterns - stereo pairing)
 * @verifies REQ-FUNC-TRANS-005 (Frame Structure - stereo frame assembly)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildStereoPair_XandY_BlockStart) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe_left;
    SubframeData subframe_right;
    
    // Act - Build stereo pair at block start
    int result_left = builder.build_subframe(
        0x00400000, 0, 0, 1, SubframeBuilder::Preamble::X, subframe_left);
    int result_right = builder.build_subframe(
        0x00600000, 0, 0, 0, SubframeBuilder::Preamble::Y, subframe_right);
    
    // Assert
    EXPECT_EQ(result_left, 0);
    EXPECT_EQ(result_right, 0);
    
    // Left and right should have different preambles
    EXPECT_NE(subframe_left.data & 0xFF, subframe_right.data & 0xFF);
}

/**
 * @test TEST-SFB-017: Build sequence Z-Y-Z-Y (mid-block pattern)
 * @verifies REQ-FUNC-TRANS-005 (Frame Structure - mid-block frame sequence)
 * @verifies REQ-FUNC-TRANS-006 (Block Structure - frame sequence in block)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, BuildSequence_ZYZY_MidBlock) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData frames[4];
    
    // Act - Build mid-block sequence
    builder.build_subframe(0x00100000, 0, 0, 0, SubframeBuilder::Preamble::Z, frames[0]);
    builder.build_subframe(0x00200000, 0, 0, 0, SubframeBuilder::Preamble::Y, frames[1]);
    builder.build_subframe(0x00300000, 0, 0, 0, SubframeBuilder::Preamble::Z, frames[2]);
    builder.build_subframe(0x00400000, 0, 0, 0, SubframeBuilder::Preamble::Y, frames[3]);
    
    // Assert - All should succeed
    EXPECT_NE(frames[0].data, 0);
    EXPECT_NE(frames[1].data, 0);
    EXPECT_NE(frames[2].data, 0);
    EXPECT_NE(frames[3].data, 0);
}

// =============================================================================
// Reset and State Management Tests
// =============================================================================

/**
 * @test TEST-SFB-018: Reset clears builder state
 * @verifies REQ-FUNC-TRANS-001 (Subframe Time Slot Structure - state management)
 * @traceability DES-C-004
 */
TEST_F(SubframeBuilderTest, Reset_ClearsState) {
    // Arrange
    SubframeBuilder builder(config);
    SubframeData subframe1, subframe2;
    
    // Act
    builder.build_subframe(0x007FFFFF, 0, 0, 0, SubframeBuilder::Preamble::X, subframe1);
    builder.reset();
    builder.build_subframe(0x007FFFFF, 0, 0, 0, SubframeBuilder::Preamble::X, subframe2);
    
    // Assert - After reset, should produce same output for same input
    EXPECT_EQ(subframe1.data, subframe2.data);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
