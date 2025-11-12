# Contributing to AES3-2009

Thank you for your interest in contributing to the AES3-2009 Hardware-Agnostic Digital Audio Interface project! 🎵

This is an **Alpha release** project seeking validation and feedback from the professional audio community.

## 🎯 Current Phase: Alpha Testing

**We need YOUR help** with:

1. **Testing with real equipment** - Professional AES3 interfaces, broadcast consoles, audio converters
2. **Field reliability data** - Hours of operation, failure rates, performance metrics
3. **Interoperability reports** - Compatibility with professional audio equipment
4. **Bug reports** - Issues, edge cases, unexpected behaviors
5. **Documentation feedback** - Clarity, completeness, accuracy

See [`ALPHA_TESTING_GUIDE.md`](ALPHA_TESTING_GUIDE.md) for detailed testing instructions.

## 📋 Ways to Contribute

### 1. Alpha Testing (Most Valuable!)

**What We Need**:
- Test with professional AES3 equipment
- Report compatibility results
- Collect field reliability data
- Identify edge cases and bugs

**How to Start**:
1. Read [`ALPHA_TESTING_GUIDE.md`](ALPHA_TESTING_GUIDE.md)
2. Set up test environment
3. Run test scenarios
4. Report findings using templates

**Report Using**:
- [Bug Report Template](.github/ISSUE_TEMPLATE/bug_report.md)
- [Equipment Compatibility Template](.github/ISSUE_TEMPLATE/equipment_compatibility.md)
- [Field Reliability Report Template](field-reliability-report-template.md)

### 2. Bug Reports

**Before Reporting**:
- Search existing issues to avoid duplicates
- Verify you're using v0.1.0-alpha.1 (or current version)
- Collect logs and error messages
- Include reproduction steps

**Use Template**: [Bug Report](.github/ISSUE_TEMPLATE/bug_report.md)

**Include**:
- Environment details (OS, hardware, configuration)
- Steps to reproduce
- Expected vs. actual behavior
- Error messages and logs
- Screenshots/recordings if applicable

### 3. Feature Requests

**Before Requesting**:
- Check if feature already planned (see [Roadmap](#roadmap))
- Consider if it aligns with AES3-2009 specification
- Provide clear use case

**Include**:
- Clear problem statement
- Proposed solution
- Use case and benefits
- AES3 specification reference (if applicable)

### 4. Documentation Improvements

**Areas Needing Help**:
- User documentation clarity
- API documentation completeness
- Testing guide improvements
- Installation instructions

**How to Contribute**:
1. Fork the repository
2. Make documentation changes
3. Submit pull request with clear description

### 5. Code Contributions

**⚠️ Alpha Phase Guidelines**:

During Alpha, code contributions are **limited** to:
- Bug fixes for reported issues
- Test coverage improvements
- Documentation corrections
- Build/CI improvements

**Not Accepting Yet**:
- New features (Beta phase)
- Major refactoring (after field validation)
- Performance optimizations (baseline must be validated first)

**Why**: Alpha phase focuses on **validation** of current implementation, not expansion.

## 🔧 Development Setup

### Prerequisites

**Required**:
- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.15+
- Git

**Optional**:
- Google Test (for running tests)
- Valgrind (memory leak detection)
- clang-tidy (static analysis)

### Build from Source

```bash
# Clone repository
git clone https://github.com/zarfld/AES3-2009.git
cd AES3-2009

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# Run tests
ctest --output-on-failure
```

### Running Tests

```bash
# Run all tests
ctest

# Run specific test
ctest -R SubframeBuilderTest

# Run with verbose output
ctest --verbose

# Run with coverage (if enabled)
cmake -DCMAKE_BUILD_TYPE=Coverage ..
make coverage
```

## 📝 Pull Request Process

### Before Submitting PR

1. **Read the guidelines** - Understand what's accepted in Alpha phase
2. **Open an issue first** - Discuss proposed changes
3. **Follow code style** - Match existing code conventions
4. **Write tests** - All code must have tests (TDD approach)
5. **Update documentation** - Keep docs in sync with code
6. **Test locally** - All tests must pass

### PR Checklist

- [ ] Issue exists for this change
- [ ] Code follows project style
- [ ] Tests added/updated (>80% coverage maintained)
- [ ] Documentation updated
- [ ] All tests passing locally
- [ ] Commit messages follow [Conventional Commits](https://www.conventionalcommits.org/)
- [ ] PR description explains changes clearly

### Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Test additions/changes
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `ci`: CI/CD changes
- `build`: Build system changes
- `chore`: Maintenance tasks

**Examples**:
```
fix(subframe): Correct preamble X detection at 192kHz

Preamble detection was failing at 192kHz due to timing window
being too narrow. Increased window from 0.5 UI to 0.75 UI.

Fixes #42
```

```
test(channel-status): Add CRCC validation tests

Added comprehensive tests for CRC-8 validation in channel
status byte 23, covering all error cases.

Increases coverage from 83.64% to 85.12%.
```

## 🎨 Code Style Guidelines

### C++ Style

**Follow**:
- C++17 standard
- Google C++ Style Guide (mostly)
- RAII principles
- Const correctness
- Modern C++ idioms (avoid raw pointers, use smart pointers)

**Naming Conventions**:
```cpp
// Classes: PascalCase
class SubframeBuilder { };

// Functions: camelCase
void buildSubframe();

// Variables: snake_case
int sample_count;

// Constants: UPPER_SNAKE_CASE
const int MAX_SAMPLES = 192;

// Namespaces: match folder structure
namespace AES::AES3::Part3::_2009::subframe { }
```

**Example**:
```cpp
namespace AES {
namespace AES3 {
namespace Part3 {
namespace _2009 {
namespace subframe {

class SubframeBuilder {
public:
    explicit SubframeBuilder(int sample_rate);
    
    // Build subframe from audio sample
    Subframe buildSubframe(const AudioSample& sample);
    
private:
    int sample_rate_;
    std::unique_ptr<PreambleGenerator> preamble_gen_;
};

} // namespace subframe
} // namespace _2009
} // namespace Part3
} // namespace AES3
} // namespace AES
```

### Documentation Style

**Use Doxygen format**:
```cpp
/**
 * @brief Parse AES3-2009 Audio Subframe from network packet
 *
 * Validates and extracts audio subframe according to AES3-2009 Part 3.
 * Performs preamble detection, biphase-mark decoding, and CRCC validation.
 *
 * @param packet_data Pointer to packet data buffer
 * @param packet_length Length of packet in bytes
 * @param audio_subframe Output structure for parsed subframe
 * @return 0 on success, error code on failure
 * 
 * @see AES3-2009 Part 3, Section 4.2 "Subframe Structure"
 */
int aes3_parse_audio_subframe(const uint8_t* packet_data, 
                               size_t packet_length,
                               aes3_audio_subframe_t* audio_subframe);
```

### Test Style

**Use Google Test**:
```cpp
TEST(SubframeBuilderTest, BuildsValidSubframe) {
    // Arrange
    SubframeBuilder builder(48000);
    AudioSample sample{0x123456, true, false};
    
    // Act
    Subframe result = builder.buildSubframe(sample);
    
    // Assert
    EXPECT_EQ(result.preamble, Preamble::X);
    EXPECT_EQ(result.audio_data, 0x123456);
    EXPECT_TRUE(result.validity_bit);
    EXPECT_FALSE(result.channel_status_bit);
}
```

## 🏗️ Architecture Guidelines

### Standards Layer Compliance

**CRITICAL**: Standards layer (`lib/Standards/`) must remain hardware-agnostic.

**✅ Allowed**:
```cpp
// Pure protocol implementation
#include "aes3_subframe.h"
#include "aes3_channel_status.h"
```

**❌ Forbidden**:
```cpp
// NO hardware-specific includes
#include "asio_audio_driver.h"     // NO
#include "../../realtek_audio/*.h"  // NO
#include <alsa/asoundlib.h>         // NO
```

**Use Dependency Injection**:
```cpp
// Hardware abstraction via interfaces
typedef struct {
    int (*send_audio_frame)(const void* frame_data, size_t length);
    int (*receive_audio_frame)(void* buffer, size_t* length);
} audio_interface_t;
```

See [`.github/copilot-instructions.md`](.github/copilot-instructions.md) for complete guidelines.

## 🧪 Testing Requirements

### Test-Driven Development (TDD)

**Required Process**:
1. **Red** - Write failing test first
2. **Green** - Write minimal code to pass
3. **Refactor** - Improve design while keeping tests green

### Coverage Requirements

- **Minimum**: 80% line coverage (XP requirement)
- **Target**: 90% line coverage (Beta requirement)
- **Critical Code**: 100% coverage for safety-critical functions

### Test Types

1. **Unit Tests** - Test individual functions/classes
2. **Integration Tests** - Test component interactions
3. **Conformance Tests** - Test AES3-2009 compliance
4. **Acceptance Tests** - Test user stories (BDD format)

## 🔒 Security Guidelines

**Alpha Phase Security**:
- No formal security assessment yet
- Report security concerns immediately
- Do not use in security-sensitive deployments

**Reporting Security Issues**:
- **Do NOT** open public issue
- Email: [security contact if provided]
- Include detailed description and reproduction steps
- Allow time for fix before disclosure

## 📊 Project Standards

### IEEE/ISO/IEC Standards Compliance

This project follows:
- **ISO/IEC/IEEE 12207:2017** - Software lifecycle processes
- **ISO/IEC/IEEE 29148:2018** - Requirements engineering
- **IEEE 1016-2009** - Software design descriptions
- **ISO/IEC/IEEE 42010:2011** - Architecture descriptions
- **IEEE 1012-2016** - Verification and validation

### XP (Extreme Programming) Practices

- Test-Driven Development (TDD)
- Continuous Integration
- Small Releases
- Simple Design (YAGNI)
- Refactoring
- Pair Programming (encouraged)

## 🗺️ Roadmap

### Alpha Phase (Current - 3-6 months)

**Focus**: Validation and feedback collection

**Goals**:
- 10+ early adopters actively testing
- 100+ hours field operation logged
- 5+ equipment vendors tested
- Field reliability data collected
- Security assessment planned

### Beta Phase (After Alpha)

**Prerequisites**:
- All P0/P1 Alpha issues resolved
- Security assessment complete
- 90%+ code coverage
- Field MTBF/MTTR data validates projections
- Independent code review complete

**Focus**: Production hardening

### Production Release (6-12 months from Alpha)

**Prerequisites**:
- All Beta requirements met
- 95%+ code coverage
- Independent third-party audit
- Real-world equipment compatibility matrix
- 1000+ hours field operation
- Community validation

## 📞 Communication Channels

### GitHub

- **Issues**: Bug reports, feature requests
- **Discussions**: Q&A, general discussion, show and tell
- **Pull Requests**: Code contributions

### Community Guidelines

**Be Respectful**:
- Treat everyone with respect
- Be constructive in feedback
- Assume good intentions
- Focus on the issue, not the person

**Be Professional**:
- Use clear, professional language
- Provide detailed, actionable feedback
- Share knowledge and help others

**Be Honest**:
- Report issues accurately
- Acknowledge limitations
- Give credit where due

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

By contributing, you agree that your contributions will be licensed under the MIT License.

## 🙏 Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Acknowledged in release notes
- Invited to shaping the project roadmap
- Given early access to Beta releases

## ❓ Questions?

- **GitHub Discussions**: https://github.com/zarfld/AES3-2009/discussions
- **Issue Tracker**: https://github.com/zarfld/AES3-2009/issues

---

**Thank you for contributing to AES3-2009!** Your participation helps build a robust, standards-compliant reference implementation for the professional audio community. 🎵
