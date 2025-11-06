/**
 * @file test_pcm_encoder.cpp
 * @brief Unit tests for PCM Encoder component (TDD Red Phase)
 * @implements TEST-PCM-001 through TEST-PCM-015
 * @traceability DES-C-003
 * 
 * RED Phase: Write failing tests for PCM encoding with 2's complement conversion,
 * MSB justification for word lengths 20-24 bits, validity bit calculation,
 * and auxiliary bits extraction (AES3-2009 Part 1, Sections 4-6)
 * 
 * Requirements Coverage:
 * - REQ-FUNC-AUDIO-001: Linear PCM Encoding (2's complement)
 * - REQ-FUNC-AUDIO-002: PCM Polarity Convention
 * - REQ-FUNC-AUDIO-003: Coding Precision Options (16-24 bits)
 * - REQ-FUNC-AUDIO-004: MSB Justification for Intermediate Precision
 * - REQ-FUNC-AUDIO-009: Validity Bit Implementation
 * 
 * @standard AES3-2009 Part 1: Audio Content
 * @copyright Copyright (c) 2025
 */

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>

// Include GREEN phase implementation
#include "../../../../../../src/AES/AES3/Part1/_2009/audio_coding/pcm_encoder.hpp"

using namespace AES::AES3::Part1::_2009::audio_coding;

// =============================================================================
// Test Fixture
// =============================================================================

class PCMEncoderTest : public ::testing::Test {
protected:
    PCMEncoder* encoder;
    
    void SetUp() override {
        // Create encoder with default configuration
        encoder = new PCMEncoder();
    }
    
    void TearDown() override {
        delete encoder;
        encoder = nullptr;
    }
    
    // Helper: Verify MSB justification
    void VerifyMSBJustification(uint32_t audio_data, WordLength word_length, int32_t original_sample) {
        // Extract expected bit positions for MSB-justified data
        uint8_t shift = 24 - static_cast<uint8_t>(word_length);
        uint32_t expected_mask = (0x00FFFFFFU >> shift) << shift;
        
        (void)original_sample;  // Reserved for future validation
        
        EXPECT_EQ(audio_data & ~expected_mask, 0U) 
            << "LSBs should be zero for " << static_cast<int>(word_length) << "-bit audio";
    }
};

// =============================================================================
// RED PHASE: Construction and Configuration Tests
// =============================================================================

/**
 * @test TEST-PCM-001: Default construction with 24-bit word length
 * @traceability DES-C-003
 * @verifies REQ-FUNC-AUDIO-003 (Coding Precision Options - 24-bit default)
 */
TEST_F(PCMEncoderTest, DefaultConstruction_24BitWordLength) {
    // Assert
    EXPECT_EQ(encoder->get_config().word_length, WordLength::BITS_24);
    EXPECT_EQ(encoder->get_config().pre_emphasis, PreEmphasis::NONE);
    EXPECT_TRUE(encoder->get_config().auto_validity);
    EXPECT_TRUE(encoder->get_config().validate_inputs);
}

/**
 * @test TEST-PCM-002: Construction with custom configuration
 * @traceability DES-C-003
 * @verifies REQ-FUNC-AUDIO-003 (Coding Precision Options - 20-bit)
 */
TEST_F(PCMEncoderTest, CustomConstruction_20BitWordLength) {
    // Arrange
    Config custom_config{WordLength::BITS_20, PreEmphasis::NONE, true, true};
    
    // Act
    PCMEncoder custom_encoder(custom_config);
    
    // Assert
    EXPECT_EQ(custom_encoder.get_config().word_length, WordLength::BITS_20);
}

/**
 * @test TEST-PCM-003: Runtime configuration change
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, SetConfig_ValidConfiguration_Success) {
    // Arrange
    Config new_config{WordLength::BITS_22, PreEmphasis::CCITT_J17, false, true};
    
    // Act
    int result = encoder->set_config(new_config);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder->get_config().word_length, WordLength::BITS_22);
    EXPECT_EQ(encoder->get_config().pre_emphasis, PreEmphasis::CCITT_J17);
}

// =============================================================================
// RED PHASE: 2's Complement and MSB Justification Tests
// =============================================================================

/**
 * @test TEST-PCM-004: Encode 24-bit sample (no justification needed)
 * @traceability DES-C-003, REQ-FUNC-001, REQ-FUNC-002
 */
TEST_F(PCMEncoderTest, Encode24BitSample_NoJustification_CorrectOutput) {
    // Arrange
    int32_t pcm_sample = 0x007FFFFF;  // Maximum positive 24-bit value
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.audio_data, 0x007FFFFFU);  // No shift needed
    EXPECT_EQ(encoded_out.validity, 0);  // Valid sample
    EXPECT_EQ(encoded_out.auxiliary_bits, 0);  // No auxiliary bits for 24-bit
}

/**
 * @test TEST-PCM-005: Encode 20-bit sample with MSB justification
 * @traceability DES-C-003, REQ-FUNC-004
 */
TEST_F(PCMEncoderTest, Encode20BitSample_MSBJustification_ShiftsLeft4Bits) {
    // Arrange
    Config config_20bit{WordLength::BITS_20, PreEmphasis::NONE, true, true};
    PCMEncoder encoder_20bit(config_20bit);
    int32_t pcm_sample = 0x0007FFFF;  // Maximum positive 20-bit value
    EncodedSample encoded_out;
    
    // Act
    int result = encoder_20bit.encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    // 20-bit sample shifted left by 4 bits: 0x0007FFFF << 4 = 0x007FFFF0
    EXPECT_EQ(encoded_out.audio_data, 0x007FFFF0U);
    EXPECT_EQ(encoded_out.validity, 0);
    // 4 auxiliary bits available (24 - 20 = 4)
}

/**
 * @test TEST-PCM-006: Encode negative 24-bit sample (2's complement)
 * @traceability DES-C-003, REQ-FUNC-002
 */
TEST_F(PCMEncoderTest, EncodeNegative24BitSample_TwosComplement_CorrectOutput) {
    // Arrange
    int32_t pcm_sample = -8388608;  // Minimum 24-bit value (0xFF800000 in 2's complement)
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.audio_data, 0x00800000U);  // MSB set for negative value
    EXPECT_EQ(encoded_out.validity, 0);
}

/**
 * @test TEST-PCM-007: Encode zero sample
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, EncodeZeroSample_OutputZero) {
    // Arrange
    int32_t pcm_sample = 0;
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.audio_data, 0x00000000U);
    EXPECT_EQ(encoded_out.validity, 0);
}

/**
 * @test TEST-PCM-008: Encode 22-bit sample with MSB justification
 * @traceability DES-C-003, REQ-FUNC-003, REQ-FUNC-004
 */
TEST_F(PCMEncoderTest, Encode22BitSample_MSBJustification_ShiftsLeft2Bits) {
    // Arrange
    Config config_22bit{WordLength::BITS_22, PreEmphasis::NONE, true, true};
    PCMEncoder encoder_22bit(config_22bit);
    int32_t pcm_sample = 0x001FFFFF;  // Maximum positive 22-bit value
    EncodedSample encoded_out;
    
    // Act
    int result = encoder_22bit.encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    // 22-bit sample shifted left by 2 bits: 0x001FFFFF << 2 = 0x007FFFFC
    EXPECT_EQ(encoded_out.audio_data, 0x007FFFFCU);
}

// =============================================================================
// RED PHASE: 16-bit Optimized Path Tests
// =============================================================================

/**
 * @test TEST-PCM-009: Encode 16-bit sample (optimized path)
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, Encode16BitSample_OptimizedPath_CorrectJustification) {
    // Arrange
    int16_t pcm_sample_16 = 0x7FFF;  // Maximum positive 16-bit value
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample_16bit(pcm_sample_16, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    // 16-bit sample shifted left by 8 bits: 0x7FFF << 8 = 0x007FFF00
    EXPECT_EQ(encoded_out.audio_data, 0x007FFF00U);
    EXPECT_EQ(encoded_out.validity, 0);
}

/**
 * @test TEST-PCM-010: Encode negative 16-bit sample
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, EncodeNegative16BitSample_TwosComplement_CorrectOutput) {
    // Arrange
    int16_t pcm_sample_16 = -32768;  // Minimum 16-bit value (0x8000)
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample_16bit(pcm_sample_16, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    // 0x8000 << 8 = 0x00800000 (MSB set for negative)
    EXPECT_EQ(encoded_out.audio_data, 0x00800000U);
}

// =============================================================================
// RED PHASE: Validity Bit Tests
// =============================================================================

/**
 * @test TEST-PCM-011: Valid sample produces validity bit = 0
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, EncodeValidSample_ValidityBitZero) {
    // Arrange
    int32_t pcm_sample = 0x00400000;
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.validity, 0);
}

/**
 * @test TEST-PCM-012: Invalid sample produces validity bit = 1
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, EncodeInvalidSample_ValidityBitOne) {
    // Arrange
    int32_t pcm_sample = 0x00400000;
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, false, encoded_out);  // sample_valid = false
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.validity, 1);
}

// =============================================================================
// RED PHASE: Auxiliary Bits Tests
// =============================================================================

/**
 * @test TEST-PCM-013: 20-bit word length provides 4 auxiliary bits
 * @traceability DES-C-003, REQ-FUNC-003
 */
TEST_F(PCMEncoderTest, Encode20BitSample_4AuxiliaryBits) {
    // Arrange
    Config config_20bit{WordLength::BITS_20, PreEmphasis::NONE, true, true};
    PCMEncoder encoder_20bit(config_20bit);
    // Valid 20-bit sample: max is (2^19 - 1) = 524287 = 0x0007FFFF
    // Use a value where lower 4 bits have a pattern
    int32_t pcm_sample = 0x0007FFFF;  // All bits set in 20-bit range
    EncodedSample encoded_out;
    
    // Act
    int result = encoder_20bit.encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    // 4 auxiliary bits available (24 - 20 = 4)
    // Auxiliary bits come from LSBs of original sample before justification
    // Lower 4 bits of 0x0007FFFF are 0xF
    EXPECT_EQ(encoded_out.auxiliary_bits, 0x0F);
    // Audio data should be MSB-justified (shifted left by 4 bits)
    // 0x0007FFFF << 4 = 0x007FFFF0
    EXPECT_EQ(encoded_out.audio_data, 0x007FFFF0U);
}

/**
 * @test TEST-PCM-014: 24-bit word length has 0 auxiliary bits
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, Encode24BitSample_0AuxiliaryBits) {
    // Arrange
    int32_t pcm_sample = 0x007FFFFF;
    EncodedSample encoded_out;
    
    // Act
    int result = encoder->encode_sample(pcm_sample, true, encoded_out);
    
    // Assert
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoded_out.auxiliary_bits, 0);
}

// =============================================================================
// RED PHASE: Performance and Edge Cases
// =============================================================================

/**
 * @test TEST-PCM-015: Multiple encodings maintain consistency
 * @traceability DES-C-003
 */
TEST_F(PCMEncoderTest, MultipleEncodings_MaintainConsistency) {
    // Arrange
    int32_t samples[] = {0x007FFFFF, 0x00000000, -8388608, 0x00400000};
    EncodedSample encoded_out;
    
    // Act & Assert
    for (int32_t sample : samples) {
        int result = encoder->encode_sample(sample, true, encoded_out);
        EXPECT_EQ(result, 0);
        EXPECT_LE(encoded_out.audio_data, 0x00FFFFFFU) 
            << "Audio data must be 24-bit max";
    }
}

// =============================================================================
// End of RED Phase Tests
// =============================================================================
// All tests above SHOULD FAIL until GREEN phase implementation is complete
// =============================================================================
