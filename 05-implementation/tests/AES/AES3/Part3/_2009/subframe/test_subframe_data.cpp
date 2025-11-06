/**
 * @file test_subframe_data.cpp
 * @brief Unit tests for SubframeData structure (TDD Red Phase)
 * @implements TEST-SUBFRAME-001
 * @traceability DES-D-001, DES-C-001
 * 
 * RED Phase: Write failing tests for 64-bit SubframeData structure
 * with bit manipulation for 32 time slots (AES3-2009 Part 3 Section 4.1)
 * 
 * @standard AES3-2009 Part 3: Transport (Subframe Structure)
 * @copyright Copyright (c) 2025
 */

#include <gtest/gtest.h>
#include <cstdint>

// Include GREEN phase implementation
#include "../../../../../../src/AES/AES3/Part3/_2009/subframe/subframe_data.hpp"

using namespace AES::AES3::Part3::_2009::subframe;

// =============================================================================
// Test Fixture
// =============================================================================

class SubframeDataTest : public ::testing::Test {
protected:
    SubframeData subframe;
    
    void SetUp() override {
        // Clear subframe before each test
        subframe.data = 0;
    }
    
    void TearDown() override {
        // Cleanup if needed
    }
};

// =============================================================================
// RED PHASE: Basic Construction and Initialization Tests
// =============================================================================

/**
 * @test TEST-SUBFRAME-001-01: Default construction initializes to zero
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, DefaultConstruction_InitializesToZero) {
    // Arrange & Act
    SubframeData default_subframe;
    
    // Assert
    EXPECT_EQ(default_subframe.data, 0ULL);
}

/**
 * @test TEST-SUBFRAME-001-02: Clear method sets all bits to zero
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, Clear_AfterDataSet_SetsAllBitsToZero) {
    // Arrange
    subframe.data = 0xFFFFFFFFFFFFFFFFULL;
    
    // Act
    subframe.clear();
    
    // Assert
    EXPECT_EQ(subframe.data, 0ULL);
}

/**
 * @test TEST-SUBFRAME-001-03: Structure size is 8 bytes (64 bits)
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, StructureSize_Is8Bytes) {
    // Assert
    EXPECT_EQ(sizeof(SubframeData), 8);
}

// =============================================================================
// RED PHASE: Bit Accessor Tests (get_bit/set_bit)
// =============================================================================

/**
 * @test TEST-SUBFRAME-001-04: get_bit returns correct value for preamble slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, GetBit_PreambleSlot0_ReturnsCorrectValue) {
    // Arrange
    subframe.data = 0x0000000000000003ULL;  // Set slot 0 to 0b11
    
    // Act
    uint8_t value = subframe.get_bit(SubframeData::PREAMBLE_START);
    
    // Assert
    EXPECT_EQ(value, 0x03);
}

/**
 * @test TEST-SUBFRAME-001-05: get_bit returns correct value for audio slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, GetBit_AudioSlot4_ReturnsCorrectValue) {
    // Arrange
    subframe.data = 0x0000000000000200ULL;  // Set slot 4 to 0b10 (bit 9)
    
    // Act
    uint8_t value = subframe.get_bit(SubframeData::AUDIO_START);
    
    // Assert
    EXPECT_EQ(value, 0x02);
}

/**
 * @test TEST-SUBFRAME-001-06: get_bit returns correct value for validity slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, GetBit_ValiditySlot28_ReturnsCorrectValue) {
    // Arrange
    // Slot 28: bit position 56-57 (28 * 2 = 56)
    subframe.data = 0x0100000000000000ULL;  // Set slot 28 to 0b01 (bit 56)
    
    // Act
    uint8_t value = subframe.get_bit(SubframeData::VALIDITY_SLOT);
    
    // Assert
    EXPECT_EQ(value, 0x01);
}

/**
 * @test TEST-SUBFRAME-001-07: get_bit returns correct value for parity slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, GetBit_ParitySlot31_ReturnsCorrectValue) {
    // Arrange
    subframe.data = 0x4000000000000000ULL;  // Set slot 31 to 0b01 (bit 62)
    
    // Act
    uint8_t value = subframe.get_bit(SubframeData::PARITY_SLOT);
    
    // Assert
    EXPECT_EQ(value, 0x01);
}

/**
 * @test TEST-SUBFRAME-001-08: set_bit sets correct value in preamble slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_PreambleSlot0_SetsCorrectValue) {
    // Arrange
    subframe.data = 0;
    
    // Act
    subframe.set_bit(SubframeData::PREAMBLE_START, 0x03);
    
    // Assert
    EXPECT_EQ(subframe.data, 0x0000000000000003ULL);
    EXPECT_EQ(subframe.get_bit(SubframeData::PREAMBLE_START), 0x03);
}

/**
 * @test TEST-SUBFRAME-001-09: set_bit sets correct value in audio slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_AudioSlot4_SetsCorrectValue) {
    // Arrange
    subframe.data = 0;
    
    // Act
    subframe.set_bit(SubframeData::AUDIO_START, 0x02);
    
    // Assert
    EXPECT_EQ(subframe.data, 0x0000000000000200ULL);
    EXPECT_EQ(subframe.get_bit(SubframeData::AUDIO_START), 0x02);
}

/**
 * @test TEST-SUBFRAME-001-10: set_bit sets correct value in validity slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_ValiditySlot28_SetsCorrectValue) {
    // Arrange
    subframe.data = 0;
    
    // Act
    subframe.set_bit(SubframeData::VALIDITY_SLOT, 0x01);
    
    // Assert
    // Slot 28: bit position 56-57 (28 * 2 = 56)
    EXPECT_EQ(subframe.data, 0x0100000000000000ULL);
    EXPECT_EQ(subframe.get_bit(SubframeData::VALIDITY_SLOT), 0x01);
}

/**
 * @test TEST-SUBFRAME-001-11: set_bit sets correct value in parity slot
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_ParitySlot31_SetsCorrectValue) {
    // Arrange
    subframe.data = 0;
    
    // Act
    subframe.set_bit(SubframeData::PARITY_SLOT, 0x01);
    
    // Assert
    EXPECT_EQ(subframe.data, 0x4000000000000000ULL);
    EXPECT_EQ(subframe.get_bit(SubframeData::PARITY_SLOT), 0x01);
}

/**
 * @test TEST-SUBFRAME-001-12: set_bit masks value to 2 bits (0x03 max)
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_ValueExceeds2Bits_MasksTo2Bits) {
    // Arrange
    subframe.data = 0;
    
    // Act - attempt to set 0xFF (8 bits)
    subframe.set_bit(0, 0xFF);
    
    // Assert - should mask to 0x03 (2 bits)
    EXPECT_EQ(subframe.get_bit(0), 0x03);
}

/**
 * @test TEST-SUBFRAME-001-13: set_bit does not affect other slots
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_Slot10_DoesNotAffectOtherSlots) {
    // Arrange
    subframe.data = 0xFFFFFFFFFFFFFFFFULL;  // All slots set to 0b11
    
    // Act - set slot 10 to 0b00
    subframe.set_bit(10, 0x00);
    
    // Assert
    EXPECT_EQ(subframe.get_bit(10), 0x00);
    EXPECT_EQ(subframe.get_bit(9), 0x03);   // Neighbor unaffected
    EXPECT_EQ(subframe.get_bit(11), 0x03);  // Neighbor unaffected
}

/**
 * @test TEST-SUBFRAME-001-14: Multiple set_bit calls accumulate correctly
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, SetBit_MultipleCalls_AccumulateCorrectly) {
    // Arrange & Act
    subframe.set_bit(SubframeData::PREAMBLE_START, 0x03);
    subframe.set_bit(SubframeData::VALIDITY_SLOT, 0x01);
    subframe.set_bit(SubframeData::PARITY_SLOT, 0x01);
    
    // Assert
    EXPECT_EQ(subframe.get_bit(SubframeData::PREAMBLE_START), 0x03);
    EXPECT_EQ(subframe.get_bit(SubframeData::VALIDITY_SLOT), 0x01);
    EXPECT_EQ(subframe.get_bit(SubframeData::PARITY_SLOT), 0x01);
}

// =============================================================================
// RED PHASE: Boundary Tests (32 time slots)
// =============================================================================

/**
 * @test TEST-SUBFRAME-001-15: All 32 time slots are accessible
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, AllTimeSlots_AreAccessible) {
    // Arrange & Act - set all slots to unique values
    for (size_t slot = 0; slot < SubframeData::TIME_SLOTS; ++slot) {
        uint8_t value = static_cast<uint8_t>(slot & 0x03);
        subframe.set_bit(slot, value);
    }
    
    // Assert - verify all slots
    for (size_t slot = 0; slot < SubframeData::TIME_SLOTS; ++slot) {
        uint8_t expected = static_cast<uint8_t>(slot & 0x03);
        EXPECT_EQ(subframe.get_bit(slot), expected) 
            << "Slot " << slot << " value mismatch";
    }
}

/**
 * @test TEST-SUBFRAME-001-16: Slot 0 (first preamble) independent from others
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, Slot0_IndependentFromOtherSlots) {
    // Arrange
    subframe.data = 0;
    subframe.set_bit(1, 0x03);
    
    // Act
    subframe.set_bit(0, 0x02);
    
    // Assert
    EXPECT_EQ(subframe.get_bit(0), 0x02);
    EXPECT_EQ(subframe.get_bit(1), 0x03);
}

/**
 * @test TEST-SUBFRAME-001-17: Slot 31 (parity) independent from others
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, Slot31_IndependentFromOtherSlots) {
    // Arrange
    subframe.data = 0;
    subframe.set_bit(30, 0x03);
    
    // Act
    subframe.set_bit(31, 0x01);
    
    // Assert
    EXPECT_EQ(subframe.get_bit(31), 0x01);
    EXPECT_EQ(subframe.get_bit(30), 0x03);
}

// =============================================================================
// RED PHASE: AES3-2009 Compliance Tests
// =============================================================================

/**
 * @test TEST-SUBFRAME-001-18: Constants match AES3-2009 specification
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, Constants_MatchAES3Specification) {
    // AES3-2009 Part 3, Section 4.1 specifies:
    // - 32 time slots
    // - 2 bits per time slot (biphase-mark)
    // - Preamble: slots 0-3
    // - Audio data: slots 4-27
    // - Validity: slot 28
    // - User: slot 29
    // - Channel status: slot 30
    // - Parity: slot 31
    
    EXPECT_EQ(SubframeData::TIME_SLOTS, 32);
    EXPECT_EQ(SubframeData::BITS_PER_TIME_SLOT, 2);
    EXPECT_EQ(SubframeData::TOTAL_BITS, 64);
    
    EXPECT_EQ(SubframeData::PREAMBLE_START, 0);
    EXPECT_EQ(SubframeData::PREAMBLE_END, 3);
    EXPECT_EQ(SubframeData::AUDIO_START, 4);
    EXPECT_EQ(SubframeData::AUDIO_END, 27);
    EXPECT_EQ(SubframeData::VALIDITY_SLOT, 28);
    EXPECT_EQ(SubframeData::USER_SLOT, 29);
    EXPECT_EQ(SubframeData::CHANNEL_STATUS_SLOT, 30);
    EXPECT_EQ(SubframeData::PARITY_SLOT, 31);
}

/**
 * @test TEST-SUBFRAME-001-19: 24 audio slots available (4-27)
 * @traceability DES-D-001
 */
TEST_F(SubframeDataTest, AudioSlots_24SlotsAvailable) {
    // AES3-2009 specifies 24 slots for audio data (4-27)
    size_t audio_slot_count = SubframeData::AUDIO_END - SubframeData::AUDIO_START + 1;
    
    EXPECT_EQ(audio_slot_count, 24);
}

// =============================================================================
// RED PHASE: Performance Constraint Tests
// =============================================================================

/**
 * @test TEST-SUBFRAME-001-20: get_bit performance under 50ns
 * @traceability REQ-PERF-001, DES-D-001
 * @note Informal performance test (precise measurement in benchmarks)
 */
TEST_F(SubframeDataTest, GetBit_PerformanceUnder50ns) {
    // Arrange
    subframe.data = 0x123456789ABCDEF0ULL;
    
    // Act - multiple calls to ensure consistent performance
    constexpr size_t ITERATIONS = 1000;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        volatile uint8_t value = subframe.get_bit(i % SubframeData::TIME_SLOTS);
        (void)value;  // Prevent optimization
    }
    
    // Assert - no timing assertion here (see benchmarks)
    // This test verifies API functionality under load
    SUCCEED();
}

/**
 * @test TEST-SUBFRAME-001-21: set_bit performance under 50ns
 * @traceability REQ-PERF-001, DES-D-001
 * @note Informal performance test (precise measurement in benchmarks)
 */
TEST_F(SubframeDataTest, SetBit_PerformanceUnder50ns) {
    // Act - multiple calls to ensure consistent performance
    constexpr size_t ITERATIONS = 1000;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        subframe.set_bit(i % SubframeData::TIME_SLOTS, static_cast<uint8_t>(i & 0x03));
    }
    
    // Assert - no timing assertion here (see benchmarks)
    // This test verifies API functionality under load
    SUCCEED();
}

// =============================================================================
// End of RED Phase Tests
// =============================================================================
// All tests above SHOULD FAIL until GREEN phase implementation is complete
// =============================================================================
