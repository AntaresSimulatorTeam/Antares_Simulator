# Sanitizers and strict-warning builds

The `ubuntu-clang-sanitizers.yml` workflow ("Ubuntu Clang sanitizers & checks") builds Antares Simulator with an
up-to-date Clang and runs the unit and end-to-end tests under sanitizers. This page explains how to reproduce a failure
of that workflow locally and how to fix the usual classes of errors.

## What the workflow checks

| Check                          | Flag / setting                                           | Failure shows up as                          |
|:-------------------------------|:---------------------------------------------------------|:---------------------------------------------|
| AddressSanitizer (ASan)        | `-fsanitize=address`                                     | `ERROR: AddressSanitizer: ...` at test time  |
| UndefinedBehaviorSanitizer     | `-fsanitize=undefined -fno-sanitize-recover=all`         | `runtime error: ...` then abort at test time |
| Warnings as errors             | `-Werror` (C++ only)                                     | `error: ...` at **build** time               |
| Hardened libstdc++ assertions  | `-D_GLIBCXX_ASSERTIONS`                                  | `Assertion '...' failed` (abort) at test time |

Other details worth knowing:

- The build type is `Debug`, with `-fno-omit-frame-pointer -g` so stack traces are readable.
- Runtime behaviour is controlled by `ASAN_OPTIONS` / `UBSAN_OPTIONS` (see the workflow `env:`). `halt_on_error=1`
  makes the first finding fatal. `detect_leaks=0`: leaks are **not** checked.
- Only tests labelled `unit` or `end-to-end` are run.
- Tests that use the prebuilt or-tools/sirius solver are excluded (`ORTOOLS_DEPENDENT_TESTS` in the workflow). That
  library is not instrumented, and those tests crash inside it under the sanitizers. They are still covered by the
  regular Ubuntu CI. If you add a new test that builds an or-tools `MPSolver`, it will likely need to be added to that
  exclusion list; the preset below must be kept in sync with it.

## Reproducing locally

### Prerequisites

- Clang (CI uses 22, installed from [apt.llvm.org](https://apt.llvm.org)) together with the sanitizer runtime. On
  Ubuntu: `sudo ./llvm.sh 22` then `sudo apt-get install clang-tools-22 libclang-rt-22-dev`. Using the same major
  version as CI matters: newer Clang releases add warnings that older ones do not have, which is the most common reason
  for a "works on my machine" `-Werror` failure.
- vcpkg and a pre-compiled OR-Tools, as described in [Build](3-Build.md).

### With the CMake preset (recommended)

`CMakePresets.json` provides `ci-clang-sanitizers` configure, build and test presets that mirror the workflow flags
and the test selection.

```sh
export VCPKG_ROOT=$PWD/vcpkg

# The preset defaults to `clang` / `clang++` from PATH; override to match CI's version
cmake --preset ci-clang-sanitizers \
      -DCMAKE_C_COMPILER=clang-22 -DCMAKE_CXX_COMPILER=clang++-22 \
      -DCMAKE_PREFIX_PATH=/path/to/ortools/install
cmake --build --preset ci-clang-sanitizers -j"$(nproc)"
ctest --preset ci-clang-sanitizers
```

The build goes to `build-sanitizers/` (keep it separate from your normal build directory: sanitizer flags are not
compatible with a regular build tree).

Machine-specific values (compiler path, `CMAKE_PREFIX_PATH`, ccache, a build directory outside the repository) belong
in your own `CMakeUserPresets.json`, which can inherit from the shared preset:

```json
{
  "version": 5,
  "configurePresets": [
    {
      "name": "my-sanitizers",
      "inherits": "ci-clang-sanitizers",
      "binaryDir": "/path/to/build_sanitizers",
      "cacheVariables": {
        "CMAKE_C_COMPILER": "clang-22",
        "CMAKE_CXX_COMPILER": "clang++-22",
        "CMAKE_PREFIX_PATH": "/path/to/ortools/install",
        "CMAKE_CXX_COMPILER_LAUNCHER": "ccache"
      }
    }
  ],
  "buildPresets": [ { "name": "my-sanitizers", "configurePreset": "my-sanitizers" } ],
  "testPresets": [ { "name": "my-sanitizers", "inherits": "ci-clang-sanitizers", "configurePreset": "my-sanitizers" } ]
}
```

!!! note
    Preset names must be unique across `CMakePresets.json` and `CMakeUserPresets.json`; CMake refuses to load both if
    a name is duplicated.

### Without presets

Equivalent to the workflow's *Configure* step:

```sh
SANITIZE_FLAGS="-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -g"
cmake -B build-sanitizers -S . \
  -DCMAKE_C_COMPILER=clang-22 -DCMAKE_CXX_COMPILER=clang++-22 \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DBUILD_TOOLS=ON \
  -DCMAKE_CXX_FLAGS="$SANITIZE_FLAGS -Werror -D_GLIBCXX_ASSERTIONS" \
  -DCMAKE_C_FLAGS="$SANITIZE_FLAGS" \
  -DCMAKE_EXE_LINKER_FLAGS="$SANITIZE_FLAGS" \
  -DCMAKE_SHARED_LINKER_FLAGS="$SANITIZE_FLAGS" \
  -DCMAKE_PREFIX_PATH=/path/to/ortools/install
cmake --build build-sanitizers -j"$(nproc)"
```

### Running a single failing test

`ctest --preset` sets the sanitizer environment for you. To iterate on one test, either keep using the preset with a
name filter, or export the variables yourself when running a binary directly:

```sh
ctest --preset ci-clang-sanitizers -R "<test name>"

export ASAN_OPTIONS="halt_on_error=1:abort_on_error=1:detect_leaks=0:strict_string_checks=1:check_initialization_order=1:detect_stack_use_after_return=1"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"
./build-sanitizers/tests/<path-to-test-binary>
```

Failed test logs are in `build-sanitizers/Testing/Temporary/LastTest.log`, which is also what the workflow uploads as
the `test-log-clang-sanitizers` artifact.

!!! tip
    Sanitizer reports only help with symbolized stack traces. If frames show up as raw addresses, make sure
    `llvm-symbolizer` is on `PATH` (or set `ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-22`).

## Reading a failure and fixing it

### `-Werror` build failures

The build fails at the offending line with a normal compiler diagnostic. Fix the cause (unused variable or parameter,
sign comparison, implicit narrowing, deprecated API, missing `override`, ...) rather than silencing it. If a warning
comes from a third-party header, include it as a system header (or wrap the include in a
`#pragma clang diagnostic push/ignored/pop` block) instead of disabling the warning globally. Warnings that depend on
the Clang version only appear with that version, so reproduce with the CI major version.

### AddressSanitizer

| Report                                  | Typical cause                                               | Typical fix                                                       |
|:----------------------------------------|:------------------------------------------------------------|:------------------------------------------------------------------|
| `heap-buffer-overflow`                  | Out-of-range index, wrong size, `memcpy` with bad length    | Fix the bound; prefer `std::vector::at`/ranges while debugging    |
| `heap-use-after-free`                   | Dangling pointer/reference, iterator invalidation           | Fix ownership (`unique_ptr`/`shared_ptr`) or lifetime of the data |
| `stack-use-after-return` / `-scope`     | Reference or pointer to a local that has gone out of scope  | Return by value, or extend the lifetime                           |
| `stack-buffer-overflow`                 | Same as heap overflow on a local array                      | Fix the bound, use `std::array`                                   |
| `initialization-order-fiasco`           | A global/static uses another global from a different file   | Replace with a function-local static                              |
| `new-delete-type-mismatch`, `alloc-dealloc-mismatch` | Deleting through a base without a virtual destructor, or `malloc`/`delete` mixes | Add a virtual destructor; match allocator and deallocator |

A report has three stacks: where the invalid access happened, where the memory was freed (for use-after-free), and
where it was allocated. Start from the first frame in Antares code in each of them.

### UndefinedBehaviorSanitizer

Reports look like `file.cpp:123:45: runtime error: <description>`, followed by a stack trace thanks to
`print_stacktrace=1`. Frequent ones:

- `signed integer overflow`: use a wider type or check before the operation.
- `shift exponent ... is too large`, `left shift of negative value`: guard the shift amount; use unsigned operands.
- `index N out of bounds for type 'T[M]'`: fix the index.
- `member call on null pointer` / `load of null pointer`: add the missing check or fix the initialization.
- `load of value N, which is not a valid value for type 'bool'` (or an enum): an uninitialized variable; initialize it.
- `division by zero` (integer): validate the divisor first.
- `call to function through pointer to incorrect function type`, `downcast of address ... which does not point to an
  object of type ...`: wrong cast; fix the type or use `dynamic_cast`.

### `_GLIBCXX_ASSERTIONS` aborts

These look like `Assertion '__n < this->size()' failed.` and come from `operator[]` on `std::vector`/`std::array`,
`std::string` or `std::span`, `front()`/`back()` on an empty container, or dereferencing an empty `std::optional`/
`std::unique_ptr`. They are real out-of-range bugs that would otherwise be silent. Fix the index or emptiness check;
the abort's backtrace (visible in the test log) points at the caller.

### Findings that are not in Antares code

If the top frames are inside or-tools/sirius (the uninstrumented prebuilt solver), the test belongs in the
`ORTOOLS_DEPENDENT_TESTS` exclusion list of the workflow (and in the preset's `filter.exclude.name`). Do not exclude a
test merely to get a green build: first check that the stack really has no Antares frame at fault.

## Checklist when the workflow is red

1. Open the failed step: *Build* means `-Werror`; *Run unit and end-to-end tests under sanitizers* means a runtime
   report (look for `ERROR: AddressSanitizer`, `runtime error:` or `Assertion`).
2. Reproduce with `ci-clang-sanitizers` using the same Clang major version as `CLANG_VERSION` in the workflow.
3. Fix the root cause, rebuild, rerun just that test, then rerun the whole `ctest --preset ci-clang-sanitizers`.
4. Before pushing, run `./src/format-code.sh`.
