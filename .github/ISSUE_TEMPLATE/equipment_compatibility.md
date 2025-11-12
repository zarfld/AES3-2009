---
name: Equipment Compatibility Report
about: Report interoperability results with professional AES3 equipment
title: '[EQUIPMENT] '
labels: 'equipment-compatibility, alpha-feedback'
assignees: ''
---

## Equipment Information

**Manufacturer**: [Brand name]

**Model**: [Model number]

**Firmware Version**: [Version if known]

**Equipment Type**:
- [ ] Professional audio interface
- [ ] Broadcast console
- [ ] Digital mixer
- [ ] Audio converter (ADC/DAC)
- [ ] Audio router/switcher
- [ ] Other: [Specify]

**Equipment Role**:
- [ ] AES3 Source (transmitter)
- [ ] AES3 Sink (receiver)
- [ ] Both (transmit and receive)

## Connection Details

**Connection Type**:
- [ ] Balanced (XLR-3, 110Ω)
- [ ] Coaxial (BNC, 75Ω)
- [ ] Optical (TOSLINK/ADAT)
- [ ] Other: [Specify]

**Cable**:
- Length: [Meters]
- Quality: [Professional/Consumer/DIY]
- Manufacturer: [Brand if known]

**Configuration**:
- Sampling Rate: [32/44.1/48/88.2/96/176.4/192 kHz]
- Bit Depth: [16/20/24 bit]
- Channel Mode: [Professional/Consumer]

## Test Environment

**AES3 Implementation Version**: v0.1.0-alpha.1

**Computer Platform**:
- OS: [Windows/Linux/macOS, version]
- CPU: [Processor]
- Interface: [USB/PCIe/Thunderbolt]

## Compatibility Test Results

### Overall Result

**Status**: [✅ Compatible / ⚠️ Partially Compatible / ❌ Incompatible]

**Summary**: [One-paragraph summary of compatibility]

### Detailed Tests

#### Test 1: Signal Detection

**Professional Equipment → AES3 Implementation (Receive)**

- [ ] ✅ Signal detected successfully
- [ ] ⚠️ Signal detected with issues (describe below)
- [ ] ❌ Signal not detected

**Details**: [Description]

#### Test 2: Signal Transmission

**AES3 Implementation → Professional Equipment (Transmit)**

- [ ] ✅ Equipment recognized signal successfully
- [ ] ⚠️ Equipment recognized signal with issues (describe below)
- [ ] ❌ Equipment did not recognize signal

**Details**: [Description]

#### Test 3: Preamble Synchronization

**Sync Time**: [Seconds to achieve lock]

- [ ] ✅ < 1 second
- [ ] ⚠️ 1-5 seconds
- [ ] ❌ > 5 seconds or failed

**Details**: [Description]

#### Test 4: Channel Status Decoding

- [ ] ✅ Channel status decoded correctly
- [ ] ⚠️ Partial decoding
- [ ] ❌ Channel status not decoded

**Details**: [What worked, what didn't]

#### Test 5: Audio Quality

- [ ] ✅ Perfect audio quality (no artifacts)
- [ ] ⚠️ Minor artifacts (occasional clicks/pops)
- [ ] ❌ Significant corruption

**Details**: [Description of audio quality]

#### Test 6: Stability

**Test Duration**: [Hours]

- [ ] ✅ Stable throughout test
- [ ] ⚠️ Occasional sync losses (< 1 per hour)
- [ ] ❌ Frequent sync losses or crashes

**Details**: [Description]

### Sampling Rate Compatibility Matrix

| Rate | Transmit | Receive | Notes |
|------|----------|---------|-------|
| 32 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 44.1 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 48 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 88.2 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 96 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 176.4 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |
| 192 kHz | [✅/⚠️/❌/NT] | [✅/⚠️/❌/NT] | [Notes] |

**Legend**: ✅ Works / ⚠️ Issues / ❌ Doesn't work / NT = Not tested

## Issues Encountered

### Issue 1: [Brief description]

**Severity**: [P0-P3]

**Description**: [Detailed description]

**Workaround** (if found): [Description]

### Issue 2: [Brief description]

[Repeat as needed]

## Positive Findings

**What Worked Well**:

- [Positive observation 1]
- [Positive observation 2]
- ...

## Recommendations

### For Users

**Would you recommend this equipment with AES3-2009 implementation?**

- [ ] Yes, works great
- [ ] Yes, with caveats (list below)
- [ ] No, too many issues
- [ ] Needs more testing

**Caveats/Notes**: [If "Yes, with caveats" selected]

### For Developers

**Suggestions for improvement**:

- [Suggestion 1]
- [Suggestion 2]
- ...

## Logs and Evidence

**Log Files**: [Attach or link to logs]

**Screenshots**: [Attach if applicable]

**Audio Recordings**: [If you captured audio, describe or link]

## Additional Information

**Other Equipment in Signal Chain**:

- [Device 1]
- [Device 2]
- ...

**Environmental Factors**:

- Temperature: [If relevant]
- Electrical noise: [Any interference noted]
- Other: [Anything else relevant]

## Checklist

- [ ] I have read the Alpha Testing Guide
- [ ] I have provided complete equipment information
- [ ] I have tested both transmit and receive (if equipment supports both)
- [ ] I have tested multiple sampling rates
- [ ] I have attached relevant logs or screenshots
- [ ] I am using v0.1.0-alpha.1

---

**Thank you for testing equipment compatibility!** This information is invaluable for building an interoperability matrix.
