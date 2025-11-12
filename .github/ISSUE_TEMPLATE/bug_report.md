---
name: Bug Report
about: Report a bug or issue with the AES3-2009 implementation
title: '[BUG] '
labels: 'bug, alpha-feedback'
assignees: ''
---

## Bug Description

**Brief Description**: [One-line summary of the bug]

**Severity**: [P0 Critical / P1 High / P2 Medium / P3 Low]
- P0 = System crash, data corruption, safety issue
- P1 = Major functionality broken, no workaround
- P2 = Minor issue, workaround exists
- P3 = Cosmetic, low impact

## Environment

**Release Version**: v0.1.0-alpha.1

**Operating System**:
- OS: [Windows/Linux/macOS]
- Version: [e.g., Windows 11, Ubuntu 22.04, macOS 13]
- Architecture: [x64/ARM64]

**Hardware**:
- CPU: [Processor model]
- RAM: [Amount]
- AES3 Interface: [Manufacturer, Model]
- Connection: [USB/PCIe/Thunderbolt]

**Configuration**:
- Sampling Rate: [32/44.1/48/88.2/96/176.4/192 kHz]
- Bit Depth: [16/20/24 bit]
- Channel Mode: [Professional/Consumer]

## Steps to Reproduce

1. [First step]
2. [Second step]
3. [Third step]
4. [Observed behavior]

**Minimal Reproducible Example** (if possible):
```bash
# Commands or code that reproduce the issue
./aes3_transmit --rate=48000 --depth=24
```

## Expected Behavior

[What should have happened]

## Actual Behavior

[What actually happened]

## Error Messages/Logs

```
[Paste error messages, stack traces, or relevant log excerpts here]
```

**Full Log File**: [Attach if large, or use GitHub Gist]

## Screenshots/Recordings

[If applicable, add screenshots or screen recordings]

## Additional Context

**Workaround** (if found):
[Description of any workaround you discovered]

**Frequency**:
- [ ] Happens every time
- [ ] Happens sometimes (intermittent)
- [ ] Happened once

**Impact on Testing**:
- [ ] Blocks further testing
- [ ] Limits testing
- [ ] Minor inconvenience

**Related Issues**: [Link to related issues if known]

## Checklist

- [ ] I have searched existing issues to avoid duplicates
- [ ] I have read the Alpha Testing Guide
- [ ] I have included sufficient information to reproduce the bug
- [ ] I have attached relevant logs
- [ ] I am using v0.1.0-alpha.1 (or specified version above)

---

**Thank you for reporting this bug!** Your detailed report helps us improve the AES3-2009 implementation.
