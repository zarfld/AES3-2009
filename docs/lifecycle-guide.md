# Software Development Lifecycle Guide

This guide walks you through the complete software development lifecycle using this template repository, integrating **IEEE/ISO/IEC standards** with **Extreme Programming (XP) practices**.

---

## Overview

The lifecycle consists of 9 phases, each with specific objectives, deliverables, and copilot instructions:

```
Phase 01: Stakeholder Requirements → Phase 02: Requirements Analysis
    ↓                                           ↓
Phase 03: Architecture Design → Phase 04: Detailed Design
    ↓                                           ↓
Phase 05: Implementation → Phase 06: Integration
    ↓                                           ↓
Phase 07: Verification & Validation → Phase 08: Transition
    ↓
Phase 09: Operation & Maintenance (Continuous)
```

---

## Phase-by-Phase Workflow

### Phase 01: Stakeholder Requirements Definition

**When to use**: At project inception

**Key Activities**:

1. Identify all stakeholders
2. Conduct stakeholder interviews
3. Document business context
4. Create Stakeholder Requirements Specification (StRS)

**Deliverables**:

- `01-stakeholder-requirements/stakeholder-requirements-specification.md`
- `01-stakeholder-requirements/stakeholders/stakeholder-register.md`
- `01-stakeholder-requirements/business-context/business-context.md`

**Copilot Support**:
Navigate to `01-stakeholder-requirements/` and GitHub Copilot will automatically load phase-specific instructions to help you:

- Ask clarifying questions to stakeholders
- Structure stakeholder requirements per IEEE 29148
- Create traceability from the start

**Example Workflow**:

```bash
cd 01-stakeholder-requirements

# Use template
cp templates/stakeholder-requirements-template.md stakeholder-requirements-specification.md

# Open in editor - Copilot will help you fill it in
code stakeholder-requirements-specification.md

# Copilot will:
# - Suggest stakeholder categories to consider
# - Help structure requirements
# - Remind you about traceability IDs (StR-XXX)
```

---

### Phase 02: Requirements Analysis & Specification

**When to use**: After stakeholder requirements are approved

**Key Activities**:

1. Transform stakeholder requirements into system requirements
2. Define functional and non-functional requirements
3. Write use cases
4. Create user stories (XP practice)
5. Establish requirements traceability

**Deliverables**:

- `02-requirements/system-requirements-specification.md`
- `02-requirements/functional/` - Functional requirements
- `02-requirements/non-functional/nfr-specification.md`
- `02-requirements/use-cases/*.md`
- `02-requirements/user-stories/*.md`

**Using Spec-Kit Template**:

```bash
cd 02-requirements

# Copy requirements spec template
cp ../spec-kit-templates/requirements-spec.md functional/feature-x-requirements.md

# Edit the spec - Copilot helps you
code functional/feature-x-requirements.md

# Copilot will:
# - Ensure every requirement has acceptance criteria
# - Help write Given-When-Then scenarios
# - Maintain traceability (REQ-XXX → StR-XXX)
# - Suggest non-functional requirements you might have missed
```

**XP Practice: User Stories**:

```bash
# Copy user story template
cp ../spec-kit-templates/user-story-template.md user-stories/STORY-001-user-login.md

# Copilot helps you:
# - Write in "As a... I want... So that..." format
# - Define acceptance criteria
# - Break down into tasks
# - Estimate story points
```

**Exit Criteria Checklist**:

```markdown
- [ ] All functional requirements documented with IDs (REQ-F-XXX)
- [ ] All non-functional requirements with measurable metrics (REQ-NF-XXX)
- [ ] Every requirement traced to stakeholder requirement
- [ ] Every requirement has acceptance criteria
- [ ] Use cases written for complex interactions
- [ ] User stories created for all features
- [ ] Requirements reviewed by stakeholders
- [ ] Requirements approved and baselined
```

---

### Phase 03: Architecture Design

**When to use**: After requirements are approved

**Key Activities**:

1. Define system architecture
2. Create Architecture Decision Records (ADRs)
3. Design architecture views (C4 model)
4. Define component boundaries
5. Document architectural patterns

**Deliverables**:

- `03-architecture/architecture-description.md`
- `03-architecture/decisions/*.md` - ADRs
- `03-architecture/diagrams/` - C4 diagrams
- `03-architecture/views/` - Architecture views

**Using Spec-Kit Template**:

```bash
cd 03-architecture

# Copy architecture spec template
cp ../spec-kit-templates/architecture-spec.md architecture-description.md

# Copilot will help you:
# - Create C4 diagrams (Context, Container, Component)
# - Write ADRs with alternatives considered
# - Ensure architecture addresses all quality attributes
# - Maintain traceability to requirements
```

**Example: Creating an ADR**:

```bash
cd decisions

# Create new ADR
cat > ADR-001-database-choice.md << 'EOF'
# ADR-001: Use PostgreSQL for Primary Database

## Status
Proposed

## Context
[Copilot helps you describe the architectural challenge]

## Decision
[Copilot suggests structure for documenting your choice]

## Consequences
[Copilot prompts for positive, negative, and neutral consequences]

## Alternatives Considered
[Copilot reminds you to document alternatives]
EOF

code ADR-001-database-choice.md
# Copilot fills in details based on your context
```

---

### Phase 04: Detailed Design

**When to use**: After architecture is approved

**Key Activities**:

1. Design individual components
2. Define class structures and interfaces
3. Specify data models
4. Document algorithms
5. Apply design patterns

**Deliverables**:

- `04-design/software-design-description.md`
- `04-design/components/*.md` - Component designs
- `04-design/data-models/data-model-specification.md`
- `04-design/interfaces/api-design.md`

**Copilot Support**:

```bash
cd 04-design

# Copilot helps you:
# - Design classes following SOLID principles
# - Apply appropriate design patterns
# - Create complete interface specifications
# - Design for testability (XP: TDD-ready)
```

---

### Phase 05: Implementation (The XP Core!)

**When to use**: After design is complete

**Key Activities** (XP Focus):

1. **Test-Driven Development (TDD)**: Write tests FIRST
2. **Pair Programming**: Complex code done in pairs
3. **Continuous Integration**: Integrate multiple times daily
4. **Refactoring**: Keep code clean continuously
5. **Simple Design**: YAGNI (You Aren't Gonna Need It)

**Deliverables**:

- `05-implementation/src/` - Source code
- `05-implementation/tests/` - Test suites
- `05-implementation/docs/` - Code documentation

**TDD Workflow with Copilot**:

```bash
cd 05-implementation

# Step 1: Write test FIRST (Red)
cat > tests/user-service.test.ts << 'EOF'
describe('UserService', () => {
  it('should create user with valid data', async () => {
    // Copilot helps write the test
  });
});
EOF

# Step 2: Run test - it FAILS (Red)
npm test

# Step 3: Write minimal implementation to pass (Green)
# Copilot suggests implementation based on the test
code src/user-service.ts

# Step 4: Run test - it PASSES (Green)
npm test

# Step 5: Refactor (Blue)
# Copilot suggests refactorings to improve design
code src/user-service.ts

# Step 6: Run test - still PASSES
npm test

# Repeat!
```

**Copilot Support for XP**:

- **TDD**: Copilot generates tests from specs, suggests test cases
- **Pair Programming**: One person types, Copilot acts as second pair member suggesting improvements
- **Refactoring**: Copilot suggests refactorings while keeping tests green
- **Simple Design**: Copilot warns about over-engineering

**Quality Gates**:

```bash
# Before every commit:
npm run lint        # Code style
npm test            # All tests
npm run coverage    # Check >80% coverage

# If all pass:
git add .
git commit -m "feat: implement user service (TDD)"
git push

# CI runs automatically:
# - Linting
# - Tests
# - Coverage check
# - Security scan
```

---

### Phase 06: Integration

**When to use**: Continuously during implementation

**Key Activities**:

1. Integrate code multiple times per day (XP)
2. Run automated integration tests
3. Deploy to staging environment
4. Monitor integration health

**Deliverables**:

- `06-integration/integration-tests/` - Integration test suites
- `06-integration/ci-config/` - CI/CD configurations
- `06-integration/deployment/` - Deployment scripts

**Continuous Integration Workflow**:

```bash
# Workflow (happens automatically on push):
1. Developer commits code
2. GitHub Actions triggers
3. Build runs
4. All tests run (unit + integration)
5. If tests pass → Deploy to staging
6. If tests fail → Alert team, fix immediately (<10 min)

# Manual integration test run:
cd 06-integration
npm run test:integration

# Copilot helps with:
# - Writing integration tests
# - Debugging integration failures
# - Configuring CI/CD pipelines
```

---

### Phase 07: Verification & Validation

**When to use**: Throughout development, final validation before release

**Key Activities**:

1. Verify code against design (verification)
2. Validate system meets user needs (validation)
3. Execute test plans
4. Run acceptance tests with customer (XP)
5. Document test results

**Deliverables**:

- `07-verification-validation/vv-plan.md`
- `07-verification-validation/test-cases/` - Test cases
- `07-verification-validation/test-results/` - Results
- `07-verification-validation/traceability/requirements-traceability-matrix.md`

**Acceptance Testing (XP)**:

```bash
cd 07-verification-validation

# Acceptance tests use BDD (Behavior-Driven Development)
cat > test-cases/acceptance/user-registration.feature << 'EOF'
Feature: User Registration
  As a new user
  I want to register for an account
  So that I can access the system
  
  Scenario: Successful registration
    Given I am on the registration page
    When I fill in valid information
    Then I should see a success message
    And my account should be created
EOF

# Run acceptance tests
npm run test:acceptance

# Customer must approve!
```

**Traceability Verification**:

```bash
# Generate traceability report
npm run generate:traceability

# Copilot helps ensure:
# - Every requirement has tests
# - Every test traces to requirements
# - 100% coverage of critical requirements
```

---

### Phase 08: Transition (Deployment)

**When to use**: When V&V is complete

**Key Activities** (XP: Small Releases):

1. Deploy to production
2. Train users
3. Provide documentation
4. Monitor closely post-deployment

**Deliverables**:

- `08-transition/deployment-plans/production-deployment-plan.md`
- `08-transition/user-documentation/user-guide.md`
- `08-transition/user-documentation/operations-manual.md`

**Deployment Workflow**:

```bash
cd 08-transition

# Review deployment plan
code deployment-plans/production-deployment-plan.md

# XP Practice: Small, Frequent Releases
# Deploy weekly or bi-weekly, not big-bang

# Deployment process (automated):
1. All tests pass in staging
2. Security scan clean
3. Product Owner approval
4. Deploy to production (blue-green)
5. Smoke tests
6. Monitor for 1 hour
7. If issues → Rollback (<5 min)
8. If success → Celebrate! 🎉

# Copilot helps with:
# - Deployment scripts
# - User documentation
# - Operations procedures
```

---

### Phase 09: Operation & Maintenance

**When to use**: After deployment, continuously

**Key Activities**:

1. Monitor system 24/7
2. Respond to incidents
3. Perform maintenance (corrective, adaptive, perfective)
4. Continuously improve (XP: Retrospectives)

**Deliverables**:

- `09-operation-maintenance/monitoring/operational-procedures.md`
- `09-operation-maintenance/incident-response/incident-response-playbook.md`
- `09-operation-maintenance/maintenance-logs/` - Change logs

**Daily Operations**:

```bash
cd 09-operation-maintenance

# Morning checklist
- Review overnight alerts
- Check system health dashboard
- Review error logs
- Verify backups

# Incident response
- Detect → Assess → Respond → Communicate → Resolve → Document

# Continuous improvement (XP)
- Bi-weekly retrospectives
- Act on lessons learned
- Update processes
```

**XP Practice: Sustainable Pace**:

- No heroics, no death marches
- 40-hour weeks
- Prevent burnout
- Collective ownership of production

---

## Using GitHub Copilot Throughout the Lifecycle

### Phase-Specific Instructions

Copilot automatically loads phase-specific instructions based on your location:

```bash
# Working in requirements?
cd 02-requirements
# Copilot knows to:
# - Help write requirements per IEEE 29148
# - Create Given-When-Then scenarios
# - Maintain traceability

# Working in implementation?
cd 05-implementation
# Copilot knows to:
# - Suggest writing tests first (TDD)
# - Help with refactoring
# - Enforce coding standards
```

### Asking Copilot for Help

**Example Prompts**:

```
Phase 01:
"Help me identify stakeholder classes for an e-commerce system"
"Generate interview questions for product managers"

Phase 02:
"Convert this stakeholder requirement into system requirements"
"Write acceptance criteria for user login feature"
"Generate use case for checkout process"

Phase 03:
"Help me choose between microservices and monolith architecture"
"Create an ADR for database selection"
"Generate C4 context diagram for this system"

Phase 04:
"Design a class structure for order processing"
"Suggest appropriate design pattern for this problem"

Phase 05:
"Write unit tests for this function (TDD)"
"Refactor this code to improve maintainability"
"Suggest performance optimizations"

Phase 07:
"Generate test cases for this requirement"
"Create BDD scenarios for user registration"
```

---

## XP Practices Summary

Throughout all phases, follow XP practices:

| XP Practice | How to Apply | When |
|-------------|-------------|------|
| **TDD** | Write tests before code | Phase 05 (Implementation) |
| **Pair Programming** | Two developers, one computer | Complex/critical code |
| **Continuous Integration** | Integrate multiple times daily | Phase 06 (Integration) |
| **Simple Design** | YAGNI, keep it minimal | All design/implementation |
| **Refactoring** | Improve continuously | While keeping tests green |
| **Collective Ownership** | Anyone can change any code | All phases |
| **Coding Standards** | Enforce with linters | Phase 05 |
| **Sustainable Pace** | 40-hour weeks, no burnout | All phases |
| **Customer Involvement** | Customer on team | All phases |
| **Small Releases** | Deploy frequently | Phase 08 |
| **Acceptance Testing** | Customer-defined tests | Phase 07 |

---

## Quick Reference

### Standards Checklist

- [ ] **IEEE 29148**: Requirements specification format
- [ ] **IEEE 42010**: Architecture description
- [ ] **IEEE 1016**: Software design descriptions
- [ ] **IEEE 1012**: Verification & validation
- [ ] **IEEE 12207**: Overall lifecycle process

### Quality Metrics Targets

- **Test Coverage**: >80%
- **Code Complexity**: <10 (cyclomatic)
- **Documentation**: 100% of public APIs
- **Defect Density**: <1 per 1000 LOC
- **Requirements Traceability**: 100%
- **Availability**: 99.9%

---

## Getting Help

**Documentation**:

- This guide
- Phase-specific `copilot-instructions.md` in each folder
- Spec-Kit templates in `spec-kit-templates/`
- Standards references in `docs/standards-reference.md`

**Copilot**:

- Ask questions in natural language
- Copilot provides context-aware suggestions
- Copilot enforces standards automatically

**Team**:

- Pair programming
- Code reviews
- Retrospectives

---

**Remember**: Standards provide the structure, XP provides the agility. Together they enable high-quality, maintainable software delivered iteratively with customer involvement. 🚀
