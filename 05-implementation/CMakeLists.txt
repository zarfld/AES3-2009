# AES3-2009 Digital Audio Interface - Implementation
# Phase 05: TDD Implementation following ISO/IEC/IEEE 12207:2017

cmake_minimum_required(VERSION 3.20)
project(AES3-2009
    VERSION 0.1.0
    DESCRIPTION "AES3-2009 Digital Audio Interface Implementation"
    LANGUAGES CXX C
)

# =============================================================================
# Project Standards and Policies
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Export compile commands for static analysis tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================================================================
# Build Options
# =============================================================================
option(ENABLE_TESTING "Enable unit testing with Google Test" ON)
option(ENABLE_COVERAGE "Enable code coverage analysis" OFF)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSanitizer" OFF)
option(ENABLE_BENCHMARKS "Enable performance benchmarks" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# =============================================================================
# Compiler Flags (C++17 Standards Layer)
# =============================================================================
if(MSVC)
    # MSVC 2017+ with C++17 support
    add_compile_options(
        /W4           # Warning level 4
        /WX           # Treat warnings as errors
        /permissive-  # Standards conformance
        /Zc:__cplusplus  # Report correct __cplusplus value
    )
    # Apply C++17 standard only to CXX files, not C files
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/std:c++17>)
    if(ENABLE_COVERAGE)
        message(WARNING "Code coverage not supported on MSVC")
    endif()
else()
    # GCC 7+ or Clang 5+ with C++17 support
    add_compile_options(
        -Wall
        -Wextra
        -Werror
        -pedantic
        -Wconversion
        -Wsign-conversion
        -Wno-unused-parameter
    )
    
    # C++17 features allowed (from ADR-002)
    add_compile_options(
        -fno-exceptions      # No exceptions in real-time code
        -fno-rtti            # No RTTI
    )
    
    if(ENABLE_COVERAGE)
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
    endif()
    
    if(ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# =============================================================================
# Directory Structure
# =============================================================================
set(AES3_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(AES3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(AES3_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(AES3_MOCK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
set(AES3_BENCH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks)

# =============================================================================
# Dependencies
# =============================================================================
if(ENABLE_TESTING)
    # Google Test (fetch from GitHub if not found)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "Google Test not found, fetching from GitHub...")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    enable_testing()
    include(GoogleTest)
endif()

# =============================================================================
# Standards Layer Libraries (C++17)
# =============================================================================

# Part 1: Audio Content (PCM Encoder)
add_library(aes3_part1_audio_coding STATIC
    ${AES3_SOURCE_DIR}/AES/AES3/Part1/_2009/audio_coding/pcm_encoder.cpp
)
target_include_directories(aes3_part1_audio_coding
    PUBLIC
        ${AES3_INCLUDE_DIR}
        ${AES3_SOURCE_DIR}
)
target_compile_features(aes3_part1_audio_coding PUBLIC cxx_std_17)

# Part 3: Transport (Subframe Builder)
add_library(aes3_part3_transport STATIC
    ${AES3_SOURCE_DIR}/AES/AES3/Part3/_2009/subframe/subframe_builder.cpp
)
target_include_directories(aes3_part3_transport
    PUBLIC
        ${AES3_INCLUDE_DIR}
        ${AES3_SOURCE_DIR}
)
target_compile_features(aes3_part3_transport PUBLIC cxx_std_17)

# =============================================================================
# Mock Implementations (for testing)
# =============================================================================
if(ENABLE_TESTING)
    add_library(aes3_mocks STATIC
        ${AES3_TEST_DIR}/Common/mock_audio_hal.c
    )
    target_include_directories(aes3_mocks
        PUBLIC
            ${AES3_INCLUDE_DIR}
            ${AES3_TEST_DIR}/Common
    )
    target_compile_features(aes3_mocks PUBLIC c_std_11)
endif()

# =============================================================================
# Unit Tests (TDD approach)
# =============================================================================
if(ENABLE_TESTING)
    # Test: SubframeData structure
    add_executable(test_subframe_data
        ${AES3_TEST_DIR}/AES/AES3/Part3/_2009/subframe/test_subframe_data.cpp
    )
    target_link_libraries(test_subframe_data
        PRIVATE
            aes3_part3_transport
            GTest::gtest_main
    )
    gtest_discover_tests(test_subframe_data
        PROPERTIES
            LABELS "unit;part3;transport"
    )
    
    # Test: PCM Encoder
    add_executable(test_pcm_encoder
        ${AES3_TEST_DIR}/AES/AES3/Part1/_2009/audio_coding/test_pcm_encoder.cpp
    )
    target_link_libraries(test_pcm_encoder
        PRIVATE
            aes3_part1_audio_coding
            GTest::gtest_main
    )
    gtest_discover_tests(test_pcm_encoder
        PROPERTIES
            LABELS "unit;part1;audio"
    )
    
    # Test: Subframe Builder
    add_executable(test_subframe_builder
        ${AES3_TEST_DIR}/AES/AES3/Part3/_2009/subframe/test_subframe_builder.cpp
    )
    target_link_libraries(test_subframe_builder
        PRIVATE
            aes3_part3_transport
            GTest::gtest_main
    )
    gtest_discover_tests(test_subframe_builder
        PROPERTIES
            LABELS "unit;part3;transport"
    )
    
    # Test: Mock Audio HAL
    add_executable(test_mock_audio_hal
        ${AES3_TEST_DIR}/Common/test_mock_audio_hal.cpp
    )
    target_link_libraries(test_mock_audio_hal
        PRIVATE
            aes3_mocks
            GTest::gtest_main
    )
    gtest_discover_tests(test_mock_audio_hal
        PROPERTIES
            LABELS "unit;mocks;infrastructure"
    )
    
    # Integration Test: Transmit Path (Complete end-to-end chain)
    add_executable(test_transmit_integration
        ${AES3_TEST_DIR}/integration/test_transmit_integration.cpp
    )
    target_link_libraries(test_transmit_integration
        PRIVATE
            aes3_part1_audio_coding
            aes3_part3_transport
            aes3_mocks
            GTest::gtest_main
    )
    gtest_discover_tests(test_transmit_integration
        PROPERTIES
            LABELS "integration;transmit;end-to-end"
            TIMEOUT 60
    )
endif()

# =============================================================================
# Performance Benchmarks
# =============================================================================
if(ENABLE_BENCHMARKS)
    add_executable(bench_pcm_encoder
        ${AES3_BENCH_DIR}/bench_pcm_encoder.cpp
    )
    target_link_libraries(bench_pcm_encoder
        PRIVATE
            aes3_part1_audio_coding
    )
    
    add_executable(bench_subframe_builder
        ${AES3_BENCH_DIR}/bench_subframe_builder.cpp
    )
    target_link_libraries(bench_subframe_builder
        PRIVATE
            aes3_part3_transport
    )
endif()

# =============================================================================
# Code Coverage Target
# =============================================================================
if(ENABLE_COVERAGE)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    
    if(LCOV AND GENHTML)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' '*/mocks/*' --output-file coverage.info
            COMMAND ${GENHTML} coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report..."
        )
        
        # Coverage threshold check
        add_custom_target(coverage_check
            COMMAND ${LCOV} --summary coverage.info | grep "lines" | awk '{print $$2}' | sed 's/%//' | awk '{if ($$1 < 90) exit 1}'
            DEPENDS coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Checking coverage threshold (>= 90%)..."
        )
    else()
        message(WARNING "lcov or genhtml not found, coverage target disabled")
    endif()
endif()

# =============================================================================
# Installation (for libraries)
# =============================================================================
install(TARGETS aes3_part1_audio_coding aes3_part3_transport
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${AES3_INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# =============================================================================
# Summary
# =============================================================================
message(STATUS "")
message(STATUS "AES3-2009 Build Configuration:")
message(STATUS "  Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "  C Standard:        ${CMAKE_C_STANDARD}")
message(STATUS "  Testing:           ${ENABLE_TESTING}")
message(STATUS "  Coverage:          ${ENABLE_COVERAGE}")
message(STATUS "  Sanitizers:        ${ENABLE_SANITIZERS}")
message(STATUS "  Benchmarks:        ${ENABLE_BENCHMARKS}")
message(STATUS "")
