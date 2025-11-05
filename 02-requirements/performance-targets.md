---
specId: REQ-PERF-TARGETS
specVersion: "1.0"
specType: performance-specification
specStatus: draft
specCategory: performance
dateCreated: 2025-11-05
dateModified: 2025-11-05
author: System Architect
reviewer: Performance Team
approver: TBD
applicableStandards:
  - id: ISO-29148
    version: "2018"
    title: "Systems and software engineering — Life cycle processes — Requirements engineering"
    section: "6.4.3.7"
  - id: AES3-2009
    version: "2009"
    title: "AES3-2009 Digital Audio Interface (Parts 1-4)"
  - id: AES5-2018
    version: "2018"
    title: "AES5-2018 Preferred Sampling Frequencies"
relatedSpecs:
  - id: STR-PERF-001
    type: stakeholder-requirement
    relationship: satisfies
    description: "Deterministic latency, jitter < 0.025 UI, up to 192 kHz"
  - id: STR-PERF-002
    type: stakeholder-requirement
    relationship: satisfies
    description: "Jitter tolerance according to AES3-2009 Part 4"
  - id: STR-PERF-003
    type: stakeholder-requirement
    relationship: satisfies
    description: "Real-time operation with bounded latency"
  - id: REQ-PERF-AUDIO-001
    type: functional-requirement
    relationship: refines
    description: "Part 1 PCM encoding performance"
  - id: REQ-PERF-TRANS-001
    type: functional-requirement
    relationship: refines
    description: "Part 3 biphase-mark encoding performance"
  - id: REQ-PERF-TRANS-002
    type: functional-requirement
    relationship: refines
    description: "Part 3 biphase-mark decoding performance"
  - id: REQ-PERF-TRANS-003
    type: functional-requirement
    relationship: refines
    description: "Part 3 preamble detection performance"
  - id: REQ-PERF-HAL-001
    type: functional-requirement
    relationship: refines
    description: "Part 4 transmitter jitter limits"
  - id: REQ-PERF-HAL-002
    type: functional-requirement
    relationship: refines
    description: "Part 4 receiver jitter tolerance"
  - id: REQ-PERF-HAL-003
    type: functional-requirement
    relationship: refines
    description: "Part 4 UI timing accuracy"
changeLog:
  - version: "1.0"
    date: 2025-11-05
    author: System Architect
    changes: "Initial performance targets specification - addresses performance gap identified in completeness audit (7.5/10 score)"
---

# Performance Targets Specification - AES3-2009 Digital Audio Interface

## 1. Introduction

### 1.1 Purpose

This document specifies comprehensive performance targets for the AES3-2009 Digital Audio Interface implementation, addressing the critical performance gap identified during Phase 02 completeness audit (7.5/10 score, with missing percentile targets, load conditions, and resource limits).

### 1.2 Scope

This specification defines:
- **Latency targets** - Percentile-based latency targets (50th, 95th, 99th, 99.9th) for all operations
- **Throughput requirements** - Sustained and peak throughput for multi-channel operation
- **Load conditions** - Normal, peak, and stress load scenarios
- **Resource limits** - CPU utilization, memory consumption, buffer sizes
- **Jitter specifications** - Time-domain jitter limits from AES3-2009 Part 4
- **Timing accuracy** - Unit Interval (UI) timing precision requirements
- **Worst-Case Execution Time (WCET)** - Real-time deadline guarantees

### 1.3 Performance Philosophy

**Guiding Principles**:
1. **Real-time first**: All operations meet hard real-time deadlines (no best-effort processing)
2. **Determinism**: Processing time predictable and bounded (no dynamic memory allocation)
3. **Scalability**: Performance scales linearly with sampling frequency (48 kHz → 192 kHz)
4. **Efficiency**: Low resource utilization enables multi-channel operation
5. **Compliance**: Meet or exceed AES3-2009 Part 4 jitter and timing specifications

### 1.4 Target Platform Baselines

Performance targets are specified for the following baseline platforms:

| Platform | CPU | Clock | Cache | RAM | Target Application |
|----------|-----|-------|-------|-----|-------------------|
| **Embedded** | ARM Cortex-M7 | 216 MHz | 16 KB I/D | 512 KB | Embedded audio devices |
| **Desktop** | Intel Core i5-12600K | 3.7 GHz | 20 MB L3 | 16 GB | Professional audio workstations |
| **Server** | AMD EPYC 7763 | 2.45 GHz | 256 MB L3 | 256 GB | Multi-channel audio servers |

**Note**: Embedded platform targets are most stringent (worst-case). Desktop/server platforms must meet or exceed embedded targets.

---

## 2. Latency Targets

### 2.1 Part 1: PCM Audio Encoding Latency

**Metric**: Time from PCM sample input to AES3-encoded subframe output

#### 2.1.1 Percentile Targets by Sampling Frequency

| Sampling Freq | 1 Sample Period | 50th %ile | 95th %ile | 99th %ile | 99.9th %ile | Max (WCET) |
|---------------|-----------------|-----------|-----------|-----------|-------------|------------|
| **32 kHz**    | 31.25 µs        | 15 µs     | 20 µs     | 25 µs     | 28 µs       | 30 µs      |
| **44.1 kHz**  | 22.68 µs        | 11 µs     | 15 µs     | 18 µs     | 20 µs       | 22 µs      |
| **48 kHz**    | 20.83 µs        | 10 µs     | 13 µs     | 16 µs     | 18 µs       | 20 µs      |
| **88.2 kHz**  | 11.34 µs        | 5.5 µs    | 7.5 µs    | 9 µs      | 10 µs       | 11 µs      |
| **96 kHz**    | 10.42 µs        | 5 µs      | 7 µs      | 8 µs      | 9 µs        | 10 µs      |
| **176.4 kHz** | 5.67 µs         | 2.8 µs    | 3.8 µs    | 4.5 µs    | 5 µs        | 5.5 µs     |
| **192 kHz**   | 5.21 µs         | 2.5 µs    | 3.5 µs    | 4 µs      | 4.5 µs      | 5 µs       |

#### 2.1.2 Load Conditions

| Load Condition | Description | Channels | Sampling Freq | Target Percentile |
|----------------|-------------|----------|---------------|-------------------|
| **Normal Load** | Typical studio operation | 2 (stereo) | 48 kHz | 50th %ile = 10 µs |
| **Peak Load** | High sampling rate stereo | 2 (stereo) | 192 kHz | 95th %ile = 3.5 µs |
| **Stress Load** | 8-channel 192 kHz | 8 (multi) | 192 kHz | 99th %ile = 4 µs per channel |

**Acceptance Criteria**:
- ✅ 99th percentile latency < 1 sample period at all sampling frequencies
- ✅ 99.9th percentile latency < 0.9 × sample period (10% margin)
- ✅ WCET documented and verified via static analysis

---

### 2.2 Part 1: PCM Audio Decoding Latency

**Metric**: Time from AES3 subframe input to decoded PCM sample output

#### 2.2.1 Percentile Targets by Sampling Frequency

| Sampling Freq | 1 Sample Period | 50th %ile | 95th %ile | 99th %ile | 99.9th %ile | Max (WCET) |
|---------------|-----------------|-----------|-----------|-----------|-------------|------------|
| **48 kHz**    | 20.83 µs        | 10 µs     | 13 µs     | 16 µs     | 18 µs       | 20 µs      |
| **96 kHz**    | 10.42 µs        | 5 µs      | 7 µs      | 8 µs      | 9 µs        | 10 µs      |
| **192 kHz**   | 5.21 µs         | 2.5 µs    | 3.5 µs    | 4 µs      | 4.5 µs      | 5 µs       |

**Symmetry Requirement**: Decoding latency shall match encoding latency within 10% (same percentile targets).

---

### 2.3 Part 2: Channel Status Processing Latency

**Metric**: Time to process 192-bit channel status block

#### 2.3.1 Channel Status Encoding Latency

| Operation | 50th %ile | 95th %ile | 99th %ile | Max (WCET) |
|-----------|-----------|-----------|-----------|------------|
| **Byte 0-22 encoding** | 5 µs | 8 µs | 10 µs | 12 µs |
| **CRCC calculation** (byte 23) | 2 µs | 3 µs | 4 µs | 5 µs |
| **Total block encoding** | 7 µs | 11 µs | 14 µs | 17 µs |

#### 2.3.2 Channel Status Decoding Latency

| Operation | 50th %ile | 95th %ile | 99th %ile | Max (WCET) |
|-----------|-----------|-----------|-----------|------------|
| **Byte 0-22 decoding** | 5 µs | 8 µs | 10 µs | 12 µs |
| **CRCC validation** (byte 23) | 2 µs | 3 µs | 4 µs | 5 µs |
| **Total block decoding** | 7 µs | 11 µs | 14 µs | 17 µs |

**Block-Level Constraint**: Channel status processing must complete within 192 frames (4 ms @ 48 kHz).

---

### 2.4 Part 3: Biphase-Mark Encoding Latency

**Metric**: Time from audio sample to biphase-mark encoded subframe (32 time slots)

#### 2.4.1 Percentile Targets by Sampling Frequency

| Sampling Freq | 1 Subframe | 50th %ile | 95th %ile | 99th %ile | 99.9th %ile | Max (WCET) |
|---------------|------------|-----------|-----------|-----------|-------------|------------|
| **48 kHz**    | 20.83 µs   | 10 µs     | 13 µs     | 16 µs     | 18 µs       | 20 µs      |
| **96 kHz**    | 10.42 µs   | 5 µs      | 7 µs      | 8 µs      | 9 µs        | 10 µs      |
| **192 kHz**   | 5.21 µs    | 2.5 µs    | 3.5 µs    | 4 µs      | 4.5 µs      | 5 µs       |

**Critical Constraint**: Biphase-mark encoding completes < 1 subframe period (64 UI).

---

### 2.5 Part 3: Biphase-Mark Decoding Latency

**Metric**: Time from biphase-mark input to decoded audio sample

#### 2.5.1 Percentile Targets by Sampling Frequency

| Sampling Freq | 1 Subframe | 50th %ile | 95th %ile | 99th %ile | 99.9th %ile | Max (WCET) |
|---------------|------------|-----------|-----------|-----------|-------------|------------|
| **48 kHz**    | 20.83 µs   | 10 µs     | 13 µs     | 16 µs     | 18 µs       | 20 µs      |
| **96 kHz**    | 10.42 µs   | 5 µs      | 7 µs      | 8 µs      | 9 µs        | 10 µs      |
| **192 kHz**   | 5.21 µs    | 2.5 µs    | 3.5 µs    | 4 µs      | 4.5 µs      | 5 µs       |

---

### 2.6 Part 3: Preamble Detection and Synchronization Latency

**Metric**: Time to achieve synchronization from unknown state

#### 2.6.1 Synchronization Latency Targets

| Sync Level | Target (Typical) | Target (Worst-Case) | 99th %ile | Max |
|------------|------------------|---------------------|-----------|-----|
| **Subframe sync** (preamble detection) | 1 subframe | 3 subframes | 2 subframes | 3 subframes |
| **Frame sync** (X/Y alternation) | 2 frames | 6 subframes | 4 subframes | 6 subframes |
| **Block sync** (Z detection) | 1 block (192 frames) | 2 blocks | 1 block | 2 blocks |
| **Re-sync after signal loss** | 2 subframes | 5 subframes | 3 subframes | 5 subframes |

#### 2.6.2 Synchronization Latency by Sampling Frequency

| Sampling Freq | Subframe Sync (99th %ile) | Frame Sync (99th %ile) | Block Sync (99th %ile) |
|---------------|---------------------------|------------------------|------------------------|
| **48 kHz**    | 41.7 µs (2 subframes)     | 83.3 µs (4 subframes)  | 4 ms (192 frames)      |
| **96 kHz**    | 20.8 µs (2 subframes)     | 41.7 µs (4 subframes)  | 2 ms (192 frames)      |
| **192 kHz**   | 10.4 µs (2 subframes)     | 20.8 µs (4 subframes)  | 1 ms (192 frames)      |

---

### 2.7 Part 4: HAL Function Call Latency

**Metric**: Time from HAL function invocation to return

#### 2.7.1 HAL Function Latency Targets

| HAL Function | 50th %ile | 95th %ile | 99th %ile | Max (WCET) | Notes |
|--------------|-----------|-----------|-----------|------------|-------|
| `aes3_hal_transmit_bit()` | 200 ns | 300 ns | 400 ns | 500 ns | 1 UI @ 48 kHz = 163 ns |
| `aes3_hal_receive_bit()` | 200 ns | 300 ns | 400 ns | 500 ns | Must sample within 1 UI |
| `aes3_hal_get_sample_clock()` | 50 ns | 100 ns | 150 ns | 200 ns | High-precision clock read |
| `aes3_hal_set_sample_timer()` | 1 µs | 2 µs | 3 µs | 5 µs | Timer configuration |
| `aes3_hal_measure_jitter()` | 10 µs | 20 µs | 30 µs | 50 µs | Statistical measurement |
| `aes3_hal_allocate_buffer()` | 5 µs | 10 µs | 20 µs | 50 µs | Memory allocation |
| `aes3_hal_deallocate_buffer()` | 2 µs | 5 µs | 10 µs | 20 µs | Memory deallocation |
| `aes3_hal_dma_setup()` | 10 µs | 20 µs | 30 µs | 50 µs | DMA configuration |

**Critical Constraint**: `transmit_bit()` and `receive_bit()` must complete within 1 UI (163 ns @ 48 kHz, 81 ns @ 96 kHz, 41 ns @ 192 kHz).

---

## 3. Throughput Requirements

### 3.1 Sustained Throughput Targets

**Metric**: Continuous operation without buffer overrun/underrun

| Sampling Freq | Frames/sec | Samples/sec (stereo) | Bits/sec (24-bit) | Sustained Throughput |
|---------------|------------|----------------------|-------------------|---------------------|
| **32 kHz**    | 32,000     | 64,000               | 12.288 Mbps       | 100% for 24 hours   |
| **44.1 kHz**  | 44,100     | 88,200               | 16.934 Mbps       | 100% for 24 hours   |
| **48 kHz**    | 48,000     | 96,000               | 18.432 Mbps       | 100% for 24 hours   |
| **88.2 kHz**  | 88,200     | 176,400              | 33.868 Mbps       | 100% for 24 hours   |
| **96 kHz**    | 96,000     | 192,000              | 36.864 Mbps       | 100% for 24 hours   |
| **176.4 kHz** | 176,400    | 352,800              | 67.738 Mbps       | 100% for 24 hours   |
| **192 kHz**   | 192,000    | 384,000              | 73.728 Mbps       | 100% for 24 hours   |

**Acceptance Criteria**:
- ✅ Zero buffer overruns during 24-hour sustained operation
- ✅ Zero buffer underruns during 24-hour sustained operation
- ✅ Audio dropout rate < 1 event per 10^9 samples

### 3.2 Peak Burst Throughput

**Metric**: Short-term peak throughput (1 second burst)

| Load Condition | Channels | Sampling Freq | Peak Throughput | Duration |
|----------------|----------|---------------|-----------------|----------|
| **Normal burst** | 2 | 48 kHz | 18.432 Mbps | 1 second |
| **High-res burst** | 2 | 192 kHz | 73.728 Mbps | 1 second |
| **Multi-channel burst** | 8 | 96 kHz | 147.456 Mbps | 1 second |
| **Stress burst** | 16 | 192 kHz | 1.180 Gbps | 100 ms |

**Acceptance Criteria**:
- ✅ Peak throughput sustained for specified duration without errors
- ✅ System recovers to normal operation within 100 ms after burst

### 3.3 Multi-Channel Scalability

**Metric**: Throughput scaling with channel count

| Channels | Sampling Freq | Aggregate Throughput | CPU Utilization Target |
|----------|---------------|---------------------|------------------------|
| 2 (stereo) | 48 kHz | 18.432 Mbps | < 10% |
| 4 | 48 kHz | 36.864 Mbps | < 20% |
| 8 | 48 kHz | 73.728 Mbps | < 40% |
| 16 | 48 kHz | 147.456 Mbps | < 70% |
| 2 (stereo) | 192 kHz | 73.728 Mbps | < 20% |
| 8 | 192 kHz | 294.912 Mbps | < 70% |

**Scalability Target**: CPU utilization shall scale linearly with channel count (±10%).

---

## 4. Resource Limits

### 4.1 CPU Utilization Limits

#### 4.1.1 CPU Utilization by Operation (Single Channel)

| Operation | Sampling Freq | Normal Load | Peak Load | Stress Load | Max Limit |
|-----------|---------------|-------------|-----------|-------------|-----------|
| **PCM Encoding** | 48 kHz | 2% | 5% | 8% | 10% |
| **PCM Encoding** | 192 kHz | 4% | 8% | 12% | 15% |
| **Biphase Encoding** | 48 kHz | 3% | 6% | 9% | 12% |
| **Biphase Encoding** | 192 kHz | 6% | 10% | 15% | 18% |
| **Biphase Decoding** | 48 kHz | 3% | 6% | 9% | 12% |
| **Biphase Decoding** | 192 kHz | 6% | 10% | 15% | 18% |
| **Channel Status** | 48 kHz | 1% | 2% | 3% | 5% |
| **Preamble Detection** | 48 kHz | 2% | 4% | 6% | 8% |

#### 4.1.2 Total CPU Utilization Budget

| Platform | Normal Load | Peak Load | Stress Load | Reserved for OS |
|----------|-------------|-----------|-------------|-----------------|
| **Embedded (ARM M7)** | < 30% | < 50% | < 70% | 30% |
| **Desktop (Core i5)** | < 10% | < 20% | < 30% | 70% |
| **Server (EPYC)** | < 5% | < 10% | < 15% | 85% |

**Critical Constraint**: System must leave ≥30% CPU headroom for error handling and OS tasks.

---

### 4.2 Memory Consumption Limits

#### 4.2.1 Static Memory Allocation (Per Channel)

| Component | RAM (bytes) | ROM/Flash (bytes) | Notes |
|-----------|-------------|-------------------|-------|
| **Part 1: PCM encoder** | 512 | 4,096 | Sample buffer + state |
| **Part 1: PCM decoder** | 512 | 4,096 | Sample buffer + state |
| **Part 2: Channel status** | 256 | 2,048 | 192-bit block + CRCC |
| **Part 3: Biphase encoder** | 1,024 | 8,192 | Symbol buffer + lookup |
| **Part 3: Biphase decoder** | 1,024 | 8,192 | Symbol buffer + PLL state |
| **Part 3: Preamble detector** | 512 | 4,096 | Pattern matching state |
| **Part 4: HAL interface** | 2,048 | 8,192 | Platform-specific buffers |
| **Error handling** | 1,024 | 4,096 | Error log + metrics |
| **Total per channel** | **6,912 bytes** | **43,008 bytes** | ~7 KB RAM, ~42 KB ROM |

#### 4.2.2 Dynamic Memory Allocation

| Allocation Type | Max Size | Frequency | Constraint |
|-----------------|----------|-----------|------------|
| **Audio frame buffer** | 4,096 bytes | Per block (192 frames) | Pre-allocated pool |
| **Error log entry** | 128 bytes | Per error | Circular buffer |
| **Temporary workspace** | 1,024 bytes | Per operation | Stack allocation only |

**Critical Constraint**: **No dynamic heap allocation** in real-time audio path (encoding/decoding).

#### 4.2.3 Total Memory Budget

| Platform | Per Channel RAM | 8-Channel RAM | Total RAM Limit |
|----------|-----------------|---------------|-----------------|
| **Embedded (ARM M7)** | 7 KB | 56 KB | 512 KB available |
| **Desktop (Core i5)** | 7 KB | 56 KB | 16 GB available |
| **Server (EPYC)** | 7 KB | 56 KB | 256 GB available |

**Memory Efficiency**: Total RAM usage < 15% of available RAM on embedded platforms.

---

### 4.3 Buffer Size Requirements

#### 4.3.1 Audio Sample Buffers

| Buffer Type | Size (samples) | Size (bytes, 24-bit) | Latency Impact |
|-------------|----------------|----------------------|----------------|
| **Input sample buffer** | 192 | 576 | 4 ms @ 48 kHz |
| **Output sample buffer** | 192 | 576 | 4 ms @ 48 kHz |
| **Biphase bit buffer** | 6,144 (32×192) | 768 | 4 ms @ 48 kHz |
| **DMA ring buffer** | 384 | 1,152 | 8 ms @ 48 kHz |

**Buffer Sizing Rule**: Buffers sized for 1 channel status block (192 frames) minimum.

#### 4.3.2 Error Log Buffers

| Buffer Type | Entries | Entry Size | Total Size |
|-------------|---------|------------|------------|
| **Error log circular buffer** | 256 | 128 bytes | 32 KB |
| **Error metrics** | 1 | 256 bytes | 256 bytes |
| **Rate limiter state** | 28 (error codes) | 32 bytes | 896 bytes |

---

## 5. Jitter Specifications (AES3-2009 Part 4)

### 5.1 Transmitter Intrinsic Jitter Limits

**Metric**: Peak-to-peak jitter measured with 700 Hz high-pass filter

#### 5.1.1 Jitter Limits by Sampling Frequency

| Sampling Freq | 1 UI | Max Jitter (0.025 UI) | 50th %ile | 95th %ile | 99th %ile |
|---------------|------|-----------------------|-----------|-----------|-----------|
| **32 kHz**    | 488 ns | 12.2 ns | 5 ns | 8 ns | 10 ns |
| **44.1 kHz**  | 354 ns | 8.9 ns | 4 ns | 6 ns | 7 ns |
| **48 kHz**    | 326 ns | 8.2 ns | 3 ns | 5 ns | 7 ns |
| **88.2 kHz**  | 177 ns | 4.4 ns | 2 ns | 3 ns | 4 ns |
| **96 kHz**    | 163 ns | 4.1 ns | 1.5 ns | 2.5 ns | 3.5 ns |
| **176.4 kHz** | 88 ns | 2.2 ns | 1 ns | 1.5 ns | 2 ns |
| **192 kHz**   | 81 ns | 2.0 ns | 0.8 ns | 1.2 ns | 1.8 ns |

**AES3-2009 Compliance**: 99.9th percentile jitter < 0.025 UI (peak-to-peak).

#### 5.1.2 Jitter Measurement Method

| Parameter | Value | AES3 Reference |
|-----------|-------|----------------|
| **Measurement filter** | 700 Hz high-pass (3 dB) | Part 4 §5.1.2 |
| **Filter roll-off** | First-order to 70 Hz | Part 4 Figure 1 |
| **Measurement duration** | 1 second minimum | Part 4 §5.1.2 |
| **Sampling points** | All zero crossings | Part 4 §5.1.1 |

### 5.2 Receiver Jitter Tolerance

**Metric**: Maximum input jitter before bit errors occur

#### 5.2.1 Jitter Tolerance Mask (AES3-2009 Part 4 §5.2)

| Jitter Frequency | Tolerance | AES3 Reference |
|------------------|-----------|----------------|
| **< 200 Hz** | ≥ 10 UI | Part 4 Figure 3 |
| **200 Hz - 8 kHz** | ≥ (2000 Hz / f) UI | Part 4 Figure 3 (linear slope) |
| **≥ 8 kHz** | ≥ 0.25 UI | Part 4 Figure 3 |

#### 5.2.2 Jitter Tolerance by Sampling Frequency

| Sampling Freq | Low-Freq Tolerance (< 200 Hz) | High-Freq Tolerance (> 8 kHz) |
|---------------|-------------------------------|-------------------------------|
| **48 kHz**    | ≥ 10 UI (3.26 µs) | ≥ 0.25 UI (81.5 ns) |
| **96 kHz**    | ≥ 10 UI (1.63 µs) | ≥ 0.25 UI (40.8 ns) |
| **192 kHz**   | ≥ 10 UI (0.81 µs) | ≥ 0.25 UI (20.3 ns) |

**Acceptance Criteria**:
- ✅ Receiver operates error-free with jitter within tolerance mask
- ✅ Bit error rate < 10^-9 within tolerance mask
- ✅ Clock recovery remains locked within tolerance mask

---

## 6. Timing Accuracy

### 6.1 Unit Interval (UI) Timing Accuracy

**Metric**: Deviation from ideal UI duration

#### 6.1.1 UI Timing Accuracy by Sampling Frequency

| Sampling Freq | Ideal UI | Max Deviation (±10 ppm) | 50th %ile | 95th %ile | 99th %ile |
|---------------|----------|-------------------------|-----------|-----------|-----------|
| **48 kHz**    | 326 ns | ±3.26 ns | ±1 ns | ±2 ns | ±3 ns |
| **96 kHz**    | 163 ns | ±1.63 ns | ±0.5 ns | ±1 ns | ±1.5 ns |
| **192 kHz**   | 81 ns | ±0.81 ns | ±0.3 ns | ±0.5 ns | ±0.7 ns |

**Acceptance Criteria**:
- ✅ UI timing accuracy ≤ ±10 ppm over 1 second
- ✅ UI timing stability over temperature (-40°C to +85°C) ≤ ±20 ppm

### 6.2 Frame Rate Accuracy

**Metric**: Frame transmission rate vs. sampling frequency

#### 6.2.1 Frame Rate Tolerance

| Sampling Freq | Ideal Frame Rate | Max Deviation | Target Accuracy |
|---------------|------------------|---------------|-----------------|
| **48 kHz**    | 48,000 Hz | ±4.8 Hz (±100 ppm) | ±0.5 Hz (±10 ppm) |
| **96 kHz**    | 96,000 Hz | ±9.6 Hz (±100 ppm) | ±1.0 Hz (±10 ppm) |
| **192 kHz**   | 192,000 Hz | ±19.2 Hz (±100 ppm) | ±2.0 Hz (±10 ppm) |

**Acceptance Criteria**:
- ✅ Frame rate deviation < ±10 ppm over 1 hour
- ✅ Frame rate jitter < 0.025 UI peak-to-peak

### 6.3 Sample Clock Stability

**Metric**: Sample clock frequency stability over time

#### 6.3.1 Short-Term Stability (1 second)

| Sampling Freq | Target Stability | Max Deviation |
|---------------|------------------|---------------|
| **48 kHz**    | ±1 ppm | ±10 ppm |
| **96 kHz**    | ±1 ppm | ±10 ppm |
| **192 kHz**   | ±1 ppm | ±10 ppm |

#### 6.3.2 Long-Term Stability (24 hours)

| Sampling Freq | Target Stability | Max Deviation |
|---------------|------------------|---------------|
| **48 kHz**    | ±5 ppm | ±20 ppm |
| **96 kHz**    | ±5 ppm | ±20 ppm |
| **192 kHz**   | ±5 ppm | ±20 ppm |

---

## 7. Worst-Case Execution Time (WCET)

### 7.1 WCET Analysis Targets

**Metric**: Maximum execution time under worst-case conditions

#### 7.1.1 Critical Path WCET

| Function | 48 kHz WCET | 96 kHz WCET | 192 kHz WCET | Deadline |
|----------|-------------|-------------|--------------|----------|
| `aes3_encode_pcm_sample()` | 20 µs | 10 µs | 5 µs | 1 sample period |
| `aes3_decode_pcm_sample()` | 20 µs | 10 µs | 5 µs | 1 sample period |
| `aes3_encode_biphase_mark()` | 20 µs | 10 µs | 5 µs | 1 subframe period |
| `aes3_decode_biphase_mark()` | 20 µs | 10 µs | 5 µs | 1 subframe period |
| `aes3_detect_preamble()` | 5 µs | 3 µs | 2 µs | 1 subframe period |
| `aes3_validate_crcc()` | 5 µs | 5 µs | 5 µs | 1 block period |

**WCET Verification Methods**:
1. **Static analysis**: LLVM-based WCET analyzer
2. **Measurement-based**: High-precision timer instrumentation
3. **Hybrid**: Combine static + measurement for tightest bounds

#### 7.1.2 WCET Safety Margin

| Priority Level | Safety Margin | Rationale |
|----------------|---------------|-----------|
| **Critical (P0)** | 20% | Hard real-time deadline |
| **High (P1)** | 30% | Soft real-time deadline |
| **Medium (P2)** | 50% | Best-effort performance |

**Example**: If measured WCET = 16 µs, critical path deadline = 16 µs × 1.2 = 19.2 µs.

---

## 8. Load Condition Definitions

### 8.1 Normal Load Conditions

**Definition**: Typical professional studio operation

| Parameter | Value | Description |
|-----------|-------|-------------|
| **Channels** | 2 (stereo) | Standard stereo audio |
| **Sampling Frequency** | 48 kHz | Professional broadcast standard |
| **Bit Depth** | 24 bits | Professional audio resolution |
| **Duration** | Continuous | 24-hour operation |
| **Environmental** | 20°C ± 5°C | Studio environment |
| **Error Rate** | < 1 event/10^9 samples | Typical studio quality |

### 8.2 Peak Load Conditions

**Definition**: High-performance professional operation

| Parameter | Value | Description |
|-----------|-------|-------------|
| **Channels** | 2-8 | Stereo to multi-channel |
| **Sampling Frequency** | 192 kHz | High-resolution audio |
| **Bit Depth** | 24 bits | Professional audio resolution |
| **Duration** | 1 hour sustained | Mixing/mastering session |
| **Environmental** | 20°C ± 10°C | Variable studio conditions |
| **Error Rate** | < 1 event/10^8 samples | High-quality requirements |

### 8.3 Stress Load Conditions

**Definition**: Maximum system capability test

| Parameter | Value | Description |
|-----------|-------|-------------|
| **Channels** | 16 | Maximum multi-channel |
| **Sampling Frequency** | 192 kHz | Highest sampling rate |
| **Bit Depth** | 24 bits | Maximum resolution |
| **Duration** | 10 minutes burst | Stress test duration |
| **Environmental** | -40°C to +85°C | Full operating range |
| **Error Rate** | < 1 event/10^7 samples | Acceptable under stress |
| **Additional Stress** | 50% CPU background load | Concurrent OS tasks |

---

## 9. Performance Testing Strategy

### 9.1 Latency Measurement

#### 9.1.1 Test Setup

| Component | Specification |
|-----------|---------------|
| **Timer** | High-precision hardware timer (1 ns resolution) |
| **Instrumentation** | Non-intrusive hardware probes (minimal overhead) |
| **Sample Size** | 10,000 measurements per test |
| **Test Duration** | 1 hour per configuration |

#### 9.1.2 Percentile Calculation

```
50th percentile = median latency
95th percentile = 95% of samples ≤ this value
99th percentile = 99% of samples ≤ this value
99.9th percentile = 99.9% of samples ≤ this value
Max = worst-case observed latency
```

### 9.2 Throughput Measurement

#### 9.2.1 Sustained Throughput Test

| Parameter | Value |
|-----------|-------|
| **Duration** | 24 hours |
| **Metrics** | Frames/sec, buffer overruns, buffer underruns |
| **Acceptance** | Zero overruns/underruns |

#### 9.2.2 Burst Throughput Test

| Parameter | Value |
|-----------|-------|
| **Duration** | 1 second burst |
| **Metrics** | Peak frames/sec, recovery time |
| **Acceptance** | Peak rate sustained, < 100 ms recovery |

### 9.3 Resource Utilization Measurement

#### 9.3.1 CPU Profiling

| Tool | Platform | Metrics |
|------|----------|---------|
| **perf** | Linux | CPU cycles, cache misses |
| **Instruments** | macOS | Time profiler, allocations |
| **ARM Keil** | Embedded | Execution time, stack usage |

#### 9.3.2 Memory Profiling

| Tool | Platform | Metrics |
|------|----------|---------|
| **Valgrind** | Linux | Heap usage, leaks |
| **Heaptrack** | Linux | Memory timeline |
| **Embedded Profiler** | Embedded | RAM usage, stack depth |

### 9.4 Jitter Measurement

#### 9.4.1 Test Equipment

| Equipment | Specification | Purpose |
|-----------|---------------|---------|
| **Audio Precision APx555** | Jitter analyzer | AES3 intrinsic jitter measurement |
| **Tektronix MSO64** | 1 GHz oscilloscope | UI timing accuracy |
| **Rhode & Schwarz UPV** | Audio analyzer | Long-term jitter stability |

#### 9.4.2 Jitter Test Procedure

1. Configure transmitter at target sampling frequency
2. Apply 700 Hz high-pass filter (AES3-2009 Part 4 Figure 1)
3. Measure zero-crossing jitter over 1 second
4. Verify peak-to-peak jitter < 0.025 UI
5. Repeat at all sampling frequencies (32 kHz - 192 kHz)

---

## 10. Compliance Checklist

### 10.1 AES3-2009 Compliance

- [ ] **Part 1**: PCM encoding/decoding latency < 1 sample period (99th %ile)
- [ ] **Part 2**: Channel status processing < 1 block period (4 ms @ 48 kHz)
- [ ] **Part 3**: Biphase-mark encoding/decoding < 1 subframe period (99th %ile)
- [ ] **Part 3**: Preamble detection within 3 subframes (99% cases)
- [ ] **Part 4**: Transmitter intrinsic jitter < 0.025 UI (AES3-2009 §5.1.2)
- [ ] **Part 4**: Receiver jitter tolerance meets AES3-2009 mask (§5.2)
- [ ] **Part 4**: UI timing accuracy ±10 ppm

### 10.2 AES5-2018 Compliance (Sampling Frequencies)

- [ ] Support all AES5-2018 base frequencies: 32, 44.1, 48 kHz
- [ ] Support 2× multiples: 88.2, 96 kHz
- [ ] Support 4× multiples: 176.4, 192 kHz
- [ ] Performance scales linearly with sampling frequency

### 10.3 ISO 29148:2018 Compliance (Performance Requirements)

- [ ] Performance requirements are measurable (quantitative targets)
- [ ] Performance requirements are testable (test strategy defined)
- [ ] Performance requirements are achievable (WCET analysis)
- [ ] Performance requirements are traceable (stakeholder requirements)

### 10.4 Real-Time System Compliance

- [ ] All real-time operations complete within deadlines
- [ ] WCET documented and verified for critical paths
- [ ] No dynamic memory allocation in audio path
- [ ] CPU utilization < 70% under stress load
- [ ] Memory consumption < 15% of available RAM (embedded)

---

## 11. References

### 11.1 Normative References

- **[AES3-1-2009]** AES3-1-2009 (R2014) Digital Audio Interface Part 1: Audio Content
- **[AES3-2-2009]** AES3-2-2009 (R2014) Digital Audio Interface Part 2: Metadata and Subcode
- **[AES3-3-2009]** AES3-3-2009 (R2014) Digital Audio Interface Part 3: Transport
- **[AES3-4-2009]** AES3-4-2009 (R2014) Digital Audio Interface Part 4: Physical and Electrical
- **[AES5-2018]** AES5-2018 Preferred Sampling Frequencies for Professional Digital Audio
- **[ISO-29148]** ISO/IEC/IEEE 29148:2018 Systems and Software Engineering — Requirements Engineering

### 11.2 Related Specifications

- **[STR-PERF-001]** Stakeholder requirement: Deterministic latency, jitter < 0.025 UI
- **[STR-PERF-002]** Stakeholder requirement: Jitter tolerance per AES3-2009 Part 4
- **[STR-PERF-003]** Stakeholder requirement: Real-time operation with bounded latency
- **[REQ-PERF-AUDIO-001]** Functional requirement: Part 1 PCM encoding performance
- **[REQ-PERF-TRANS-001]** Functional requirement: Part 3 biphase-mark encoding performance
- **[REQ-PERF-TRANS-002]** Functional requirement: Part 3 biphase-mark decoding performance
- **[REQ-PERF-TRANS-003]** Functional requirement: Part 3 preamble detection performance
- **[REQ-PERF-HAL-001]** Functional requirement: Part 4 transmitter jitter limits
- **[REQ-PERF-HAL-002]** Functional requirement: Part 4 receiver jitter tolerance
- **[REQ-PERF-HAL-003]** Functional requirement: Part 4 UI timing accuracy

### 11.3 Informative References

- **[WCET]** Wilhelm, R. et al. "The Worst-Case Execution-Time Problem — Overview of Methods and Survey of Tools" ACM TECS 2008
- **[RT-AUDIO]** Wenger, E. et al. "Real-Time Audio Signal Processing" IEEE 2019
- **[AES-JITTER]** Dunn, J. "Jitter Theory, Analysis and Measurement in Digital Audio" AES 1999

---

## 12. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-11-05 | System Architect | Initial performance targets specification addressing completeness audit gap (7.5/10) |

---

**Document Status**: Draft  
**Review Date**: 2025-12-09 (Week 6 end)  
**Approval Target**: 2025-12-16 (Week 7 stakeholder review)
