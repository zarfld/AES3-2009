## AES3-2009 Hardware/Platform-Agnostic Implementation Brainstorm

### Lane 1: Problems / Pain Points

* Pain: Vendor SDKs tightly coupled to specific audio hardware (Realtek, Cirrus Logic).
* Pain: Biphase-mark coding is complex and error-prone when implemented manually.
* Pain: No comprehensive conformity test suite available for developers.
* Pain: Channel status block structure (192 bits) is confusing and poorly documented.
* Pain: Many AES3 libraries (e.g., thinx-aes-lib, embedded firmware code, and FPGA HDL cores) don’t specify which AES3 revision (1985 / 1992 / 2003 / 2009 / 2019) they implement — causing hidden interoperability failures.
* Pain: Spec evolution (e.g., jitter tolerances, preamble timing, channel status interpretation) makes backporting or forward compatibility unpredictable.
* Pain: Developers often rely on “AES/EBU-like” assumptions instead of referencing the exact normative text.
* Pain: Open-source examples usually merge AES3, IEC 60958, and S/PDIF code paths, creating ambiguous hybrids.
* Pain: Version drift between physical, transport, and metadata layers (Parts 1–4) leads to non-compliance with modern AES3-2009 (R2014) or forthcoming 2019 updates.
* Pain: Test vectors for one revision are reused incorrectly across others, invalidating conformance testing.
* Pain: Most AES3 libraries are platform-locked (Arduino, STM32, Linux ALSA, or FPGA HDL) — no separation between protocol logic and hardware I/O.
* Pain: GPIO and DMA dependencies are hardcoded, blocking reuse across RTOS, Linux, or bare-metal systems.
* Pain: Developers must rewrite low-level drivers for every platform rather than reusing the same AES3 logic.
* Pain: Timing and buffering assumptions differ drastically between MCU timers, Linux sound subsystems, and pro-audio interfaces.
* Pain: No shared abstraction layer or standard “porting interface” for AES3 digital I/O exists.
* Pain: Cross-compilation is painful because most existing projects rely on Arduino build tools or specific HALs.
* Pain: Lack of clean separation between “AES3 core logic” (framing, BMC, metadata) and “transport adapter” (UART, I²S, GPIO, SPI, etc.).

---

### Lane 2: Desired Outcomes

* Outcome: 100% AES3-2009 conformity (all 4 parts).
* Outcome: Developers can integrate AES3 in <1 day instead of 2–4 weeks.
* Outcome: Test coverage >95% with automated conformity validation.
* Outcome: Single codebase works on Windows/Linux/macOS/RTOS without modification.
* Outcome: Each AES3 library clearly declares and enforces the implemented revision (e.g., AES3-2009 R2014) in metadata or build flags.
* Outcome: Provide an AES3-version abstraction layer so that multiple revisions can coexist — selectable at compile-time or runtime.
* Outcome: Create a compatibility matrix documenting cross-revision behavior (timing, jitter, preambles, channel status differences).
* Outcome: Include automated conformance profiles per AES3 revision, allowing unit tests and hardware tests to verify spec-level compliance.
* Outcome: Define a spec fingerprint (hash or checksum of normative text references) embedded in builds to ensure traceability.
* Outcome: Enable auto-validation tools to detect if a binary or firmware image matches the intended AES3 spec revision.
* Outcome: AES3 core library is platform-agnostic, compiling unmodified on Linux, Windows, RTOS, or embedded MCUs.
* Outcome: Hardware abstraction is achieved through modular wrappers (`aes3_hal.c/.h`), similar to HAL layers in LinuxCNC or STM32.
* Outcome: Developers can port AES3 by implementing <10 functions in a hardware-binding wrapper.
* Outcome: Clear layered architecture — core (protocol) → platform (I/O abstraction) → wrapper (integration).
* Outcome: Build once, run anywhere — the same AES3 reference test suite runs on Raspberry Pi, x86 Linux, and microcontrollers.
* Outcome: Continuous integration validates builds for at least three reference targets (e.g., POSIX, Pi 5, STM32).
* Outcome: OS-specific wrappers live in subfolders (`/platforms/linux`, `/platforms/arduino`, `/platforms/rpi`), all calling the same core API.

---

### Lane 3: Stakeholders & Motivations

* Stakeholder: Embedded Audio Engineer – Needs drop-in AES3 library to avoid reimplementing protocol.
* Stakeholder: Test Engineer – Needs automated conformity tests for certification submission.
* Stakeholder: Audio Equipment Manufacturer – Needs standards-compliant implementation to pass AES certification.
* Stakeholder: System Integrator – Needs interoperability guarantees across vendor equipment.
* Stakeholder: Embedded Developer – Wants to reuse the same AES3 code on MCU and Linux without rewriting the protocol.
* Stakeholder: Open-source Maintainer – Seeks a single canonical AES3 implementation portable across toolchains.
* Stakeholder: DIY Maker – Needs plug-and-play AES3 support for Arduino/RPi shields without understanding the full spec.
* Stakeholder: OS Integration Engineer – Wants thin platform wrappers (ALSA, JACK, CoreAudio) that plug into one AES3 core.

---

### Lane 4: Opportunities / Differentiators

* Opportunity: First open-source hardware-agnostic AES3 reference implementation.
* Opportunity: Include conformity test suite (worth $5K–$50K if using Audio Precision).
* Opportunity: Educational resource for understanding AES3 standard.
* Opportunity: Foundation for AES67/Milan implementations (protocol layer reuse).
* Opportunity: First truly hardware-agnostic AES3 reference core with clean HAL interface.
* Opportunity: Serves as teaching and testbed for comparing embedded vs. OS-level implementations.
* Opportunity: Enables cross-platform test harnesses — same conformance suite runs on Raspberry Pi and Linux desktop.
* Opportunity: Acts as bridge layer to AES67/MADI/AVB development — shared transport primitives.
* Opportunity: Could become the de-facto open AES3 compliance test core integrated into OpenAvnu, PoKeys, or pro-audio driver stacks.

---

### Lane 5: Risks / Failure Modes

* Risk: Biphase-mark coding implementation has subtle bugs | Impact: Non-conformity, failed certification | Mitigation: Extensive unit tests, reference test vectors.
* Risk: Real-time performance insufficient for embedded systems | Impact: Adoption blocked for embedded use cases | Mitigation: Zero-copy design, WCET analysis, benchmarking.
* Risk: Platform abstraction too complex/leaky | Impact: Defeats platform-agnostic goal | Mitigation: Minimal interface design, extensive platform testing.
* Risk: Abstraction layer introduces latency or jitter | Impact: Breaks real-time constraints | Mitigation: Design zero-copy, lock-free buffers; benchmark per platform.
* Risk: Hardware-agnostic design drifts into over-complex framework | Impact: Difficult adoption | Mitigation: Keep HAL minimal (<10 functions).
* Risk: Platform wrappers out-of-sync with core API | Impact: Build failures, confusion | Mitigation: CI builds all supported platforms on each commit.
* Risk: Developers misuse generic API for unsupported bit depths or rates | Impact: Spec violations | Mitigation: Compile-time spec checks via macros.

---

### Lane 6: Constraints

* Constraint: Technical – Must compile on C99/C++11 minimum for wide compatibility.
* Constraint: Scope – AES3-2009 only, no AES67/Milan/AES70 in this project.
* Constraint: Architecture – Hardware-agnostic, no vendor-specific code in Standards layer.
* Constraint: Testing – Must validate against AES3-2009 specification requirements.
* Constraint: Build – Should build with GCC, Clang, and MSVC.
* Constraint: Documentation – Must map each code module to corresponding AES3 section (Parts 1–4).

---

### Lane 7: Success Metrics

* Metric: AES3 Conformity Score – (Passed Tests / Total Tests) × 100% – Target: 100% – Horizon: v1.0 release.
* Metric: Test Coverage – (Covered Lines / Total Lines) × 100% – Target: >95% – Horizon: v1.0 release.
* Metric: Integration Time – Time from download to first AES3 stream – Target: <1 day – Horizon: v1.0 release.
* Metric: Jitter Performance – Measured intrinsic jitter – Target: <0.025 UI per Part 4 – Horizon: v1.0 release.
* Metric: Cross-Platform Compatibility – (# Platforms Passed / # Platforms Tested) × 100% – Target: 100% – Horizon: v1.0 release.
* Metric: CI Pipeline Reliability – (Successful CI Runs / Total Runs) × 100% – Target: >98% – Horizon: continuous.
* Metric: Porting Effort – # of functions required to adapt to new platform – Target: <10 – Horizon: continuous.
