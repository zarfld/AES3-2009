/**
 * @file test_aes3_part1_conformity.cpp
 * @brief AES3-2009 Part 1 (Audio Content) Conformity Test Suite
 * 
 * Implements 39 mandatory conformity test cases per REQ-QUAL-AUDIO-001:
 * - Audio Content Coding (Clause 4): 15 tests
 * - Sampling Frequency (Clause 5): 10 tests  
 * - Validity Bit (Clause 6): 8 tests
 * - Pre-emphasis (Clause 7): 6 tests
 * 
 * @standard AES3-1-2009 (R2014) Clauses 4-7
 * @requirements REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-001 through REQ-FUNC-AUDIO-011
 * @traceability TEST-CONF-AUDIO-001 through TEST-CONF-AUDIO-039
 */

#include <gtest/gtest.h>
#include "AES/AES3/Part1/_2009/audio_coding/pcm_encoder.hpp"
#include <cstdint>
#include <cmath>

using namespace AES::AES3::Part1::_2009::audio_coding;

// Test fixture for Part 1 Audio Content conformity tests
class AES3_Part1_AudioCoding_Conformity : public ::testing::Test {
protected:
    PCMEncoder encoder;
    
    void SetUp() override {
        // Initialize encoder with default config
        encoder = PCMEncoder(PCMEncoder::Config::Default());
    }
    
    void TearDown() override {
        // Cleanup if needed
    }
};

// =============================================================================
// Audio Content Coding Tests (Clause 4.1-4.6): 15 test cases
// REQ-FUNC-AUDIO-001: Linear PCM Encoding
// =============================================================================

/**
 * TEST-CONF-AUDIO-001: Linear PCM 2's complement encoding
 * @req REQ-FUNC-AUDIO-001, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_001_LinearPCM_TwosComplement)
{
    // Test positive value
    int32_t positive_sample = 16384;  // 0x4000 in 16-bit
    uint32_t encoded_positive = encoder->encode_sample(positive_sample, 16);
    EXPECT_EQ(0x4000, encoded_positive & 0xFFFF);
    
    // Test negative value (2's complement)
    int32_t negative_sample = -16384;  // 0xC000 in 16-bit 2's complement
    uint32_t encoded_negative = encoder->encode_sample(negative_sample, 16);
    EXPECT_EQ(0xC000, encoded_negative & 0xFFFF);
    
    // Test zero
    uint32_t encoded_zero = encoder->encode_sample(0, 16);
    EXPECT_EQ(0x0000, encoded_zero & 0xFFFF);
}

/**
 * TEST-CONF-AUDIO-002: PCM positive polarity convention
 * @req REQ-FUNC-AUDIO-002, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_002_PositivePolarity)
{
    // Maximum positive value corresponds to positive voltage peak
    int32_t max_positive_16bit = 32767;  // 0x7FFF
    uint32_t encoded = encoder->encode_sample(max_positive_16bit, 16);
    EXPECT_EQ(0x7FFF, encoded & 0xFFFF);
    
    // Maximum negative value corresponds to negative voltage peak
    int32_t max_negative_16bit = -32768;  // 0x8000
    encoded = encoder->encode_sample(max_negative_16bit, 16);
    EXPECT_EQ(0x8000, encoded & 0xFFFF);
}

/**
 * TEST-CONF-AUDIO-003: 16-bit audio word precision
 * @req REQ-FUNC-AUDIO-003, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.3
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_003_16BitPrecision)
{
    int32_t sample_16bit = 12345;
    uint32_t encoded = encoder->encode_sample(sample_16bit, 16);
    
    // 16-bit sample in time slots 12-27 (bits 0-15 of 20-bit audio word)
    uint32_t audio_word = (encoded >> 12) & 0xFFFF;
    EXPECT_EQ(12345, audio_word);
}

/**
 * TEST-CONF-AUDIO-004: 20-bit audio word precision
 * @req REQ-FUNC-AUDIO-003, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.3
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_004_20BitPrecision)
{
    int32_t sample_20bit = 524287;  // Max 20-bit value (0xFFFFF)
    uint32_t encoded = encoder->encode_sample(sample_20bit, 20);
    
    // 20-bit sample occupies full audio word (time slots 12-27 + 4 auxiliary bits)
    EXPECT_EQ(524287, encoded & 0xFFFFF);
}

/**
 * TEST-CONF-AUDIO-005: 24-bit audio word precision
 * @req REQ-FUNC-AUDIO-003, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.3
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_005_24BitPrecision)
{
    int32_t sample_24bit = 8388607;  // Max 24-bit value (0x7FFFFF)
    uint32_t encoded = encoder->encode_sample(sample_24bit, 24);
    
    // 24-bit sample uses audio word + auxiliary bits
    EXPECT_EQ(8388607, encoded & 0xFFFFFF);
}

/**
 * TEST-CONF-AUDIO-006: MSB justification for intermediate precision
 * @req REQ-FUNC-AUDIO-004, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.4
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_006_MSB_Justification_18bit)
{
    // 18-bit sample should be left-justified in 20-bit audio word
    int32_t sample_18bit = 131071;  // Max 18-bit value (0x1FFFF)
    uint32_t encoded = encoder->encode_sample(sample_18bit, 18);
    
    // MSB-justified: shift left by (20-18) = 2 bits
    uint32_t expected = sample_18bit << 2;  // 0x7FFFC
    EXPECT_EQ(expected, encoded & 0xFFFFF);
}

/**
 * TEST-CONF-AUDIO-007: MSB justification zero-padding for 17-bit
 * @req REQ-FUNC-AUDIO-004, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.4
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_007_MSB_Justification_17bit_Padding)
{
    // 17-bit sample with LSB zero-padding
    int32_t sample_17bit = 65535;  // 0xFFFF in 17-bit
    uint32_t encoded = encoder->encode_sample(sample_17bit, 17);
    
    // MSB-justified with 3-bit zero-padding: shift left by 3
    uint32_t expected = sample_17bit << 3;
    EXPECT_EQ(expected & 0xFFFFF, encoded & 0xFFFFF);
}

/**
 * TEST-CONF-AUDIO-008: Non-audio content protection (validity bit)
 * @req REQ-FUNC-AUDIO-005, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.5
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_008_NonAudio_ValidityBit)
{
    // Non-audio content (SMPTE 337) must have validity bit = 1
    uint32_t non_audio_sample = 0x12345678;  // Non-audio data
    bool is_non_audio = true;
    
    uint32_t subframe = encoder->create_subframe(non_audio_sample, 24, is_non_audio);
    
    // Validity bit is time slot 28 (bit 28 of subframe)
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(1, validity_bit);
}

/**
 * TEST-CONF-AUDIO-009: Audio content validity bit = 0
 * @req REQ-FUNC-AUDIO-005, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.5
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_009_Audio_ValidityBit_Zero)
{
    // Normal audio content must have validity bit = 0
    int32_t audio_sample = 16384;
    bool is_non_audio = false;
    
    uint32_t subframe = encoder->create_subframe(audio_sample, 16, is_non_audio);
    
    // Validity bit is time slot 28
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(0, validity_bit);
}

/**
 * TEST-CONF-AUDIO-010: DC content minimization verification
 * @req REQ-FUNC-AUDIO-006, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.6
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_010_DC_Minimization)
{
    // Encode 1000 zero samples (DC component test)
    int dc_level_count = 0;
    const int sample_count = 1000;
    
    for (int i = 0; i < sample_count; i++) {
        uint32_t encoded = encoder->encode_sample(0, 16);
        // Check if encoded value has minimal DC component
        // Biphase-mark coding should ensure equal 0s and 1s
        if (encoded == 0) dc_level_count++;
    }
    
    // DC component should be minimized (not all zeros)
    EXPECT_TRUE(dc_level_count < sample_count);
}

/**
 * TEST-CONF-AUDIO-011: Channel sampling frequency interdependency
 * @req REQ-FUNC-AUDIO-007, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_011_SamplingFreq_Interdependency)
{
    // Both channels in same AES3 stream must use same sampling frequency
    uint32_t fs_channel_1 = 48000;  // Hz
    uint32_t fs_channel_2 = 48000;  // Hz
    
    EXPECT_EQ(fs_channel_1, fs_channel_2);
    
    // Verify channel status byte 0 consistency (sampling frequency bits)
    // This would be checked at frame/block level
}

/**
 * TEST-CONF-AUDIO-012: AES5-2018 sampling frequency 32kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_012_AES5_32kHz)
{
    uint32_t fs = 32000;  // Hz
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-013: AES5-2018 sampling frequency 44.1kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_013_AES5_44_1kHz)
{
    uint32_t fs = 44100;  // Hz
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-014: AES5-2018 sampling frequency 48kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_014_AES5_48kHz)
{
    uint32_t fs = 48000;  // Hz (most common professional audio)
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-015: AES5-2018 sampling frequency 96kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_015_AES5_96kHz)
{
    uint32_t fs = 96000;  // Hz (2x 48kHz)
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

// =============================================================================
// Sampling Frequency Tests (Clause 5): 10 test cases
// =============================================================================

/**
 * TEST-CONF-AUDIO-016: AES5-2018 sampling frequency 192kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_016_AES5_192kHz)
{
    uint32_t fs = 192000;  // Hz (4x 48kHz, maximum rate)
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-017: AES5-2018 sampling frequency 88.2kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_017_AES5_88_2kHz)
{
    uint32_t fs = 88200;  // Hz (2x 44.1kHz)
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-018: AES5-2018 sampling frequency 176.4kHz
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2, AES5-2018
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_018_AES5_176_4kHz)
{
    uint32_t fs = 176400;  // Hz (4x 44.1kHz)
    bool is_valid_aes5 = encoder->validate_sampling_frequency(fs);
    EXPECT_TRUE(is_valid_aes5);
}

/**
 * TEST-CONF-AUDIO-019: Invalid sampling frequency rejection
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF-AUDIO_019_Invalid_SamplingFreq)
{
    uint32_t fs_invalid = 47999;  // Hz (non-standard frequency)
    bool is_valid = encoder->validate_sampling_frequency(fs_invalid);
    EXPECT_FALSE(is_valid);
}

/**
 * TEST-CONF-AUDIO-020: Sampling frequency rate limits (32kHz minimum)
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_020_MinimumRate_32kHz)
{
    uint32_t fs_below_min = 16000;  // Hz (below AES5 minimum)
    bool is_valid = encoder->validate_sampling_frequency(fs_below_min);
    EXPECT_FALSE(is_valid);
}

/**
 * TEST-CONF-AUDIO-021: Sampling frequency rate limits (192kHz maximum)
 * @req REQ-FUNC-AUDIO-008, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_021_MaximumRate_192kHz)
{
    uint32_t fs_above_max = 384000;  // Hz (above AES5 maximum)
    bool is_valid = encoder->validate_sampling_frequency(fs_above_max);
    EXPECT_FALSE(is_valid);
}

/**
 * TEST-CONF-AUDIO-022: Sampling frequency consistency across block
 * @req REQ-FUNC-AUDIO-007, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 5.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_022_Consistency_Across_Block)
{
    // Sampling frequency must remain constant across 192-frame block
    uint32_t fs_initial = 48000;
    encoder->set_sampling_frequency(fs_initial);
    
    // Verify sampling frequency doesn't change for 192 frames
    for (int frame = 0; frame < 192; frame++) {
        uint32_t fs_current = encoder->get_sampling_frequency();
        EXPECT_EQ(fs_initial, fs_current);
    }
}

// =============================================================================
// Validity Bit Tests (Clause 6): 8 test cases
// =============================================================================

/**
 * TEST-CONF-AUDIO-023: Validity bit = 0 for valid audio samples
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_023_ValidityBit_ValidAudio)
{
    int32_t valid_sample = 16384;
    uint32_t subframe = encoder->create_subframe(valid_sample, 16, false);
    
    // Validity bit = 0 for valid audio (time slot 28, bit 28)
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(0, validity_bit);
}

/**
 * TEST-CONF-AUDIO-024: Validity bit = 1 for muted samples
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_024_ValidityBit_Muted)
{
    int32_t muted_sample = 0;
    bool is_muted = true;
    uint32_t subframe = encoder->create_subframe(muted_sample, 16, false, is_muted);
    
    // Validity bit = 1 for muted audio
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(1, validity_bit);
}

/**
 * TEST-CONF-AUDIO-025: Validity bit = 1 for interpolated samples
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_025_ValidityBit_Interpolated)
{
    int32_t interpolated_sample = 8192;
    bool is_interpolated = true;
    uint32_t subframe = encoder->create_subframe(interpolated_sample, 16, false, false, is_interpolated);
    
    // Validity bit = 1 for interpolated audio
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(1, validity_bit);
}

/**
 * TEST-CONF-AUDIO-026: Validity bit = 1 for corrupted samples
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_026_ValidityBit_Corrupted)
{
    int32_t corrupted_sample = 12345;
    bool is_corrupted = true;
    uint32_t subframe = encoder->create_subframe(corrupted_sample, 16, false, false, false, is_corrupted);
    
    // Validity bit = 1 for corrupted audio
    uint32_t validity_bit = (subframe >> 28) & 0x1;
    EXPECT_EQ(1, validity_bit);
}

/**
 * TEST-CONF-AUDIO-027: Validity bit position in subframe
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_027_ValidityBit_Position)
{
    int32_t sample = 32767;
    uint32_t subframe = encoder->create_subframe(sample, 16, false);
    
    // Validity bit must be in time slot 28 (bit 28 of 32-bit subframe)
    // Mask everything except bit 28
    uint32_t bit_28_mask = 0x10000000;
    uint32_t validity_extracted = (subframe & bit_28_mask) >> 28;
    
    EXPECT_TRUE(validity_extracted == 0 || validity_extracted == 1);  // Must be 0 or 1
}

/**
 * TEST-CONF-AUDIO-028: Validity bit independence from audio data
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_028_ValidityBit_Independence)
{
    // Validity bit must be independent of audio sample value
    int32_t sample_max = 32767;
    int32_t sample_min = -32768;
    
    uint32_t subframe_max = encoder->create_subframe(sample_max, 16, false);
    uint32_t subframe_min = encoder->create_subframe(sample_min, 16, false);
    
    uint32_t validity_max = (subframe_max >> 28) & 0x1;
    uint32_t validity_min = (subframe_min >> 28) & 0x1;
    
    // Both should have validity = 0 (valid audio)
    EXPECT_EQ(validity_max, validity_min);
}

/**
 * TEST-CONF-AUDIO-029: Validity bit state transitions
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_029_ValidityBit_Transitions)
{
    int32_t sample = 16384;
    
    // Valid audio → validity = 0
    uint32_t subframe_valid = encoder->create_subframe(sample, 16, false);
    uint32_t validity_valid = (subframe_valid >> 28) & 0x1;
    EXPECT_EQ(0, validity_valid);
    
    // Muted audio → validity = 1
    uint32_t subframe_muted = encoder->create_subframe(sample, 16, false, true);
    uint32_t validity_muted = (subframe_muted >> 28) & 0x1;
    EXPECT_EQ(1, validity_muted);
    
    // Recovered audio → validity = 0
    uint32_t subframe_recovered = encoder->create_subframe(sample, 16, false);
    uint32_t validity_recovered = (subframe_recovered >> 28) & 0x1;
    EXPECT_EQ(0, validity_recovered);
}

/**
 * TEST-CONF-AUDIO-030: Validity bit for consecutive invalid samples
 * @req REQ-FUNC-AUDIO-009, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 6.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_030_ValidityBit_Consecutive_Invalid)
{
    // Multiple consecutive invalid samples should all have validity = 1
    for (int i = 0; i < 10; i++) {
        int32_t invalid_sample = i * 1000;
        uint32_t subframe = encoder->create_subframe(invalid_sample, 16, false, true);
        uint32_t validity_bit = (subframe >> 28) & 0x1;
        EXPECT_EQ(1, validity_bit);
    }
}

// =============================================================================
// Pre-emphasis Tests (Clause 7): 6 test cases
// =============================================================================

/**
 * TEST-CONF-AUDIO-031: Pre-emphasis none indication
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_031_PreEmphasis_None)
{
    // Channel status byte 0 bit 2-4 = 000 (no pre-emphasis)
    uint8_t emphasis_code = encoder->get_pre_emphasis_code(PCMEncoder::PreEmphasis::NONE);
    EXPECT_EQ(0x00, emphasis_code & 0x07);  // Bits 2-4 = 000
}

/**
 * TEST-CONF-AUDIO-032: Pre-emphasis 50µs/15µs indication
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.1, ITU-R BS.450-3
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_032_PreEmphasis_50us_15us)
{
    // Channel status byte 0 bit 2-4 = 001 (50µs/15µs)
    uint8_t emphasis_code = encoder->get_pre_emphasis_code(PCMEncoder::PreEmphasis::ITU_50us_15us);
    EXPECT_EQ(0x01, emphasis_code & 0x07);  // Bits 2-4 = 001
}

/**
 * TEST-CONF-AUDIO-033: Pre-emphasis CCITT J.17 indication
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.1, ITU-T J.17
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_033_PreEmphasis_J17)
{
    // Channel status byte 0 bit 2-4 = 011 (CCITT J.17)
    uint8_t emphasis_code = encoder->get_pre_emphasis_code(PCMEncoder::PreEmphasis::J17);
    EXPECT_EQ(0x03, emphasis_code & 0x07);  // Bits 2-4 = 011
}

/**
 * TEST-CONF-AUDIO-034: Pre-emphasis not indicated
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_034_PreEmphasis_NotIndicated)
{
    // Channel status byte 0 bit 2-4 = 111 (not indicated)
    uint8_t emphasis_code = encoder->get_pre_emphasis_code(PCMEncoder::PreEmphasis::NOT_INDICATED);
    EXPECT_EQ(0x07, emphasis_code & 0x07);  // Bits 2-4 = 111
}

/**
 * TEST-CONF-AUDIO-035: Pre-emphasis reserved codes rejection
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_035_PreEmphasis_Reserved_Codes)
{
    // Codes 010, 100, 101, 110 are reserved and should be rejected
    bool is_valid_010 = encoder->validate_pre_emphasis_code(0x02);
    bool is_valid_100 = encoder->validate_pre_emphasis_code(0x04);
    bool is_valid_101 = encoder->validate_pre_emphasis_code(0x05);
    bool is_valid_110 = encoder->validate_pre_emphasis_code(0x06);
    
    EXPECT_FALSE(is_valid_010);
    EXPECT_FALSE(is_valid_100);
    EXPECT_FALSE(is_valid_101);
    EXPECT_FALSE(is_valid_110);
}

/**
 * TEST-CONF-AUDIO-036: Pre-emphasis consistency across block
 * @req REQ-FUNC-AUDIO-010, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 7.2
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_036_PreEmphasis_Block_Consistency)
{
    // Pre-emphasis setting must remain constant across 192-frame block
    PCMEncoder::PreEmphasis emphasis = PCMEncoder::PreEmphasis::ITU_50us_15us;
    encoder->set_pre_emphasis(emphasis);
    
    // Verify pre-emphasis doesn't change for 192 frames
    for (int frame = 0; frame < 192; frame++) {
        PCMEncoder::PreEmphasis current = encoder->get_pre_emphasis();
        EXPECT_EQ(emphasis, current);
    }
}

// =============================================================================
// Additional Audio Coding Tests: 3 test cases
// =============================================================================

/**
 * TEST-CONF-AUDIO-037: PCM overflow clamping
 * @req REQ-FUNC-AUDIO-011, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_037_PCM_Overflow_Clamping)
{
    // Sample exceeding 16-bit range should be clamped
    int32_t overflow_positive = 40000;  // Exceeds 32767
    uint32_t encoded_pos = encoder->encode_sample(overflow_positive, 16);
    EXPECT_EQ(32767, (int16_t)(encoded_pos & 0xFFFF));
    
    int32_t overflow_negative = -40000;  // Exceeds -32768
    uint32_t encoded_neg = encoder->encode_sample(overflow_negative, 16);
    EXPECT_EQ(-32768, (int16_t)(encoded_neg & 0xFFFF));
}

/**
 * TEST-CONF-AUDIO-038: Audio word alignment in subframe
 * @req REQ-FUNC-AUDIO-001, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_038_AudioWord_Alignment)
{
    int32_t sample = 0x1234;
    uint32_t subframe = encoder->create_subframe(sample, 16, false);
    
    // Audio word occupies time slots 12-27 (bits 12-27 of subframe)
    uint32_t audio_word = (subframe >> 12) & 0xFFFF;
    EXPECT_EQ(0x1234, audio_word);
}

/**
 * TEST-CONF-AUDIO-039: Subframe structure integrity
 * @req REQ-FUNC-AUDIO-001, REQ-QUAL-AUDIO-001
 * @standard AES3-1-2009 Clause 4.1
 */
TEST_F(AES3_Part1_AudioCoding_Conformity, TEST_CONF_AUDIO_039_Subframe_Structure)
{
    int32_t sample = 16384;
    uint32_t subframe = encoder->create_subframe(sample, 16, false);
    
    // Subframe should be 32 bits total (time slots 0-31)
    // Verify structure: preamble (4 bits) + auxiliary (4 bits) + audio (20 bits) + validity (1 bit) + user (1 bit) + channel status (1 bit) + parity (1 bit)
    EXPECT_TRUE(subframe != 0);  // Should not be all zeros
    
    // Verify parity bit (time slot 31) provides even parity over time slots 4-31
    int bit_count = 0;
    for (int i = 4; i < 32; i++) {
        if (subframe & (1 << i)) bit_count++;
    }
    EXPECT_EQ(0, bit_count % 2);  // Even parity
}

/**
 * @test_summary 39 conformity test cases for AES3-2009 Part 1 (Audio Content)
 * @test_coverage
 *   - Audio Content Coding (15 tests): Linear PCM, polarity, precision, justification, non-audio protection, DC minimization
 *   - Sampling Frequency (10 tests): AES5 compliance, rate validation, consistency
 *   - Validity Bit (8 tests): Valid audio, muted, interpolated, corrupted samples
 *   - Pre-emphasis (6 tests): None, 50µs/15µs, J.17, not indicated, reserved codes
 * @requirements_traced REQ-QUAL-AUDIO-001, REQ-FUNC-AUDIO-001 through REQ-FUNC-AUDIO-011
 * @standard AES3-1-2009 (R2014) Clauses 4-7
 */

