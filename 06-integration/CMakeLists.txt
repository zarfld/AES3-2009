cmake_minimum_required(VERSION 3.20)

project(AES3_2009_Integration
    VERSION 1.0.0
    DESCRIPTION "AES3-2009 Integration Tests - Phase 06"
    LANGUAGES CXX C
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include Phase 05 implementation
set(IMPLEMENTATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../05-implementation")
set(AES3_INCLUDE_DIR "${IMPLEMENTATION_DIR}/include")
set(AES3_SOURCE_DIR "${IMPLEMENTATION_DIR}/src")

# Add Phase 05 implementation libraries (only if not already added by root CMakeLists.txt)
if(NOT TARGET aes3_part1_audio_coding)
    message(STATUS "Adding Phase 05 implementation from Phase 06")
    add_subdirectory(${IMPLEMENTATION_DIR} ${CMAKE_CURRENT_BINARY_DIR}/implementation)
else()
    message(STATUS "Phase 05 implementation already available (added by root)")
endif()

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

# Integration Tests
add_executable(test_aes3_end_to_end
    tests/integration/test_aes3_end_to_end.cpp
)

target_include_directories(test_aes3_end_to_end
    PRIVATE 
        ${AES3_SOURCE_DIR}
        ${AES3_INCLUDE_DIR}
)

target_link_libraries(test_aes3_end_to_end
    PRIVATE
        aes3_part1_audio_coding
        aes3_part2_metadata
        aes3_part3_transport
        GTest::gtest_main
)

if(USE_GTEST_DISCOVER_TESTS)
    gtest_discover_tests(test_aes3_end_to_end
        PROPERTIES
            LABELS "integration;e2e;aes3;phase06"
            TIMEOUT 30
    )
else()
    add_test(NAME test_aes3_end_to_end COMMAND test_aes3_end_to_end)
    set_tests_properties(test_aes3_end_to_end PROPERTIES 
        LABELS "integration;e2e;aes3;phase06"
        TIMEOUT 30
    )
endif()

# Coverage target (optional)
if(CMAKE_BUILD_TYPE MATCHES Coverage)
    include(CodeCoverage)
    setup_target_for_coverage_gcovr_html(
        NAME integration_coverage
        EXECUTABLE test_aes3_end_to_end
        DEPENDENCIES test_aes3_end_to_end
    )
endif()

# Summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "AES3-2009 Integration Tests (Phase 06)")
message(STATUS "========================================")
message(STATUS "Integration test target: test_aes3_end_to_end")
message(STATUS "Implementation path: ${IMPLEMENTATION_DIR}")
message(STATUS "Test count: 6 end-to-end scenarios")
message(STATUS "========================================")
message(STATUS "")
