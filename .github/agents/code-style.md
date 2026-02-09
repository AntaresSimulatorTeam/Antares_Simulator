# Code Style & Formatting

## Auto-Formatting

```bash
# Format all code
cd src && ./format-code.sh

# Format specific files
clang-format -i -style=file:../.clang-format file.cpp file.hpp

# Reference clang-format version: 18.1.3
```

## Formatting Rules

- **Indentation:** 4 spaces, NO tabs
- **Line width:** Advised 100 characters (not strict, keep reasonable)
- **Brace style:** Allman (braces on new lines)
- **Braces:** ALWAYS required for if/for/while/switch (even single-line)
- **Spacing:**
  - Space after keywords: `if (condition)`, `for (int i = 0; ...)`
  - Space after semicolon in for loops
  - Spaces between binary operators: `x = y * y + z * z`
  - No spaces in parentheses, square brackets, or angles
- **Pointers/References:** Left-aligned: `const string& s`, `int* ptr`
- **Header guards:** `#pragma once`
- **Constants:** Mark as `constexpr` when possible

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
