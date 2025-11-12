# Alpha Testing Guide - AES3-2009 Implementation

**Version**: 1.0  
**Date**: 2025-11-12  
**Project**: AES3-2009 Hardware-Agnostic Digital Audio Interface  
**Release**: v0.1.0-alpha.1

---

## ⚠️ READ THIS FIRST

**THIS IS ALPHA SOFTWARE - NOT PRODUCTION READY**

### Before You Begin

**DO NOT USE** this software for:
- ❌ Production broadcast transmission
- ❌ Professional recording sessions
- ❌ Live performance systems
- ❌ Any safety-critical application
- ❌ 24/7 continuous operation without monitoring

**SAFE TO USE** for:
- ✅ Technical evaluation in test labs
- ✅ Integration testing with test equipment
- ✅ Educational demonstrations
- ✅ Research and development
- ✅ Standards compliance verification

**Known Limitations**: See [`07-verification-validation/vv-summary-report.md`](07-verification-validation/vv-summary-report.md) Section 1

---

## 🎯 Purpose of Alpha Testing

We need **YOUR HELP** to validate this implementation in real-world conditions that we cannot replicate in our lab.

### What We're Testing

1. **Standards Compliance**: Does it correctly implement AES3-2009 specification?
2. **Interoperability**: Does it work with professional AES3 equipment?
3. **Reliability**: Does it operate stably over extended periods?
4. **Edge Cases**: What scenarios break it that we didn't anticipate?
5. **Usability**: Is the API clear? Is documentation adequate?

### What We're NOT Testing Yet

- Performance optimization (baseline implementation)
- Security hardening (no security audit yet)
- Enterprise features (redundancy, failover, etc.)
- Consumer audio applications (different scope)

---

## 🚀 Getting Started

### Prerequisites

**Hardware Requirements**:
- Computer with available USB/PCIe/Thunderbolt ports
- **Professional AES3 interface** (e.g., RME, Focusrite, Apogee, etc.)
- XLR-3 or BNC cables for balanced/coaxial connections
- Test audio source (optional: professional audio equipment)

**Software Requirements**:
- Operating System: Windows 10/11, Linux (Ubuntu 20.04+), or macOS 11+
- Build tools (if compiling from source):
  - CMake 3.15+
  - C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
  - Git

**Knowledge Requirements**:
- Basic understanding of digital audio concepts
- Familiarity with AES3 standard (helpful but not required)
- Command-line comfort (for logs and testing)

### Installation

**Option 1: Pre-built Binaries** (Coming Soon)
```bash
# Download from GitHub Releases
# https://github.com/zarfld/AES3-2009/releases/tag/v0.1.0-alpha.1

# Extract archive
# Run installation script
```

**Option 2: Build from Source**
```bash
# Clone repository
git clone https://github.com/zarfld/AES3-2009.git
cd AES3-2009
git checkout v0.1.0-alpha.1

# Build (Linux/macOS)
mkdir build
cd build
cmake ..
make -j$(nproc)

# Build (Windows PowerShell)
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

**Verify Installation**:
```bash
# Run self-test
./aes3_test --self-test

# Expected output: All tests passing
```

---

## 🧪 What to Test

### Test Scenarios

#### 1. Basic Functionality Test (30 minutes)

**Objective**: Verify basic AES3 transmission and reception

**Setup**:
- Connect AES3 interface to computer
- Configure loopback (output → input)
- Run basic transmit/receive test

**Test Steps**:
```bash
# Start AES3 transmitter (48kHz, 24-bit, stereo)
./aes3_transmit --rate=48000 --depth=24 --channels=2 --duration=60

# Monitor receiver in another terminal
./aes3_receive --rate=48000 --depth=24 --validate

# Expected: Clean reception, no errors, valid channel status
```

**What to Report**:
- ✅ Did transmission start successfully?
- ✅ Did receiver synchronize to preambles?
- ✅ Were audio samples transmitted without corruption?
- ✅ Was channel status decoded correctly?
- ❌ Any errors or warnings in logs?
- ❌ Any unexpected behavior?

#### 2. Sampling Rate Test (45 minutes)

**Objective**: Test all AES5-2018 sampling frequencies

**Test Steps**:
Test each rate for 5 minutes:
- 32 kHz
- 44.1 kHz
- 48 kHz
- 88.2 kHz
- 96 kHz
- 176.4 kHz
- 192 kHz

```bash
for rate in 32000 44100 48000 88200 96000 176400 192000; do
    echo "Testing $rate Hz..."
    ./aes3_transmit --rate=$rate --duration=300
    # Monitor for errors
done
```

**What to Report**:
- Which rates worked correctly?
- Which rates had issues?
- Did receiver lock to all rates?
- Any drift or synchronization issues?

#### 3. Bit Depth Test (30 minutes)

**Objective**: Test word length handling (16/20/24 bit)

**Test Steps**:
```bash
# Test 16-bit
./aes3_transmit --rate=48000 --depth=16 --duration=300

# Test 20-bit
./aes3_transmit --rate=48000 --depth=20 --duration=300

# Test 24-bit
./aes3_transmit --rate=48000 --depth=24 --duration=300
```

**What to Report**:
- Audio sample accuracy at each bit depth
- Word length justification (MSB alignment)
- Any quantization artifacts

#### 4. Channel Status Test (30 minutes)

**Objective**: Verify 192-bit channel status block decoding

**Test Steps**:
```bash
# Transmit with specific channel status
./aes3_transmit --rate=48000 --channel-mode=professional \
                --emphasis=none --sample-rate-code=48k

# Verify channel status decoding
./aes3_receive --dump-channel-status --duration=10

# Verify CRCC (CRC-8 checksum in byte 23)
./aes3_receive --validate-crcc
```

**What to Report**:
- Channel status decoded correctly?
- CRCC validation passing?
- User data channel working?
- Auxiliary bits handling correct?

#### 5. Preamble Detection Test (20 minutes)

**Objective**: Test synchronization with preambles X, Y, Z

**Test Steps**:
```bash
# Test preamble detection
./aes3_receive --log-preambles --duration=600

# Verify preamble sequence: Z (block start), X, Y, X, Y, ... (191 times), then Z again
```

**What to Report**:
- Preamble detection accuracy
- Synchronization time (time to lock)
- False preamble detections?
- Loss of sync events?

#### 6. Long-Duration Stability Test (8+ hours)

**Objective**: Detect rare bugs that only appear over time

**Test Steps**:
```bash
# Run overnight or over weekend
./aes3_transmit --rate=48000 --depth=24 --duration=28800 \
                --log-file=stability-test.log

# Monitor in separate terminal
./aes3_monitor --alert-on-error
```

**What to Report**:
- Total hours of operation
- Number of errors encountered
- Error timestamps and descriptions
- CPU/memory usage trends
- Any performance degradation over time?

#### 7. Equipment Interoperability Test (Variable)

**Objective**: Test with real professional audio equipment

**Test Steps**:
```bash
# Connect to professional AES3 device
# (e.g., broadcast console, audio interface, converter)

# Test transmission TO professional device
./aes3_transmit --output-device=<your_interface>

# Test reception FROM professional device
./aes3_receive --input-device=<your_interface>
```

**Equipment Info to Report**:
- Manufacturer and model
- Firmware version
- Connection type (balanced XLR / coaxial BNC / optical)
- Cable length

**What to Report**:
- Did professional device recognize AES3 signal?
- Did our software recognize signal from device?
- Any interoperability issues?
- Any timing/jitter issues?
- Any unexpected behavior?

#### 8. Error Handling Test (30 minutes)

**Objective**: Test response to error conditions

**Test Scenarios**:
- Disconnect cable during transmission
- Connect/disconnect interface during operation
- Send invalid AES3 data (corrupt preambles)
- Overload input (excessive jitter)

**What to Report**:
- How did software handle disconnection?
- Did it recover gracefully?
- Were error messages clear?
- Any crashes or hangs?

#### 9. Platform-Specific Test (Per platform)

**Objective**: Verify multi-platform compatibility

**Test on your OS**:
- Windows 10/11
- Linux (Ubuntu, Fedora, etc.)
- macOS (Intel and Apple Silicon if possible)

**What to Report**:
- Which OS/version tested?
- Any platform-specific issues?
- Performance differences?
- Build issues?

---

## 📊 Collecting Data

### Test Results Template

For each test scenario, use this template:

```markdown
## Test Results: [Scenario Name]

**Date**: [YYYY-MM-DD]  
**Tester**: [Your Name/Organization]  
**Equipment**: [Hardware details]  
**Platform**: [OS and version]

### Configuration
- Sampling Rate: [Rate]
- Bit Depth: [Depth]
- Channels: [Number]
- Connection: [Balanced/Coaxial/Optical]
- Duration: [Test duration]

### Results
**Status**: [✅ PASS / ⚠️ PARTIAL / ❌ FAIL]

**Observations**:
- [What worked]
- [What didn't work]
- [Unexpected behavior]

**Performance**:
- CPU Usage: [%]
- Memory Usage: [MB]
- Latency: [ms]

**Errors/Warnings**:
```
[Paste error messages or logs here]
```

**Screenshots/Logs**: [Attach if applicable]

### Recommendations
- [Suggestions for improvement]
```

### Field Reliability Data

Please track and report:

**Operational Metrics**:
- **Total Hours**: Cumulative operation time
- **Failures**: Number and description
- **Recovery**: How quickly did it recover?
- **Uptime**: Percentage of time operational

**Use Field Reliability Report Template**: [`field-reliability-report-template.md`](field-reliability-report-template.md)

---

## 🐛 Reporting Issues

### Where to Report

**GitHub Issues**: https://github.com/zarfld/AES3-2009/issues/new

### Issue Templates

Use the appropriate template:

1. **Bug Report** - Something isn't working
2. **Feature Request** - New functionality suggestion
3. **Test Results** - Share your testing findings
4. **Equipment Compatibility** - Report interoperability results

### What to Include

**For Bug Reports**:
- Clear title (e.g., "Preamble detection fails at 192kHz")
- Steps to reproduce
- Expected behavior
- Actual behavior
- Environment details (OS, hardware, versions)
- **Logs** (attach full logs)
- Screenshots/recordings if applicable

**Log Collection**:
```bash
# Enable verbose logging
./aes3_transmit --verbose --log-file=debug.log

# Attach debug.log to GitHub issue
```

### Priority Guidelines

Help us triage by labeling severity:

- **P0 - Critical**: Crash, data corruption, safety issue
- **P1 - High**: Major functionality broken, no workaround
- **P2 - Medium**: Minor issue, workaround exists
- **P3 - Low**: Enhancement, cosmetic issue

---

## 💡 Tips for Effective Testing

### Do's ✅

- **Test incrementally** - Start simple, increase complexity
- **Document everything** - Even "it worked" is valuable data
- **Be specific** - "Doesn't work" is not helpful; "Preamble X not detected at 192kHz after 5 minutes" is helpful
- **Include logs** - Logs are your best friend for debugging
- **Test edge cases** - Weird configs often reveal bugs
- **Be patient** - Some tests take hours (stability testing)
- **Share positive results** - Knowing what works is as important as what doesn't

### Don'ts ❌

- **Don't use in production** - Alpha software is unstable
- **Don't skip disclaimers** - Read limitations before testing
- **Don't test on critical systems** - Use dedicated test equipment
- **Don't assume it's your fault** - If it breaks, report it!
- **Don't ignore warnings** - Warnings often indicate real issues
- **Don't forget to backup** - Save your test data/configs

### Test Environment Setup

**Ideal Test Environment**:
- Dedicated test system (not your production machine)
- Spare AES3 interface (not your primary audio interface)
- Test cables and adapters
- Logging tools (terminal multiplexer, log viewer)
- Performance monitoring (htop, Task Manager, Activity Monitor)

**Safety Precautions**:
- Test in isolated network (if networked)
- Disable automatic updates during tests
- Have backup equipment ready
- Monitor system resources (CPU, memory, disk)

---

## 📚 Additional Resources

### Documentation

- [V&V Summary Report](07-verification-validation/vv-summary-report.md) - Known limitations
- [Code Coverage Gap Analysis](07-verification-validation/code-coverage-gap-analysis.md) - Untested code paths
- [Architecture Documentation](03-architecture/) - System design
- [Requirements Specifications](02-requirements/) - What it should do
- [API Documentation](docs/api/) - Programming interface

### AES3-2009 Specification

To fully validate compliance, reference the AES3-2009 specification:
- AES3-1-2009: Audio Content
- AES3-2-2009: Metadata and Subcode
- AES3-3-2009: Transport
- AES3-4-2009: Physical and Electrical

Available from: https://www.aes.org/publications/standards/

### Community Support

- **GitHub Discussions**: https://github.com/zarfld/AES3-2009/discussions
- **Q&A**: Ask questions, share experiences
- **Show and Tell**: Share your testing setup/results

---

## 🤝 Thank You!

Your participation in Alpha testing is **invaluable**. Every bug report, compatibility finding, and hour of operation logged helps make this implementation production-ready.

### Recognition

Alpha testers will be:
- Acknowledged in Beta release notes
- Listed in CONTRIBUTORS.md
- Given early access to Beta releases
- Invited to shaping the roadmap

### Questions?

- **GitHub Discussions**: https://github.com/zarfld/AES3-2009/discussions
- **Email**: [Maintainer email if provided]
- **Issue Tracker**: https://github.com/zarfld/AES3-2009/issues

---

## Appendix: Quick Reference

### Common Commands

```bash
# Basic transmission test
./aes3_transmit --rate=48000 --depth=24 --duration=60

# Basic reception test
./aes3_receive --rate=48000 --validate

# Verbose logging
./aes3_transmit --verbose --log-file=debug.log

# Self-test
./aes3_test --self-test

# Monitor system
./aes3_monitor --alert-on-error

# Dump channel status
./aes3_receive --dump-channel-status
```

### Sampling Rates (AES5-2018)

| Rate | Use Case |
|------|----------|
| 32 kHz | Broadcast (legacy) |
| 44.1 kHz | CD audio |
| 48 kHz | Professional audio, video |
| 88.2 kHz | High-res audio (2x CD) |
| 96 kHz | High-res professional |
| 176.4 kHz | High-res audio (4x CD) |
| 192 kHz | High-res professional |

### Bit Depths

| Depth | Description |
|-------|-------------|
| 16-bit | Standard resolution |
| 20-bit | Enhanced resolution |
| 24-bit | Professional resolution |

### Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | General error |
| 2 | Invalid arguments |
| 3 | Hardware error |
| 4 | Protocol error |

---

**Last Updated**: 2025-11-12  
**Document Version**: 1.0  
**For Release**: v0.1.0-alpha.1
