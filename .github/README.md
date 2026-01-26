# .github Directory - Antares Simulator Configuration

This directory contains GitHub-related configuration and guidelines for the Antares Simulator project.

---

## 📁 Files in This Directory

### 1. **copilot-instructions.md** (Main Document)
**Purpose**: Authoritative source for all coding standards and quality requirements

**Contents**:
- Building and Testing requirements (CRITICAL)
- C++ Code Standards with C++20 features
- Modern C++ features (constexpr, consteval, [[nodiscard]])
- Naming conventions (strict adherence)
- Code formatting rules (4 spaces, 100 chars, clang-format)
- Best practices (memory, error handling, functions)
- Testing standards (Boost, coverage >80%, no decrease)
- CMake guidelines (Debug-vcpkg preset)
- Workflow checklist (18 mandatory items)
- Priority principles for conflict resolution

**Size**: 661 lines, ~21 KB

**Usage**: 
- Reference for all code generation with GitHub Copilot
- Base for code review checklists
- Guide for developer training
- Authoritative standard for the project

---

### 2. **COPILOT_USAGE_EXAMPLE.md** (Examples)
**Purpose**: Practical examples showing how to apply coding standards

**Contents**:
- 10 detailed before/after examples
- Simple Calculator class
- Data processing functions
- Memory management patterns
- Template usage
- Enum declarations
- Error handling approaches
- Unit testing structure
- CMakeLists.txt configuration
- Complete real-world example (Validator class)
- Build and test commands

**Usage**:
- Learning resource for developers
- Reference templates for new code
- Guidelines for GitHub Copilot prompts
- Code review training material

---

## 🎯 Quick Reference

### For Developers
1. Read `copilot-instructions.md` - understand the standards
2. Review `COPILOT_USAGE_EXAMPLE.md` - see practical examples
3. Apply standards to your code:
   - Use `constexpr` on all compile-time constants
   - Use `[[nodiscard]]` on all value-returning functions
   - Use `consteval` for compile-time-only functions
   - Build with `cmake --preset Debug-vcpkg`
   - Run tests: `ctest -j 4 --output-on-failure`
   - Ensure coverage maintained/increased

### For Code Review
1. Check workflow checklist in `copilot-instructions.md`
2. Verify:
   - No compiler warnings (any platform)
   - constexpr/consteval used appropriately
   - [[nodiscard]] on value-returning functions
   - Tests pass and coverage maintained
   - Naming conventions followed
   - Code formatted with clang-format
   - Memory safety (smart pointers only)
   - Error handling (exceptions or std::optional)

### For GitHub Copilot Requests
1. Reference `copilot-instructions.md` in your prompt
2. Specify coding standards to follow
3. Request examples if needed
4. Emphasize:
   - "No warnings on clang++, g++, MSVC"
   - "Use constexpr with k prefix"
   - "Mark all value-returning functions with [[nodiscard]]"
   - "Write unit tests with Boost"
   - "Ensure coverage >80%"

---

## 📋 Key Standards Summary

### MANDATORY Requirements

1. **constexpr** on ALL compile-time constants
   ```cpp
   constexpr uint16_t kBufferSize = 255;  // ✅ REQUIRED
   ```

2. **[[nodiscard]]** on ALL value-returning functions
   ```cpp
   [[nodiscard]] bool isValid() const { return valid_; }  // ✅ REQUIRED
   ```

3. **consteval** for compile-time-only functions
   ```cpp
   consteval uint32_t computeHash(std::string_view);  // ✅ REQUIRED
   ```

4. **Zero Warnings** - Non-negotiable
   - No warnings on clang++, g++, or MSVC

5. **All Tests Pass** - Before submission
   - Coverage >80% on new code
   - Coverage maintained or increased
   - Command: `ctest -j 4 --output-on-failure`

6. **Build Success** - Immediate after changes
   - Command: `cmake --preset Debug-vcpkg && cmake --build --preset Debug-vcpkg`
   - Fix errors before proceeding

---

## 🚀 Workflow

### Before Starting Development
```bash
# Clone the repository and set up build
cd Projects
cmake --preset Debug-vcpkg
cmake --build --preset Debug-vcpkg
```

### During Development
```bash
# After each change, immediately:
cmake --build --preset Debug-vcpkg  # Fix errors
clang-format -i src/myfile.cpp      # Format code
ctest -j 4                           # Run tests
```

### Before Submission
1. ✅ Code formatted: `clang-format -i file.cpp`
2. ✅ All tests pass: `ctest -j 4 --output-on-failure`
3. ✅ Coverage maintained: baseline vs. current
4. ✅ Zero compiler warnings
5. ✅ Naming conventions followed
6. ✅ constexpr/consteval applied
7. ✅ [[nodiscard]] on value-returns
8. ✅ Smart pointers used (no raw pointers)
9. ✅ Comments complete (full sentences)
10. ✅ Includes properly ordered

---

## 📚 Additional Resources

### In This Repository
- `docs/developer-guide/6-Contributing.md` - Project-specific practices
- `src/.clang-format` - Formatting configuration (18.x reference)
- `src/CMakePresets.json` - Build presets
- `src/format-code.sh` - Auto-format script
- `tests/macros.cmake` - Test configuration

### External References
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) - Base reference
- [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) - Modern C++ practices
- [Boost Unit Test Framework](https://www.boost.org/doc/libs/release/libs/test/) - Testing

---

## 🔍 Important Notes

### Authority
- `copilot-instructions.md` is the **authoritative source** for code quality
- Updates require team consensus
- All code must align with these standards before merging

### Quality Philosophy
When requirements conflict, prioritize in this order:
1. **No Warnings** - Zero compiler warnings (non-negotiable)
2. **Readability** - Code clarity and maintainability
3. **Performance** - Optimize only when necessary
4. **Brevity** - Don't sacrifice clarity
5. **Convention** - Follow project patterns

### Code Quality
- "Good enough" is not acceptable
- State-of-the-art quality standards
- Modern C++20 practices required
- Comprehensive test coverage required
- Zero technical debt in new code

---

## ✨ Getting Started

### Step 1: Read Documentation
1. Start with `copilot-instructions.md` overview
2. Review sections relevant to your task
3. Check `COPILOT_USAGE_EXAMPLE.md` for examples

### Step 2: Understand Requirements
- C++ Standard: C++20 minimum
- Build Preset: Debug-vcpkg
- Testing: Boost Unit Test Framework
- Coverage: >80% on new code
- Warnings: Zero on all compilers

### Step 3: Set Up Environment
```bash
cmake --preset Debug-vcpkg
cmake --build --preset Debug-vcpkg
```

### Step 4: Write Code
- Apply naming conventions
- Use constexpr/consteval appropriately
- Mark functions with [[nodiscard]]
- Write comprehensive tests
- Format with clang-format

### Step 5: Validate
- Build: `cmake --build --preset Debug-vcpkg`
- Test: `ctest -j 4 --output-on-failure`
- Check: No compiler warnings
- Format: `clang-format -i yourfile.cpp`

### Step 6: Submit
- Verify workflow checklist
- Ensure coverage maintained
- Zero warnings on all compilers
- All tests passing

---

## 📞 Questions?

If you have questions about coding standards:
1. Check `copilot-instructions.md` first
2. Review examples in `COPILOT_USAGE_EXAMPLE.md`
3. Ask the development team before starting code

---

## 📝 Version History

| Date | Description |
|------|-------------|
| 2026-01-26 | Initial comprehensive documentation created |

---

**Last Updated**: January 26, 2026  
**Status**: Complete ✅  
**Authority**: Authoritative for Antares Simulator project
