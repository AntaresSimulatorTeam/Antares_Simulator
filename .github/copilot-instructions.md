# GitHub Copilot Instructions for Antares Simulator

## Overview
This file specifies comprehensive coding standards and practices for the Antares Simulator project. All generated code must follow these guidelines to maintain state-of-the-art code quality, consistency, and project integrity.

---

## Building and Testing (CRITICAL)

### Build Process - Always Execute After Changes
- **Always use the CMake preset**: `Debug-vcpkg`
- Build command: `cmake --preset Debug-vcpkg && cmake --build --preset Debug-vcpkg`
- All code must build **without any warnings** on `clang++`, `g++`, and `MSVC`
- **After EVERY code change, IMMEDIATELY build and fix errors before proceeding**
- No incomplete code or "to-be-fixed-later" commits

### Testing - Non-Negotiable
- **All tests must pass** before code is considered complete
- Build and run tests with coverage enabled
- Coverage metrics must stay the same or **increase** with new code
- Use Boost Unit Test Framework for all unit tests
- Test executable pattern: `add_boost_test(test_name SRC file.cpp LIBS lib1 lib2)`
- Run tests: `ctest -j 4 --output-on-failure` from build directory

### Coverage Requirements - Mandatory
- Enable coverage in build: Add `--coverage` compiler flag or use appropriate coverage tools
- **Coverage must NOT decrease** with new contributions
- **Aim for high coverage on new and modified code (>80% minimum)**
- Include integration tests where appropriate using Boost Test with `LABELS integ` property
- Baseline coverage must be measured before changes and verified after

---

## C++ Code Standards (State-of-the-Art Quality)

### Language Standard
- **Required Standard**: C++20 minimum
- Use all C++20 and C++17 features appropriately and intentionally
- Compile with C++20 compiler: `clang++`, `g++`, or `MSVC`
- All code must compile without warnings on all supported compilers

### Modern C++ Features - MANDATORY Usage

#### `constexpr` - Required for Compile-Time Constants
Use `constexpr` for ALL compile-time constant expressions. This is NOT optional.

```cpp
// CORRECT - compile-time evaluation
constexpr uint16_t kBufferSize = 255;
constexpr uint32_t kMaxRetries = 3;
constexpr double kPi = 3.14159265359;

// For static arrays and sizes
constexpr size_t kArraySize = 1024;

// For methods that can be evaluated at compile-time
class Vector
{
public:
    constexpr Vector(double x, double y) : x_(x), y_(y) {}
    [[nodiscard]] constexpr double getX() const { return x_; }
    [[nodiscard]] constexpr double getY() const { return y_; }
private:
    double x_;
    double y_;
};
```

#### `consteval` - For Compile-Time Only Functions
Use `consteval` for functions that MUST be evaluated at compile-time.

```cpp
// This function ONLY exists at compile-time
consteval uint32_t computeHash(std::string_view str)
{
    // Algorithm that must complete at compile-time
}

// Usage
constexpr auto hash = computeHash("key");  // Evaluated at compile-time
```

#### `[[nodiscard]]` - REQUIRED on All Value-Returning Functions
Mark ALL functions that return important values that should not be ignored. This is a quality requirement.

```cpp
// CORRECT - all of these are marked [[nodiscard]]
[[nodiscard]] bool isValid() const { return valid_; }
[[nodiscard]] int calculate() const { return result_; }
[[nodiscard]] std::optional<Value> tryParse(std::string_view str);
[[nodiscard]] std::unique_ptr<Resource> createResource();

// Even small helper functions
[[nodiscard]] bool isEmpty() const { return size_ == 0; }
[[nodiscard]] size_t getCount() const { return count_; }

// For getters
[[nodiscard]] const std::string& getName() const { return name_; }

// INCORRECT - should not compile
void calculateValue()  // Returns nothing, fine
{
}

// INCORRECT - missing [[nodiscard]]
// int getValue() const { return value_; }
```

#### Other C++20/17 Features - Preferred Over Legacy Patterns
- Use `std::optional<T>` instead of pointers for optional values
- Use `std::variant<T>` for type-safe unions
- Use `std::string_view` for non-owning string references (NOT `const std::string&` for parameters)
- Use structured bindings: `auto [x, y] = getCoordinates();`
- Use if-with-initializer: `if (auto result = compute(); result.has_value())`
- Use range-based for loops: `for (const auto& item : items)`
- Use `auto` type deduction where type is obvious from context
- Use `std::unique_ptr` and `std::shared_ptr` exclusively

### Naming Conventions - Strict Adherence

#### Constants
- Compile-time constants: `lowerCamelCase` with `k` prefix
  - Example: `kMaxBufferSize`, `kDefaultTimeout`, `kPi`, `kMinValue`
- ALWAYS mark with `constexpr` keyword
- Runtime constants use `UPPER_CASE` only if absolutely necessary (prefer `constexpr`)

#### Functions
- Free and member functions: `lowerCamelCase`
  - Example: `isPrime()`, `calculateSum()`, `validateInput()`, `processData()`
  - Verb-first naming for actions: `generate`, `compute`, `fetch`, `validate`

#### Classes and Types
- Class names: `PascalCase`
  - Example: `DataValidator`, `NetworkSocket`, `SolverEngine`
- Type aliases: `PascalCase` starting with `T` or ending with `Type`
  - Example: `using TComplexNumber = double;`, `using NodeId = uint32_t;`
  - Or: `using ConfigType = json;`

#### Enumerations
- Enum class values: `UPPER_CASE` (required for type safety)
  ```cpp
  enum class Color { RED, GREEN, LIGHT_BLUE };
  enum class Status { IDLE, RUNNING, STOPPED, ERROR };
  ```
- Old-style C enums: **Avoid completely**. If required for legacy compatibility:
  - Use `UPPER_CASE` with enum prefix
  - `enum Type { TYPE_INTEGER, TYPE_FLOAT, TYPE_STRING };`
- **Always prefer `enum class` over unscoped enums**

#### Variables
- Local/member variables: `lowerCamelCase`
  - Example: `bufferSize`, `userCount`, `isActive`, `currentIndex`
- Pointers/References: No prefix (STRICT - avoid Hungarian Notation)
  - CORRECT: `const std::string& name`, `std::unique_ptr<Resource> resource`
  - INCORRECT: `pName`, `psName`, `ptrResource`, `refValue`

### Code Formatting - Strict Compliance

#### General Rules
- **Indentation**: 4 spaces (never tabs - this is enforced)
- **Line width**: 100 characters (enforced via `.clang-format`)
- **Whitespace**: No trailing whitespace
- **Tool**: Use `clang-format` version 18.x (reference: 18.1.3)
- **Auto-format BEFORE submission**: `clang-format -i file.cpp file.hpp`
- **Batch format script**: Use `./src/format-code.sh` for entire project
- **IDE Integration**: Configure your IDE to auto-format on save (strongly recommended)

#### Include Order - Must Follow Exactly
1. Current file header: `#include "current_dir/current_file.h"`
2. Related includes from other directories (sorted by dependency depth)
3. Project internal headers: `#include "defines.h"`
4. C++ standard library: `#include <vector>`, `#include <string>`, `#include <algorithm>`
5. Third-party libraries: `#include <boost/...>`, `#include <fmt/...>`
6. **Blank lines between each group**

Example:
```cpp
#pragma once

#include "utils/Validation.h"
#include "utils/Logger.h"

#include "antares/solver/Solver.h"
#include "antares/study/Study.h"
#include "antares/study/Load.h"

#include "defines.h"

#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include <boost/numeric/ublas/matrix.hpp>
#include <fmt/format.h>
```

#### Header Files
- Use `#pragma once` (not `#define` guards)
- Place at the very beginning of header file
- Always end with newline

```cpp
#pragma once

// ... content ...

```

#### Spacing and Operators
- Space after keywords: `if (condition)`, `for (size_t i = 0; ...)`, `while (true)`
- Space around binary operators: `x = y * 2 + z`
- No space between unary operators and operands: `++i`, `!condition`, `-value`, `*ptr`
- Space after commas: `func(a, b, c)`
- Space after `//` in comments: `// This is a comment`
- Space before opening brace in functions: `void func() {` (not `void func(){`)

#### Control Flow
```cpp
// Always use braces, even for single statements
// This is REQUIRED for easier debugging
if (condition)
{
    doSomething();
}
else
{
    doOtherThing();
}

for (size_t i = 0; i < size; ++i)
{
    processItem(i);
}

while (condition)
{
    iterate();
}

// Switch formatting
switch (value)
{
case 1:
    handle1();
    break;
case 2:
{
    auto result = compute();
    handle2(result);
    break;
}
default:
    handleDefault();
    break;
}

// Do NOT use one-liners: if (condition) doSomething();  <- WRONG
```

#### Class Definition
```cpp
class MyClass
{
public:
    // Constructor with initializer list
    MyClass(int value, std::string_view name):
        value_(value),
        name_(name)
    {
    }

    // Destructor if needed (prefer RAII)
    ~MyClass() = default;

    // Deleted copy/move constructors if needed
    MyClass(const MyClass&) = delete;
    MyClass& operator=(const MyClass&) = delete;

    // Mark const methods and use [[nodiscard]]
    [[nodiscard]] int getValue() const
    {
        return value_;
    }

    [[nodiscard]] std::string_view getName() const
    {
        return name_;
    }

    // Mutating method
    void setValue(int newValue)
    {
        value_ = newValue;
    }

private:
    int value_;
    std::string name_;
};
```

#### Templates
```cpp
// Generic template function with [[nodiscard]]
template<typename T>
[[nodiscard]] T maximum(T a, T b)
{
    return a > b ? a : b;
}

// Template with constraints (C++20 concepts preferred where applicable)
template<typename Container>
void processAll(Container& items)
{
    for (auto& item : items)
    {
        item.process();
    }
}

// Class template
template<typename T, typename Allocator = std::allocator<T>>
class MyContainer
{
    // ...
};
```

### Best Practices - Quality Standards

#### Memory Management - Strictly Enforced
- **Prefer stack allocation** when possible
- Use `std::unique_ptr<T>` for exclusive ownership (default choice)
- Use `std::shared_ptr<T>` for shared ownership (document why it's needed)
- **Avoid raw `new`/`delete`** completely
- Use RAII for resource management
- No memory leaks allowed (verify with memory analysis tools)

```cpp
// CORRECT
std::unique_ptr<Resource> resource = std::make_unique<Resource>(args);
std::shared_ptr<Data> data = std::make_shared<Data>(args);

// INCORRECT
Resource* resource = new Resource();  // NEVER do this
delete resource;                       // NEVER do this
```

#### Error Handling
- Use exceptions for error conditions (derived from `std::exception`)
- Use `std::optional<T>` for optional return values
- Use `std::variant<T, Error>` for result types
- Use asserts for programming errors: `assert(ptr != nullptr);`
- Never use exceptions for control flow

```cpp
// CORRECT
[[nodiscard]] std::optional<int> parseNumber(std::string_view str)
{
    // Return std::optional
}

void riskyOperation()
{
    throw std::invalid_argument("Input is invalid");
}

// Usage
if (auto result = parseNumber(input))
{
    // Use result.value()
}
```

#### Functions - State of the Art
- **NEVER use one-line `if` statements without braces** (required for debugging clarity)
- Use `constexpr` when function can be evaluated at compile-time
- Mark with `[[nodiscard]]` if return value matters
- Use default parameters instead of function overloads when reasonable
- Mark methods `const` when they don't modify state
- Use `std::string_view` for string parameters (not `const std::string&`)
- Avoid unused parameters; if unavoidable, comment them: `int unused /* param_name */`

```cpp
// CORRECT
[[nodiscard]] constexpr int fibonacci(int n)
{
    return n <= 1 ? n : fibonacci(n-1) + fibonacci(n-2);
}

[[nodiscard]] std::string formatOutput(std::string_view format, int value);

// INCORRECT
int fibonacci(int n) { return n <= 1 ? n : fibonacci(n-1) + fibonacci(n-2); }  // Not constexpr
std::string formatOutput(const std::string& format, int value);  // Should use string_view
```

#### Global/Static Variables - Discouraged
- **Avoid global variables completely**
- Use file-scope static or anonymous namespace for internal linkage
- Use `constexpr` for compile-time constants
- Use inline functions with static variables for lazy initialization if absolutely needed
- Prefer dependency injection

```cpp
// In .cpp file - ACCEPTABLE for internal use only
namespace
{
    constexpr int kInternalMax = 100;  // Correct
    static int gCounter = 0;           // Avoid if possible
}

// BETTER - use class static or const
class MyClass
{
    static constexpr int kInternalMax = 100;
};
```

#### Comments and Documentation - Full Sentences Required
- Use complete, grammatically correct sentences: `// Calculate the average value.` (not `// calc avg`)
- Space after `//`: proper formatting required
- Use Doxygen-style comments for all public APIs

```cpp
/// \brief Compute the sum of all elements.
/// 
/// This function iterates through all elements and accumulates their values.
/// \param items The collection to sum. Must not be empty.
/// \return The total sum of all elements.
/// \throws std::invalid_argument If items is empty.
[[nodiscard]] double computeSum(const std::vector<double>& items);

// For incomplete work (must be fixed)
// TODO(DeveloperName): Optimize this loop to use SIMD operations.
```

#### Logging
- Use project logging system (avoid `std::cout` in production code)
- Use appropriate log levels: DEBUG, INFO, WARNING, ERROR
- Log important state transitions and errors only

#### Code Quality - Zero Tolerance
- **Avoid macros completely**; use `const` or `constexpr` instead
- **Avoid old-style C enums**; use `enum class` exclusively
- **Avoid implicit type conversions**
- **Enable compiler warnings and treat ALL as errors** (no warnings accepted)
- **No compiler warnings on any platform** (clang++, g++, MSVC)
- **Remove dead code immediately** - never leave commented-out code
- Use static analysis tools and address all warnings
- Code must be self-documenting with clear, expressive names

#### Type Safety
- Use type aliases for clarity and intent: `using UserId = uint32_t;`
- **Avoid `void*` pointers** completely
- Use `static_cast`, `dynamic_cast` explicitly (never C-style casts)
- Prefer explicit constructors unless copy/move semantics require otherwise
- Use `final` on classes when inheritance is not intended

---

## Testing Standards - Comprehensive

### Unit Testing Framework
- Use Boost Unit Test Framework for all tests
- Required test structure:

```cpp
#define BOOST_TEST_MODULE MyComponentTests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_basic_functionality)
{
    // Arrange
    int input = 5;
    
    // Act
    int result = calculate(input);
    
    // Assert
    BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_CASE(test_error_condition)
{
    BOOST_CHECK_THROW(riskyOperation(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_with_tolerance)
{
    double actual = computeValue();
    BOOST_TEST(actual == expected, boost::test_tools::tolerance(0.0001));
}
```

### Test File Organization
```cmake
# In CMakeLists.txt (use provided macro)
include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test_myfeature
    SRC 
        test_myfeature.cpp
        test_myfeature_edge_cases.cpp
    INCLUDE 
        "${CMAKE_SOURCE_DIR}/libs/mylib"
    LIBS 
        Antares::mylib 
        test_utils_unit
)
```

### Test Coverage - Mandatory
- Measure baseline coverage before changes: `lcov --capture ...`
- Ensure new code has unit tests
- Aim for >80% coverage on new/modified code
- **Coverage must NOT decrease** with patches
- Use `ctest --rerun-failed` to diagnose failures

### Test Execution - Before Submission
```bash
# 1. Build with Debug-vcpkg preset
cmake --preset Debug-vcpkg
cmake --build --preset Debug-vcpkg

# 2. Run ALL tests with output
ctest -j 4 --output-on-failure

# 3. Check coverage (if tool available)
# Use gcov, lcov, or IDE integration

# 4. If tests fail, debug and fix IMMEDIATELY
```

---

## CMake Guidelines

### Preset-based Building - MANDATORY
- **Primary preset**: `Debug-vcpkg` (REQUIRED)
- Use CMake presets for configuration: See `src/CMakePresets.json`
- Never build without presets; ensures vcpkg consistency
- Specify vcpkg triplet in CMakeUserPresets.json if platform-specific

### Target Configuration
```cmake
# Add library with proper settings
add_library(mylib source1.cpp source2.cpp)

# Link dependencies
target_link_libraries(mylib 
    PUBLIC 
        dependency_public
    PRIVATE 
        dependency_internal
)

# Include directories
target_include_directories(mylib 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set properties
set_target_properties(mylib PROPERTIES
    FOLDER "Libraries"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET hidden
)
```

### Compiler Flags
- Use target-specific properties (not global flags)
- C++ Standard: **Always set `set(CMAKE_CXX_STANDARD 20)`**
- Add warnings: `target_compile_options(target PRIVATE -Wall -Wextra -Werror -Wpedantic)`
- Enable coverage for testing: `-fprofile-arcs -ftest-coverage`

---

## Project Structure

### Directory Organization
```
Projects/
├── src/
│   ├── CMakeLists.txt           # Main build configuration
│   ├── CMakePresets.json        # Build presets
│   ├── .clang-format            # Formatting rules (reference 18.x)
│   ├── format-code.sh           # Auto-format script
│   ├── libs/                    # Shared libraries
│   ├── solver/                  # Solver implementation
│   ├── api/                     # API implementation
│   ├── tests/                   # Unit test suite
│   └── ...other components
├── docs/                        # Documentation
├── vcpkg/                       # Dependency management
└── .github/
    └── copilot-instructions.md  # This file (AUTHORITATIVE)
```

---

## Workflow Checklist - MANDATORY

Before code is ready for submission, verify ALL items:

- [ ] **CONSTEXPR**: All compile-time constants use `constexpr` with `k` prefix
- [ ] **CONSTEVAL**: Compile-time-only functions marked `consteval` 
- [ ] **[[nodiscard]]**: All value-returning functions marked (CRITICAL)
- [ ] **Formatting**: Code formatted with `clang-format -i` 
- [ ] **NO WARNINGS**: Zero warnings on gcc, clang, MSVC
- [ ] **Tests Written**: Unit tests cover new functionality
- [ ] **Tests Pass**: `ctest --output-on-failure` shows all passing
- [ ] **Coverage**: Coverage maintained or improved (measure with baseline)
- [ ] **Build Success**: `cmake --preset Debug-vcpkg && cmake --build --preset Debug-vcpkg` succeeds
- [ ] **Includes**: Proper ordering and grouping per guidelines
- [ ] **#pragma once**: All headers have `#pragma once` (not ifdef guards)
- [ ] **Comments**: Complete sentences with proper spacing
- [ ] **Memory**: No raw pointers; use smart pointers exclusively
- [ ] **No Globals**: Minimize global/static variables
- [ ] **Type Safety**: No C-style casts; use `static_cast`, `dynamic_cast`
- [ ] **Naming**: Follow naming conventions (PascalCase classes, lowerCamelCase functions)
- [ ] **Error Handling**: Exceptions or `std::optional`, never error codes

---

## Additional Resources

- **Google C++ Style Guide**: https://google.github.io/styleguide/cppguide.html (base reference)
- **C++ Core Guidelines**: https://github.com/isocpp/CppCoreGuidelines (modern C++)
- **Boost Documentation**: https://www.boost.org/doc/ (testing, utilities)
- **Antares Contributing**: See `CONTRIBUTING.md` in repo root
- **Antares Developer Guide**: See `docs/developer-guide/6-Contributing.md`
- **clang-format Config**: See `src/.clang-format` (canonical formatting)

---

## Priority Principles

When requirements seem to conflict, apply in this order:

1. **No Warnings** - Zero compiler warnings is non-negotiable
2. **Readability** - Code must be clear and maintainable
3. **Performance** - Optimize only when necessary
4. **Brevity** - Don't sacrifice clarity for fewer lines
5. **Convention** - Follow project patterns

---

## Final Notes

- **This file is the authoritative source** for code quality in Antares Simulator
- Updates to this file require team consensus
- All new code must align with these standards before merging
- "Good enough" is not acceptable - aim for excellence
- If you have any questions, ask before writing code
