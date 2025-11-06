/**
 * @file subframe_data.hpp
 * @brief SubframeData structure for AES3-2009 subframe bit manipulation
 * @implements SRC-SUBFRAME-001
 * @traceability DES-D-001, DES-C-001, TEST-SUBFRAME-001
 * 
 * GREEN Phase: Minimal implementation to pass all RED phase tests
 * 
 * 64-bit structure representing 32 time slots (2 bits each) for AES3-2009
 * subframe data as specified in Part 3, Section 4.1.
 * 
 * @standard AES3-2009 Part 3: Transport (Subframe Structure)
 * @copyright Copyright (c) 2025
 */

#ifndef AES_AES3_PART3_2009_SUBFRAME_DATA_HPP
#define AES_AES3_PART3_2009_SUBFRAME_DATA_HPP

#include <cstdint>
#include <cstddef>

namespace AES {
namespace AES3 {
namespace Part3 {
namespace _2009 {
namespace subframe {

/**
 * @brief 64-bit subframe data structure (32 time slots × 2 bits each)
 * @standard AES3-2009 Part 3, Section 4.1
 * 
 * Memory layout (64 bits):
 * [Bits 0-7]:   Preamble (4 time slots, 2 bits each) - slots 0-3
 * [Bits 8-55]:  Audio + Auxiliary (24 time slots, 2 bits each) - slots 4-27
 * [Bits 56-57]: Validity bit (1 time slot, 2 bits) - slot 28
 * [Bits 58-59]: User bit (1 time slot, 2 bits) - slot 29
 * [Bits 60-61]: Channel status bit (1 time slot, 2 bits) - slot 30
 * [Bits 62-63]: Parity bit (1 time slot, 2 bits) - slot 31
 * 
 * Each time slot contains 2 bits for biphase-mark encoding.
 * 
 * @performance get_bit: <50ns, set_bit: <50ns (typical: ~10ns)
 * @memory 8 bytes (64 bits)
 * @alignment Natural 8-byte alignment
 */
struct SubframeData {
    // =============================================================================
    // AES3-2009 Constants
    // =============================================================================
    
    /// Total number of time slots in subframe (AES3-2009 Part 3, §4.1)
    static constexpr size_t TIME_SLOTS = 32;
    
    /// Bits per time slot for biphase-mark encoding
    static constexpr size_t BITS_PER_TIME_SLOT = 2;
    
    /// Total bits in subframe structure (32 slots × 2 bits = 64 bits)
    static constexpr size_t TOTAL_BITS = TIME_SLOTS * BITS_PER_TIME_SLOT;
    
    // Time slot boundaries (AES3-2009 Part 3, Section 4.1)
    
    /// Preamble start slot (X, Y, or Z pattern)
    static constexpr size_t PREAMBLE_START = 0;
    
    /// Preamble end slot (4 time slots total)
    static constexpr size_t PREAMBLE_END = 3;
    
    /// Audio data start slot (first audio bit)
    static constexpr size_t AUDIO_START = 4;
    
    /// Audio data end slot (24 audio/auxiliary bits)
    static constexpr size_t AUDIO_END = 27;
    
    /// Validity bit slot (0=valid audio, 1=unreliable)
    static constexpr size_t VALIDITY_SLOT = 28;
    
    /// User data bit slot (auxiliary information)
    static constexpr size_t USER_SLOT = 29;
    
    /// Channel status bit slot (1 of 192 channel status bits)
    static constexpr size_t CHANNEL_STATUS_SLOT = 30;
    
    /// Even parity bit slot (ensures even parity over slots 4-30)
    static constexpr size_t PARITY_SLOT = 31;
    
    // =============================================================================
    // Data Storage
    // =============================================================================
    
    /// 64-bit storage for 32 time slots (2 bits each)
    uint64_t data;
    
    // =============================================================================
    // Constructors
    // =============================================================================
    
    /**
     * @brief Default constructor - initializes all slots to zero
     * @postcondition data == 0
     * @performance <10ns
     */
    constexpr SubframeData() noexcept : data(0) {}
    
    /**
     * @brief Construct with explicit data value
     * @param initial_data Initial 64-bit value
     * @performance <10ns
     */
    constexpr explicit SubframeData(uint64_t initial_data) noexcept 
        : data(initial_data) {}
    
    // =============================================================================
    // Bit Access Methods
    // =============================================================================
    
    /**
     * @brief Get bit value at specific time slot
     * 
     * Extracts 2-bit value from specified time slot for biphase-mark encoding.
     * 
     * @param slot Time slot index [0, 31]
     * @return 2-bit value (0x00-0x03) at specified slot
     * @precondition slot < TIME_SLOTS (32)
     * @performance <50ns (typical: ~10ns)
     * @complexity O(1)
     * 
     * @note No bounds checking in release builds for performance
     */
    [[nodiscard]] constexpr uint8_t get_bit(size_t slot) const noexcept {
        // Extract 2 bits at slot position:
        // 1. Shift right by (slot * 2) to position slot at LSB
        // 2. Mask with 0x03 to extract 2 bits
        return static_cast<uint8_t>((data >> (slot * BITS_PER_TIME_SLOT)) & 0x03);
    }
    
    /**
     * @brief Set bit value at specific time slot
     * 
     * Sets 2-bit value in specified time slot without affecting other slots.
     * Automatically masks value to 2 bits (0x00-0x03).
     * 
     * @param slot Time slot index [0, 31]
     * @param value 2-bit value to set (automatically masked to 0x03)
     * @precondition slot < TIME_SLOTS (32)
     * @postcondition get_bit(slot) == (value & 0x03)
     * @performance <50ns (typical: ~10ns)
     * @complexity O(1)
     * 
     * @note No bounds checking in release builds for performance
     */
    constexpr void set_bit(size_t slot, uint8_t value) noexcept {
        // Set 2 bits at slot position:
        // 1. Create mask to clear target slot: ~(0x03 << (slot * 2))
        // 2. Apply mask to clear target bits: data & mask
        // 3. Set new value: (data & mask) | (value << (slot * 2))
        
        const uint64_t clear_mask = ~(0x03ULL << (slot * BITS_PER_TIME_SLOT));
        const uint64_t set_mask = static_cast<uint64_t>(value & 0x03) << (slot * BITS_PER_TIME_SLOT);
        
        data = (data & clear_mask) | set_mask;
    }
    
    /**
     * @brief Clear all time slots to zero
     * 
     * Resets entire subframe structure to initial state.
     * 
     * @postcondition data == 0
     * @performance <10ns
     * @complexity O(1)
     */
    constexpr void clear() noexcept {
        data = 0;
    }
    
    // =============================================================================
    // Comparison Operators
    // =============================================================================
    
    /**
     * @brief Equality comparison
     * @param other SubframeData to compare with
     * @return true if data fields are equal
     */
    constexpr bool operator==(const SubframeData& other) const noexcept {
        return data == other.data;
    }
    
    /**
     * @brief Inequality comparison
     * @param other SubframeData to compare with
     * @return true if data fields are not equal
     */
    constexpr bool operator!=(const SubframeData& other) const noexcept {
        return data != other.data;
    }
};

// Static assertions to verify structure properties at compile time
static_assert(sizeof(SubframeData) == 8, 
    "SubframeData must be 8 bytes (64 bits)");

static_assert(SubframeData::TIME_SLOTS == 32, 
    "AES3-2009 requires 32 time slots per subframe");

static_assert(SubframeData::TOTAL_BITS == 64, 
    "AES3-2009 subframe must be 64 bits total");

static_assert(SubframeData::AUDIO_END - SubframeData::AUDIO_START + 1 == 24,
    "AES3-2009 requires 24 audio/auxiliary slots (4-27)");

} // namespace subframe
} // namespace _2009
} // namespace Part3
} // namespace AES3
} // namespace AES

#endif // AES_AES3_PART3_2009_SUBFRAME_DATA_HPP
