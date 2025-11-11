# AES3-2009 Digital Audio Interface - Root Project
# Multi-Phase Standards-Compliant Software Development
# Standards: ISO/IEC/IEEE 12207:2017, AES3-2009 Parts 1-4

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
option(BUILD_TESTING "Enable testing" ON)
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
    
    # C++17 features allowed (from ADR-002) - Apply only to C++ files
    add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>      # No exceptions in real-time code
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>            # No RTTI
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
# Dependencies
# =============================================================================
if(ENABLE_TESTING OR BUILD_TESTING)
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
        
        # Disable strict warnings for GoogleTest/GoogleMock (external dependency)
        if(NOT MSVC)
            target_compile_options(gtest PRIVATE -Wno-conversion -Wno-sign-conversion)
            target_compile_options(gtest_main PRIVATE -Wno-conversion -Wno-sign-conversion)
            target_compile_options(gmock PRIVATE -Wno-conversion -Wno-sign-conversion)
            target_compile_options(gmock_main PRIVATE -Wno-conversion -Wno-sign-conversion)
        endif()
    endif()
    enable_testing()
    include(GoogleTest)
endif()

# =============================================================================
# Phase 05: Implementation (Core Libraries and Conformity Tests)
# =============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Phase 05: Implementation")
message(STATUS "========================================")

# Add Phase 05 implementation as subdirectory
add_subdirectory(05-implementation)

# =============================================================================
# Phase 06: Integration Tests (End-to-End Pipeline Verification)
# =============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Phase 06: Integration")
message(STATUS "========================================")

# Check if Phase 06 exists before adding
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/06-integration/CMakeLists.txt")
    add_subdirectory(06-integration)
    message(STATUS "Phase 06 integration tests enabled")
else()
    message(STATUS "Phase 06 integration tests not yet available")
endif()

# =============================================================================
# Installation (for libraries)
# =============================================================================
# Installation targets are defined in subdirectory CMakeLists

# =============================================================================
# Summary
# =============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "AES3-2009 Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "  Project Version:   ${PROJECT_VERSION}")
message(STATUS "  Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "  C Standard:        ${CMAKE_C_STANDARD}")
message(STATUS "  Testing:           ${ENABLE_TESTING}")
message(STATUS "  Coverage:          ${ENABLE_COVERAGE}")
message(STATUS "  Sanitizers:        ${ENABLE_SANITIZERS}")
message(STATUS "  Benchmarks:        ${ENABLE_BENCHMARKS}")
message(STATUS "")
message(STATUS "Lifecycle Phases:")
message(STATUS "  Phase 05 (Implementation):  ✓ Included")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/06-integration/CMakeLists.txt")
    message(STATUS "  Phase 06 (Integration):     ✓ Included")
else()
    message(STATUS "  Phase 06 (Integration):     ⊗ Not available")
endif()
message(STATUS "========================================")
message(STATUS "")
