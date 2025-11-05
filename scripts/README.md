# Scripts Directory

This directory contains automation scripts for specification validation, creation, and management.

## 🎯 Core Tools

### Spec Creation & Validation

| Script | Purpose | Usage |
|--------|---------|-------|
| `create-spec.py` | Interactive spec creation wizard | `python scripts/create-spec.py requirements --interactive` |
| `validate-spec-structure.py` | Validate YAML front matter against schemas | `python scripts/validate-spec-structure.py` |
| `check-spec-numbering.py` | Check for ID duplicates and gaps | `python scripts/check-spec-numbering.py` |
| `spec-cli.py` | Unified CLI for all spec operations | `python scripts/spec-cli.py validate` |
| `pre-commit-hook.py` | Git pre-commit validation hook | Setup: `pre-commit install` |

### Traceability & Analysis

| Script | Purpose | Usage |
|--------|---------|-------|
| `generate-traceability-matrix.py` | Generate traceability matrix | `python scripts/generate-traceability-matrix.py` |
| `generators/spec_parser.py` | Parse specs to JSON index | `python scripts/generators/spec_parser.py` |
| `generators/build_trace_json.py` | Build traceability JSON | `python scripts/generators/build_trace_json.py` |
| `generators/gen_tests.py` | Generate test skeletons | `python scripts/generators/gen_tests.py` |

## 📦 Dependencies

```bash
pip install pyyaml jsonschema
```

## 🚀 Quick Start

### Create a new specification

```bash
# Interactive mode with all prompts
python scripts/create-spec.py requirements --interactive

# Quick create
python scripts/create-spec.py architecture
```

### Validate specifications

```bash
# Validate all specs
python scripts/spec-cli.py validate

# Validate specific file
python scripts/validate-spec-structure.py 02-requirements/functional/auth.md

# Check ID numbering
python scripts/check-spec-numbering.py
```

### Setup pre-commit hooks

```bash
# Using pre-commit framework (recommended)
pip install pre-commit
pre-commit install

# Manual installation
cp scripts/pre-commit-hook.py .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

### Generate compliance report

```bash
# Text format
python scripts/spec-cli.py report

# Markdown format
python scripts/spec-cli.py report --format markdown > compliance-report.md
```

## 📋 Spec Creation Workflow

1. **Create spec** using wizard (auto-numbers, validates)
2. **Edit content** - fill in requirements, acceptance criteria
3. **Validate** before committing
4. **Commit** - pre-commit hook validates automatically

See [docs/spec-creation-workflow.md](../docs/spec-creation-workflow.md) for detailed guide.

## 🔧 Script Details

### create-spec.py

**Features**:
- Auto-generates next available ID
- Validates against JSON schema
- Supports categorized IDs (e.g., REQ-AUTH-F-001)
- Pre-fills template with metadata
- Interactive prompts for all required fields

**Example**:
```bash
python scripts/create-spec.py requirements --interactive
```

### validate-spec-structure.py

**Checks**:
- YAML front matter present
- All required fields exist
- Field formats match schema
- Traceability arrays valid
- ID patterns correct

**Exit codes**:
- 0: Success
- 1: Validation errors
- 2: Internal error

### check-spec-numbering.py

**Reports**:
- Duplicate IDs (error)
- Numbering gaps (warning)
- ID distribution by type
- Coverage statistics

**Modes**:
- Standard: Gaps are warnings
- Strict (`--strict`): Gaps are errors

### spec-cli.py

**Unified interface** for:
- Creating specs
- Validating specs
- Checking numbering
- Generating reports

**Subcommands**:
```bash
spec-cli.py validate [files...]
spec-cli.py create <type> [--interactive]
spec-cli.py check-numbering [--strict]
spec-cli.py report [--format text|markdown]
```

### pre-commit-hook.py

**Automatic validation** on git commit:
- Validates only staged spec files
- Blocks commit on errors
- Can be bypassed with `--no-verify`

## 🎓 Standards Compliance

All scripts enforce:
- **ISO/IEC/IEEE 29148:2018** - Requirements specifications
- **ISO/IEC/IEEE 42010:2011** - Architecture descriptions
- **JSON Schema Draft 7** - YAML front matter validation

## 📚 Related Documentation

- [Spec Creation Workflow](../docs/spec-creation-workflow.md) - Complete workflow guide
- [ID Taxonomy Guide](../docs/id-taxonomy-guide.md) - Category identifier usage
- [Lifecycle Guide](../docs/lifecycle-guide.md) - Full development process

## 🐛 Troubleshooting

**Import errors**: Scripts use runtime path manipulation - linter warnings are expected

**Missing dependencies**:
```bash
pip install pyyaml jsonschema
```

**Pre-commit not running**:
```bash
pre-commit install --force
```

**Schema validation fails**: Check YAML front matter matches schema exactly (case-sensitive, proper types)

---

For questions, see [docs/spec-creation-workflow.md](../docs/spec-creation-workflow.md) or open an issue.
