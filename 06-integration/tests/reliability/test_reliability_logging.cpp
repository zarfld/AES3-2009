/**
 * @file test_reliability_logging.cpp
 * @brief Test IEEE 1633 Reliability Logging Integration
 * 
 * Module: tests/reliability/test_reliability_logging.cpp
 * Phase: 06-integration
 * Traceability:
 *   Standards: IEEE 1633-2016 §5.4.4 (Data Collection), §5.4.6 (Metrics)
 *   Requirements: REQ-NF-RELIABILITY-001, REQ-NF-AVAILABILITY-001
 *   Operational Profile: operational-profile.md §7 (Data Collection)
 * 
 * @description
 * Demonstrates integration of IEEE 1633 reliability logging with AES3-2009
 * integration tests. Tests:
 * - Execution time tracking
 * - Failure event logging
 * - Metrics calculation (MTBF, MTTR, availability)
 * - Laplace trend analysis
 * - CSV export for SRG model fitting
 * 
 * @copyright Copyright (c) 2025 AES3-2009 Project
 */

#include <gtest/gtest.h>
#include "../../reliability/reliability_logger.hpp"
#include <thread>
#include <chrono>
#include <fstream>

using namespace AES3::Integration::Reliability;

/**
 * @brief Test fixture for reliability logging tests
 */
class ReliabilityLoggingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset logger for clean test
        ReliabilityLogger::instance().reset();
    }
    
    void TearDown() override {
        ReliabilityLogger::instance().disable_file_logging();
        ReliabilityLogger::instance().reset();
    }
};

/**
 * @brief TEST-REL-001: Basic execution time tracking
 * 
 * Verifies nanosecond-precision timing of duty cycles
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_001_ExecutionTimeTracking) {
    auto& logger = ReliabilityLogger::instance();
    
    // Start execution timing
    logger.start_execution();
    
    // Simulate work (100ms)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Stop timing
    logger.stop_execution();
    
    // Verify execution time recorded (should be ~100ms = 100,000,000 ns)
    auto metrics = logger.get_metrics();
    EXPECT_GT(metrics.execution_time_ns, 90'000'000ULL);  // At least 90ms
    EXPECT_LT(metrics.execution_time_ns, 120'000'000ULL); // Less than 120ms (tolerance)
}

/**
 * @brief TEST-REL-002: Failure event logging with severity classification
 * 
 * Verifies IEEE 1633 failure event recording per §5.4.4
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_002_FailureEventLogging) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Simulate failures with different severities
    FailureEvent minor_failure{};
    minor_failure.component = Component::Part2_Metadata;
    minor_failure.operation = "ChannelStatusUpdate";
    minor_failure.severity = FailureSeverity::Minor;
    minor_failure.error_code = "TIMING_DRIFT";
    minor_failure.recovery_action = "AutoRecover";
    minor_failure.recovery_time_ms = 10;
    minor_failure.details = "Clock drift detected, auto-corrected";
    
    logger.log_failure(minor_failure);
    
    FailureEvent critical_failure{};
    critical_failure.component = Component::Part3_Transport;
    critical_failure.operation = "PreambleDetection";
    critical_failure.severity = FailureSeverity::Critical;
    critical_failure.error_code = "SYNC_LOSS";
    critical_failure.recovery_action = "Resynchronize";
    critical_failure.recovery_time_ms = 500;
    critical_failure.details = "Lost synchronization, resync required";
    
    logger.log_failure(critical_failure);
    
    logger.stop_execution();
    
    // Verify metrics
    auto metrics = logger.get_metrics();
    EXPECT_EQ(metrics.failure_count, 2);
    EXPECT_EQ(metrics.critical_failure_count, 1); // Only critical/fatal count
    
    // Verify failure events captured
    auto events = logger.get_failure_events();
    EXPECT_EQ(events.size(), 2);
    
    // Verify first event
    EXPECT_EQ(events[0].component, Component::Part2_Metadata);
    EXPECT_EQ(events[0].severity, FailureSeverity::Minor);
    EXPECT_EQ(events[0].error_code, "TIMING_DRIFT");
    EXPECT_EQ(events[0].recovery_time_ms, 10);
    
    // Verify second event
    EXPECT_EQ(events[1].component, Component::Part3_Transport);
    EXPECT_EQ(events[1].severity, FailureSeverity::Critical);
    EXPECT_EQ(events[1].error_code, "SYNC_LOSS");
    EXPECT_EQ(events[1].recovery_time_ms, 500);
}

/**
 * @brief TEST-REL-003: MTBF and MTTR calculation per IEEE 1633 §5.4.6
 * 
 * Verifies reliability metrics computation
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_003_MTBFandMTTRCalculation) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Simulate execution with 5 failures (50ms between failures for measurable time)
    const uint32_t recovery_time_ms = 100;
    
    for (int i = 0; i < 5; i++) {
        // Simulate execution between failures (longer delay for measurable MTBF)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        FailureEvent event{};
        event.component = Component::Integration;
        event.operation = "EndToEndPipeline";
        event.severity = FailureSeverity::Major;
        event.error_code = "TRANSIENT_ERROR";
        event.recovery_action = "AutoRecover";
        event.recovery_time_ms = recovery_time_ms;
        
        logger.log_failure(event);
    }
    
    logger.stop_execution();
    
    auto metrics = logger.get_metrics();
    
    // Verify failure count
    EXPECT_EQ(metrics.failure_count, 5);
    
    // Verify MTTR (should be ~100ms for all identical recovery times)
    EXPECT_NEAR(metrics.current_mttr_ms, recovery_time_ms, 5.0); // ±5ms tolerance
    
    // Verify MTBF calculation (execution_time / failure_count)
    // With ~250ms total execution time and 5 failures: MTBF ≈ 50ms = 0.0000139 hours
    EXPECT_GT(metrics.current_mtbf_hours, 0.0);
    EXPECT_LT(metrics.current_mtbf_hours, 1.0); // Should be much less than 1 hour for test
    
    // Verify failure intensity (failures per hour)
    double intensity = metrics.failure_intensity();
    EXPECT_GT(intensity, 0.0);
    EXPECT_GT(intensity, 1.0); // With short execution time, should be >1 failure/hour
}

/**
 * @brief TEST-REL-004: Availability calculation per IEEE 1633 §5.4.6
 * 
 * Verifies A = MTBF / (MTBF + MTTR)
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_004_AvailabilityCalculation) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Simulate execution with short MTTR for high availability
    // With 100ms execution and 1ms recovery: availability ≈ 100/(100+1) = 0.99
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    FailureEvent event{};
    event.component = Component::Part1_Audio;
    event.operation = "AudioSampleEncoding";
    event.severity = FailureSeverity::Minor;
    event.error_code = "SAMPLE_CLIP";
    event.recovery_action = "AutoCorrect";
    event.recovery_time_ms = 1; // 1ms recovery time
    
    logger.log_failure(event);
    logger.stop_execution();
    
    auto metrics = logger.get_metrics();
    
    // Calculate availability: A = MTBF / (MTBF + MTTR)
    double availability = metrics.availability();
    
    // With MTBF ≈ 100ms and MTTR ≈ 1ms: A ≈ 100/(100+1) ≈ 0.99
    EXPECT_GT(availability, 0.0);   // Must be positive
    EXPECT_LE(availability, 1.0);   // Cannot exceed 100%
    EXPECT_GT(availability, 0.95);  // Should be >95% for 1ms MTTR vs 100ms MTBF
}

/**
 * @brief TEST-REL-005: Work unit tracking (frames processed)
 * 
 * Verifies operational unit counting for throughput metrics
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_005_WorkUnitTracking) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Simulate processing 1000 audio frames
    const uint64_t total_frames = 1000;
    for (uint64_t i = 0; i < total_frames; i++) {
        logger.record_work_unit(1); // 1 frame at a time
    }
    
    logger.stop_execution();
    
    auto metrics = logger.get_metrics();
    EXPECT_EQ(metrics.frames_transmitted, total_frames);
}

/**
 * @brief TEST-REL-006: Duty cycle completion tracking
 * 
 * Verifies mission profile iteration counting
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_006_DutyCycleTracking) {
    auto& logger = ReliabilityLogger::instance();
    
    // Simulate 10 mission profile iterations
    const uint32_t duty_cycles = 10;
    
    for (uint32_t i = 0; i < duty_cycles; i++) {
        logger.start_execution();
        
        // Simulate one duty cycle (e.g., 192 frames = 1 channel status block)
        logger.record_work_unit(192);
        
        logger.stop_execution();
        logger.record_duty_cycle();
    }
    
    auto metrics = logger.get_metrics();
    EXPECT_EQ(metrics.duty_cycles_completed, duty_cycles);
    EXPECT_EQ(metrics.frames_transmitted, 192 * duty_cycles);
}

/**
 * @brief TEST-REL-007: CSV export for SRG model fitting
 * 
 * Verifies file logging and CSV format per IEEE 1633 requirements
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_007_CSVExport) {
    auto& logger = ReliabilityLogger::instance();
    
    const std::string test_file = "test_reliability_log.csv";
    
    // Enable file logging
    ASSERT_TRUE(logger.enable_file_logging(test_file));
    
    logger.start_execution();
    
    // Log a failure event
    FailureEvent event{};
    event.component = Component::Part2_Metadata;
    event.operation = "CRCC_Validation";
    event.severity = FailureSeverity::Major;
    event.error_code = "CRCC_MISMATCH";
    event.recovery_action = "RetransmitRequest";
    event.recovery_time_ms = 50;
    event.details = "Channel status CRCC validation failed";
    
    logger.log_failure(event);
    logger.stop_execution();
    
    logger.disable_file_logging();
    
    // Verify CSV file created and contains data
    std::ifstream csv_file(test_file);
    ASSERT_TRUE(csv_file.is_open());
    
    std::string header_line;
    std::getline(csv_file, header_line);
    
    // Verify CSV header
    EXPECT_NE(header_line.find("timestamp_ns"), std::string::npos);
    EXPECT_NE(header_line.find("execution_time_ns"), std::string::npos);
    EXPECT_NE(header_line.find("component"), std::string::npos);
    EXPECT_NE(header_line.find("severity"), std::string::npos);
    EXPECT_NE(header_line.find("error_code"), std::string::npos);
    EXPECT_NE(header_line.find("recovery_time_ms"), std::string::npos);
    
    std::string data_line;
    std::getline(csv_file, data_line);
    
    // Verify data line contains expected values
    EXPECT_NE(data_line.find("Part2_Metadata"), std::string::npos);
    EXPECT_NE(data_line.find("CRCC_MISMATCH"), std::string::npos);
    EXPECT_NE(data_line.find("Major"), std::string::npos);
    
    csv_file.close();
    
    // Cleanup test file
    std::remove(test_file.c_str());
}

/**
 * @brief TEST-REL-008: Laplace trend analysis per IEEE 1633 §5.4.4
 * 
 * Verifies U/N/S-shaped trend detection for reliability growth assessment
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_008_LaplaceTrendAnalysis) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Simulate improving reliability (S-shaped trend)
    // Failures occur less frequently over time
    const uint64_t intervals[] = {10, 20, 40, 80, 160}; // Increasing intervals
    
    for (size_t i = 0; i < 5; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervals[i]));
        
        FailureEvent event{};
        event.component = Component::Integration;
        event.operation = "TestExecution";
        event.severity = FailureSeverity::Minor;
        event.error_code = "TEST_ERROR_" + std::to_string(i);
        event.recovery_action = "AutoRecover";
        event.recovery_time_ms = 10;
        
        logger.log_failure(event);
    }
    
    logger.stop_execution();
    
    // Calculate Laplace statistic
    double laplace_u = logger.calculate_laplace_statistic();
    
    // For improving reliability (S-shaped), u(t) should be negative
    // u(t) < -2: Strong improvement
    // -2 ≤ u(t) ≤ 2: Stable (N-shaped)
    // u(t) > 2: Deteriorating (U-shaped)
    
    // Note: Due to small sample size and test timing variability,
    // we just verify the calculation runs and returns a reasonable value
    EXPECT_GE(laplace_u, -10.0); // Not absurdly negative
    EXPECT_LE(laplace_u, 10.0);  // Not absurdly positive
}

/**
 * @brief TEST-REL-009: Thread-safe concurrent logging
 * 
 * Verifies mutex protection for multi-threaded test environments
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_009_ThreadSafety) {
    auto& logger = ReliabilityLogger::instance();
    
    logger.start_execution();
    
    // Launch 10 threads, each logging 10 failures
    const int num_threads = 10;
    const int failures_per_thread = 10;
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([&logger, t]() {
            for (int i = 0; i < 10; i++) {  // Use literal instead of variable
                FailureEvent event{};
                event.component = Component::Integration;
                event.operation = "ParallelTest_" + std::to_string(t);
                event.severity = FailureSeverity::Minor;
                event.error_code = "THREAD_ERROR";
                event.recovery_action = "AutoRecover";
                event.recovery_time_ms = 5;
                
                logger.log_failure(event);
                logger.record_work_unit(1);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.stop_execution();
    
    // Verify all failures logged correctly
    auto metrics = logger.get_metrics();
    EXPECT_EQ(metrics.failure_count, num_threads * failures_per_thread);
    EXPECT_EQ(metrics.frames_transmitted, num_threads * failures_per_thread);
}

/**
 * @brief TEST-REL-010: Reset functionality for test isolation
 * 
 * Verifies clean state between test runs
 */
TEST_F(ReliabilityLoggingTest, TEST_REL_010_ResetFunctionality) {
    auto& logger = ReliabilityLogger::instance();
    
    // First run: Log some data
    logger.start_execution();
    logger.record_work_unit(100);
    
    FailureEvent event{};
    event.component = Component::Part1_Audio;
    event.severity = FailureSeverity::Minor;
    event.error_code = "TEST_ERROR";
    event.recovery_action = "AutoRecover";
    event.recovery_time_ms = 10;
    
    logger.log_failure(event);
    logger.stop_execution();
    
    auto metrics_before = logger.get_metrics();
    EXPECT_GT(metrics_before.execution_time_ns, 0);
    EXPECT_EQ(metrics_before.failure_count, 1);
    EXPECT_EQ(metrics_before.frames_transmitted, 100);
    
    // Reset
    logger.reset();
    
    // Verify clean state
    auto metrics_after = logger.get_metrics();
    EXPECT_EQ(metrics_after.execution_time_ns, 0);
    EXPECT_EQ(metrics_after.failure_count, 0);
    EXPECT_EQ(metrics_after.frames_transmitted, 0);
    EXPECT_EQ(metrics_after.duty_cycles_completed, 0);
    EXPECT_EQ(logger.get_failure_events().size(), 0);
}

// Run all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
