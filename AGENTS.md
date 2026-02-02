# AGENTS.md - Guidelines for Coding Agents

This document provides essential information for agentic coding agents working on the Antares Simulator codebase.

## Build Commands

```bash
# Configure build (from src/ directory)
cmake -B ../build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -B ../build-release -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build ../build-debug -j$(nproc)
cmake --build ../build-release -j$(nproc)

# Build specific target (e.g., solver)
cmake --build ../build-debug --target antares-solver
```

## Test Commands

```bash
# Run all tests using ctest
cd build-debug && ctest --output-on-failure
cd build-debug && ctest -R "test_name" --output-on-failure

# Run specific Boost.Test binary (from build-debug directory)
./tests/unit_tests_antares/optimization_options_test

# Run Python pytest tests
cd src/tests/run-study-tests
python -m pytest -m json --antares-simu-path=<path_to_solver>
python -m pytest test_file.py::test_function_name

# Run single test via ctest with regex
ctest -R "OptimizationOptionsTests" --output-on-failure -V
```

## Code Style & Formatting

**Auto-formatting:**
```bash
# Format all code
cd src && ./format-code.sh

# Format specific files
clang-format -i -style=file:../.clang-format file.cpp file.hpp

# Reference clang-format version: 18.1.3
```

**Critical Formatting Rules:**
- 4-space indentation, NO tabs
- 100 character line limit
- Allman brace style (braces on new lines)
- Braces ALWAYS required for if/for/while/switch (even single-line)
- Space after keywords: `if (condition)`, `for (int i = 0; ...)`
- Space after semicolon in for loops
- Spaces between binary operators: `x = y * y + z * z`
- No spaces in parentheses, square brackets, or angles
- Left-aligned pointers/references: `const string& s`, `int* ptr`
- `#pragma once` for header guards
- `constexpr` for compile-time constants

## Naming Conventions

- **Files:** CamelCase.cpp, CamelCase.h (new files)
- **Functions:** lowerCamelCase() - `isPrime(unsigned int n)`
- **Classes:** CamelCase - `ComplexClass`
- **Constants:** kCamelCase - `constexpr int kBufferSize = 255`
- **Enums:** enum class with CAPITAL values - `enum class Color { RED, GREEN };`
- **Namespaces:** lowercase - `namespace lower_case {}`
- **Macros:** UPPER_CASE (avoid when possible)

## Include Ordering

1. Current directory header: `"current_file.h"`
2. Local includes (sorted by dependency: indexer, coding, base)
3. `"defines.h"`
4. C++ standard library headers: `<vector>`, `<string>`, etc.
5. Boost headers: `<boost/...>`
6. Yuni headers: `<yuni/...>` (avoid for new code)
7. Antares headers: `"antares/..."` or `<antares/...>`

Use blank lines between groups. Use `#include ""` for local headers, `#include <>` for system headers.

## Language & Types

- **Standard:** C++20 strictly required
- **Encoding:** UTF-8
- **Use exceptions:** Derive from `std::exception`
- **Typedef:** Use `using` instead of `typedef`
- **Smart pointers:** Prefer `std::unique_ptr`, `std::shared_ptr`
- **Enum classes:** Preferred over C-style enums (implicit conversions are dangerous)
- **Avoid:** Hungarian notation, global/static variables (threading issues)
- **Prefer:** C++20 types over Boost counterparts
- **Avoid boost libraries requiring linking** (prefer C++20 standard)

## Error Handling & Logging

```cpp
// Logging levels - Use Antares::logs
Antares::logs.fatal()   // DON'T USE (use error instead)
Antares::logs.error()   // Errors causing simulation to stop
Antares::logs.warning() // Non-critical issues, simulation continues
Antares::logs.info()    // Information for users
Antares::logs.debug()   // Debugging info for developers
```

Define custom exceptions derived from `std::exception`. DO NOT use `logs.fatal()` - use `logs.error()` instead.

## Best Practices

- Mark non-derived classes as `final`
- Always check warnings on clang++, g++, MSVC
- Write unit tests covering new functionality (see examples in `src/tests/libs/`)
- Test on both Ubuntu and Windows platforms
- Update documentation when modifying documented features
- Use `const` and `constexpr` where appropriate
- Avoid one-line control flow statements for easier debugging
- Use `// TODO(DeveloperName): need to fix it` for incomplete work
- Avoid Yuni framework for new code (it's legacy)

## Branch Naming (CI requirements)

Use these patterns for CI to run:
- `feature/*` or `features/*`
- `fix/*`
- `release/*`
- `issue-*`
- `doc/*`
- `tests/*`

Enable git hooks: `git config core.hooksPath .githooks`

## Key Directories

- `src/libs/` - Core libraries
- `src/solver/` - Simulation and optimization
- `src/tools/` - Dataset management tools
- `src/api/` - API components
- `src/tests/` - Test suites (Boost.Test + pytest)
- `docs/` - Documentation source

## Pull Request Guidelines

- PR name must be self-explanatory (becomes commit title)
- Include description: motivation (why) and method (how)
- Mark as draft if not ready for review
- Set PR to ready for review when complete
