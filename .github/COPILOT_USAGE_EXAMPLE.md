# GitHub Copilot Usage Examples

This file shows how to apply the coding standards from `copilot-instructions.md` with concrete examples.

---

## Example 1: Simple Calculator Class

### ❌ INCORRECT (Before)
```cpp
#include <iostream>
#include <string>

// Wrong: missing constexpr, missing [[nodiscard]], wrong naming
class calculator
{
public:
    int Add(int a, int b)
    {
        return a + b;
    }
    
    int Sub(int a, int b)
    {
        return a - b;
    }
    
private:
    int BUFFER_SIZE = 255;  // Wrong: not const/constexpr, wrong naming
};
```

### ✅ CORRECT (After)
```cpp
#pragma once

#include <cstdint>

// Correct: class name PascalCase, proper member naming
class Calculator
{
public:
    // Constructor with initializer list
    Calculator() = default;
    
    // Correct: [[nodiscard]], constexpr, lowerCamelCase, k-prefix constant
    [[nodiscard]] constexpr int add(int a, int b) const
    {
        return a + b;
    }
    
    // Correct: [[nodiscard]], constexpr
    [[nodiscard]] constexpr int subtract(int a, int b) const
    {
        return a - b;
    }
    
private:
    // Correct: constexpr with k prefix, lowerCamelCase member variables
    static constexpr uint16_t kBufferSize = 255;
};
```

---

## Example 2: Data Processing Function

### ❌ INCORRECT (Before)
```cpp
// Missing [[nodiscard]], no error handling, wrong naming
std::string process_data(const std::string& input)
{
    // Warning: const std::string& should be std::string_view
    if (input.empty()) return "";  // One-liner, no braces
    return input + "_processed";
}

// Not tested, no constexpr where possible
int getValue() { return 42; }
```

### ✅ CORRECT (After)
```cpp
#pragma once

#include <string>
#include <string_view>
#include <optional>

// Correct: [[nodiscard]], std::string_view, proper error handling
[[nodiscard]] std::optional<std::string> processData(std::string_view input)
{
    // Correct: always use braces for clarity
    if (input.empty())
    {
        return std::nullopt;  // Better than returning empty string
    }
    
    // Correct: use auto where type is obvious
    auto result = std::string(input) + "_processed";
    return result;
}

// Correct: [[nodiscard]], constexpr where possible
[[nodiscard]] constexpr int getValue()
{
    return 42;
}
```

---

## Example 3: Memory Management

### ❌ INCORRECT (Before)
```cpp
// Raw pointers, manual memory management
class DataManager
{
public:
    void initialize()
    {
        data_ = new double[1024];  // NEVER: raw new
    }
    
    ~DataManager()
    {
        delete[] data_;  // NEVER: raw delete
    }
    
private:
    double* data_;  // NEVER: raw pointer
};
```

### ✅ CORRECT (After)
```cpp
#pragma once

#include <memory>
#include <vector>

// Correct: use smart pointers, proper RAII
class DataManager
{
public:
    DataManager() : data_(1024)
    {
        // Smart pointer initialization via constructor
    }
    
    // Correct: no manual memory management needed
    ~DataManager() = default;
    
    // Correct: use const reference and [[nodiscard]]
    [[nodiscard]] const std::vector<double>& getData() const
    {
        return data_;
    }
    
private:
    // Correct: use std::vector (stack allocated if possible)
    std::vector<double> data_;
};
```

---

## Example 4: Template with Constraints

### ❌ INCORRECT (Before)
```cpp
// Missing [[nodiscard]], no constexpr, wrong formatting
template<typename T>
T maximum(T a, T b)
{
    if (a > b) return a; else return b;  // One-liner, no braces
}
```

### ✅ CORRECT (After)
```cpp
#pragma once

// Correct: [[nodiscard]], constexpr, proper formatting
template<typename T>
[[nodiscard]] constexpr T maximum(T a, T b)
{
    // Correct: always use braces
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}
```

---

## Example 5: Enum Usage

### ❌ INCORRECT (Before)
```cpp
// Old-style enum with implicit conversions
enum Color
{
    Red = 0,
    Green = 1,
    Blue = 2
};

// Can implicitly convert to int
int x = Red;  // Dangerous!
```

### ✅ CORRECT (After)
```cpp
#pragma once

// Correct: enum class, UPPER_CASE values
enum class Color
{
    RED,      // Correct: UPPER_CASE
    GREEN,    // Correct: UPPER_CASE
    BLUE      // Correct: UPPER_CASE
};

// Correct: explicit, type-safe
[[nodiscard]] constexpr Color getColor()
{
    return Color::RED;  // Type-safe!
}
```

---

## Example 6: Error Handling

### ❌ INCORRECT (Before)
```cpp
// Error code return (fragile, easy to ignore)
int parseNumber(const char* str, int& result)
{
    if (!str) return -1;
    if (strlen(str) == 0) return -2;
    result = atoi(str);
    return 0;
}

// Usage: caller must check return code
int value;
if (parseNumber("123", value) != 0)
{
    // handle error
}
```

### ✅ CORRECT (After)
```cpp
#pragma once

#include <optional>
#include <string_view>

// Correct: [[nodiscard]], std::optional for safe error handling
[[nodiscard]] std::optional<int> parseNumber(std::string_view str)
{
    // Correct: validate input
    if (str.empty())
    {
        return std::nullopt;
    }
    
    // Correct: use proper parsing
    try
    {
        auto result = std::stoi(std::string(str));
        return result;
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

// Usage: clear and safe
if (auto result = parseNumber("123"))
{
    // Use result.value()
}
```

---

## Example 7: Unit Test

### ❌ INCORRECT (Before)
```cpp
// No proper testing structure
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_add)
{
    int x = 5;
    int y = 3;
    int z = x + y;
    BOOST_TEST(z == 8);
}
```

### ✅ CORRECT (After)
```cpp
// Correct: proper test structure with setup/act/assert
#define BOOST_TEST_MODULE CalculatorTests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_add_positive_numbers)
{
    // Arrange
    int firstOperand = 5;
    int secondOperand = 3;
    int expectedResult = 8;
    
    // Act
    auto calculator = Calculator();
    int actualResult = calculator.add(firstOperand, secondOperand);
    
    // Assert
    BOOST_TEST(actualResult == expectedResult);
}

BOOST_AUTO_TEST_CASE(test_add_negative_numbers)
{
    BOOST_TEST(Calculator().add(-5, -3) == -8);
}

BOOST_AUTO_TEST_CASE(test_add_zero)
{
    BOOST_TEST(Calculator().add(0, 0) == 0);
}
```

---

## Example 8: CMakeLists.txt

### ❌ INCORRECT (Before)
```cmake
add_library(mylib mylib.cpp)
# Missing configuration, warnings, standards
```

### ✅ CORRECT (After)
```cmake
# Correct: proper target configuration
add_library(mylib 
    src/mylib.cpp
    src/helper.cpp
)

# Correct: set C++ standard to 20
set_target_properties(mylib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    FOLDER "Libraries"
)

# Correct: add compiler warnings
target_compile_options(mylib PRIVATE 
    -Wall -Wextra -Werror -Wpedantic
)

# Correct: link required libraries
target_link_libraries(mylib 
    PUBLIC 
        Boost::headers
    PRIVATE 
        fmt::fmt
)

# Correct: include directories
target_include_directories(mylib 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

---

## Example 9: Comments and Documentation

### ❌ INCORRECT (Before)
```cpp
// calc avg  <- Not a complete sentence
int calcAvg(int a, int b)
{
    return (a + b) / 2;
}

//TODO fix this  <- Incomplete TODO comment
void process()
{
}
```

### ✅ CORRECT (After)
```cpp
/// \brief Calculate the average of two integers.
/// 
/// This function computes the arithmetic mean of two input values.
/// For odd sums, the result is truncated (integer division).
/// 
/// \param a The first operand.
/// \param b The second operand.
/// \return The average of a and b.
/// 
/// \note Integer division may lose precision.
[[nodiscard]] constexpr int calculateAverage(int a, int b)
{
    return (a + b) / 2;
}

// TODO(DeveloperName): Replace with floating-point calculation for better precision.
void process()
{
}
```

---

## Example 10: Complete Example - Validator Class

### ✅ CORRECT COMPLETE EXAMPLE

**Header File: `Validator.h`**
```cpp
#pragma once

#include <string_view>
#include <optional>
#include <stdexcept>

/// \brief Validates and processes user input.
class Validator
{
public:
    // Constructor
    Validator() = default;
    
    // Destructor
    ~Validator() = default;
    
    // Deleted copy/move to prevent unintended sharing
    Validator(const Validator&) = delete;
    Validator& operator=(const Validator&) = delete;
    
    /// \brief Validate if a string represents a valid integer.
    /// \param input The input string to validate.
    /// \return The parsed integer, or std::nullopt if invalid.
    [[nodiscard]] std::optional<int> validateInteger(std::string_view input) const;
    
    /// \brief Check if email format is valid.
    /// \param email The email address to validate.
    /// \return true if valid, false otherwise.
    [[nodiscard]] bool isValidEmail(std::string_view email) const;
    
    /// \brief Get the validation error message.
    /// \return The last error message.
    [[nodiscard]] std::string_view getLastError() const
    {
        return lastError_;
    }
    
private:
    // Compile-time constants with k prefix
    static constexpr std::string_view kEmailPattern = ".*@.*\\..*";
    
    // Member variables
    mutable std::string lastError_;
    
    /// \brief Set error message.
    void setError(std::string_view message)
    {
        lastError_ = message;
    }
};
```

**Implementation File: `Validator.cpp`**
```cpp
#include "Validator.h"

#include <regex>
#include <charconv>

[[nodiscard]] std::optional<int> Validator::validateInteger(std::string_view input) const
{
    // Validate input
    if (input.empty())
    {
        setError("Input is empty");
        return std::nullopt;
    }
    
    // Try to parse
    int value = 0;
    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
    
    if (ec != std::errc())
    {
        setError("Failed to parse integer");
        return std::nullopt;
    }
    
    return value;
}

[[nodiscard]] bool Validator::isValidEmail(std::string_view email) const
{
    if (email.empty())
    {
        setError("Email is empty");
        return false;
    }
    
    // Simple validation: must contain @
    if (email.find('@') == std::string_view::npos)
    {
        setError("Email must contain @");
        return false;
    }
    
    return true;
}
```

**Test File: `test_validator.cpp`**
```cpp
#define BOOST_TEST_MODULE ValidatorTests
#include <boost/test/unit_test.hpp>
#include "Validator.h"

BOOST_AUTO_TEST_CASE(test_validate_integer_valid)
{
    // Arrange
    Validator validator;
    
    // Act & Assert
    auto result = validator.validateInteger("42");
    BOOST_TEST(result.has_value());
    BOOST_TEST(result.value() == 42);
}

BOOST_AUTO_TEST_CASE(test_validate_integer_invalid)
{
    // Arrange
    Validator validator;
    
    // Act & Assert
    auto result = validator.validateInteger("not_a_number");
    BOOST_TEST(!result.has_value());
}

BOOST_AUTO_TEST_CASE(test_validate_email_valid)
{
    // Arrange
    Validator validator;
    
    // Act & Assert
    BOOST_TEST(validator.isValidEmail("user@example.com") == true);
}

BOOST_AUTO_TEST_CASE(test_validate_email_invalid)
{
    // Arrange
    Validator validator;
    
    // Act & Assert
    BOOST_TEST(validator.isValidEmail("invalid_email") == false);
}
```

---

## Build and Test Command Examples

```bash
# Build with Debug-vcpkg preset
cmake --preset Debug-vcpkg
cmake --build --preset Debug-vcpkg

# Run all tests
ctest -j 4 --output-on-failure

# Run specific test
ctest -R ValidatorTests --output-on-failure

# Format code
clang-format -i Validator.h Validator.cpp test_validator.cpp

# Or use batch formatting
./src/format-code.sh
```

---

## Summary

These examples demonstrate:
- ✅ Modern C++20 features (constexpr, consteval, optional)
- ✅ Proper naming conventions
- ✅ [[nodiscard]] on value-returning functions
- ✅ Proper error handling
- ✅ Smart pointer usage
- ✅ RAII principles
- ✅ Comprehensive testing
- ✅ Complete documentation
- ✅ Proper CMake configuration
- ✅ Code formatting rules

Use these examples as templates when writing new code or asking GitHub Copilot to generate code.
