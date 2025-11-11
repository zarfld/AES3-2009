/**
 * @file channel_status.hpp
 * @brief AES3-2009 Part 2 Metadata - Channel Status Block Implementation
 * 
 * Module: src/AES/AES3/Part2/_2009/metadata/channel_status.hpp
 * Phase: 05-implementation
 * Traceability:
 *   Design: DES-C-002
 *   Requirements: REQ-FUNC-META-001, REQ-FUNC-META-002, REQ-FUNC-META-003, 
 *                 REQ-FUNC-META-004, REQ-FUNC-META-005, REQ-FUNC-META-006
 *   Tests: TEST-CONF-META-001 through TEST-CONF-META-041
 * 
 * @standard AES3-2-2009 (R2014) Part 2: Metadata and Subcode
 *   - Clause 5.2: Channel status block structure (192 bits)
 *   - Clause 5.5.0: Byte 0 professional/PCM/emphasis/lock/sampling
 *   - Clause 5.5.2: Byte 2 auxiliary bits/word length/alignment
 *   - Clause 5.5.11: Byte 23 CRCC validation
 *   - Annex C: CRCC computation algorithm
 * 
 * @copyright Copyright (c) 2025 AES3-2009 Project
 */

#ifndef AES_AES3_PART2_2009_METADATA_CHANNEL_STATUS_HPP
#define AES_AES3_PART2_2009_METADATA_CHANNEL_STATUS_HPP

#include <array>
#include <cstddef>  // For size_t
#include <cstdint>

namespace AES {
namespace AES3 {
namespace Part2 {
namespace _2009 {
namespace metadata {

/**
 * @brief Channel Status Block (192 bits = 24 bytes)
 * @standard AES3-2-2009 Clause 5.2
 * 
 * Contains metadata about audio stream transmitted alongside audio samples.
 * One bit per subframe (192 frames = 1 block = 24 bytes).
 * 
 * Byte Structure:
 * - Byte 0: Professional/consumer, PCM/non-PCM, emphasis, lock, sampling frequency
 * - Byte 1: Channel mode, user bits management
 * - Byte 2: Auxiliary bits usage, word length, alignment level
 * - Bytes 3-22: Extended data (source/destination, time code, etc.)
 * - Byte 23: CRCC (CRC-8 checksum)
 * 
 * @performance O(1) for all accessor operations
 * @memory 24 bytes
 */
struct ChannelStatusBlock {
    std::array<uint8_t, 24> bytes;
    
    constexpr ChannelStatusBlock() noexcept : bytes{} {}
    
    /**
     * @brief Clear all channel status bits to zero
     * @postcondition All 192 bits set to 0
     */
    void clear() noexcept {
        bytes.fill(0);
    }
    
    // =========================================================================
    // Byte 0 Accessors (AES3-2-2009 Clause 5.5.0)
    // =========================================================================
    
    /**
     * @brief Check if professional use (bit 0)
     * @return true if professional (bit=1), false if consumer (bit=0)
     * @standard AES3-2-2009 Clause 5.5.0, bit 0
     */
    bool is_professional() const noexcept { 
        return (bytes[0] & 0x01) != 0; 
    }
    
    /**
     * @brief Set professional/consumer use flag
     * @param prof true for professional, false for consumer
     */
    void set_professional(bool prof) noexcept { 
        if (prof) 
            bytes[0] |= 0x01; 
        else 
            bytes[0] &= static_cast<uint8_t>(~0x01); 
    }
    
    /**
     * @brief Check if audio format is linear PCM (bit 1)
     * @return true if linear PCM (bit=0), false if non-PCM (bit=1)
     * @standard AES3-2-2009 Clause 5.5.0, bit 1
     */
    bool is_linear_pcm() const noexcept { 
        return (bytes[0] & 0x02) == 0; 
    }
    
    /**
     * @brief Set audio format (linear PCM or non-PCM)
     * @param pcm true for linear PCM, false for non-PCM
     */
    void set_linear_pcm(bool pcm) noexcept {
        if (!pcm) 
            bytes[0] |= 0x02; 
        else 
            bytes[0] &= static_cast<uint8_t>(~0x02);
    }
    
    /**
     * @brief Get pre-emphasis indicator (bits 2-4)
     * @return Pre-emphasis value (0-7)
     *   - 000: No indication
     *   - 001: No pre-emphasis
     *   - 010: Reserved
     *   - 011: 50µs+15µs pre-emphasis (ITU-R BS.450-3)
     *   - 100: Reserved
     *   - 101: Reserved
     *   - 110: ITU-T J.17
     *   - 111: Reserved
     * @standard AES3-2-2009 Clause 5.5.0, bits 2-4
     */
    uint8_t get_pre_emphasis() const noexcept { 
        return (bytes[0] >> 2) & 0x07; 
    }
    
    /**
     * @brief Set pre-emphasis indicator
     * @param emphasis Pre-emphasis value (0-7, see get_pre_emphasis())
     */
    void set_pre_emphasis(uint8_t emphasis) noexcept {
        bytes[0] = (bytes[0] & 0xE3) | ((emphasis & 0x07) << 2);
    }
    
    /**
     * @brief Check if sampling frequency is locked (bit 5)
     * @return true if locked (bit=0), false if unlocked (bit=1)
     * @standard AES3-2-2009 Clause 5.5.0, bit 5
     */
    bool is_locked() const noexcept { 
        return (bytes[0] & 0x20) == 0; 
    }
    
    /**
     * @brief Set sampling frequency lock status
     * @param locked true if locked, false if unlocked
     */
    void set_lock_status(bool locked) noexcept {
        if (!locked) 
            bytes[0] |= 0x20; 
        else 
            bytes[0] &= static_cast<uint8_t>(~0x20);
    }
    
    /**
     * @brief Get sampling frequency indicator (bits 6-7)
     * @return Sampling frequency value (0-3)
     *   - 00: 48 kHz (professional) or not indicated (consumer)
     *   - 01: 48 kHz
     *   - 10: 44.1 kHz
     *   - 11: 32 kHz
     * @standard AES3-2-2009 Clause 5.5.0, bits 6-7
     */
    uint8_t get_sampling_frequency() const noexcept { 
        return (bytes[0] >> 6) & 0x03; 
    }
    
    /**
     * @brief Set sampling frequency indicator
     * @param fs Sampling frequency value (0-3, see get_sampling_frequency())
     */
    void set_sampling_frequency(uint8_t fs) noexcept {
        bytes[0] = (bytes[0] & 0x3F) | ((fs & 0x03) << 6);
    }
    
    // =========================================================================
    // Byte 2 Accessors (AES3-2-2009 Clause 5.5.2)
    // =========================================================================
    
    /**
     * @brief Get auxiliary bits usage indicator (bits 0-2)
     * @return Auxiliary bits usage value (0-7)
     *   - 000: Max audio 20 bits, undefined usage of auxiliary bits
     *   - 001: Max audio 24 bits (auxiliary bits part of audio)
     *   - 010: Max audio 20 bits, auxiliary bits for coordination signal
     *   - 011: Reserved
     *   - 100: Max audio 24 bits (auxiliary bits part of audio)
     *   - 101-111: Reserved
     * @standard AES3-2-2009 Clause 5.5.2, bits 0-2
     */
    uint8_t get_aux_bits_usage() const noexcept { 
        return bytes[2] & 0x07; 
    }
    
    /**
     * @brief Set auxiliary bits usage indicator
     * @param usage Auxiliary bits usage value (0-7, see get_aux_bits_usage())
     */
    void set_aux_bits_usage(uint8_t usage) noexcept {
        bytes[2] = (bytes[2] & 0xF8) | (usage & 0x07);
    }
    
    /**
     * @brief Get audio word length indicator (bits 3-5)
     * @return Word length value (0-7)
     *   When max 20 bits:
     *   - 000: Not indicated
     *   - 001: 16 bits
     *   - 010: 17 bits
     *   - 011: 18 bits
     *   - 100: 19 bits
     *   - 101: 20 bits
     *   - 110-111: Reserved
     *   When max 24 bits:
     *   - 000: Not indicated
     *   - 001: 20 bits
     *   - 010: 21 bits
     *   - 011: 22 bits
     *   - 100: 23 bits
     *   - 101: 24 bits
     *   - 110-111: Reserved
     * @standard AES3-2-2009 Clause 5.5.2, bits 3-5
     */
    uint8_t get_word_length() const noexcept { 
        return (bytes[2] >> 3) & 0x07; 
    }
    
    /**
     * @brief Set audio word length indicator
     * @param wl Word length value (0-7, see get_word_length())
     */
    void set_word_length(uint8_t wl) noexcept {
        bytes[2] = (bytes[2] & 0xC7) | ((wl & 0x07) << 3);
    }
    
    /**
     * @brief Get multi-channel alignment level (bits 6-7)
     * @return Alignment level value (0-3)
     *   - 00: Not indicated
     *   - 01: Reserved
     *   - 10: SMPTE RP155 (-20 dBFS alignment level)
     *   - 11: Reserved
     * @standard AES3-2-2009 Clause 5.5.2, bits 6-7
     */
    uint8_t get_alignment_level() const noexcept { 
        return (bytes[2] >> 6) & 0x03; 
    }
    
    /**
     * @brief Set multi-channel alignment level
     * @param align Alignment level value (0-3, see get_alignment_level())
     */
    void set_alignment_level(uint8_t align) noexcept {
        bytes[2] = (bytes[2] & 0x3F) | ((align & 0x03) << 6);
    }
};

/**
 * @brief CRCC (Cyclic Redundancy Check Code) Calculator for Channel Status
 * @standard AES3-2-2009 Clause 5.5.11, Annex C
 * 
 * Implements CRC-8 checksum for channel status block error detection.
 * Polynomial: G(x) = x^8 + x^4 + x^3 + x^2 + 1 (0x1D normal form)
 * Initial value: 0xFF
 * Processing: LSB first (per AES3 specification)
 * 
 * CRCC covers bytes 0-22 (184 bits) and is stored in byte 23.
 * 
 * @performance O(n) where n=184 bits, typical <1µs
 */
class CRCCCalculator {
public:
    /**
     * @brief Compute CRCC for channel status bytes 0-22
     * @param block Channel status block (bytes 0-22 used for calculation)
     * @return 8-bit CRCC value to be stored in byte 23
     * 
     * @standard AES3-2-2009 Annex C
     * @performance <1µs typical
     */
    static uint8_t compute(const ChannelStatusBlock& block) noexcept {
        // CRC-8 with polynomial x^8 + x^4 + x^3 + x^2 + 1 (0x1D)
        // Initial condition: 0xFF
        // LSB-first processing per AES3 specification
        uint8_t crc = 0xFF;
        
        // Process bytes 0-22 (184 bits total), bit-by-bit LSB first
        for (size_t i = 0; i < 23; i++) {
            uint8_t byte = block.bytes[i];
            for (int bit = 0; bit < 8; bit++) {
                bool bit_val = (byte >> bit) & 0x01;  // Extract LSB first
                bool crc_msb = (crc >> 7) & 0x01;      // Check CRC MSB
                crc <<= 1;                             // Shift left
                if (crc_msb ^ bit_val) {               // XOR condition
                    crc ^= 0x1D;                       // Apply polynomial
                }
            }
        }
        
        return crc;
    }
    
    /**
     * @brief Validate CRCC in channel status block
     * @param block Channel status block with CRCC in byte 23
     * @return true if CRCC is valid, false if corrupted
     * 
     * @standard AES3-2-2009 Clause 5.5.11
     * @performance <1µs typical
     */
    static bool validate(const ChannelStatusBlock& block) noexcept {
        uint8_t computed = compute(block);
        return computed == block.bytes[23];
    }
};

/**
 * @brief User Data Channel (1 bit per subframe)
 * @standard AES3-2-2009 Clause 4
 * 
 * User data provides 1 bit per subframe for application-defined purposes.
 * At 48 kHz sampling: 48,000 bits/sec capacity per channel.
 * Format is not standardized - application-specific.
 * 
 * Default value: logic 0
 * 
 * @memory 1 byte (stores 1 bit)
 */
struct UserData {
    bool bit;  ///< Single bit value
    
    constexpr UserData() noexcept : bit(false) {}
    explicit constexpr UserData(bool b) noexcept : bit(b) {}
};

/**
 * @brief Auxiliary Bits (4 LSBs when word length ≤ 20 bits)
 * @standard AES3-2-2009 Clause 6
 * 
 * When audio word length is 20 bits or less, the 4 least significant bits
 * of the 24-bit audio sample can be used for auxiliary purposes:
 * - Coordination signal (12-bit samples at Fs/3, 4 bits per frame)
 * - Extended audio data
 * - Application-specific data
 * 
 * When audio word length is 24 bits, auxiliary bits are part of audio data.
 * 
 * @memory 1 byte (stores 4 bits)
 */
struct AuxiliaryBits {
    uint8_t bits;  ///< 4-bit value (0-15)
    
    constexpr AuxiliaryBits() noexcept : bits(0) {}
    explicit constexpr AuxiliaryBits(uint8_t b) noexcept : bits(b & 0x0F) {}
};

} // namespace metadata
} // namespace _2009
} // namespace Part2
} // namespace AES3
} // namespace AES

#endif // AES_AES3_PART2_2009_METADATA_CHANNEL_STATUS_HPP
