# C++ Style Guide

In general, [Google's coding standard](https://google.github.io/styleguide/cppguide.html) is used, and we strongly encourage to read it.

You can find all the steps needed to build & install Antares Simulator in the [documentation website](https://antares-simulator.readthedocs.io/) or [its sources](https://github.com/AntaresSimulatorTeam/Antares_Simulator).

Below are our specific (but not all!) exceptions to the Google's coding standard:

- All C++ code should conform to the C++20 standard.
- We use `.cpp` files and `.h` for headers, in UTF-8 encoding.
- For new files we use CamelCase, like `FileReader.cpp`.
- We use `#pragma once` instead of the `#define` Guard in header files.
- Includes are sorted and grouped by directory, there should be newlines between different directories.
- Order of directories in includes: "current_dir/current_file.h", includes from other dirs sorted by dependencies (e.g. indexer, then coding, then base), "defines.h", C++ standard library headers, boost headers.
- We ARE using C++ exceptions. Feel free to define your own exceptions, derived from `std::exception` or any child class.
- We are using all features of C++17 and C++20
- We try to limit the usage of boost libraries which require linking (and prefer C++20 types over their boost counterparts).

Naming and formatting

- We ALWAYS use 4 spaces indent and don't use tabs.
- We don't have strict limits on line width, but keep it reasonable to fit on the screen. The advised width is that written in the [src/.clang-format](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/src/.clang-format) file (currently 100).
- Doxygen-style comments can be used.
- Use left-to-right order for variables/params: `const string& s` (reference to the const string).
- In one line `if`, `for`, `while` we use brackets.
- Space after the keyword in conditions and loops. Space after `;` in `for` loop.
- Space between binary operators: `x = y * y + z * z`.
- Space after double dash.
- We use `using` keyword instead of `typedef`.
- We do not use the Systems Hungarian Notation: do not add the "p" suffix to your pointer variable names and the "T" prefix or suffix to your type names.
- Compile-time constants must be named in CamelCase, starting with a lower-case `k`, e.g. `kCompileTimeConstant` and marked as `constexpr` when possible.
- Functions (free or member) should be named using `lowerCamelCase`, e.g `isPrime(unsigned int n)`.
- Values of enum classes must be named in CAPITAL, e.g. `enum class Color { RED, GREEN, LIGHT_BLUE };`.
- Macros and C-style enums should be avoided. If necessary, they must be named in UPPER_CASE, and enum values must be prefixed with a capitalized enum name.

  Note that macros complicate debugging, and old-style enums have dangerous implicit conversions to integers, and tend to clutter
  containing namespaces. Avoid them when possible - use `const` or `constexpr` instead of macros, and enum classes instead of enums.

**We write code without warnings on clang++, g++ and MSVC !**

## Global/static variables
When using `static` variables, be aware that some of Antares Simulator's functions run on multiple threads. Please avoid introducing global variables.

## Yuni

Yuni is a C++ framework that fullfiled some of the lacking features pre-C++11. Even though you'll see it used widely through the existing code base, we recommend against using it for new code. It is namespaced under `Yuni`.

## Branch names
Currently, CI is run only for specific branch names:
- `feature/*`
- `features/*`
- `fix/*`
- `release/*`
- `issue-*`
- `doc/*`

If you create a branch with a different name, no CI will be run, but you will receive a notification indicating that your branch name is incorrect.

In order to avoid pushing with invalid branch name, a git hook is provided for pre-commit check.
This hook is available in the `.githooks` directory.

By default, git use hooks in `.git/hooks` directory which is not under version control. You can
define a new hooks directory with this command in Antares Simulator root directory :
```
git config core.hooksPath .githooks
```

## Pull Requests
A pull request name must be self-explanatory: this will be the default commit title when merging.

Please provide a description in the head comment of the PR. This description will be the details of the merge commit.
The description should be short but proportional to the length or complexity of the PR. Try to explain the motivation of the PR (why) and the method employed (how).

When a pull request is opened, please set it to draft if it is still being worked on or not ready for review.

If your Pull Request changes a part of the code that is [documented](https://antares-simulator.readthedocs.io/), 
please update the documentation also, in the ["docs"](https://github.com/AntaresSimulatorTeam/Antares_Simulator/tree/develop/docs) directory.

## ClangFormat

Most of our coding style is specified in a configuration file for [ClangFormat](http://clang.llvm.org/docs/ClangFormat.html).
To automatically format a file, install `clang-format` and run:

    clang-format -i file.cpp file.hpp other_file.cpp

clang-format 18.1.3 is the reference version, but any 18.x version should work as well. We strongly advise that you configure your IDE / text editor to automatically format code according to the clang-format style. Non-conforming code can't be merged to the develop branch.

You may also use script [src/format-code.sh](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/src/format-code.sh) to format all the code. Generated code (ANTLR, etc.) won't be automatically formatted.

## Formatting Example/Guide/Reference

```cpp
#pragma once

#include <math>

uint16_t constexpr kBufferSize = 255;

// C-style enums are ALL_CAPS. But remember that C++11 enum classes are preferred.
enum Type
{
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING
};

using TMyTypeStartsWithCapitalTLetter = double;

class ComplexClass
{
public:
    Complex(double rePart, double imPart):
        re(rePart),
        im(imPart)
    {
    }

    double Modulus() const
    {
        const double rere = re * re;
        const double imim = im * im;
        return sqrt(rere + imim);
    }

    double OneLineMethod() const
    {
        return re;
    }

private:
    double re;
    double im;
};

namespace
{
void lowerCamelCaseFunctionName(int lowerCamelCaseVar)
{
    static int counter = 0;
    counter += lowerCamelCaseVar;
}
} // namespace

namespace lower_case
{
template<typename TypenameWithoutAffixes>
void SomeFoo(int a, int b, TypenameWithoutAffixes /* We avoid compilation warnings. */)
{
    for (int i = 0; i < a; ++i)
    {
        // IMPORTANT! We DON'T use one-liners for if statements for easier debugging.
        // The following syntax is invalid: if (i < b) Bar(i);
        if (i < b)
        {
            Bar(i);
        }
        else
        {
            Bar(i);
            Bar(b);
            // Commented out the call.
            // Bar(c);
        }
    }
}
} // namespace lower_case

// Switch formatting.
int Foo(int a)
{
    switch (a)
    {
    case 1:
        Bar(1);
        break;
    case 2:
    {
        Bar(2);
        break;
    }
    case 3:
    default:
        Bar(3);
        break;
    }
    return 0;
}

// Loops formatting.

if (condition)
{
    foo();
}
else
{
    bar();
}

if (condition)
{
    if (condition)
    {
        foo();
    }
    else
    {
        bar();
    }
}

for (size_t i = 0; i < size; ++i)
{
    foo(i);
}

while (true)
{
    if (condition)
    {
        break;
    }
}

// Space after the keyword.
if (condition)
{
}

for (size_t i = 0; i < 5; ++i)
{
}

while (condition)
{
}

switch (i)
{
}

// Space between operators, and don't use space between unary operator and expression.
x = 0;
x = -5;
++x;
x--;
x *= 5;
if (x && !y)
{
}
v = w * x + y / z;
v = w * (x + z);


// Space after double dash. And full sentences in comments.
```

## Tips and Hints

- If you see outdated code which can be improved, DO IT NOW (but in a separate pull request or commit)!
- Your code should work at least on [ubuntu|windows] platforms.
- Your code should compile with a C++20 compiler (currently supported clang++/g++/MSVC)
- Consult with the dev team before using any new 3party library
- Cover your code with unit tests. See examples for existing libraries
- Check Base and Coding libraries for most of the basic functions
- Ask your team if you have any questions
- Release builds contain debugging information (for profiling), production builds do not
- If you don't have enough time to make it right, leave a `// TODO(DeveloperName): need to fix it` comment

## Logging functions:

- Use `Antares::logs.[level]() << message` for logging, below is more detailed description for level:
  - `logs.info() << msg` - always prints log message
  - `logs.debug() << msg` - logs only in DEBUG
  - `logs.warning() << msg` - the same as `logs.info()` but catches your attention
  - `logs.error()` - the same as `logs.warning()`, but triggers an error when loading a study, with exceptions
  - `logs.fatal()` - same as `logs.error()`
