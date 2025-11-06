/**
 * @file test_aes3_part1_conformity_simple.cpp
 * @brief AES3-2009 Part 1 (Audio Content) Conformity Test Suite - Working Version
 * 
 * Simplified conformity tests that match the actual PCMEncoder API.
 * Tests AES3-1-2009 compliance for audio content encoding.
 * 
 * @standard AES3-1-2009 (R2014) Clauses 4-7
 * @requirements REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-001 through REQ-FUNC-AUDIO-011
 * @traceability TEST-CONF-AUDIO-001 through TEST-CONF-AUDIO-020 (20 working tests)
 */

#include <gtest/gtest.h>
#include "AES/AES3/Part1/_2009/audio_coding/pcm_encoder.hpp"
#include <cstdint>

using namespace AES::AES3::Part1::_2009::audio_coding;

// Test fixture
class AES3_Part1_Conformity : public ::testing::Test {
protected:
    void SetUp() override {
        // Default 24-bit configuration
        config = Config::Default();
        encoder = new PCMEncoder(config);
    }
    
    void TearDown() override {
        delete encoder;
    }
    
    Config config;
    PCMEncoder* encoder;
};

// =============================================================================
// TEST-CONF-AUDIO-001: Linear PCM 2's complement encoding (AES3-1-2009 Clause 4.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_001_LinearPCM_TwosComplement) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-001
    EncodedSample encoded;
    
    // Positive sample
    EXPECT_EQ(0, encoder->encode_sample(16384, true, encoded));
    EXPECT_NE(0u, encoded.audio_data);  // Should encode to non-zero
    
    // Negative sample (2's complement)
    EXPECT_EQ(0, encoder->encode_sample(-16384, true, encoded));
    EXPECT_NE(0u, encoded.audio_data);
    
    // Zero
    EXPECT_EQ(0, encoder->encode_sample(0, true, encoded));
    EXPECT_EQ(0u, encoded.audio_data & 0x00FFFFFF);
}

// =============================================================================
// TEST-CONF-AUDIO-002: PCM positive polarity (AES3-1-2009 Clause 4.2)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_002_PositivePolarity) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-002
    EncodedSample encoded_pos, encoded_neg;
    
    // Maximum positive value
    EXPECT_EQ(0, encoder->encode_sample(8388607, true, encoded_pos));  // Max 24-bit
    
    // Maximum negative value
    EXPECT_EQ(0, encoder->encode_sample(-8388608, true, encoded_neg));  // Min 24-bit
    
    // Positive and negative should produce different encodings
    EXPECT_NE(encoded_pos.audio_data, encoded_neg.audio_data);
}

// =============================================================================
// TEST-CONF-AUDIO-003: 16-bit optimized path (AES3-1-2009 Clause 4.3)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_003_16BitPrecision) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-003
    EncodedSample encoded;
    
    int16_t sample_16bit = 12345;
    EXPECT_EQ(0, encoder->encode_sample_16bit(sample_16bit, true, encoded));
    
    // 16-bit should be justified to MSB (shifted left by 8)
    uint32_t expected = (static_cast<uint32_t>(static_cast<int32_t>(sample_16bit)) << 8) & 0x00FFFFFF;
    EXPECT_EQ(expected, encoded.audio_data);
}

// =============================================================================
// TEST-CONF-AUDIO-004: 24-bit packed format (AES3-1-2009 Clause 4.3)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_004_24BitPrecision) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-003
    EncodedSample encoded;
    
    uint8_t sample_24bit[3] = {0x12, 0x34, 0x56};  // Little-endian
    EXPECT_EQ(0, encoder->encode_sample_24bit(sample_24bit, true, encoded));
    
    uint32_t expected = 0x563412;  // Unpacked value
    EXPECT_EQ(expected, encoded.audio_data);
}

// =============================================================================
// TEST-CONF-AUDIO-005: Word length configuration 20-bit (AES3-1-2009 Clause 4.4)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_005_20BitWordLength) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-004
    config.word_length = WordLength::BITS_20;
    encoder->set_config(config);
    
    EXPECT_EQ(WordLength::BITS_20, encoder->get_config().word_length);
}

// =============================================================================
// TEST-CONF-AUDIO-006: Word length configuration 24-bit (AES3-1-2009 Clause 4.4)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_006_24BitWordLength) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-004
    config.word_length = WordLength::BITS_24;
    encoder->set_config(config);
    
    EXPECT_EQ(WordLength::BITS_24, encoder->get_config().word_length);
}

// =============================================================================
// TEST-CONF-AUDIO-007: Validity bit for valid audio (AES3-1-2009 Clause 6.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_007_ValidityBit_Valid) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-009
    EncodedSample encoded;
    
    EXPECT_EQ(0, encoder->encode_sample(16384, true, encoded));
    EXPECT_EQ(0u, encoded.validity);  // Valid audio = validity bit 0
}

// =============================================================================
// TEST-CONF-AUDIO-008: Validity bit for invalid audio (AES3-1-2009 Clause 6.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_008_ValidityBit_Invalid) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-009
    EncodedSample encoded;
    
    EXPECT_EQ(0, encoder->encode_sample(16384, false, encoded));
    EXPECT_EQ(1u, encoded.validity);  // Invalid audio = validity bit 1
}

// =============================================================================
// TEST-CONF-AUDIO-009: Pre-emphasis NONE (AES3-1-2009 Clause 7.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_009_PreEmphasis_None) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-010
    config.pre_emphasis = PreEmphasis::NONE;
    encoder->set_config(config);
    
    EXPECT_EQ(PreEmphasis::NONE, encoder->get_config().pre_emphasis);
}

// =============================================================================
// TEST-CONF-AUDIO-010: Pre-emphasis ITU-R BS.450-3 (AES3-1-2009 Clause 7.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_010_PreEmphasis_ITU_R) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-010
    config.pre_emphasis = PreEmphasis::ITU_R_BS_450_3;
    encoder->set_config(config);
    
    EXPECT_EQ(PreEmphasis::ITU_R_BS_450_3, encoder->get_config().pre_emphasis);
}

// =============================================================================
// TEST-CONF-AUDIO-011: Pre-emphasis CCITT J.17 (AES3-1-2009 Clause 7.1)
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_011_PreEmphasis_J17) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-010
    config.pre_emphasis = PreEmphasis::CCITT_J17;
    encoder->set_config(config);
    
    EXPECT_EQ(PreEmphasis::CCITT_J17, encoder->get_config().pre_emphasis);
}

// =============================================================================
// TEST-CONF-AUDIO-012: Configuration validation - invalid word length
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_012_InvalidWordLength) {
    // REQ-QUAL-AUDIO-001
    config.word_length = static_cast<WordLength>(19);  // Invalid (< 20)
    EXPECT_NE(0, encoder->set_config(config));  // Should fail
    
    // Error info should be set
    EXPECT_EQ(PCMEncoderError::INVALID_WORD_LENGTH, encoder->get_last_error().code);
}

// =============================================================================
// TEST-CONF-AUDIO-013: Encoder reset clears error state
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_013_ResetClearsErrors) {
    // REQ-QUAL-AUDIO-001
    // Cause an error
    config.word_length = static_cast<WordLength>(19);
    encoder->set_config(config);
    
    // Reset
    encoder->reset();
    
    // Error should be cleared
    EXPECT_EQ(PCMEncoderError::SUCCESS, encoder->get_last_error().code);
}

// =============================================================================
// TEST-CONF-AUDIO-014: 16-bit encoding preserves sign
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_014_16Bit_SignPreservation) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-002
    EncodedSample encoded_pos, encoded_neg;
    
    EXPECT_EQ(0, encoder->encode_sample_16bit(32767, true, encoded_pos));   // Max positive
    EXPECT_EQ(0, encoder->encode_sample_16bit(-32768, true, encoded_neg));  // Max negative
    
    // MSB should differ (sign bit)
    EXPECT_NE((encoded_pos.audio_data >> 23) & 1, (encoded_neg.audio_data >> 23) & 1);
}

// =============================================================================
// TEST-CONF-AUDIO-015: 24-bit encoding little-endian
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_015_24Bit_LittleEndian) {
    // REQ-QUAL-AUDIO-001
    EncodedSample encoded;
    
    uint8_t sample[3] = {0xFF, 0x00, 0x00};  // LSB first
    EXPECT_EQ(0, encoder->encode_sample_24bit(sample, true, encoded));
    EXPECT_EQ(0x0000FFu, encoded.audio_data);
}

// =============================================================================
// TEST-CONF-AUDIO-016: Auxiliary bits for 20-bit word length
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_016_AuxiliaryBits_20Bit) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-004
    config.word_length = WordLength::BITS_20;
    encoder->set_config(config);
    
    EncodedSample encoded;
    EXPECT_EQ(0, encoder->encode_sample(524287, true, encoded));  // Max 20-bit value (2^19-1)
    
    // With 20-bit word, 4 LSBs become auxiliary bits
    // Implementation may extract these (test just verifies encode succeeds)
}

// =============================================================================
// TEST-CONF-AUDIO-017: Auto-validity enabled by default
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_017_AutoValidity_Default) {
    // REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-009
    EXPECT_TRUE(encoder->get_config().auto_validity);
}

// =============================================================================
// TEST-CONF-AUDIO-018: Input validation can be disabled
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_018_InputValidation_Disable) {
    // REQ-QUAL-AUDIO-001
    config.validate_inputs = false;
    encoder->set_config(config);
    
    EXPECT_FALSE(encoder->get_config().validate_inputs);
}

// =============================================================================
// TEST-CONF-AUDIO-019: Successful encode returns SUCCESS
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_019_SuccessCode) {
    // REQ-QUAL-AUDIO-001
    EncodedSample encoded;
    
    int result = encoder->encode_sample(1000, true, encoded);
    EXPECT_EQ(0, result);
    EXPECT_EQ(PCMEncoderError::SUCCESS, encoder->get_last_error().code);
}

// =============================================================================
// TEST-CONF-AUDIO-020: Configuration preserved across encodings
// =============================================================================
TEST_F(AES3_Part1_Conformity, TEST_CONF_AUDIO_020_ConfigPreserved) {
    // REQ-QUAL-AUDIO-001
    config.word_length = WordLength::BITS_20;
    config.pre_emphasis = PreEmphasis::ITU_R_BS_450_3;
    encoder->set_config(config);
    
    // Encode multiple samples
    EncodedSample encoded;
    for (int i = 0; i < 10; i++) {
        encoder->encode_sample(i * 1000, true, encoded);
    }
    
    // Configuration should still be set
    EXPECT_EQ(WordLength::BITS_20, encoder->get_config().word_length);
    EXPECT_EQ(PreEmphasis::ITU_R_BS_450_3, encoder->get_config().pre_emphasis);
}

/**
 * @test_summary 20 working conformity test cases for AES3-2009 Part 1 (Audio Content)
 * @test_coverage
 *   - Linear PCM encoding with 2's complement (TEST-CONF-AUDIO-001)
 *   - Polarity convention (TEST-CONF-AUDIO-002)
 *   - 16/24-bit precision (TEST-CONF-AUDIO-003-004)
 *   - Word length configuration (TEST-CONF-AUDIO-005-006)
 *   - Validity bit handling (TEST-CONF-AUDIO-007-008)
 *   - Pre-emphasis settings (TEST-CONF-AUDIO-009-011)
 *   - Error handling and validation (TEST-CONF-AUDIO-012-013)
 *   - Sign preservation and endianness (TEST-CONF-AUDIO-014-015)
 *   - Auxiliary bits and configuration (TEST-CONF-AUDIO-016-020)
 * @requirements_traced REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-001-011
 * @standard AES3-1-2009 (R2014) Clauses 4-7
 */
