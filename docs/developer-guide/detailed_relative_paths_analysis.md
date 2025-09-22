# Relative Paths Analysis Report
==================================================
Total relative paths found: 3899

## Summary by Category
------------------------------
- cmake_build: 77 occurrences
- include_statement: 1261 occurrences
- other: 1890 occurrences
- runtime_resource: 136 occurrences
- test_path: 3 occurrences
- tool_path: 532 occurrences

## Cmake Build (77 occurrences)
--------------------------------------------------
### Pattern: `../"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:174`
**Line:** `include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/"`

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:176`
**Line:** `include_directories("${CMAKE_CURRENT_BINARY_DIR}/../")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/"`

### Pattern: `../.."` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:30`
**Line:** `include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../..")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/tools/.."`

### Pattern: `../../../bin/mt.exe"` (1 occurrences)

**File:** `src/ui/simulator/CMakeLists.txt:183`
**Line:** `"${CMAKE_CURRENT_BINARY_DIR}/../../../bin/mt.exe"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/bin/mt.exe"`

### Pattern: `../../cmake/common-settings.cmake` (2 occurrences)

**File:** `src/tools/batchrun/CMakeLists.txt:10`
**Line:** `Include(../../cmake/common-settings.cmake)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/cmake/common-settings.cmake`

**File:** `src/tools/yby-aggregator/CMakeLists.txt:8`
**Line:** `include(../../cmake/common-settings.cmake)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/cmake/common-settings.cmake`

### Pattern: `../../cmake/common-settings.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:26`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common-settings.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/common-settings.cmake"`

### Pattern: `../../cmake/message.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:13`
**Line:** `Include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/message.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/message.cmake"`

### Pattern: `../../cmake/messages.cmake` (2 occurrences)

**File:** `src/tools/batchrun/CMakeLists.txt:4`
**Line:** `include(../../cmake/messages.cmake)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/cmake/messages.cmake`

**File:** `src/tools/yby-aggregator/CMakeLists.txt:4`
**Line:** `include(../../cmake/messages.cmake)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/cmake/messages.cmake`

### Pattern: `../../cmake/modules.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:27`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/modules.cmake"`

### Pattern: `../../src/build/debug/bin/"` (1 occurrences)

**File:** `src/ext/yuni/packages/cmake/FindYuni.cmake:62`
**Line:** `"${__Yuni_CurrentFolder}/../../src/build/debug/bin/"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/build/debug/bin/"`

### Pattern: `../../src/build/release/bin/"` (1 occurrences)

**File:** `src/ext/yuni/packages/cmake/FindYuni.cmake:61`
**Line:** `"${__Yuni_CurrentFolder}/../../src/build/release/bin/"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/build/release/bin/"`

### Pattern: `../../yuni/core/getopt/option.cpp` (6 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:59`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.cpp`

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:60`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.cpp`

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:61`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.cpp`

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:62`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/option.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.cpp`

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:63`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/option.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.cpp`

... and 1 more occurrences

### Pattern: `../../yuni/core/getopt/option.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:62`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/option.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/option.h`

### Pattern: `../../yuni/core/getopt/parser.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:59`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/parser.cpp`

### Pattern: `../../yuni/core/getopt/parser.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:60`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/parser.h`

### Pattern: `../../yuni/core/getopt/parser.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:61`
**Line:** `../../yuni/core/getopt/option.cpp ../../yuni/core/getopt/parser.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/parser.hxx`

### Pattern: `../../yuni/core/string/escape.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:44`
**Line:** `../../yuni/core/string/escape.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/escape.h`

### Pattern: `../../yuni/core/string/escape.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:45`
**Line:** `../../yuni/core/string/escape.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/escape.hxx`

### Pattern: `../../yuni/core/string/iterator.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:46`
**Line:** `../../yuni/core/string/iterator.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/iterator.hxx`

### Pattern: `../../yuni/core/string/iterator.inc.hpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:47`
**Line:** `../../yuni/core/string/iterator.inc.hpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/iterator.inc.hpp`

### Pattern: `../../yuni/core/string/operators.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:48`
**Line:** `../../yuni/core/string/operators.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/operators.hxx`

### Pattern: `../../yuni/core/string/string.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:49`
**Line:** `../../yuni/core/string/string.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/string.cpp`

### Pattern: `../../yuni/core/string/string.h` (2 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:43`
**Line:** `../../yuni/core/string/string.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/string.h`

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:50`
**Line:** `../../yuni/core/string/string.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/string.h`

### Pattern: `../../yuni/core/string/string.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:51`
**Line:** `../../yuni/core/string/string.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/string.hxx`

### Pattern: `../../yuni/core/string/traits/traits.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:71`
**Line:** `../../yuni/core/string/traits/traits.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/traits/traits.cpp`

### Pattern: `../../yuni/core/string/utf8char.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:52`
**Line:** `../../yuni/core/string/utf8char.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/utf8char.h`

### Pattern: `../../yuni/core/string/utf8char.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:53`
**Line:** `../../yuni/core/string/utf8char.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/utf8char.hxx`

### Pattern: `../../yuni/core/string/wstring.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:56`
**Line:** `../../yuni/core/string/wstring.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.cpp`

### Pattern: `../../yuni/core/string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:54`
**Line:** `../../yuni/core/string/wstring.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../../yuni/core/string/wstring.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:55`
**Line:** `../../yuni/core/string/wstring.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.hxx`

### Pattern: `../../yuni/core/version/version.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:58`
**Line:** `../../yuni/core/version/version.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/version/version.cpp`

### Pattern: `../../yuni/io/constants.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:64`
**Line:** `../../yuni/io/constants.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/constants.cpp`

### Pattern: `../../yuni/io/directory/current.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:70`
**Line:** `../../yuni/io/directory/current.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/current.cpp`

### Pattern: `../../yuni/io/directory/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:76`
**Line:** `../../yuni/io/directory/iterator.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/iterator.h`

### Pattern: `../../yuni/io/directory/iterator/detachedthread.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:80`
**Line:** `../../yuni/io/directory/iterator/detachedthread.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/iterator/detachedthread.h`

### Pattern: `../../yuni/io/directory/iterator/iterator.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:79`
**Line:** `../../yuni/io/directory/iterator/iterator.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/iterator/iterator.cpp`

### Pattern: `../../yuni/io/directory/iterator/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:77`
**Line:** `../../yuni/io/directory/iterator/iterator.h`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/iterator/iterator.h`

### Pattern: `../../yuni/io/directory/iterator/iterator.hxx` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:78`
**Line:** `../../yuni/io/directory/iterator/iterator.hxx`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx`

### Pattern: `../../yuni/io/exists.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:68`
**Line:** `../../yuni/io/exists.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/exists.cpp`

### Pattern: `../../yuni/io/file/file.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:65`
**Line:** `../../yuni/io/file/file.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file/file.cpp`

### Pattern: `../../yuni/io/file/openmode.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:66`
**Line:** `../../yuni/io/file/openmode.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file/openmode.cpp`

### Pattern: `../../yuni/io/file/stream.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:67`
**Line:** `../../yuni/io/file/stream.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file/stream.cpp`

### Pattern: `../../yuni/io/filename-manipulation.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:69`
**Line:** `../../yuni/io/filename-manipulation.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/filename-manipulation.cpp`

### Pattern: `../../yuni/thread/mutex.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:73`
**Line:** `../../yuni/thread/mutex.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/mutex.cpp`

### Pattern: `../../yuni/thread/thread.cpp` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/CMakeLists.txt:72`
**Line:** `../../yuni/thread/thread.cpp`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.cpp`

### Pattern: `../>` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/cmake/core/core.cmake:512`
**Line:** `$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/cmake/>`

### Pattern: `../bin"` (1 occurrences)

**File:** `src/ext/yuni/src/cmake/devpack.cmake:19`
**Line:** `Find_Program(DevPackSourceZIP NAMES "unzip.exe" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../bin")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/bin"`

### Pattern: `../bin/${YUNI_TARGET_MODE}` (1 occurrences)

**File:** `src/CMakeLists.txt:183`
**Line:** `#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${YUNI_TARGET_MODE})`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/bin/${YUNI_TARGET_MODE}`

### Pattern: `../bin/mt.exe"` (1 occurrences)

**File:** `src/cmake/common-settings.cmake:32`
**Line:** `"${PROJECT_SOURCE_DIR}/../bin/mt.exe"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/bin/mt.exe"`

### Pattern: `../build/${YUNI_TARGET}/lib"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:10`
**Line:** `set(LIBRARY_OUTPUT_PATH      "${CMAKE_CURRENT_BINARY_DIR}/../build/${YUNI_TARGET}/lib")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/build/${YUNI_TARGET}/lib"`

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:11`
**Line:** `set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../build/${YUNI_TARGET}/lib")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/build/${YUNI_TARGET}/lib"`

### Pattern: `../cmake/FindFFMPEG.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/cmake/devpack-standard-imports.cmake:13`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindFFMPEG.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/FindFFMPEG.cmake"`

### Pattern: `../cmake/YuniConfig.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:29`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/YuniConfig.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/YuniConfig.cmake"`

### Pattern: `../cmake/common-settings.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:30`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common-settings.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/common-settings.cmake"`

### Pattern: `../cmake/modules.cmake"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:31`
**Line:** `include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules.cmake")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/cmake/modules.cmake"`

### Pattern: `../compiler-flags-debug-cc"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:193`
**Line:** `file("WRITE" "${CMAKE_CURRENT_BINARY_DIR}/../compiler-flags-debug-cc"    "${CMAKE_C_FLAGS_DEBUG}")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/compiler-flags-debug-cc"`

### Pattern: `../compiler-flags-debug-cxx"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:191`
**Line:** `file("WRITE" "${CMAKE_CURRENT_BINARY_DIR}/../compiler-flags-debug-cxx"   "${CMAKE_CXX_FLAGS_DEBUG}")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/compiler-flags-debug-cxx"`

### Pattern: `../compiler-flags-release-cc"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:194`
**Line:** `file("WRITE" "${CMAKE_CURRENT_BINARY_DIR}/../compiler-flags-release-cc"  "${CMAKE_C_FLAGS_RELEASE}")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/compiler-flags-release-cc"`

### Pattern: `../compiler-flags-release-cxx"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/CMakeLists.txt:192`
**Line:** `file("WRITE" "${CMAKE_CURRENT_BINARY_DIR}/../compiler-flags-release-cxx" "${CMAKE_CXX_FLAGS_RELEASE}")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/compiler-flags-release-cxx"`

### Pattern: `../devpacks"` (1 occurrences)

**File:** `src/ext/yuni/src/cmake/devpack.cmake:5`
**Line:** `set(DevPackSourceFolder     "${CMAKE_CURRENT_SOURCE_DIR}/../devpacks")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/devpacks"`

### Pattern: `../devpacks/receipts"` (1 occurrences)

**File:** `src/ext/yuni/src/cmake/devpack.cmake:6`
**Line:** `set(DevPackReceiptsFolder   "${CMAKE_CURRENT_SOURCE_DIR}/../devpacks/receipts")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/devpacks/receipts"`

### Pattern: `../docs/developer-guide/CHANGELOG.md` (1 occurrences)

**File:** `src/cmake/changelog.cmake:3`
**Line:** `file(READ "../docs/developer-guide/CHANGELOG.md" changelog_content)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/docs/developer-guide/CHANGELOG.md`

### Pattern: `../docs/developer-guide/CHANGELOG.md"` (1 occurrences)

**File:** `src/cmake/changelog.cmake:3`
**Line:** `file(READ "../docs/developer-guide/CHANGELOG.md" changelog_content)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/docs/developer-guide/CHANGELOG.md"`

### Pattern: `../docs/ref_guides/"` (1 occurrences)

**File:** `src/ui/simulator/CMakeLists.txt:196`
**Line:** `FILE(COPY "${PROJECT_SOURCE_DIR}/../docs/ref_guides/"  			DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/help/" FILES_MATCHING PATTERN "*.pdf")`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/docs/ref_guides/"`

### Pattern: `../ortools_tag` (1 occurrences)

**File:** `src/CMakeLists.txt:23`
**Line:** `file(READ "../ortools_tag" ORTOOLS_TAG)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/ortools_tag`

### Pattern: `../ortools_tag"` (1 occurrences)

**File:** `src/CMakeLists.txt:23`
**Line:** `file(READ "../ortools_tag" ORTOOLS_TAG)`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/ortools_tag"`

### Pattern: `..\..\..\bin\zip.exe` (1 occurrences)

**File:** `src/distrib/win32/make-zip-from-installer.cmake:20`
**Line:** `..\..\..\bin\zip.exe -9 -r "%INSTALLER%.zip" "%INSTALLER%"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/distrib/win32/..\..\..\bin\zip.exe`

### Pattern: `..\\..\\..\\changelog.txt` (1 occurrences)

**File:** `src/distrib/win32/make-zip-from-installer.cmake:18`
**Line:** `copy "..\\..\\..\\changelog.txt" "%INSTALLER%"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/distrib/win32/..\\..\\..\\changelog.txt`

### Pattern: `..\\..\\..\\changelog.txt"` (1 occurrences)

**File:** `src/distrib/win32/make-zip-from-installer.cmake:18`
**Line:** `copy "..\\..\\..\\changelog.txt" "%INSTALLER%"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/distrib/win32/..\\..\\..\\changelog.txt"`


## Include Statement (1261 occurrences)
--------------------------------------------------
### Pattern: `../../../../../../application/study.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.cpp:23`
**Line:** `#include "../../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h` (1 occurrences)

**File:** `src/io/inputs/yml-system/include/antares/io/inputs/yml-system/converter.h:27`
**Line:** `#include "../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/io/inputs/yml-model/include/antares/io/inputs/yml-model/parser.h`

### Pattern: `../../../../../application/study.h` (10 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:26`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:29`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:23`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 5 more occurrences

### Pattern: `../../../../../windows/inspector.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:24`
**Line:** `#include "../../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../../application/main.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:25`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.cpp:24`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:29`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

### Pattern: `../../../../application/main/main.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:27`
**Line:** `#include "../../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

### Pattern: `../../../../application/study.h` (16 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:26`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.cpp:25`
**Line:** `#include "../../../../application/study.h" // OnStudyChanged`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:23`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:29`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:25`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 11 more occurrences

### Pattern: `../../../../application/wait.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:26`
**Line:** `#include "../../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../../../../input/area.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:25`
**Line:** `#include "../../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../../input/renewable-cluster.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:27`
**Line:** `#include "../../../../input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

### Pattern: `../../../../input/thermal-cluster.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:26`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

### Pattern: `../../../../toolbox/components/refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.cpp:24`
**Line:** `#include "../../../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../../../toolbox/input/area.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../../validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:24`
**Line:** `#include "../../../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../../../windows/inspector.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:22`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:29`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:26`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../../windows/message.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:26`
**Line:** `#include "../../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../../application/main.h` (10 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:36`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:34`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:25`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:48`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:30`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 5 more occurrences

### Pattern: `../../../application/main/internal-ids.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:32`
**Line:** `#include "../../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h`

### Pattern: `../../../application/main/main.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:28`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:36`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

### Pattern: `../../../application/menus.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:49`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:31`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

### Pattern: `../../../application/study.h` (19 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:33`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:29`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer.cpp:23`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:50`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 14 more occurrences

### Pattern: `../../../application/wait.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:28`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:51`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:33`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

... and 4 more occurrences

### Pattern: `../../../common/lock.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:41`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/application/main/refresh.cpp:28`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:25`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/components/refresh.cpp:26`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:29`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

... and 1 more occurrences

### Pattern: `../../../core/iterator/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:13`
**Line:** `#include "../../../core/iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h`

### Pattern: `../../../core/noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:16`
**Line:** `#include "../../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../../core/slist.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:17`
**Line:** `#include "../../../core/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist.h`

### Pattern: `../../../core/smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:13`
**Line:** `#include "../../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../../core/static/remove.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:13`
**Line:** `#include "../../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

### Pattern: `../../../core/string.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

### Pattern: `../../../core/system/windows.hdr.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.cpp:20`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.cpp:18`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:26`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../../core/traits/cstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:14`
**Line:** `#include "../../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../../../core/traits/length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:15`
**Line:** `#include "../../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../../datetime/timestamp.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:30`
**Line:** `#include "../../../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h`

### Pattern: `../../../input/area.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:25`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:24`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../input/connection.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/connection.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h`

### Pattern: `../../../io/directory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:31`
**Line:** `#include "../../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../../io/file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:15`
**Line:** `#include "../../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../../job/queue/service.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:13`
**Line:** `#include "../../../job/queue/service.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/queue/service.h`

### Pattern: `../../../renderer.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h:26`
**Line:** `#include "../../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

### Pattern: `../../../resources.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:25`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

### Pattern: `../../../simulator/application/study.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:24`
**Line:** `#include "../../../simulator/application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../../../thread/policy.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:30`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:29`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:13`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

### Pattern: `../../../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:14`
**Line:** `#include "../../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../../../thread/thread.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:14`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/event/flow/timer.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/event/flow/continuous.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

... and 1 more occurrences

### Pattern: `../../../toolbox/clipboard/clipboard.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:54`
**Line:** `#include "../../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../../toolbox/components/button.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:53`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../../toolbox/components/captionpanel.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:52`
**Line:** `#include "../../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../../toolbox/components/datagrid/component.h` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.cpp:23`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

... and 1 more occurrences

### Pattern: `../../../toolbox/components/datagrid/renderer.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/timeseries.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h` (2 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:28`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/mc-playlist.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/mc-playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/select-variables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/select-variables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/select-variables.h`

### Pattern: `../../../toolbox/components/notebook/notebook.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:24`
**Line:** `#include "../../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

### Pattern: `../../../toolbox/components/wizardheader.h` (5 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:27`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:35`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../../../toolbox/create.h` (10 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:25`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:24`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:26`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:32`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:36`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 5 more occurrences

### Pattern: `../../../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:26`
**Line:** `#include "../../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../../../toolbox/input/area.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:23`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../toolbox/resources.h` (13 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:25`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:27`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:26`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 8 more occurrences

### Pattern: `../../../toolbox/validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:58`
**Line:** `#include "../../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../../windows/calendar/calendar.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:55`
**Line:** `#include "../../../windows/calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h`

### Pattern: `../../../windows/correlation/correlation.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:24`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

### Pattern: `../../../windows/hydro/allocation.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:29`
**Line:** `#include "../../../windows/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/allocation.h`

### Pattern: `../../../windows/hydro/localdatahydro.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:28`
**Line:** `#include "../../../windows/hydro/localdatahydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/localdatahydro.h`

### Pattern: `../../../windows/hydro/prepro.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:27`
**Line:** `#include "../../../windows/hydro/prepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/prepro.h`

### Pattern: `../../../windows/hydro/series.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:26`
**Line:** `#include "../../../windows/hydro/series.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/series.h`

### Pattern: `../../../windows/inspector.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/build-context.cpp:23`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:38`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:30`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:37`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../windows/message.h` (3 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/optimization/optimization.cpp:30`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../../windows/notes.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/notes.cpp:23`
**Line:** `#include "../../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../../../windows/sets.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/sets.cpp:23`
**Line:** `#include "../../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h`

### Pattern: `../../../windows/xcast/xcast.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

### Pattern: `../../../yuni.h` (20 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:12`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:19`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/system/console/console.h:13`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 15 more occurrences

### Pattern: `../../application/main.h` (15 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:46`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:34`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:26`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 10 more occurrences

### Pattern: `../../application/main/internal-ids.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:41`
**Line:** `#include "../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h`

### Pattern: `../../application/main/main.h` (6 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:28`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:26`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:48`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

... and 1 more occurrences

### Pattern: `../../application/menus.h` (12 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/action-panel.cpp:29`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:25`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/output/output.cpp:28`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

... and 7 more occurrences

### Pattern: `../../application/study.h` (26 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:27`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:26`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:25`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 21 more occurrences

### Pattern: `../../application/wait.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:39`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../../atomic/int.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:30`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:15`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:14`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

### Pattern: `../../bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:16`
**Line:** `#include "../../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../../button.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:27`
**Line:** `#include "../../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../clipboard/clipboard.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:31`
**Line:** `#include "../../clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../component.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.cpp:24`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

### Pattern: `../../components/button.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:37`
**Line:** `#include "../../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../components/captionpanel.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:23`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../components/datagrid/filter/component.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:59`
**Line:** `#include "../../components/datagrid/filter/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/component.h`

### Pattern: `../../components/map/component.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:37`
**Line:** `#include "../../components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

### Pattern: `../../components/map/settings.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../../core/atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:15`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:13`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../../core/bind.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:15`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:13`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../../core/dictionary.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:14`
**Line:** `#include "../../core/dictionary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dictionary.h`

### Pattern: `../../core/noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:15`
**Line:** `#include "../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../core/slist/slist.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:16`
**Line:** `#include "../../core/slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

### Pattern: `../../core/smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:15`
**Line:** `#include "../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../core/static/assert.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:16`
**Line:** `#include "../../core/static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../../core/static/remove.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:17`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

### Pattern: `../../core/static/types.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:14`
**Line:** `#include "../../core/static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

### Pattern: `../../core/string.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:27`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:18`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.hxx:13`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:16`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:14`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 1 more occurrences

### Pattern: `../../core/string/wstring.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:28`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:38`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:21`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

... and 1 more occurrences

### Pattern: `../../core/system/cpu.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:14`
**Line:** `#include "../../core/system/cpu.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/cpu.h`

### Pattern: `../../core/system/environment.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/system.cpp:14`
**Line:** `#include "../../core/system/environment.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/environment.h`

### Pattern: `../../core/system/windows.hdr.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:34`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:30`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:22`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/commons.h:14`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../core/traits/cstring.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../../core/traits/length.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../create.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:57`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:26`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:35`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:36`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

### Pattern: `../../default.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:25`
**Line:** `#include "../../default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/default.h`

### Pattern: `../../directory.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:18`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../dispatcher.h` (1 occurrences)

**File:** `src/ui/common/component/frame/registry.cpp:26`
**Line:** `#include "../../dispatcher.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher.h`

### Pattern: `../../dispatcher/gui.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/mini-frame.cpp:23`
**Line:** `#include "../../dispatcher/gui.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher/gui.h`

### Pattern: `../../event/event.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:15`
**Line:** `#include "../../event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h`

### Pattern: `../../file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.cpp:14`
**Line:** `#include "../../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../fwd.h` (13 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster_list.h:32`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster.h:34`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:35`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/ecoInput.h:27`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:24`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

... and 8 more occurrences

### Pattern: `../../gridhelper.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../../input/input.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:27`
**Line:** `#include "../../input/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h`

### Pattern: `../../io.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:16`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:19`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

### Pattern: `../../io/directory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:22`
**Line:** `#include "../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../io/file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:23`
**Line:** `#include "../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../iterator/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:20`
**Line:** `#include "../../iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h`

### Pattern: `../../jobs.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:35`
**Line:** `#include "../../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../../lock.h` (3 occurrences)

**File:** `src/ui/common/component/panel/panel.cpp:23`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/common/component/spotlight/listbox-panel.cpp:25`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/common/component/spotlight/spotlight.cpp:31`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

### Pattern: `../../math/base.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/integer.h:15`
**Line:** `#include "../../math/base.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/base.h`

### Pattern: `../../math/math.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.h:19`
**Line:** `#include "../../math/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

### Pattern: `../../message.h` (3 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:14`
**Line:** `#include "../../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../private/jobs/queue/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:15`
**Line:** `#include "../../private/jobs/queue/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/private/jobs/queue/thread.h`

### Pattern: `../../refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.cpp:23`
**Line:** `#include "../../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../renderer.h` (8 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

... and 3 more occurrences

### Pattern: `../../resources.h` (7 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:56`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:25`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/button/button.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 2 more occurrences

### Pattern: `../../smartptr.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:16`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/stream.h:14`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/triangle.h:13`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

... and 1 more occurrences

### Pattern: `../../smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:21`
**Line:** `#include "../../smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../smartptr/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:22`
**Line:** `#include "../../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h`

### Pattern: `../../spotlight/constraint.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:38`
**Line:** `#include "../../spotlight/constraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/constraint.h`

### Pattern: `../../state.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/commons/links/links.h:30`
**Line:** `#include "../../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h`

### Pattern: `../../static/assert.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:31`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:15`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../../static/if.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:23`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:16`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

### Pattern: `../../static/method.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:32`
**Line:** `#include "../../static/method.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/method.h`

### Pattern: `../../string.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:14`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/string.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

### Pattern: `../../string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:12`
**Line:** `#include "../../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../../system/console.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/verbositylevel.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

### Pattern: `../../system/windows.hdr.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:10`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.cpp:14`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../thread/array.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:16`
**Line:** `#include "../../thread/array.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/array.h`

### Pattern: `../../thread/policy.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:42`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/event/interfaces.h:16`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

### Pattern: `../../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:16`
**Line:** `#include "../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../../thread/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:15`
**Line:** `#include "../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

### Pattern: `../../thread/timer.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.hxx:13`
**Line:** `#include "../../thread/timer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/timer.h`

### Pattern: `../../toolbox/clipboard/clipboard.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:25`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

**File:** `src/ui/simulator/windows/inspector/inspector.cpp:32`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../toolbox/components/button.h` (14 occurrences)

**File:** `src/ui/simulator/application/main/internal-data.h:28`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/output/output.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/calendar/calendar.h:25`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:47`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

... and 9 more occurrences

### Pattern: `../../toolbox/components/captionpanel.h` (4 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:25`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:26`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:37`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/scenario-builder/panel.cpp:29`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../toolbox/components/datagrid/component.h` (19 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:34`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/xcast/xcast.h:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/renewables/panel.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:25`
**Line:** `// #include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

... and 14 more occurrences

### Pattern: `../../toolbox/components/datagrid/gridhelper.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/application/main/main.cpp:35`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/creditmodulations.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/creditmodulations.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydro/allocation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/allocation.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydroprepro.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/inflowpattern.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h` (1 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/renewable.areasummary.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/reservoirlevels.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/reservoirlevels.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/reservoirlevels.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermal.areasummary.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalmodulation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalprepro.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/timeseries.h` (4 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/hydro/series.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/watervalues.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/watervalues.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-coefficients.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-conversion.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-conversion.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-k.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-k.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-k.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-translation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-translation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-translation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/data.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h`

### Pattern: `../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h` (1 occurrences)

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.h:38`
**Line:** `#include "../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.h`

### Pattern: `../../toolbox/components/datagrid/renderer/correlation.h` (2 occurrences)

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h`

**File:** `src/ui/simulator/windows/correlation/datasources.hxx:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h`

### Pattern: `../../toolbox/components/datagrid/selectionoperation.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:35`
**Line:** `#include "../../toolbox/components/datagrid/selectionoperation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/selectionoperation.h`

### Pattern: `../../toolbox/components/htmllistbox/item/error.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/logs.cpp:29`
**Line:** `#include "../../toolbox/components/htmllistbox/item/error.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/error.h`

### Pattern: `../../toolbox/components/mainpanel.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:29`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/application/main/main.cpp:38`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:32`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:34`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

### Pattern: `../../toolbox/components/map/component.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:27`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/application/main/events.edit.cpp:26`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/application/main/main.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:29`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

... and 1 more occurrences

### Pattern: `../../toolbox/components/map/settings.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:36`
**Line:** `#include "../../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../../toolbox/components/notebook/notebook.h` (15 occurrences)

**File:** `src/ui/simulator/application/main/main.h:37`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:27`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/xcast/xcast.h:24`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/renewables/panel.h:26`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:25`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

... and 10 more occurrences

### Pattern: `../../toolbox/components/refresh.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/refresh.cpp:27`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:26`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:33`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../toolbox/components/wizardheader.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:30`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:25`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:44`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/simulation/run.cpp:49`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../../toolbox/create.h` (19 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:33`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/output/output.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 14 more occurrences

### Pattern: `../../toolbox/dispatcher/study.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/help.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/drag-drop.hxx:25`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:27`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:31`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

### Pattern: `../../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:24`
**Line:** `#include "../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../../toolbox/ext-source/handler.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:27`
**Line:** `#include "../../toolbox/ext-source/handler.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/handler.h`

### Pattern: `../../toolbox/input/area.h` (9 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.h:26`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/renewables/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/thermal/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/hydro/management.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

... and 4 more occurrences

### Pattern: `../../toolbox/input/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../../toolbox/input/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/bindingconstraint.h`

### Pattern: `../../toolbox/input/renewable-cluster.h` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:26`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

### Pattern: `../../toolbox/input/thermal-cluster.h` (2 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

### Pattern: `../../toolbox/jobs.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:48`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:26`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../../toolbox/resources.h` (17 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:28`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:32`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/main/menu.cpp:27`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 12 more occurrences

### Pattern: `../../toolbox/system/diskfreespace.hxx` (2 occurrences)

**File:** `src/ui/simulator/windows/memorystatistics/memorystatistics.cpp:31`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx`

**File:** `src/ui/simulator/windows/simulation/run.cpp:51`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx`

### Pattern: `../../toolbox/validator.h` (10 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:30`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:27`
**Line:** `// #include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:27`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/hydro/management.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

... and 5 more occurrences

### Pattern: `../../toolbox/wx-wrapper.h` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.h:24`
**Line:** `// #include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/windows/thermal/panel.h:24`
**Line:** `#include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

### Pattern: `../../traits/length.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/fill.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/assign.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/append.h:19`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/vnsprintf.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:34`
**Line:** `#include "../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../variable.h` (10 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowQuad.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionFee.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionProbability.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowLinear.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/marginalCost.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

... and 5 more occurrences

### Pattern: `../../windows/aboutbox.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:29`
**Line:** `#include "../../windows/aboutbox.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/aboutbox.h`

### Pattern: `../../windows/bindingconstraint/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:55`
**Line:** `#include "../../windows/bindingconstraint/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/bindingconstraint/bindingconstraint.h`

### Pattern: `../../windows/constraints-builder/constraintsbuilder.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:28`
**Line:** `#include "../../windows/constraints-builder/constraintsbuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/constraints-builder/constraintsbuilder.h`

### Pattern: `../../windows/exportmap.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:27`
**Line:** `#include "../../windows/exportmap.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/exportmap.h`

### Pattern: `../../windows/inspector.h` (12 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:39`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/application/main/events.edit.cpp:28`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/application/main/main.cpp:56`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:31`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:27`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

... and 7 more occurrences

### Pattern: `../../windows/inspector/data.h` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

### Pattern: `../../windows/inspector/frame.h` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.cpp:24`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:29`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:36`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

### Pattern: `../../windows/memorystatistics.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:38`
**Line:** `#include "../../windows/memorystatistics.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/memorystatistics.h`

### Pattern: `../../windows/message.h` (16 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:40`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/options.cpp:32`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/help.cpp:31`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/menu.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

... and 11 more occurrences

### Pattern: `../../windows/notes.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:23`
**Line:** `#include "../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-areas.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:30`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-areas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-options.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:29`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h`

### Pattern: `../../windows/options/advanced/advanced.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:31`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

**File:** `src/ui/simulator/application/main/main.cpp:58`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:30`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

### Pattern: `../../windows/options/geographic-trimming/geographic-trimming.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:25`
**Line:** `#include "../../windows/options/geographic-trimming/geographic-trimming.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h`

### Pattern: `../../windows/options/optimization/optimization.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:28`
**Line:** `#include "../../windows/options/optimization/optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/optimization/optimization.h`

### Pattern: `../../windows/options/playlist/playlist.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:27`
**Line:** `#include "../../windows/options/playlist/playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/playlist/playlist.h`

### Pattern: `../../windows/options/select-output/select-output.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:26`
**Line:** `#include "../../windows/options/select-output/select-output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/select-output/select-output.h`

### Pattern: `../../windows/options/temp-folder/temp-folder.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:24`
**Line:** `#include "../../windows/options/temp-folder/temp-folder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/temp-folder/temp-folder.h`

### Pattern: `../../windows/saveas.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:26`
**Line:** `#include "../../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h`

### Pattern: `../../windows/sets.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:24`
**Line:** `#include "../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h`

### Pattern: `../../windows/simulation/run.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:29`
**Line:** `#include "../../windows/simulation/run.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/simulation/run.h`

### Pattern: `../../windows/startupwizard.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:42`
**Line:** `#include "../../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h`

### Pattern: `../../windows/studylogs.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:37`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h`

**File:** `src/ui/simulator/application/main/logs.cpp:23`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h`

### Pattern: `../../windows/textinput/input.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:33`
**Line:** `#include "../../windows/textinput/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/textinput/input.h`

### Pattern: `../../windows/version.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:24`
**Line:** `#include "../../windows/version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/version.h`

### Pattern: `../../wx-wrapper.h` (4 occurrences)

**File:** `src/ui/common/component/frame/local-frame.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/panel/panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/panel/group.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/spotlight/listbox-panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

### Pattern: `../../xcast.h` (3 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/solar/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:25`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

**File:** `src/libs/antares/study/include/antares/study/parts/load/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

### Pattern: `../../yuni.h` (55 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.cpp:14`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:17`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:13`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:41`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/smartptr/intrusive.h:12`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 50 more occurrences

### Pattern: `../action.h` (4 occurrences)

**File:** `src/ui/action/include/action/settings/suffix-for-cluster-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/decal-area-position.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/suffix-for-constraint-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/suffix-for-area-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

### Pattern: `../application.h` (1 occurrences)

**File:** `src/solver/signal-handling/windows.cpp:25`
**Line:** `#include "../application.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/application.h`

### Pattern: `../application/main.h` (4 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:36`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:32`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

### Pattern: `../application/menus.h` (2 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:24`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:35`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

### Pattern: `../application/recentfiles.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:34`
**Line:** `#include "../application/recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

### Pattern: `../application/study.h` (10 occurrences)

**File:** `src/ui/simulator/windows/saveas.h:25`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/saveas.cpp:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/sets.cpp:39`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/studylogs.h:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/exportmap.h:26`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 5 more occurrences

### Pattern: `../application/wait.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:36`
**Line:** `#include "../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../apply.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:23`
**Line:** `#include "../apply.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/apply.h`

### Pattern: `../area.h` (14 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

... and 9 more occurrences

### Pattern: `../area/links.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:37`
**Line:** `#include "../area/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/area/links.h`

### Pattern: `../atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:20`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:17`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/event.h:13`
**Line:** `#include "../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../bindConstraints.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:27`
**Line:** `#include "../bindConstraints.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/bindConstraints.h`

### Pattern: `../button.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/toolbox/components/map/component.h:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../calendar/calendar.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:26`
**Line:** `#include "../calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h`

### Pattern: `../categories.h` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:30`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/storage/fwd.h:24`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/storage/results.h:26`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:34`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/surveyresults.h:32`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

### Pattern: `../common/cluster.h` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:36`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster.h:30`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h`

### Pattern: `../common/cluster_list.h` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster_list.h:26`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster_list.h:28`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h`

### Pattern: `../commons/hydro.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:28`
**Line:** `#include "../commons/hydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/hydro.h`

### Pattern: `../commons/join.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:29`
**Line:** `#include "../commons/join.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/join.h`

### Pattern: `../commons/links/links.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links.h:36`
**Line:** `#include "../commons/links/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/links/links.h`

### Pattern: `../commons/load.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:30`
**Line:** `#include "../commons/load.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/load.h`

### Pattern: `../commons/miscGenMinusRowPSP.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:31`
**Line:** `#include "../commons/miscGenMinusRowPSP.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/miscGenMinusRowPSP.h`

### Pattern: `../commons/psp.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:32`
**Line:** `#include "../commons/psp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/psp.h`

### Pattern: `../commons/rowBalance.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:33`
**Line:** `#include "../commons/rowBalance.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/rowBalance.h`

### Pattern: `../commons/solar.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:34`
**Line:** `#include "../commons/solar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/solar.h`

### Pattern: `../commons/spatial-aggregate.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:35`
**Line:** `#include "../commons/spatial-aggregate.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/spatial-aggregate.h`

### Pattern: `../commons/wind.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:36`
**Line:** `#include "../commons/wind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/wind.h`

### Pattern: `../component.h` (7 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:24`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:27`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

... and 2 more occurrences

### Pattern: `../components/button.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../components/captionpanel.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/connection.cpp:25`
**Line:** `#include "../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../components/datagrid/gridhelper.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.h:27`
**Line:** `#include "../components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../components/htmllistbox/component.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.h:29`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:33`
**Line:** `// #include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:32`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

### Pattern: `../components/htmllistbox/datasource/renewable-cluster-order.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:36`
**Line:** `#include "../components/htmllistbox/datasource/renewable-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h`

### Pattern: `../components/htmllistbox/datasource/thermal-cluster-order.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:34`
**Line:** `#include "../components/htmllistbox/datasource/thermal-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.h`

### Pattern: `../components/htmllistbox/item/renewable-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:37`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

### Pattern: `../components/htmllistbox/item/thermal-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:33`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

### Pattern: `../components/htmllistbox/item/thermal-cluster.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:34`
**Line:** `// #include "../components/htmllistbox/item/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster.h`

### Pattern: `../components/progressbar.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:36`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.h:29`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h`

### Pattern: `../components/refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:48`
**Line:** `#include "../components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../components/wizardheader.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:44`
**Line:** `#include "../components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../constants.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:16`
**Line:** `#include "../constants.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/constants.h`

### Pattern: `../constraints/ConstraintBuilder.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h:24`
**Line:** `#include "../constraints/ConstraintBuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h`

### Pattern: `../content.h` (3 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:26`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:27`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:25`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

### Pattern: `../core/atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:16`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/thread/timer.h:13`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../core/atomic/int.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.cpp:16`
**Line:** `#include "../core/atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

### Pattern: `../core/bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:13`
**Line:** `#include "../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../core/event/event.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:13`
**Line:** `#include "../core/event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h`

### Pattern: `../core/math.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.cpp:14`
**Line:** `#include "../core/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

### Pattern: `../core/noncopyable.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:14`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:15`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:13`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../core/nonmovable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:14`
**Line:** `#include "../core/nonmovable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/nonmovable.h`

### Pattern: `../core/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.h:13`
**Line:** `#include "../core/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

### Pattern: `../core/smartptr/intrusive.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:17`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:15`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:16`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../core/static/inherit.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/array.h:15`
**Line:** `#include "../core/static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

### Pattern: `../core/string.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:14`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/uuid/uuid.cpp:17`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/job/job.h:15`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:16`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:13`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 4 more occurrences

### Pattern: `../core/string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/exists.cpp:15`
**Line:** `#include "../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../core/system/gettimeofday.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.cpp:17`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:24`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.cpp:27`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

### Pattern: `../core/system/windows.hdr.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:18`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/semaphore.cpp:29`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:26`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/policy.h:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

... and 4 more occurrences

### Pattern: `../create.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:37`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/wizardheader.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

### Pattern: `../cstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:17`
**Line:** `#include "../cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../daily/h2o_j_donnees_optimisation.h` (1 occurrences)

**File:** `src/solver/hydro/include/antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h:38`
**Line:** `#include "../daily/h2o_j_donnees_optimisation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/hydro/include/antares/solver/hydro/daily/h2o_j_donnees_optimisation.h`

### Pattern: `../datetime/timestamp.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:12`
**Line:** `#include "../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h`

### Pattern: `../define.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

### Pattern: `../directory.h` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:19`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

... and 2 more occurrences

### Pattern: `../directory/info/info.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:15`
**Line:** `#include "../directory/info/info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info/info.h`

### Pattern: `../drawingcontext.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.h:25`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:309`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.h:27`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

### Pattern: `../dynamiclibrary/symbol.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:48`
**Line:** `#include "../dynamiclibrary/symbol.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dynamiclibrary/symbol.h`

### Pattern: `../economy/profitByPlant.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/adequacy/all.h:70`
**Line:** `#include "../economy/profitByPlant.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/economy/profitByPlant.h`

### Pattern: `../endoflist.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:35`
**Line:** `#include "../endoflist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/endoflist.h`

### Pattern: `../event/interfaces.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:8`
**Line:** `#include "../event/interfaces.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/interfaces.h`

### Pattern: `../file.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/copy.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../filename-manipulation.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:13`
**Line:** `#include "../filename-manipulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/filename-manipulation.h`

### Pattern: `../filter.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/month.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/week.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/columnindex.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/weekday.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/houryear.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

... and 4 more occurrences

### Pattern: `../fwd.h` (9 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/variables.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/progression/progression.h:36`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

... and 4 more occurrences

### Pattern: `../gridhelper.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.h:28`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.cpp:23`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../info.h` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:36`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/info.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:20`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info.h`

### Pattern: `../input.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h`

### Pattern: `../inspector.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:34`
**Line:** `#include "../inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../inspector/data.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.h:24`
**Line:** `#include "../inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

### Pattern: `../io.h` (8 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.h:15`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:16`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:159`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

... and 3 more occurrences

### Pattern: `../item/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:24`
**Line:** `#include "../item/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/bindingconstraint.h`

### Pattern: `../item/cluster-item.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.h:27`
**Line:** `#include "../item/cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.h`

### Pattern: `../item/group.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:25`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

### Pattern: `../item/renewable-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:25`
**Line:** `#include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h:27`
**Line:** `// #include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

### Pattern: `../item/thermal-cluster-item.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:25`
**Line:** `#include "../item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

### Pattern: `../iterator.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:17`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:45`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h`

### Pattern: `../job.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:17`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:18`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

### Pattern: `../job/job.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:14`
**Line:** `#include "../job/job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

### Pattern: `../jobs.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/execute/execute.cpp:24`
**Line:** `#include "../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../layer.h` (2 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:25`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:26`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h`

### Pattern: `../length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:18`
**Line:** `#include "../length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../load-options.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/area/area.h:817`
**Line:** `#include "../load-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/load-options.h`

### Pattern: `../locales.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.cpp:24`
**Line:** `#include "../locales.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/locales.h`

### Pattern: `../main.h` (13 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/help.cpp:24`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:30`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/notes.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/menu.cpp:29`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 8 more occurrences

### Pattern: `../manager.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:23`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:310`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:25`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

### Pattern: `../math.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/functional/binaryfunctions.h:14`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/functional/loop.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:17`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/range.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

... and 4 more occurrences

### Pattern: `../matrix.h` (15 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

... and 10 more occurrences

### Pattern: `../menus.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:32`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/help.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/refresh.cpp:24`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/menu.cpp:30`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

... and 1 more occurrences

### Pattern: `../message.h` (2 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:40`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:51`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../modeler/mockModelerObjects.h` (1 occurrences)

**File:** `src/tests/src/expressions/test_DeepWideTrees.cpp:32`
**Line:** `#include "../modeler/mockModelerObjects.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/tests/src/modeler/mockModelerObjects.h`

### Pattern: `../noncopyable.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:18`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:22`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../notebook/mapnotebook.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.h:29`
**Line:** `#include "../notebook/mapnotebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/mapnotebook.h`

### Pattern: `../notebook/notebook.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/control.h:29`
**Line:** `#include "../notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

### Pattern: `../notes.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/frame.h:27`
**Line:** `#include "../notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../null.h` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/stdcout.h:36`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.h:13`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:14`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

... and 2 more occurrences

### Pattern: `../operator.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:28`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/equals.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/modulo.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/greaterthan.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/lessthanorequalsto.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

... and 4 more occurrences

### Pattern: `../opt_fonctions.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:29`
**Line:** `#include "../opt_fonctions.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_fonctions.h`

### Pattern: `../opt_rename_problem.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:30`
**Line:** `#include "../opt_rename_problem.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_rename_problem.h`

### Pattern: `../output.h` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:26`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:24`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:25`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

... and 1 more occurrences

### Pattern: `../panel.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.h:25`
**Line:** `#include "../panel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel.h`

### Pattern: `../panel/group.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:32`
**Line:** `#include "../panel/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel/group.h`

### Pattern: `../parts/thermal/cluster.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:39`
**Line:** `#include "../parts/thermal/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/thermal/cluster.h`

### Pattern: `../preprocessor/capabilities.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/capabilities.h:13`
**Line:** `#include "../preprocessor/capabilities.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/capabilities.h`

### Pattern: `../preprocessor/std.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/assert.h:13`
**Line:** `#include "../preprocessor/std.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/std.h`

### Pattern: `../recentfiles.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:24`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

**File:** `src/ui/simulator/application/main/menu.cpp:31`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

### Pattern: `../refresh.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:28`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../renderer.h` (13 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

... and 8 more occurrences

### Pattern: `../resources.h` (8 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:45`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:35`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:28`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 3 more occurrences

### Pattern: `../setofareas.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:37`
**Line:** `#include "../setofareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/setofareas.h`

### Pattern: `../settings.h` (6 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/connectioncreator.cpp:23`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:25`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:22`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

... and 1 more occurrences

### Pattern: `../slist/slist.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:21`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:18`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

### Pattern: `../smartptr.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:46`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/event/event.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

### Pattern: `../smartptr/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/types.h:14`
**Line:** `#include "../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h`

### Pattern: `../spotlight/area.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.cpp:33`
**Line:** `#include "../spotlight/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/area.h`

### Pattern: `../state.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-utils.h:5`
**Line:** `#include "../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h`

### Pattern: `../static/assert.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:44`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.hxx:13`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:15`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:19`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../static/dynamiccast.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:9`
**Line:** `#include "../static/dynamiccast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/dynamiccast.h`

### Pattern: `../static/if.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:17`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:14`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/functional/view.hxx:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/iterator/iterator.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

### Pattern: `../static/inherit.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:10`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:20`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

### Pattern: `../static/moveconstructor.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:18`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:15`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h`

### Pattern: `../static/remove.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:45`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:15`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/traits/cstring.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/traits/length.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:16`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

... and 1 more occurrences

### Pattern: `../static/typedef.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:17`
**Line:** `#include "../static/typedef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/typedef.h`

### Pattern: `../static/types.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:43`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:15`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:16`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/math/distance.hxx:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

### Pattern: `../string.h` (11 occurrences)

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/system/environment.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/getopt/option.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/logs/null.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 6 more occurrences

### Pattern: `../string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:19`
**Line:** `#include "../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../study.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:25`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:31`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/menu.cpp:32`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/main.cpp:28`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../surveyresults.h` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:31`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:37`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h`

### Pattern: `../system/console.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/verbosity.h:13`
**Line:** `#include "../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

### Pattern: `../system/memory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/memory.cpp:14`
**Line:** `#include "../system/memory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/memory.h`

### Pattern: `../system/suspend.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:21`
**Line:** `#include "../system/suspend.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/suspend.h`

### Pattern: `../system/windows.hdr.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:15`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.cpp:13`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../thermal-cluster/common.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:37`
**Line:** `#include "../thermal-cluster/common.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/common.h`

### Pattern: `../thermal-cluster/create.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:35`
**Line:** `#include "../thermal-cluster/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/create.h`

### Pattern: `../thermal-cluster/root-node.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:36`
**Line:** `#include "../thermal-cluster/root-node.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/root-node.h`

### Pattern: `../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:17`
**Line:** `#include "../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../thread/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:14`
**Line:** `#include "../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

### Pattern: `../thread/utility.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/spawn.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h`

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h`

### Pattern: `../toolbox/components/button.h` (6 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/sets.cpp:27`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:39`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/startupwizard.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:30`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

... and 1 more occurrences

### Pattern: `../toolbox/components/datagrid/component.h` (1 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:40`
**Line:** `#include "../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

### Pattern: `../toolbox/components/datagrid/renderer/connection.h` (1 occurrences)

**File:** `src/ui/simulator/windows/connection.h:24`
**Line:** `#include "../toolbox/components/datagrid/renderer/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/connection.h`

### Pattern: `../toolbox/components/datagrid/renderer/logfile.h` (2 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:41`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h`

**File:** `src/ui/simulator/windows/studylogs.h:35`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h`

### Pattern: `../toolbox/components/mainpanel.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:40`
**Line:** `#include "../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

### Pattern: `../toolbox/components/map/component.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:41`
**Line:** `#include "../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

### Pattern: `../toolbox/components/map/settings.h` (2 occurrences)

**File:** `src/ui/simulator/windows/exportmap.h:27`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:38`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../toolbox/components/wizardheader.h` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../toolbox/create.h` (9 occurrences)

**File:** `src/ui/simulator/windows/connection.cpp:23`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/saveas.cpp:33`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/sets.cpp:28`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:38`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:26`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 4 more occurrences

### Pattern: `../toolbox/dispatcher/study.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:41`
**Line:** `#include "../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

### Pattern: `../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:42`
**Line:** `#include "../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../toolbox/jobs.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:43`
**Line:** `#include "../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../toolbox/resources.h` (7 occurrences)

**File:** `src/ui/simulator/application/application.cpp:33`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/menus.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/sets.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:37`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/message.cpp:32`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 2 more occurrences

### Pattern: `../toolbox/validator.h` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../toolbox/wx-wrapper.h` (4 occurrences)

**File:** `src/ui/simulator/application/application.h:24`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/study.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/menus.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/recentfiles.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

### Pattern: `../tools/connectioncreator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:23`
**Line:** `#include "../tools/connectioncreator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/connectioncreator.h`

### Pattern: `../tools/remover.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:23`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:24`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h`

### Pattern: `../tools/tool.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.h:26`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:24`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h`

### Pattern: `../traits/cstring.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/username.hxx:15`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:18`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../traits/length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:19`
**Line:** `#include "../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../trigonometric.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vector3D.h:14`
**Line:** `#include "../trigonometric.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/trigonometric.h`

### Pattern: `../undef.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/time.h:126`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:160`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:89`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:41`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

### Pattern: `../unit.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

### Pattern: `../validator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:17`
**Line:** `#include "../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/validator.h`

### Pattern: `../validator/text/default.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/getopt/parser.h:17`
**Line:** `#include "../validator/text/default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/text/default.h`

### Pattern: `../variable.h` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/lolpCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/priceCSR.h:31`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/overallCostCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/loldCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-csr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

### Pattern: `../variables/VariableManagement.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:31`
**Line:** `#include "../variables/VariableManagement.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagement.h`

### Pattern: `../variables/VariableManagerUtils.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h:32`
**Line:** `#include "../variables/VariableManagerUtils.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagerUtils.h`

### Pattern: `../version.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/finder/finder.h:32`
**Line:** `#include "../version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/version.h`

### Pattern: `../wait.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/create.cpp:83`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/application/main/main.cpp:59`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:31`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../windows.hdr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/console/console.hxx:13`
**Line:** `#include "../windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../windows/inspector/inspector.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:44`
**Line:** `#include "../windows/inspector/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/inspector.h`

### Pattern: `../windows/message.h` (2 occurrences)

**File:** `src/ui/simulator/application/application.cpp:40`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/study.cpp:45`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../windows/saveas.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:46`
**Line:** `#include "../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h`

### Pattern: `../windows/startupwizard.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:47`
**Line:** `#include "../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h`

### Pattern: `../wx-wrapper.h` (2 occurrences)

**File:** `src/ui/common/dispatcher/settings.h:24`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/dispatcher/gui.h:25`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

### Pattern: `../yuni.h` (18 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/job.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:18`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:14`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/dictionary.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 13 more occurrences


## Other (1890 occurrences)
--------------------------------------------------
### Pattern: `../";` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/versions.cpp:435`
**Line:** `norm += "/../";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/tools/";`

### Pattern: `../..";` (1 occurrences)

**File:** `src/ui/simulator/application/application.cpp:82`
**Line:** `Forms::StudyToLoadAtStartup.clear() << t << "/../..";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/..";`

### Pattern: `../../.."` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/versions.cpp:53`
**Line:** `loadFromPath(root + "/../../..");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/.."`

### Pattern: `../../../../../../application/study.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.cpp:23`
**Line:** `#include "../../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../../../../../../application/study.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.cpp:23`
**Line:** `#include "../../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

### Pattern: `../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h` (1 occurrences)

**File:** `src/io/inputs/yml-system/include/antares/io/inputs/yml-system/converter.h:27`
**Line:** `#include "../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/io/inputs/yml-model/include/antares/io/inputs/yml-model/parser.h`

### Pattern: `../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h"` (1 occurrences)

**File:** `src/io/inputs/yml-system/include/antares/io/inputs/yml-system/converter.h:27`
**Line:** `#include "../../../../../../yml-model/include/antares/io/inputs/yml-model/parser.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/io/inputs/yml-model/include/antares/io/inputs/yml-model/parser.h"`

### Pattern: `../../../../../application/study.h` (10 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:26`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:29`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:23`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 5 more occurrences

### Pattern: `../../../../../application/study.h"` (10 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:26`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:29`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:23`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:24`
**Line:** `#include "../../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

... and 5 more occurrences

### Pattern: `../../../../../windows/inspector.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:24`
**Line:** `#include "../../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../../../windows/inspector.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:24`
**Line:** `#include "../../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

### Pattern: `../../../../application/main.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:25`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.cpp:24`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:29`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

### Pattern: `../../../../application/main.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:25`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.cpp:24`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:29`
**Line:** `#include "../../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

### Pattern: `../../../../application/main/main.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:27`
**Line:** `#include "../../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

### Pattern: `../../../../application/main/main.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:27`
**Line:** `#include "../../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

### Pattern: `../../../../application/study.h` (16 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:26`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.cpp:25`
**Line:** `#include "../../../../application/study.h" // OnStudyChanged`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:23`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:29`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:25`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 11 more occurrences

### Pattern: `../../../../application/study.h"` (16 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:26`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.cpp:25`
**Line:** `#include "../../../../application/study.h" // OnStudyChanged`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:23`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:29`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:25`
**Line:** `#include "../../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

... and 11 more occurrences

### Pattern: `../../../../application/wait.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:26`
**Line:** `#include "../../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../../../../application/wait.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.hxx:26`
**Line:** `#include "../../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

### Pattern: `../../../../input/area.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:25`
**Line:** `#include "../../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../../input/area.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:25`
**Line:** `#include "../../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

### Pattern: `../../../../input/renewable-cluster.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:27`
**Line:** `#include "../../../../input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

### Pattern: `../../../../input/renewable-cluster.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:27`
**Line:** `#include "../../../../input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h"`

### Pattern: `../../../../input/thermal-cluster.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:26`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

### Pattern: `../../../../input/thermal-cluster.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:26`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h:24`
**Line:** `#include "../../../../input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h"`

### Pattern: `../../../../validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:24`
**Line:** `#include "../../../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../../../validator.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:24`
**Line:** `#include "../../../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

### Pattern: `../../../../windows/inspector.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:22`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:29`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:26`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../../windows/inspector.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:22`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:29`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:26`
**Line:** `#include "../../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

### Pattern: `../../../../windows/message.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:26`
**Line:** `#include "../../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../../../windows/message.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:26`
**Line:** `#include "../../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

### Pattern: `../../../application/main.h` (10 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:36`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:34`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:25`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:48`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:30`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 5 more occurrences

### Pattern: `../../../application/main.h"` (10 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:36`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:34`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:25`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:48`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:30`
**Line:** `#include "../../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

... and 5 more occurrences

### Pattern: `../../../application/main/internal-ids.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:32`
**Line:** `#include "../../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h`

### Pattern: `../../../application/main/internal-ids.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:32`
**Line:** `#include "../../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h"`

### Pattern: `../../../application/main/main.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:28`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:36`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

### Pattern: `../../../application/main/main.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:28`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:36`
**Line:** `#include "../../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

### Pattern: `../../../application/menus.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:49`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:31`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

### Pattern: `../../../application/menus.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:49`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:31`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:30`
**Line:** `#include "../../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

### Pattern: `../../../application/study.h` (19 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:33`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:29`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer.cpp:23`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:50`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 14 more occurrences

### Pattern: `../../../application/study.h"` (19 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:33`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:29`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer.cpp:23`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:50`
**Line:** `#include "../../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

... and 14 more occurrences

### Pattern: `../../../application/wait.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:28`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:51`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:33`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

... and 4 more occurrences

### Pattern: `../../../application/wait.h"` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/dbgrid.cpp:28`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:51`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:29`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:33`
**Line:** `#include "../../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

... and 4 more occurrences

### Pattern: `../../../common/lock.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:41`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/application/main/refresh.cpp:28`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:25`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/components/refresh.cpp:26`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:29`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

... and 1 more occurrences

### Pattern: `../../../common/lock.h"` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:41`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/simulator/application/main/refresh.cpp:28`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:25`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/simulator/toolbox/components/refresh.cpp:26`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:29`
**Line:** `#include "../../../common/lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

... and 1 more occurrences

### Pattern: `../../../core/iterator/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:13`
**Line:** `#include "../../../core/iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h`

### Pattern: `../../../core/iterator/iterator.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:13`
**Line:** `#include "../../../core/iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h"`

### Pattern: `../../../core/noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:16`
**Line:** `#include "../../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../../core/noncopyable.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:16`
**Line:** `#include "../../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

### Pattern: `../../../core/slist.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:17`
**Line:** `#include "../../../core/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist.h`

### Pattern: `../../../core/slist.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:17`
**Line:** `#include "../../../core/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist.h"`

### Pattern: `../../../core/smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:13`
**Line:** `#include "../../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../../core/smartptr/intrusive.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:13`
**Line:** `#include "../../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

### Pattern: `../../../core/static/remove.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:13`
**Line:** `#include "../../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

### Pattern: `../../../core/static/remove.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:13`
**Line:** `#include "../../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

### Pattern: `../../../core/string.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

### Pattern: `../../../core/string.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:14`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.h:13`
**Line:** `#include "../../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

### Pattern: `../../../core/system/windows.hdr.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.cpp:20`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.cpp:18`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:26`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../../core/system/windows.hdr.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.cpp:20`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.cpp:18`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:26`
**Line:** `#include "../../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

### Pattern: `../../../core/traits/cstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:14`
**Line:** `#include "../../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../../../core/traits/cstring.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:14`
**Line:** `#include "../../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

### Pattern: `../../../core/traits/length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:15`
**Line:** `#include "../../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../../core/traits/length.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:15`
**Line:** `#include "../../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

### Pattern: `../../../datetime/timestamp.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:30`
**Line:** `#include "../../../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h`

### Pattern: `../../../datetime/timestamp.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:30`
**Line:** `#include "../../../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h"`

### Pattern: `../../../input/area.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:25`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:24`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../input/area.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:25`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:24`
**Line:** `#include "../../../input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

### Pattern: `../../../input/connection.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/connection.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h`

### Pattern: `../../../input/connection.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/connection.h:24`
**Line:** `#include "../../../input/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/connection.h"`

### Pattern: `../../../io/directory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:31`
**Line:** `#include "../../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../../io/directory.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:31`
**Line:** `#include "../../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

### Pattern: `../../../io/file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:15`
**Line:** `#include "../../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../../io/file.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:15`
**Line:** `#include "../../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

### Pattern: `../../../job/queue/service.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:13`
**Line:** `#include "../../../job/queue/service.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/queue/service.h`

### Pattern: `../../../job/queue/service.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:13`
**Line:** `#include "../../../job/queue/service.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/queue/service.h"`

### Pattern: `../../../renderer.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h:26`
**Line:** `#include "../../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

### Pattern: `../../../renderer.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h:26`
**Line:** `#include "../../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

### Pattern: `../../../simulator/application/study.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:24`
**Line:** `#include "../../../simulator/application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../../../simulator/application/study.h"` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:24`
**Line:** `#include "../../../simulator/application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

### Pattern: `../../../thread/policy.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:30`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:29`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:13`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

### Pattern: `../../../thread/policy.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:30`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:29`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:13`
**Line:** `#include "../../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

### Pattern: `../../../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:14`
**Line:** `#include "../../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../../../thread/signal.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:14`
**Line:** `#include "../../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h"`

### Pattern: `../../../thread/thread.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:14`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/event/flow/timer.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

**File:** `src/ext/yuni/src/yuni/core/event/flow/continuous.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

... and 1 more occurrences

### Pattern: `../../../thread/thread.h"` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/private/jobs/queue/thread.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.cpp:14`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

**File:** `src/ext/yuni/src/yuni/core/event/flow/timer.h:15`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

**File:** `src/ext/yuni/src/yuni/core/event/flow/continuous.h:13`
**Line:** `#include "../../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

... and 1 more occurrences

### Pattern: `../../../windows/calendar/calendar.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:55`
**Line:** `#include "../../../windows/calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h`

### Pattern: `../../../windows/calendar/calendar.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:55`
**Line:** `#include "../../../windows/calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h"`

### Pattern: `../../../windows/correlation/correlation.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:24`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h`

### Pattern: `../../../windows/correlation/correlation.h"` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h"`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h"`

**File:** `src/ui/simulator/application/main/build/load.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h"`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:24`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h"`

**File:** `src/ui/simulator/application/main/build/wind.cpp:25`
**Line:** `#include "../../../windows/correlation/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/correlation/correlation.h"`

### Pattern: `../../../windows/hydro/allocation.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:29`
**Line:** `#include "../../../windows/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/allocation.h`

### Pattern: `../../../windows/hydro/allocation.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:29`
**Line:** `#include "../../../windows/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/allocation.h"`

### Pattern: `../../../windows/hydro/localdatahydro.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:28`
**Line:** `#include "../../../windows/hydro/localdatahydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/localdatahydro.h`

### Pattern: `../../../windows/hydro/localdatahydro.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:28`
**Line:** `#include "../../../windows/hydro/localdatahydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/localdatahydro.h"`

### Pattern: `../../../windows/hydro/prepro.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:27`
**Line:** `#include "../../../windows/hydro/prepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/prepro.h`

### Pattern: `../../../windows/hydro/prepro.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:27`
**Line:** `#include "../../../windows/hydro/prepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/prepro.h"`

### Pattern: `../../../windows/hydro/series.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:26`
**Line:** `#include "../../../windows/hydro/series.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/series.h`

### Pattern: `../../../windows/hydro/series.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/hydro.cpp:26`
**Line:** `#include "../../../windows/hydro/series.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/hydro/series.h"`

### Pattern: `../../../windows/inspector.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/build-context.cpp:23`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:38`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:30`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:37`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../../../windows/inspector.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/build-context.cpp:23`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:38`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:30`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/components/map/control.cpp:37`
**Line:** `#include "../../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

### Pattern: `../../../windows/message.h` (3 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/optimization/optimization.cpp:30`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../../windows/message.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/windows/options/advanced/advanced.cpp:31`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/windows/options/optimization/optimization.cpp:30`
**Line:** `#include "../../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

### Pattern: `../../../windows/notes.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/notes.cpp:23`
**Line:** `#include "../../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../../../windows/notes.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/notes.cpp:23`
**Line:** `#include "../../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h"`

### Pattern: `../../../windows/sets.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/sets.cpp:23`
**Line:** `#include "../../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h`

### Pattern: `../../../windows/sets.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/sets.cpp:23`
**Line:** `#include "../../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h"`

### Pattern: `../../../windows/xcast/xcast.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h`

### Pattern: `../../../windows/xcast/xcast.h"` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h"`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h"`

**File:** `src/ui/simulator/application/main/build/load.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h"`

**File:** `src/ui/simulator/application/main/build/wind.cpp:23`
**Line:** `#include "../../../windows/xcast/xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/xcast/xcast.h"`

### Pattern: `../../../yuni.h` (20 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:12`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:19`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/system/console/console.h:13`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 15 more occurrences

### Pattern: `../../../yuni.h"` (20 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/policies.h:12`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:19`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/system/console/console.h:13`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:15`
**Line:** `#include "../../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

... and 15 more occurrences

### Pattern: `../../application/main.h` (15 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:46`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:34`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:26`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 10 more occurrences

### Pattern: `../../application/main.h"` (15 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:46`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:34`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/input/area.cpp:26`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:23`
**Line:** `#include "../../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

... and 10 more occurrences

### Pattern: `../../application/main/internal-ids.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:41`
**Line:** `#include "../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h`

### Pattern: `../../application/main/internal-ids.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:41`
**Line:** `#include "../../application/main/internal-ids.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/internal-ids.h"`

### Pattern: `../../application/main/main.h` (6 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:28`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:26`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:48`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h`

... and 1 more occurrences

### Pattern: `../../application/main/main.h"` (6 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:28`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

**File:** `src/ui/simulator/toolbox/dispatcher/study.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

**File:** `src/ui/simulator/toolbox/components/mainpanel.cpp:26`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

**File:** `src/ui/simulator/windows/textinput/input.cpp:24`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:48`
**Line:** `#include "../../application/main/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/main.h"`

... and 1 more occurrences

### Pattern: `../../application/menus.h` (12 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/action-panel.cpp:29`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:25`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/output/output.cpp:28`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

... and 7 more occurrences

### Pattern: `../../application/menus.h"` (12 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/action-panel.cpp:29`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:30`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:25`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/windows/output/output.cpp:28`
**Line:** `#include "../../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

... and 7 more occurrences

### Pattern: `../../application/study.h` (26 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:27`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:26`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:25`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 21 more occurrences

### Pattern: `../../application/study.h"` (26 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:27`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:26`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/input/area.cpp:25`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:22`
**Line:** `#include "../../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

... and 21 more occurrences

### Pattern: `../../application/wait.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:39`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../../application/wait.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:24`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:39`
**Line:** `#include "../../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

### Pattern: `../../atomic/int.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:30`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:15`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:14`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

### Pattern: `../../atomic/int.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:30`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h"`

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:15`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:14`
**Line:** `#include "../../atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h"`

### Pattern: `../../button.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:27`
**Line:** `#include "../../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../button.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:27`
**Line:** `#include "../../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

### Pattern: `../../clipboard/clipboard.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:31`
**Line:** `#include "../../clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../clipboard/clipboard.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/manager.cpp:31`
**Line:** `#include "../../clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h"`

### Pattern: `../../component.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.cpp:24`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

### Pattern: `../../component.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.hxx:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.cpp:25`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.cpp:24`
**Line:** `#include "../../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

### Pattern: `../../components/button.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:37`
**Line:** `#include "../../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../components/button.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:37`
**Line:** `#include "../../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

### Pattern: `../../components/captionpanel.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:23`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../components/captionpanel.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:23`
**Line:** `#include "../../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

### Pattern: `../../components/datagrid/filter/component.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:59`
**Line:** `#include "../../components/datagrid/filter/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/component.h`

### Pattern: `../../components/datagrid/filter/component.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:59`
**Line:** `#include "../../components/datagrid/filter/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/component.h"`

### Pattern: `../../components/map/component.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:37`
**Line:** `#include "../../components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

### Pattern: `../../components/map/component.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:37`
**Line:** `#include "../../components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

### Pattern: `../../components/map/settings.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../../components/map/settings.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:27`
**Line:** `#include "../../components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

### Pattern: `../../core/atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:15`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:13`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../../core/atomic/bool.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:15`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:13`
**Line:** `#include "../../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

### Pattern: `../../core/dictionary.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:14`
**Line:** `#include "../../core/dictionary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dictionary.h`

### Pattern: `../../core/dictionary.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:14`
**Line:** `#include "../../core/dictionary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dictionary.h"`

### Pattern: `../../core/noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:15`
**Line:** `#include "../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../core/noncopyable.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:15`
**Line:** `#include "../../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

### Pattern: `../../core/slist/slist.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:16`
**Line:** `#include "../../core/slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

### Pattern: `../../core/slist/slist.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:16`
**Line:** `#include "../../core/slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h"`

### Pattern: `../../core/smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:15`
**Line:** `#include "../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../core/smartptr/intrusive.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:15`
**Line:** `#include "../../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

### Pattern: `../../core/static/assert.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:16`
**Line:** `#include "../../core/static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../../core/static/assert.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:16`
**Line:** `#include "../../core/static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

### Pattern: `../../core/static/remove.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:17`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

### Pattern: `../../core/static/remove.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:17`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:13`
**Line:** `#include "../../core/static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

### Pattern: `../../core/static/types.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:14`
**Line:** `#include "../../core/static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

### Pattern: `../../core/static/types.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:14`
**Line:** `#include "../../core/static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

### Pattern: `../../core/string.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:27`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:18`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.hxx:13`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:16`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:14`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 1 more occurrences

### Pattern: `../../core/string.h"` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:27`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/file/stream.h:18`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/file/stream.hxx:13`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:16`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:14`
**Line:** `#include "../../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

... and 1 more occurrences

### Pattern: `../../core/string/wstring.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:28`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:38`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:21`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

... and 1 more occurrences

### Pattern: `../../core/string/wstring.h"` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:28`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:18`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:38`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:21`
**Line:** `#include "../../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

... and 1 more occurrences

### Pattern: `../../core/system/cpu.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:14`
**Line:** `#include "../../core/system/cpu.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/cpu.h`

### Pattern: `../../core/system/cpu.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:14`
**Line:** `#include "../../core/system/cpu.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/cpu.h"`

### Pattern: `../../core/system/environment.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/system.cpp:14`
**Line:** `#include "../../core/system/environment.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/environment.h`

### Pattern: `../../core/system/environment.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/system.cpp:14`
**Line:** `#include "../../core/system/environment.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/environment.h"`

### Pattern: `../../core/system/windows.hdr.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:34`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:30`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:22`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/io/directory/commons.h:14`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../core/system/windows.hdr.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/stream.cpp:34`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:30`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:22`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/commons.h:14`
**Line:** `#include "../../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

### Pattern: `../../core/traits/cstring.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../../core/traits/cstring.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:14`
**Line:** `#include "../../core/traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

### Pattern: `../../core/traits/length.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../core/traits/length.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:15`
**Line:** `#include "../../core/traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

### Pattern: `../../create.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:57`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:26`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:35`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:36`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

### Pattern: `../../create.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:57`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:26`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:35`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/components/map/component.cpp:36`
**Line:** `#include "../../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

### Pattern: `../../default.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:25`
**Line:** `#include "../../default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/default.h`

### Pattern: `../../default.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.cpp:25`
**Line:** `#include "../../default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/default.h"`

### Pattern: `../../directory.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:18`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../directory.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:16`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:18`
**Line:** `#include "../../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

### Pattern: `../../dispatcher.h` (1 occurrences)

**File:** `src/ui/common/component/frame/registry.cpp:26`
**Line:** `#include "../../dispatcher.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher.h`

### Pattern: `../../dispatcher.h"` (1 occurrences)

**File:** `src/ui/common/component/frame/registry.cpp:26`
**Line:** `#include "../../dispatcher.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher.h"`

### Pattern: `../../dispatcher/gui.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/mini-frame.cpp:23`
**Line:** `#include "../../dispatcher/gui.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher/gui.h`

### Pattern: `../../dispatcher/gui.h"` (1 occurrences)

**File:** `src/ui/common/component/spotlight/mini-frame.cpp:23`
**Line:** `#include "../../dispatcher/gui.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/dispatcher/gui.h"`

### Pattern: `../../event/event.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:15`
**Line:** `#include "../../event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h`

### Pattern: `../../event/event.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:15`
**Line:** `#include "../../event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h"`

### Pattern: `../../file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.cpp:14`
**Line:** `#include "../../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../file.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.cpp:14`
**Line:** `#include "../../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

### Pattern: `../../fwd.h` (13 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster_list.h:32`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster.h:34`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:35`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/ecoInput.h:27`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:24`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

... and 8 more occurrences

### Pattern: `../../fwd.h"` (13 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster_list.h:32`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/common/cluster.h:34`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:35`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/ecoInput.h:27`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:24`
**Line:** `#include "../../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

... and 8 more occurrences

### Pattern: `../../gridhelper.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../../gridhelper.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h:24`
**Line:** `#include "../../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

### Pattern: `../../input/input.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:27`
**Line:** `#include "../../input/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h`

### Pattern: `../../input/input.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:27`
**Line:** `#include "../../input/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h"`

### Pattern: `../../io.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:16`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:19`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

### Pattern: `../../io.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/iterator/iterator.hxx:16`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/info.h:17`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:19`
**Line:** `#include "../../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

### Pattern: `../../io/directory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:22`
**Line:** `#include "../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

### Pattern: `../../io/directory.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:22`
**Line:** `#include "../../io/directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

### Pattern: `../../io/file.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:23`
**Line:** `#include "../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../../io/file.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:23`
**Line:** `#include "../../io/file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

### Pattern: `../../iterator/iterator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:20`
**Line:** `#include "../../iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h`

### Pattern: `../../iterator/iterator.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:20`
**Line:** `#include "../../iterator/iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator/iterator.h"`

### Pattern: `../../jobs.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:35`
**Line:** `#include "../../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../../jobs.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:35`
**Line:** `#include "../../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h"`

### Pattern: `../../lock.h` (3 occurrences)

**File:** `src/ui/common/component/panel/panel.cpp:23`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/common/component/spotlight/listbox-panel.cpp:25`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

**File:** `src/ui/common/component/spotlight/spotlight.cpp:31`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h`

### Pattern: `../../lock.h"` (3 occurrences)

**File:** `src/ui/common/component/panel/panel.cpp:23`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/common/component/spotlight/listbox-panel.cpp:25`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

**File:** `src/ui/common/component/spotlight/spotlight.cpp:31`
**Line:** `#include "../../lock.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/lock.h"`

### Pattern: `../../math/base.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/integer.h:15`
**Line:** `#include "../../math/base.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/base.h`

### Pattern: `../../math/base.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/integer.h:15`
**Line:** `#include "../../math/base.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/base.h"`

### Pattern: `../../math/math.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.h:19`
**Line:** `#include "../../math/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

### Pattern: `../../math/math.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/traits.h:19`
**Line:** `#include "../../math/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h"`

### Pattern: `../../message.h` (3 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../../message.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:36`
**Line:** `#include "../../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

### Pattern: `../../noncopyable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:14`
**Line:** `#include "../../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../../noncopyable.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/process-info.h:14`
**Line:** `#include "../../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

### Pattern: `../../private/jobs/queue/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:15`
**Line:** `#include "../../private/jobs/queue/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/private/jobs/queue/thread.h`

### Pattern: `../../private/jobs/queue/thread.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:15`
**Line:** `#include "../../private/jobs/queue/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/private/jobs/queue/thread.h"`

### Pattern: `../../refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.cpp:23`
**Line:** `#include "../../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../refresh.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.cpp:23`
**Line:** `#include "../../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

### Pattern: `../../renderer.h` (8 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

... and 3 more occurrences

### Pattern: `../../renderer.h"` (8 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/links/summary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/common.areasummary.h:24`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h:25`
**Line:** `#include "../../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

... and 3 more occurrences

### Pattern: `../../smartptr.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:16`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/process/program/stream.h:14`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/triangle.h:13`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

... and 1 more occurrences

### Pattern: `../../smartptr.h"` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:16`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/process/program/stream.h:14`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:17`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/triangle.h:13`
**Line:** `#include "../../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

... and 1 more occurrences

### Pattern: `../../smartptr/intrusive.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:21`
**Line:** `#include "../../smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../../smartptr/intrusive.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:21`
**Line:** `#include "../../smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

### Pattern: `../../smartptr/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:22`
**Line:** `#include "../../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h`

### Pattern: `../../smartptr/smartptr.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:22`
**Line:** `#include "../../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h"`

### Pattern: `../../spotlight/constraint.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:38`
**Line:** `#include "../../spotlight/constraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/constraint.h`

### Pattern: `../../spotlight/constraint.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:38`
**Line:** `#include "../../spotlight/constraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/constraint.h"`

### Pattern: `../../state.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/commons/links/links.h:30`
**Line:** `#include "../../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h`

### Pattern: `../../state.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/commons/links/links.h:30`
**Line:** `#include "../../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h"`

### Pattern: `../../static/assert.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:31`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:15`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../../static/assert.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:31`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:15`
**Line:** `#include "../../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

### Pattern: `../../static/if.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:23`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:16`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

### Pattern: `../../static/if.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/tree/n/treeN.h:23`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:16`
**Line:** `#include "../../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

### Pattern: `../../static/method.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:32`
**Line:** `#include "../../static/method.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/method.h`

### Pattern: `../../static/method.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/policies/ownership.h:32`
**Line:** `#include "../../static/method.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/method.h"`

### Pattern: `../../string.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:14`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/string.h`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

### Pattern: `../../string.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:14`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/string.h"`

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vertex.h:16`
**Line:** `#include "../../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

### Pattern: `../../string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:12`
**Line:** `#include "../../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../../string/wstring.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:12`
**Line:** `#include "../../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

### Pattern: `../../system/console.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/verbositylevel.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

### Pattern: `../../system/console.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/verbositylevel.h:13`
**Line:** `#include "../../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h"`

### Pattern: `../../system/windows.hdr.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:10`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.cpp:14`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../../system/windows.hdr.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/windows.inc.hpp:10`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.cpp:14`
**Line:** `#include "../../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

### Pattern: `../../thread/array.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:16`
**Line:** `#include "../../thread/array.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/array.h`

### Pattern: `../../thread/array.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.cpp:16`
**Line:** `#include "../../thread/array.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/array.h"`

### Pattern: `../../thread/policy.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:42`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

**File:** `src/ext/yuni/src/yuni/core/event/interfaces.h:16`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h`

### Pattern: `../../thread/policy.h"` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:42`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:13`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

**File:** `src/ext/yuni/src/yuni/core/event/interfaces.h:16`
**Line:** `#include "../../thread/policy.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/policy.h"`

### Pattern: `../../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:16`
**Line:** `#include "../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../../thread/signal.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:16`
**Line:** `#include "../../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h"`

### Pattern: `../../thread/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:15`
**Line:** `#include "../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

### Pattern: `../../thread/thread.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:15`
**Line:** `#include "../../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

### Pattern: `../../thread/timer.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.hxx:13`
**Line:** `#include "../../thread/timer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/timer.h`

### Pattern: `../../thread/timer.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/service.hxx:13`
**Line:** `#include "../../thread/timer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/timer.h"`

### Pattern: `../../traits/length.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/fill.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/assign.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/append.h:19`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

**File:** `src/ext/yuni/src/yuni/core/string/traits/vnsprintf.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../../traits/length.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/traits/fill.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

**File:** `src/ext/yuni/src/yuni/core/string/traits/assign.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

**File:** `src/ext/yuni/src/yuni/core/string/traits/append.h:19`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

**File:** `src/ext/yuni/src/yuni/core/string/traits/vnsprintf.h:13`
**Line:** `#include "../../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

### Pattern: `../../validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:34`
**Line:** `#include "../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../validator.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/infos.cpp:34`
**Line:** `#include "../../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

### Pattern: `../../variable.h` (10 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowQuad.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionFee.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionProbability.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowLinear.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/marginalCost.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

... and 5 more occurrences

### Pattern: `../../variable.h"` (10 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowQuad.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionFee.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/congestionProbability.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/flowLinear.h:24`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/links/marginalCost.h:26`
**Line:** `#include "../../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

... and 5 more occurrences

### Pattern: `../../windows/aboutbox.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:29`
**Line:** `#include "../../windows/aboutbox.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/aboutbox.h`

### Pattern: `../../windows/aboutbox.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:29`
**Line:** `#include "../../windows/aboutbox.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/aboutbox.h"`

### Pattern: `../../windows/constraints-builder/constraintsbuilder.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:28`
**Line:** `#include "../../windows/constraints-builder/constraintsbuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/constraints-builder/constraintsbuilder.h`

### Pattern: `../../windows/constraints-builder/constraintsbuilder.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:28`
**Line:** `#include "../../windows/constraints-builder/constraintsbuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/constraints-builder/constraintsbuilder.h"`

### Pattern: `../../windows/exportmap.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:27`
**Line:** `#include "../../windows/exportmap.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/exportmap.h`

### Pattern: `../../windows/exportmap.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:27`
**Line:** `#include "../../windows/exportmap.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/exportmap.h"`

### Pattern: `../../windows/inspector.h` (12 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:39`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/application/main/events.edit.cpp:28`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/application/main/main.cpp:56`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:31`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:27`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

... and 7 more occurrences

### Pattern: `../../windows/inspector.h"` (12 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:39`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/application/main/events.edit.cpp:28`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/application/main/main.cpp:56`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:31`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:27`
**Line:** `#include "../../windows/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

... and 7 more occurrences

### Pattern: `../../windows/inspector/data.h` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

### Pattern: `../../windows/inspector/data.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.h:28`
**Line:** `#include "../../windows/inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h"`

### Pattern: `../../windows/inspector/frame.h` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.cpp:24`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:29`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:36`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h`

### Pattern: `../../windows/inspector/frame.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/cluster.cpp:24`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:29`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h"`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:36`
**Line:** `#include "../../windows/inspector/frame.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/frame.h"`

### Pattern: `../../windows/memorystatistics.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:38`
**Line:** `#include "../../windows/memorystatistics.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/memorystatistics.h`

### Pattern: `../../windows/memorystatistics.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:38`
**Line:** `#include "../../windows/memorystatistics.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/memorystatistics.h"`

### Pattern: `../../windows/message.h` (16 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:40`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/options.cpp:32`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/help.cpp:31`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/main/menu.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

... and 11 more occurrences

### Pattern: `../../windows/message.h"` (16 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:40`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/application/main/options.cpp:32`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/application/main/help.cpp:31`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/application/main/events.file.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:28`
**Line:** `#include "../../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

... and 11 more occurrences

### Pattern: `../../windows/notes.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:23`
**Line:** `#include "../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../../windows/notes.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:23`
**Line:** `#include "../../windows/notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h"`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-areas.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:30`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-areas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-areas.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:30`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-areas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h"`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-options.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:29`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h`

### Pattern: `../../windows/options/adequacy-patch/adequacy-patch-options.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:29`
**Line:** `#include "../../windows/options/adequacy-patch/adequacy-patch-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h"`

### Pattern: `../../windows/options/advanced/advanced.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:31`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

**File:** `src/ui/simulator/application/main/main.cpp:58`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:30`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h`

### Pattern: `../../windows/options/advanced/advanced.h"` (3 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:31`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h"`

**File:** `src/ui/simulator/application/main/main.cpp:58`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h"`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:30`
**Line:** `#include "../../windows/options/advanced/advanced.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/advanced/advanced.h"`

### Pattern: `../../windows/options/geographic-trimming/geographic-trimming.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:25`
**Line:** `#include "../../windows/options/geographic-trimming/geographic-trimming.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h`

### Pattern: `../../windows/options/geographic-trimming/geographic-trimming.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:25`
**Line:** `#include "../../windows/options/geographic-trimming/geographic-trimming.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h"`

### Pattern: `../../windows/options/optimization/optimization.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:28`
**Line:** `#include "../../windows/options/optimization/optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/optimization/optimization.h`

### Pattern: `../../windows/options/optimization/optimization.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:28`
**Line:** `#include "../../windows/options/optimization/optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/optimization/optimization.h"`

### Pattern: `../../windows/options/playlist/playlist.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:27`
**Line:** `#include "../../windows/options/playlist/playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/playlist/playlist.h`

### Pattern: `../../windows/options/playlist/playlist.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:27`
**Line:** `#include "../../windows/options/playlist/playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/playlist/playlist.h"`

### Pattern: `../../windows/options/select-output/select-output.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:26`
**Line:** `#include "../../windows/options/select-output/select-output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/select-output/select-output.h`

### Pattern: `../../windows/options/select-output/select-output.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:26`
**Line:** `#include "../../windows/options/select-output/select-output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/select-output/select-output.h"`

### Pattern: `../../windows/saveas.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:26`
**Line:** `#include "../../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h`

### Pattern: `../../windows/saveas.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:26`
**Line:** `#include "../../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h"`

### Pattern: `../../windows/sets.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:24`
**Line:** `#include "../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h`

### Pattern: `../../windows/sets.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/notes.cpp:24`
**Line:** `#include "../../windows/sets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/sets.h"`

### Pattern: `../../windows/simulation/run.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:29`
**Line:** `#include "../../windows/simulation/run.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/simulation/run.h`

### Pattern: `../../windows/simulation/run.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.simulation.cpp:29`
**Line:** `#include "../../windows/simulation/run.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/simulation/run.h"`

### Pattern: `../../windows/startupwizard.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:42`
**Line:** `#include "../../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h`

### Pattern: `../../windows/startupwizard.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:42`
**Line:** `#include "../../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h"`

### Pattern: `../../windows/studylogs.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:37`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h`

**File:** `src/ui/simulator/application/main/logs.cpp:23`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h`

### Pattern: `../../windows/studylogs.h"` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:37`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h"`

**File:** `src/ui/simulator/application/main/logs.cpp:23`
**Line:** `#include "../../windows/studylogs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/studylogs.h"`

### Pattern: `../../windows/textinput/input.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:33`
**Line:** `#include "../../windows/textinput/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/textinput/input.h`

### Pattern: `../../windows/textinput/input.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:33`
**Line:** `#include "../../windows/textinput/input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/textinput/input.h"`

### Pattern: `../../windows/version.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:24`
**Line:** `#include "../../windows/version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/version.h`

### Pattern: `../../windows/version.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:24`
**Line:** `#include "../../windows/version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/version.h"`

### Pattern: `../../wx-wrapper.h` (4 occurrences)

**File:** `src/ui/common/component/frame/local-frame.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/panel/panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/panel/group.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/component/spotlight/listbox-panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

### Pattern: `../../wx-wrapper.h"` (4 occurrences)

**File:** `src/ui/common/component/frame/local-frame.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

**File:** `src/ui/common/component/panel/panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

**File:** `src/ui/common/component/panel/group.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

**File:** `src/ui/common/component/spotlight/listbox-panel.h:25`
**Line:** `#include "../../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

### Pattern: `../../xcast.h` (3 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/solar/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:25`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

**File:** `src/libs/antares/study/include/antares/study/parts/load/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h`

### Pattern: `../../xcast.h"` (3 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/solar/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/wind/prepro.h:25`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/load/prepro.h:24`
**Line:** `#include "../../xcast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/xcast.h"`

### Pattern: `../../yuni.h` (55 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.cpp:14`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:17`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:13`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:41`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/smartptr/intrusive.h:12`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 50 more occurrences

### Pattern: `../../yuni.h"` (55 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.cpp:14`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:17`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:13`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:41`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/smartptr/intrusive.h:12`
**Line:** `#include "../../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

... and 50 more occurrences

### Pattern: `../action.h` (4 occurrences)

**File:** `src/ui/action/include/action/settings/suffix-for-cluster-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/decal-area-position.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/suffix-for-constraint-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

**File:** `src/ui/action/include/action/settings/suffix-for-area-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h`

### Pattern: `../action.h"` (4 occurrences)

**File:** `src/ui/action/include/action/settings/suffix-for-cluster-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h"`

**File:** `src/ui/action/include/action/settings/decal-area-position.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h"`

**File:** `src/ui/action/include/action/settings/suffix-for-constraint-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h"`

**File:** `src/ui/action/include/action/settings/suffix-for-area-names.h:25`
**Line:** `#include "../action.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/include/action/action.h"`

### Pattern: `../application.h` (1 occurrences)

**File:** `src/solver/signal-handling/windows.cpp:25`
**Line:** `#include "../application.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/application.h`

### Pattern: `../application.h"` (1 occurrences)

**File:** `src/solver/signal-handling/windows.cpp:25`
**Line:** `#include "../application.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/application.h"`

### Pattern: `../application/main.h` (4 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:36`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:32`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

### Pattern: `../application/main.h"` (4 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/windows/studylogs.cpp:36`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/windows/exportmap.cpp:35`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/windows/startupwizard.cpp:32`
**Line:** `#include "../application/main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

### Pattern: `../application/menus.h` (2 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:24`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:35`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

### Pattern: `../application/menus.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:24`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/windows/startupwizard.cpp:35`
**Line:** `#include "../application/menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

### Pattern: `../application/recentfiles.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:34`
**Line:** `#include "../application/recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

### Pattern: `../application/recentfiles.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:34`
**Line:** `#include "../application/recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h"`

### Pattern: `../application/study.h` (10 occurrences)

**File:** `src/ui/simulator/windows/saveas.h:25`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/saveas.cpp:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/sets.cpp:39`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/studylogs.h:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/windows/exportmap.h:26`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

... and 5 more occurrences

### Pattern: `../application/study.h"` (10 occurrences)

**File:** `src/ui/simulator/windows/saveas.h:25`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/windows/saveas.cpp:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/windows/sets.cpp:39`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/windows/studylogs.h:34`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/windows/exportmap.h:26`
**Line:** `#include "../application/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

... and 5 more occurrences

### Pattern: `../application/wait.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:36`
**Line:** `#include "../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../application/wait.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:36`
**Line:** `#include "../application/wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

### Pattern: `../apply.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:23`
**Line:** `#include "../apply.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/apply.h`

### Pattern: `../apply.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/handler/com.rte-france.antares.study.cpp:23`
**Line:** `#include "../apply.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/apply.h"`

### Pattern: `../area.h` (14 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h`

... and 9 more occurrences

### Pattern: `../area.h"` (14 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:24`
**Line:** `#include "../area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area.h"`

... and 9 more occurrences

### Pattern: `../area/links.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:37`
**Line:** `#include "../area/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/area/links.h`

### Pattern: `../area/links.h"` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:37`
**Line:** `#include "../area/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/area/links.h"`

### Pattern: `../atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:20`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:17`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../atomic/bool.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:20`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:17`
**Line:** `#include "../atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

### Pattern: `../button.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/toolbox/components/map/component.h:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../button.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/toolbox/components/map/component.h:28`
**Line:** `#include "../button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

### Pattern: `../calendar/calendar.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:26`
**Line:** `#include "../calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h`

### Pattern: `../calendar/calendar.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/editor-calendar.cpp:26`
**Line:** `#include "../calendar/calendar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/calendar/calendar.h"`

### Pattern: `../categories.h` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:30`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/storage/fwd.h:24`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/storage/results.h:26`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:34`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/surveyresults.h:32`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h`

### Pattern: `../categories.h"` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:30`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h"`

**File:** `src/solver/variable/include/antares/solver/variable/storage/fwd.h:24`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h"`

**File:** `src/solver/variable/include/antares/solver/variable/storage/results.h:26`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h"`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:34`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h"`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/surveyresults.h:32`
**Line:** `#include "../categories.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/categories.h"`

### Pattern: `../common/cluster.h` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:36`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster.h:30`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h`

### Pattern: `../common/cluster.h"` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:36`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster.h:30`
**Line:** `#include "../common/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster.h"`

### Pattern: `../common/cluster_list.h` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster_list.h:26`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster_list.h:28`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h`

### Pattern: `../common/cluster_list.h"` (2 occurrences)

**File:** `src/libs/antares/study/include/antares/study/parts/thermal/cluster_list.h:26`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h"`

**File:** `src/libs/antares/study/include/antares/study/parts/renewable/cluster_list.h:28`
**Line:** `#include "../common/cluster_list.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/common/cluster_list.h"`

### Pattern: `../commons/hydro.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:28`
**Line:** `#include "../commons/hydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/hydro.h`

### Pattern: `../commons/hydro.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:28`
**Line:** `#include "../commons/hydro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/hydro.h"`

### Pattern: `../commons/join.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:29`
**Line:** `#include "../commons/join.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/join.h`

### Pattern: `../commons/join.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:29`
**Line:** `#include "../commons/join.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/join.h"`

### Pattern: `../commons/links/links.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links.h:36`
**Line:** `#include "../commons/links/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/links/links.h`

### Pattern: `../commons/links/links.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/links.h:36`
**Line:** `#include "../commons/links/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/links/links.h"`

### Pattern: `../commons/load.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:30`
**Line:** `#include "../commons/load.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/load.h`

### Pattern: `../commons/load.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:30`
**Line:** `#include "../commons/load.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/load.h"`

### Pattern: `../commons/miscGenMinusRowPSP.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:31`
**Line:** `#include "../commons/miscGenMinusRowPSP.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/miscGenMinusRowPSP.h`

### Pattern: `../commons/miscGenMinusRowPSP.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:31`
**Line:** `#include "../commons/miscGenMinusRowPSP.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/miscGenMinusRowPSP.h"`

### Pattern: `../commons/psp.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:32`
**Line:** `#include "../commons/psp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/psp.h`

### Pattern: `../commons/psp.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:32`
**Line:** `#include "../commons/psp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/psp.h"`

### Pattern: `../commons/rowBalance.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:33`
**Line:** `#include "../commons/rowBalance.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/rowBalance.h`

### Pattern: `../commons/rowBalance.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:33`
**Line:** `#include "../commons/rowBalance.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/rowBalance.h"`

### Pattern: `../commons/solar.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:34`
**Line:** `#include "../commons/solar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/solar.h`

### Pattern: `../commons/solar.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:34`
**Line:** `#include "../commons/solar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/solar.h"`

### Pattern: `../commons/spatial-aggregate.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:35`
**Line:** `#include "../commons/spatial-aggregate.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/spatial-aggregate.h`

### Pattern: `../commons/spatial-aggregate.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:35`
**Line:** `#include "../commons/spatial-aggregate.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/spatial-aggregate.h"`

### Pattern: `../commons/wind.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:36`
**Line:** `#include "../commons/wind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/wind.h`

### Pattern: `../commons/wind.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:36`
**Line:** `#include "../commons/wind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/commons/wind.h"`

### Pattern: `../component.h` (7 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:24`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:27`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

... and 2 more occurrences

### Pattern: `../component.h"` (7 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/correlation.cpp:24`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:27`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:23`
**Line:** `#include "../component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h"`

... and 2 more occurrences

### Pattern: `../components/button.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../components/button.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:28`
**Line:** `#include "../components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

### Pattern: `../components/captionpanel.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/connection.cpp:25`
**Line:** `#include "../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../components/captionpanel.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/connection.cpp:25`
**Line:** `#include "../components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

### Pattern: `../components/datagrid/gridhelper.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.h:27`
**Line:** `#include "../components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../components/datagrid/gridhelper.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.h:27`
**Line:** `#include "../components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

### Pattern: `../components/htmllistbox/component.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.h:29`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:33`
**Line:** `// #include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:32`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h`

### Pattern: `../components/htmllistbox/component.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.h:29`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:33`
**Line:** `// #include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:32`
**Line:** `#include "../components/htmllistbox/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/component.h"`

### Pattern: `../components/htmllistbox/datasource/renewable-cluster-order.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:36`
**Line:** `#include "../components/htmllistbox/datasource/renewable-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h`

### Pattern: `../components/htmllistbox/datasource/renewable-cluster-order.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:36`
**Line:** `#include "../components/htmllistbox/datasource/renewable-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h"`

### Pattern: `../components/htmllistbox/datasource/thermal-cluster-order.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:34`
**Line:** `#include "../components/htmllistbox/datasource/thermal-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.h`

### Pattern: `../components/htmllistbox/datasource/thermal-cluster-order.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:34`
**Line:** `#include "../components/htmllistbox/datasource/thermal-cluster-order.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.h"`

### Pattern: `../components/htmllistbox/item/renewable-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:37`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

### Pattern: `../components/htmllistbox/item/renewable-cluster-item.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:37`
**Line:** `#include "../components/htmllistbox/item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h"`

### Pattern: `../components/htmllistbox/item/thermal-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:33`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

### Pattern: `../components/htmllistbox/item/thermal-cluster-item.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:33`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.h:35`
**Line:** `#include "../components/htmllistbox/item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h"`

### Pattern: `../components/htmllistbox/item/thermal-cluster.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:34`
**Line:** `// #include "../components/htmllistbox/item/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster.h`

### Pattern: `../components/htmllistbox/item/thermal-cluster.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.h:34`
**Line:** `// #include "../components/htmllistbox/item/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster.h"`

### Pattern: `../components/progressbar.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:36`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.h:29`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h`

### Pattern: `../components/progressbar.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:36`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h"`

**File:** `src/ui/simulator/toolbox/ext-source/performer.h:29`
**Line:** `#include "../components/progressbar.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/progressbar.h"`

### Pattern: `../components/refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:48`
**Line:** `#include "../components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../components/refresh.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:48`
**Line:** `#include "../components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

### Pattern: `../components/wizardheader.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:44`
**Line:** `#include "../components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../components/wizardheader.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:44`
**Line:** `#include "../components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

### Pattern: `../constants.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:16`
**Line:** `#include "../constants.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/constants.h`

### Pattern: `../constants.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/directory.hxx:16`
**Line:** `#include "../constants.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/constants.h"`

### Pattern: `../constraints/ConstraintBuilder.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h:24`
**Line:** `#include "../constraints/ConstraintBuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h`

### Pattern: `../constraints/ConstraintBuilder.h"` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h:24`
**Line:** `#include "../constraints/ConstraintBuilder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h"`

### Pattern: `../content.h` (3 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:26`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:27`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:25`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h`

### Pattern: `../content.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:26`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h"`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:27`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h"`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:25`
**Line:** `#include "../content.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/content.h"`

### Pattern: `../core/atomic/bool.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:16`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

**File:** `src/ext/yuni/src/yuni/thread/timer.h:13`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h`

### Pattern: `../core/atomic/bool.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:16`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

**File:** `src/ext/yuni/src/yuni/thread/timer.h:13`
**Line:** `#include "../core/atomic/bool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/bool.h"`

### Pattern: `../core/atomic/int.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.cpp:16`
**Line:** `#include "../core/atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h`

### Pattern: `../core/atomic/int.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.cpp:16`
**Line:** `#include "../core/atomic/int.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/atomic/int.h"`

### Pattern: `../core/event/event.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:13`
**Line:** `#include "../core/event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h`

### Pattern: `../core/event/event.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:13`
**Line:** `#include "../core/event/event.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/event.h"`

### Pattern: `../core/math.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.cpp:14`
**Line:** `#include "../core/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

### Pattern: `../core/math.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.cpp:14`
**Line:** `#include "../core/math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h"`

### Pattern: `../core/noncopyable.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:14`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:15`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:13`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../core/noncopyable.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:14`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:15`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:13`
**Line:** `#include "../core/noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

### Pattern: `../core/nonmovable.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:14`
**Line:** `#include "../core/nonmovable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/nonmovable.h`

### Pattern: `../core/nonmovable.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/mutex.h:14`
**Line:** `#include "../core/nonmovable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/nonmovable.h"`

### Pattern: `../core/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.h:13`
**Line:** `#include "../core/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

### Pattern: `../core/smartptr.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.h:13`
**Line:** `#include "../core/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

### Pattern: `../core/smartptr/intrusive.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:17`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:15`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:16`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h`

### Pattern: `../core/smartptr/intrusive.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:17`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:15`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

**File:** `src/ext/yuni/src/yuni/thread/thread.h:16`
**Line:** `#include "../core/smartptr/intrusive.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/intrusive.h"`

### Pattern: `../core/static/inherit.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/array.h:15`
**Line:** `#include "../core/static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

### Pattern: `../core/static/inherit.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/array.h:15`
**Line:** `#include "../core/static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h"`

### Pattern: `../core/string.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:14`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/uuid/uuid.cpp:17`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/job/job.h:15`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:16`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:13`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 4 more occurrences

### Pattern: `../core/string.h"` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:14`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/uuid/uuid.cpp:17`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/job/job.h:15`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:16`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:13`
**Line:** `#include "../core/string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

... and 4 more occurrences

### Pattern: `../core/string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/exists.cpp:15`
**Line:** `#include "../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../core/string/wstring.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/exists.cpp:15`
**Line:** `#include "../core/string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

### Pattern: `../core/system/gettimeofday.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.cpp:17`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:24`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

**File:** `src/ext/yuni/src/yuni/thread/thread.cpp:27`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h`

### Pattern: `../core/system/gettimeofday.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.cpp:17`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h"`

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:24`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h"`

**File:** `src/ext/yuni/src/yuni/thread/thread.cpp:27`
**Line:** `#include "../core/system/gettimeofday.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/gettimeofday.h"`

### Pattern: `../core/system/windows.hdr.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:18`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/semaphore.cpp:29`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:26`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/thread/policy.h:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

... and 4 more occurrences

### Pattern: `../core/system/windows.hdr.h"` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/signal.cpp:18`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/thread/semaphore.cpp:29`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/thread/id.cpp:26`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/thread/policy.h:21`
**Line:** `#include "../core/system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

... and 4 more occurrences

### Pattern: `../create.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:37`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/components/wizardheader.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

### Pattern: `../create.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.h:37`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/components/wizardheader.cpp:27`
**Line:** `#include "../create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

### Pattern: `../cstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:17`
**Line:** `#include "../cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../cstring.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/into-cstring.h:17`
**Line:** `#include "../cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

### Pattern: `../daily/h2o_j_donnees_optimisation.h` (1 occurrences)

**File:** `src/solver/hydro/include/antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h:38`
**Line:** `#include "../daily/h2o_j_donnees_optimisation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/hydro/include/antares/solver/hydro/daily/h2o_j_donnees_optimisation.h`

### Pattern: `../daily/h2o_j_donnees_optimisation.h"` (1 occurrences)

**File:** `src/solver/hydro/include/antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h:38`
**Line:** `#include "../daily/h2o_j_donnees_optimisation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/hydro/include/antares/solver/hydro/daily/h2o_j_donnees_optimisation.h"`

### Pattern: `../datetime/timestamp.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:12`
**Line:** `#include "../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h`

### Pattern: `../datetime/timestamp.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:12`
**Line:** `#include "../datetime/timestamp.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/datetime/timestamp.h"`

### Pattern: `../define.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h`

### Pattern: `../define.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:15`
**Line:** `#include "../define.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/define.h"`

### Pattern: `../directory.h` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:19`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h`

... and 2 more occurrences

### Pattern: `../directory.h"` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:19`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/current.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:12`
**Line:** `#include "../directory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory.h"`

... and 2 more occurrences

### Pattern: `../directory/info/info.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:15`
**Line:** `#include "../directory/info/info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info/info.h`

### Pattern: `../directory/info/info.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:15`
**Line:** `#include "../directory/info/info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info/info.h"`

### Pattern: `../drawingcontext.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.h:25`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:309`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.h:27`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h`

### Pattern: `../drawingcontext.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.h:25`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:309`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.h:27`
**Line:** `#include "../drawingcontext.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/drawingcontext.h"`

### Pattern: `../dynamiclibrary/symbol.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:48`
**Line:** `#include "../dynamiclibrary/symbol.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dynamiclibrary/symbol.h`

### Pattern: `../dynamiclibrary/symbol.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:48`
**Line:** `#include "../dynamiclibrary/symbol.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/dynamiclibrary/symbol.h"`

### Pattern: `../economy/profitByPlant.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/adequacy/all.h:70`
**Line:** `#include "../economy/profitByPlant.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/economy/profitByPlant.h`

### Pattern: `../economy/profitByPlant.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/adequacy/all.h:70`
**Line:** `#include "../economy/profitByPlant.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/economy/profitByPlant.h"`

### Pattern: `../endoflist.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:35`
**Line:** `#include "../endoflist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/endoflist.h`

### Pattern: `../endoflist.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:35`
**Line:** `#include "../endoflist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/endoflist.h"`

### Pattern: `../event/interfaces.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:8`
**Line:** `#include "../event/interfaces.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/interfaces.h`

### Pattern: `../event/interfaces.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:8`
**Line:** `#include "../event/interfaces.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/event/interfaces.h"`

### Pattern: `../file.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/copy.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h`

### Pattern: `../file.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/copy.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

**File:** `src/ext/yuni/src/yuni/io/file/file.cpp:12`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/create.cpp:13`
**Line:** `#include "../file.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/file.h"`

### Pattern: `../filename-manipulation.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:13`
**Line:** `#include "../filename-manipulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/filename-manipulation.h`

### Pattern: `../filename-manipulation.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:13`
**Line:** `#include "../filename-manipulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/filename-manipulation.h"`

### Pattern: `../filter.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/month.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/week.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/columnindex.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/weekday.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/houryear.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h`

... and 4 more occurrences

### Pattern: `../filter.h"` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/month.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/week.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/columnindex.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/weekday.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/all/houryear.h:24`
**Line:** `#include "../filter.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/filter.h"`

... and 4 more occurrences

### Pattern: `../fwd.h` (9 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/variables.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h`

**File:** `src/libs/antares/study/include/antares/study/progression/progression.h:36`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h`

... and 4 more occurrences

### Pattern: `../fwd.h"` (9 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h"`

**File:** `src/ui/simulator/windows/output/provider/variables.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h"`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h"`

**File:** `src/ui/simulator/windows/output/provider/outputs.h:25`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/fwd.h"`

**File:** `src/libs/antares/study/include/antares/study/progression/progression.h:36`
**Line:** `#include "../fwd.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/fwd.h"`

... and 4 more occurrences

### Pattern: `../gridhelper.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.h:28`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.cpp:23`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../gridhelper.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.h:28`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/component.cpp:23`
**Line:** `#include "../gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

### Pattern: `../info.h` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:36`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/info.h`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:20`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info.h`

### Pattern: `../info.h"` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:36`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/info.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/info/platform.cpp:20`
**Line:** `#include "../info.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/directory/info.h"`

### Pattern: `../input.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h`

### Pattern: `../input.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../input.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/input.h"`

### Pattern: `../inspector.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:34`
**Line:** `#include "../inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h`

### Pattern: `../inspector.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:34`
**Line:** `#include "../inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector.h"`

### Pattern: `../inspector/data.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.h:24`
**Line:** `#include "../inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h`

### Pattern: `../inspector/data.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.h:24`
**Line:** `#include "../inspector/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/data.h"`

### Pattern: `../io.h` (8 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.h:15`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:16`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:159`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h`

... and 3 more occurrences

### Pattern: `../io.h"` (8 occurrences)

**File:** `src/ext/yuni/src/yuni/io/file/file.h:15`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.cpp:16`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/copy.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:159`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/remove.cpp:14`
**Line:** `#include "../io.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/io.h"`

... and 3 more occurrences

### Pattern: `../item/cluster-item.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.h:27`
**Line:** `#include "../item/cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.h`

### Pattern: `../item/cluster-item.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/cluster-order.h:27`
**Line:** `#include "../item/cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.h"`

### Pattern: `../item/group.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:25`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h`

### Pattern: `../item/group.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:22`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:25`
**Line:** `#include "../item/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/group.h"`

### Pattern: `../item/renewable-cluster-item.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:25`
**Line:** `#include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h:27`
**Line:** `// #include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h`

### Pattern: `../item/renewable-cluster-item.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp:25`
**Line:** `#include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/renewable-cluster-order.h:27`
**Line:** `// #include "../item/renewable-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/renewable-cluster-item.h"`

### Pattern: `../item/thermal-cluster-item.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:25`
**Line:** `#include "../item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h`

### Pattern: `../item/thermal-cluster-item.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp:25`
**Line:** `#include "../item/thermal-cluster-item.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/thermal-cluster-item.h"`

### Pattern: `../iterator.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:17`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:45`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h`

### Pattern: `../iterator.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:17`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h"`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:45`
**Line:** `#include "../iterator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/iterator.h"`

### Pattern: `../job.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:17`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:18`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

### Pattern: `../job.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/job/queue/waitingroom.h:17`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h"`

**File:** `src/ext/yuni/src/yuni/job/queue/service.h:18`
**Line:** `#include "../job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h"`

### Pattern: `../job/job.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:14`
**Line:** `#include "../job/job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h`

### Pattern: `../job/job.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:14`
**Line:** `#include "../job/job.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/job/job.h"`

### Pattern: `../jobs.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/execute/execute.cpp:24`
**Line:** `#include "../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../jobs.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/execute/execute.cpp:24`
**Line:** `#include "../jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h"`

### Pattern: `../layer.h` (2 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:25`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:26`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h`

### Pattern: `../layer.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:25`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h"`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:26`
**Line:** `#include "../layer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/layer.h"`

### Pattern: `../length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:18`
**Line:** `#include "../length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../length.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/traits/extension/length.h:18`
**Line:** `#include "../length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

### Pattern: `../load-options.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/area/area.h:817`
**Line:** `#include "../load-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/load-options.h`

### Pattern: `../load-options.h"` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/area/area.h:817`
**Line:** `#include "../load-options.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/load-options.h"`

### Pattern: `../locales.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.cpp:24`
**Line:** `#include "../locales.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/locales.h`

### Pattern: `../locales.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/clipboard/clipboard.cpp:24`
**Line:** `#include "../locales.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/locales.h"`

### Pattern: `../main.h` (13 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/help.cpp:24`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:30`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/notes.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

**File:** `src/ui/simulator/application/main/menu.cpp:29`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h`

... and 8 more occurrences

### Pattern: `../main.h"` (13 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/application/main/help.cpp:24`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:30`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/application/main/notes.cpp:22`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:29`
**Line:** `#include "../main.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main.h"`

... and 8 more occurrences

### Pattern: `../manager.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:23`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:310`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:25`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h`

### Pattern: `../manager.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:23`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/item.h:310`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h"`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:25`
**Line:** `#include "../manager.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/manager.h"`

### Pattern: `../math.h` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/functional/binaryfunctions.h:14`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/functional/loop.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:17`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

**File:** `src/ext/yuni/src/yuni/core/math/random/range.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h`

... and 4 more occurrences

### Pattern: `../math.h"` (9 occurrences)

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h"`

**File:** `src/ext/yuni/src/yuni/core/functional/binaryfunctions.h:14`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h"`

**File:** `src/ext/yuni/src/yuni/core/functional/loop.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/table.h:17`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h"`

**File:** `src/ext/yuni/src/yuni/core/math/random/range.h:13`
**Line:** `#include "../math.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/math.h"`

... and 4 more occurrences

### Pattern: `../matrix.h` (15 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h`

... and 10 more occurrences

### Pattern: `../matrix.h"` (15 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h:25`
**Line:** `#include "../matrix.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h"`

... and 10 more occurrences

### Pattern: `../menus.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:32`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/help.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/refresh.cpp:24`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

**File:** `src/ui/simulator/application/main/menu.cpp:30`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h`

... and 1 more occurrences

### Pattern: `../menus.h"` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:32`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/application/main/help.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/application/main/refresh.cpp:24`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/application/main/events.file.cpp:25`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:30`
**Line:** `#include "../menus.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/menus.h"`

... and 1 more occurrences

### Pattern: `../message.h` (2 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:40`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:51`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../message.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:40`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:51`
**Line:** `#include "../message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

### Pattern: `../noncopyable.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:18`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:22`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h`

### Pattern: `../noncopyable.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:18`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/logs.h:22`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.h:14`
**Line:** `#include "../noncopyable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/noncopyable.h"`

### Pattern: `../notebook/mapnotebook.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.h:29`
**Line:** `#include "../notebook/mapnotebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/mapnotebook.h`

### Pattern: `../notebook/mapnotebook.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/component.h:29`
**Line:** `#include "../notebook/mapnotebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/mapnotebook.h"`

### Pattern: `../notebook/notebook.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/control.h:29`
**Line:** `#include "../notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

### Pattern: `../notebook/notebook.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/control.h:29`
**Line:** `#include "../notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

### Pattern: `../notes.h` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/frame.h:27`
**Line:** `#include "../notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h`

### Pattern: `../notes.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/inspector/frame.h:27`
**Line:** `#include "../notes.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/notes.h"`

### Pattern: `../null.h` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/stdcout.h:36`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.h:13`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:14`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h`

... and 2 more occurrences

### Pattern: `../null.h"` (7 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/handler/stdcout.h:36`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/callback.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/handler/file.h:16`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/time.h:13`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/decorators/message.h:14`
**Line:** `#include "../null.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/logs/null.h"`

... and 2 more occurrences

### Pattern: `../operator.h` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:28`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/equals.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/modulo.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/greaterthan.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/lessthanorequalsto.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h`

... and 4 more occurrences

### Pattern: `../operator.h"` (9 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/parameter/parameter.cpp:28`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/equals.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/modulo.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/greaterthan.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/operator/lessthanorequalsto.h:24`
**Line:** `#include "../operator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/filter/operator.h"`

... and 4 more occurrences

### Pattern: `../opt_fonctions.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:29`
**Line:** `#include "../opt_fonctions.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_fonctions.h`

### Pattern: `../opt_fonctions.h"` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:29`
**Line:** `#include "../opt_fonctions.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_fonctions.h"`

### Pattern: `../opt_rename_problem.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:30`
**Line:** `#include "../opt_rename_problem.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_rename_problem.h`

### Pattern: `../opt_rename_problem.h"` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:30`
**Line:** `#include "../opt_rename_problem.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/opt_rename_problem.h"`

### Pattern: `../output.h` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:26`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:24`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:25`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h`

... and 1 more occurrences

### Pattern: `../output.h"` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h"`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:26`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h"`

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:27`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h"`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:24`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h"`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:25`
**Line:** `#include "../output.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/output/output.h"`

... and 1 more occurrences

### Pattern: `../panel.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.h:25`
**Line:** `#include "../panel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel.h`

### Pattern: `../panel.h"` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.h:25`
**Line:** `#include "../panel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel.h"`

### Pattern: `../panel/group.h` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:32`
**Line:** `#include "../panel/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel/group.h`

### Pattern: `../panel/group.h"` (1 occurrences)

**File:** `src/ui/common/component/spotlight/spotlight.cpp:32`
**Line:** `#include "../panel/group.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/component/panel/group.h"`

### Pattern: `../parts/thermal/cluster.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:39`
**Line:** `#include "../parts/thermal/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/thermal/cluster.h`

### Pattern: `../parts/thermal/cluster.h"` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/binding_constraint/BindingConstraint.h:39`
**Line:** `#include "../parts/thermal/cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/parts/thermal/cluster.h"`

### Pattern: `../preprocessor/capabilities.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/capabilities.h:13`
**Line:** `#include "../preprocessor/capabilities.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/capabilities.h`

### Pattern: `../preprocessor/capabilities.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/capabilities.h:13`
**Line:** `#include "../preprocessor/capabilities.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/capabilities.h"`

### Pattern: `../preprocessor/std.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/assert.h:13`
**Line:** `#include "../preprocessor/std.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/std.h`

### Pattern: `../preprocessor/std.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/assert.h:13`
**Line:** `#include "../preprocessor/std.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/preprocessor/std.h"`

### Pattern: `../recentfiles.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:24`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

**File:** `src/ui/simulator/application/main/menu.cpp:31`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h`

### Pattern: `../recentfiles.h"` (2 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:24`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:31`
**Line:** `#include "../recentfiles.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/recentfiles.h"`

### Pattern: `../refresh.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:28`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../refresh.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/notebook.h:28`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:32`
**Line:** `#include "../refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

### Pattern: `../renderer.h` (13 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

... and 8 more occurrences

### Pattern: `../renderer.h"` (13 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/matrix.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.h:25`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h:24`
**Line:** `#include "../renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

... and 8 more occurrences

### Pattern: `../setofareas.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:37`
**Line:** `#include "../setofareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/setofareas.h`

### Pattern: `../setofareas.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:37`
**Line:** `#include "../setofareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/setofareas.h"`

### Pattern: `../settings.h` (6 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/connectioncreator.cpp:23`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:25`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:22`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

... and 1 more occurrences

### Pattern: `../settings.h"` (6 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/tools/connectioncreator.cpp:23`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/toolbox/components/map/tools/remover.cpp:25`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:22`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/toolbox/components/map/nodes/node.cpp:24`
**Line:** `#include "../settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

... and 1 more occurrences

### Pattern: `../slist/slist.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:21`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:18`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h`

### Pattern: `../slist/slist.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/traits.h:21`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h"`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:18`
**Line:** `#include "../slist/slist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/slist/slist.h"`

### Pattern: `../smartptr.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:46`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

**File:** `src/ext/yuni/src/yuni/core/event/event.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h`

### Pattern: `../smartptr.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:46`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

**File:** `src/ext/yuni/src/yuni/core/event/event.h:14`
**Line:** `#include "../smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr.h"`

### Pattern: `../smartptr/smartptr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/types.h:14`
**Line:** `#include "../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h`

### Pattern: `../smartptr/smartptr.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/static/types.h:14`
**Line:** `#include "../smartptr/smartptr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/smartptr/smartptr.h"`

### Pattern: `../spotlight/area.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.cpp:33`
**Line:** `#include "../spotlight/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/area.h`

### Pattern: `../spotlight/area.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/input/area.cpp:33`
**Line:** `#include "../spotlight/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/spotlight/area.h"`

### Pattern: `../state.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-utils.h:5`
**Line:** `#include "../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h`

### Pattern: `../state.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-utils.h:5`
**Line:** `#include "../state.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/state.h"`

### Pattern: `../static/assert.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:44`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.hxx:13`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:15`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:19`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h`

### Pattern: `../static/assert.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:44`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

**File:** `src/ext/yuni/src/yuni/core/singleton/singleton.hxx:13`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:15`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:19`
**Line:** `#include "../static/assert.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/assert.h"`

### Pattern: `../static/dynamiccast.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:9`
**Line:** `#include "../static/dynamiccast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/dynamiccast.h`

### Pattern: `../static/dynamiccast.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:9`
**Line:** `#include "../static/dynamiccast.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/dynamiccast.h"`

### Pattern: `../static/if.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:17`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:14`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/functional/view.hxx:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

**File:** `src/ext/yuni/src/yuni/core/iterator/iterator.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h`

### Pattern: `../static/if.h"` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:17`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:14`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

**File:** `src/ext/yuni/src/yuni/core/functional/view.hxx:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

**File:** `src/ext/yuni/src/yuni/core/iterator/iterator.h:13`
**Line:** `#include "../static/if.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/if.h"`

### Pattern: `../static/inherit.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:10`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:20`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h`

### Pattern: `../static/inherit.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/traits.h:10`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h"`

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:20`
**Line:** `#include "../static/inherit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/inherit.h"`

### Pattern: `../static/moveconstructor.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:18`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:15`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h`

### Pattern: `../static/moveconstructor.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/smartptr/smartptr.h:18`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h"`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:15`
**Line:** `#include "../static/moveconstructor.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/moveconstructor.h"`

### Pattern: `../static/remove.h` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:45`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:15`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/traits/cstring.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/traits/length.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:16`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h`

... and 1 more occurrences

### Pattern: `../static/remove.h"` (6 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:45`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:15`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/core/traits/cstring.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/core/traits/length.h:14`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:16`
**Line:** `#include "../static/remove.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/remove.h"`

... and 1 more occurrences

### Pattern: `../static/typedef.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:17`
**Line:** `#include "../static/typedef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/typedef.h`

### Pattern: `../static/typedef.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:17`
**Line:** `#include "../static/typedef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/typedef.h"`

### Pattern: `../static/types.h` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:43`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:15`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:16`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/math/distance.hxx:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h`

### Pattern: `../static/types.h"` (5 occurrences)

**File:** `src/ext/yuni/src/yuni/core/bind/bind.h:43`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:15`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

**File:** `src/ext/yuni/src/yuni/core/slist/slist.h:16`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

**File:** `src/ext/yuni/src/yuni/core/math/distance.hxx:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

**File:** `src/ext/yuni/src/yuni/core/nullable/nullable.h:13`
**Line:** `#include "../static/types.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/static/types.h"`

### Pattern: `../string.h` (11 occurrences)

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/system/environment.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/getopt/option.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

**File:** `src/ext/yuni/src/yuni/core/logs/null.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h`

... and 6 more occurrences

### Pattern: `../string.h"` (11 occurrences)

**File:** `src/ext/yuni/src/yuni/core/utils/hexdump.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/core/color/fwd.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/core/system/environment.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/core/getopt/option.h:16`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

**File:** `src/ext/yuni/src/yuni/core/logs/null.h:14`
**Line:** `#include "../string.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string.h"`

... and 6 more occurrences

### Pattern: `../string/wstring.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:19`
**Line:** `#include "../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h`

### Pattern: `../string/wstring.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/environment.cpp:19`
**Line:** `#include "../string/wstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/string/wstring.h"`

### Pattern: `../study.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:25`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:31`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/menu.cpp:32`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

**File:** `src/ui/simulator/application/main/main.cpp:28`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h`

### Pattern: `../study.h"` (4 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:25`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:31`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:32`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

**File:** `src/ui/simulator/application/main/main.cpp:28`
**Line:** `#include "../study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/study.h"`

### Pattern: `../surveyresults.h` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:31`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:37`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h`

### Pattern: `../surveyresults.h"` (2 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/storage/intermediate.h:31`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h"`

**File:** `src/solver/variable/include/antares/solver/variable/surveyresults/reportbuilder.hxx:37`
**Line:** `#include "../surveyresults.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/surveyresults.h"`

### Pattern: `../system/console.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/verbosity.h:13`
**Line:** `#include "../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h`

### Pattern: `../system/console.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/logs/verbosity.h:13`
**Line:** `#include "../system/console.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/console.h"`

### Pattern: `../system/memory.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/memory.cpp:14`
**Line:** `#include "../system/memory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/memory.h`

### Pattern: `../system/memory.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/memory.cpp:14`
**Line:** `#include "../system/memory.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/memory.h"`

### Pattern: `../system/suspend.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:21`
**Line:** `#include "../system/suspend.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/suspend.h`

### Pattern: `../system/suspend.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/loop.h:21`
**Line:** `#include "../system/suspend.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/suspend.h"`

### Pattern: `../system/windows.hdr.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:15`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.cpp:13`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../system/windows.hdr.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/atomic/traits.h:15`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/core/atomic/int.h:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/core/string/wstring.cpp:13`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

**File:** `src/ext/yuni/src/yuni/core/dynamiclibrary/file.cpp:14`
**Line:** `#include "../system/windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

### Pattern: `../thermal-cluster/common.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:37`
**Line:** `#include "../thermal-cluster/common.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/common.h`

### Pattern: `../thermal-cluster/common.h"` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:37`
**Line:** `#include "../thermal-cluster/common.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/common.h"`

### Pattern: `../thermal-cluster/create.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:35`
**Line:** `#include "../thermal-cluster/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/create.h`

### Pattern: `../thermal-cluster/create.h"` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:35`
**Line:** `#include "../thermal-cluster/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/create.h"`

### Pattern: `../thermal-cluster/root-node.h` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:36`
**Line:** `#include "../thermal-cluster/root-node.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/root-node.h`

### Pattern: `../thermal-cluster/root-node.h"` (1 occurrences)

**File:** `src/ui/action/handler/antares-study/area/create.cpp:36`
**Line:** `#include "../thermal-cluster/root-node.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/action/handler/antares-study/thermal-cluster/root-node.h"`

### Pattern: `../thread/signal.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:17`
**Line:** `#include "../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h`

### Pattern: `../thread/signal.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:17`
**Line:** `#include "../thread/signal.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/signal.h"`

### Pattern: `../thread/thread.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:14`
**Line:** `#include "../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h`

### Pattern: `../thread/thread.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/job/job.h:14`
**Line:** `#include "../thread/thread.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/thread.h"`

### Pattern: `../thread/utility.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/spawn.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h`

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h`

### Pattern: `../thread/utility.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/spawn.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h"`

**File:** `src/ext/yuni/src/yuni/thread/every.cpp:11`
**Line:** `#include "../thread/utility.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/thread/utility.h"`

### Pattern: `../traits/cstring.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/username.hxx:15`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:18`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h`

### Pattern: `../traits/cstring.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/username.hxx:15`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

**File:** `src/ext/yuni/src/yuni/core/string/string.h:18`
**Line:** `#include "../traits/cstring.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/cstring.h"`

### Pattern: `../traits/length.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:19`
**Line:** `#include "../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h`

### Pattern: `../traits/length.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/string/string.h:19`
**Line:** `#include "../traits/length.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/traits/length.h"`

### Pattern: `../trigonometric.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vector3D.h:14`
**Line:** `#include "../trigonometric.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/trigonometric.h`

### Pattern: `../trigonometric.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/math/geometry/vector3D.h:14`
**Line:** `#include "../trigonometric.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/math/trigonometric.h"`

### Pattern: `../undef.h` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/time.h:126`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:160`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:89`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:41`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h`

### Pattern: `../undef.h"` (4 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/time.h:126`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/undef.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:160`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:89`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:41`
**Line:** `#include "../undef.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/undef.h"`

### Pattern: `../unit.h` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h`

### Pattern: `../unit.h"` (3 occurrences)

**File:** `src/ext/yuni/src/yuni/core/unit/length/extra.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/metric.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h"`

**File:** `src/ext/yuni/src/yuni/core/unit/length/length.h:16`
**Line:** `#include "../unit.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/unit/unit.h"`

### Pattern: `../validator.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:17`
**Line:** `#include "../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/validator.h`

### Pattern: `../validator.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/validator/text/validator.h:17`
**Line:** `#include "../validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/validator.h"`

### Pattern: `../validator/text/default.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/getopt/parser.h:17`
**Line:** `#include "../validator/text/default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/text/default.h`

### Pattern: `../validator/text/default.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/getopt/parser.h:17`
**Line:** `#include "../validator/text/default.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/validator/text/default.h"`

### Pattern: `../variable.h` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/lolpCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/priceCSR.h:31`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/overallCostCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/loldCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-csr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h`

### Pattern: `../variable.h"` (5 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/lolpCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/priceCSR.h:31`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/overallCostCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/loldCsr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

**File:** `src/solver/variable/include/antares/solver/variable/economy/max-mrg-csr.h:29`
**Line:** `#include "../variable.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/variable.h"`

### Pattern: `../variables/VariableManagement.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:31`
**Line:** `#include "../variables/VariableManagement.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagement.h`

### Pattern: `../variables/VariableManagement.h"` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/constraints/ConstraintBuilder.h:31`
**Line:** `#include "../variables/VariableManagement.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagement.h"`

### Pattern: `../variables/VariableManagerUtils.h` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h:32`
**Line:** `#include "../variables/VariableManagerUtils.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagerUtils.h`

### Pattern: `../variables/VariableManagerUtils.h"` (1 occurrences)

**File:** `src/solver/optimisation/include/antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h:32`
**Line:** `#include "../variables/VariableManagerUtils.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/optimisation/include/antares/solver/optimisation/variables/VariableManagerUtils.h"`

### Pattern: `../version.h` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/finder/finder.h:32`
**Line:** `#include "../version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/version.h`

### Pattern: `../version.h"` (1 occurrences)

**File:** `src/libs/antares/study/include/antares/study/finder/finder.h:32`
**Line:** `#include "../version.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/study/include/antares/study/version.h"`

### Pattern: `../wait.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/create.cpp:83`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/application/main/main.cpp:59`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:31`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h`

### Pattern: `../wait.h"` (3 occurrences)

**File:** `src/ui/simulator/application/main/create.cpp:83`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/application/main/main.cpp:59`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:31`
**Line:** `#include "../wait.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/wait.h"`

### Pattern: `../windows.hdr.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/console/console.hxx:13`
**Line:** `#include "../windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h`

### Pattern: `../windows.hdr.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/system/console/console.hxx:13`
**Line:** `#include "../windows.hdr.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/system/windows.hdr.h"`

### Pattern: `../windows/inspector/inspector.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:44`
**Line:** `#include "../windows/inspector/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/inspector.h`

### Pattern: `../windows/inspector/inspector.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:44`
**Line:** `#include "../windows/inspector/inspector.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/inspector/inspector.h"`

### Pattern: `../windows/message.h` (2 occurrences)

**File:** `src/ui/simulator/application/application.cpp:40`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

**File:** `src/ui/simulator/application/study.cpp:45`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h`

### Pattern: `../windows/message.h"` (2 occurrences)

**File:** `src/ui/simulator/application/application.cpp:40`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

**File:** `src/ui/simulator/application/study.cpp:45`
**Line:** `#include "../windows/message.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/message.h"`

### Pattern: `../windows/saveas.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:46`
**Line:** `#include "../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h`

### Pattern: `../windows/saveas.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:46`
**Line:** `#include "../windows/saveas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/saveas.h"`

### Pattern: `../windows/startupwizard.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:47`
**Line:** `#include "../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h`

### Pattern: `../windows/startupwizard.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:47`
**Line:** `#include "../windows/startupwizard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/startupwizard.h"`

### Pattern: `../wx-wrapper.h` (2 occurrences)

**File:** `src/ui/common/dispatcher/settings.h:24`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

**File:** `src/ui/common/dispatcher/gui.h:25`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h`

### Pattern: `../wx-wrapper.h"` (2 occurrences)

**File:** `src/ui/common/dispatcher/settings.h:24`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

**File:** `src/ui/common/dispatcher/gui.h:25`
**Line:** `#include "../wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/common/wx-wrapper.h"`

### Pattern: `../yuni.h` (18 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/job.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:18`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:14`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

**File:** `src/ext/yuni/src/yuni/core/dictionary.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h`

... and 13 more occurrences

### Pattern: `../yuni.h"` (18 occurrences)

**File:** `src/ext/yuni/src/yuni/uuid/uuid.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/job/job.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/job/taskgroup.h:18`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/datetime/timestamp.h:14`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

**File:** `src/ext/yuni/src/yuni/core/dictionary.h:12`
**Line:** `#include "../yuni.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/yuni.h"`

... and 13 more occurrences

### Pattern: `..\\..";` (1 occurrences)

**File:** `src/ui/simulator/application/application.cpp:80`
**Line:** `Forms::StudyToLoadAtStartup.clear() << t << "\\..\\..";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/..\\..";`

### Pattern: `..\\..\\.."` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/versions.cpp:51`
**Line:** `loadFromPath(root + "\\..\\..\\..");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/tools/yuni-config/..\\..\\.."`

### Pattern: `..\\..\\..\\.."` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/versions.cpp:62`
**Line:** `loadFromPath(root + "\\..\\..\\..\\..");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/tools/yuni-config/..\\..\\..\\.."`

### Pattern: `..\\..\\..\\constraints-builder\\Debug"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:108`
**Line:** `<< "\\..\\..\\..\\constraints-builder\\Debug"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\constraints-builder\\Debug"`

### Pattern: `..\\..\\..\\constraints-builder\\Release"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:105`
**Line:** `<< "\\..\\..\\..\\constraints-builder\\Release"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\constraints-builder\\Release"`

### Pattern: `..\\mark-for-yuni-sources"` (1 occurrences)

**File:** `src/ext/yuni/src/tools/yuni-config/versions.cpp:60`
**Line:** `if (IO::File::Exists(String() << root << "\\..\\mark-for-yuni-sources"))`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/tools/yuni-config/..\\mark-for-yuni-sources"`

### Pattern: `..\n",` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/getopt/parser.cpp:366`
**Line:** `std::cout.write(" [FILE]...\n", 11);`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/getopt/..\n",`

### Pattern: `..\n\t...To"` (1 occurrences)

**File:** `src/solver/variable/surveyresults/surveyresults.cpp:43`
**Line:** `buffer.append("\n\n\n\n\t").append(title).append("\n\t\tFrom...\n\t...To");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/surveyresults/..\n\t...To"`

### Pattern: `..\tCtrl+O"` (1 occurrences)

**File:** `src/ui/simulator/application/main/menu.cpp:105`
**Line:** `wxT("&Open...\tCtrl+O"),`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/application/main/..\tCtrl+O"`


## Runtime Resource (136 occurrences)
--------------------------------------------------
### Pattern: `../../../resources.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:25`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

### Pattern: `../../../resources.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/filter/input.cpp:25`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/item/cluster-item.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/map/tools/tool.cpp:23`
**Line:** `#include "../../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

### Pattern: `../../../toolbox/resources.h` (13 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:25`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:27`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:26`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 8 more occurrences

### Pattern: `../../../toolbox/resources.h"` (13 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:25`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/output/provider/output-comparison.cpp:27`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/output/provider/outputs.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/output/provider/variables.cpp:26`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:28`
**Line:** `#include "../../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

... and 8 more occurrences

### Pattern: `../../constraints-builder"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:115`
**Line:** `searchpaths.directories.push_back((s = root) << "/../../constraints-builder");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/constraints-builder"`

### Pattern: `../../resources.h` (7 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:56`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:25`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/components/button/button.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 2 more occurrences

### Pattern: `../../resources.h"` (7 occurrences)

**File:** `src/ui/simulator/toolbox/input/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:56`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/notebook/mapnotebook.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/htmllistbox/component.cpp:25`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/components/button/button.cpp:23`
**Line:** `#include "../../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

... and 2 more occurrences

### Pattern: `../../solver"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:67`
**Line:** `searchpaths.directories.push_back((s = root) << "/../../solver");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/solver"`

### Pattern: `../../toolbox/resources.h` (17 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:28`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:32`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/main/menu.cpp:27`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 12 more occurrences

### Pattern: `../../toolbox/resources.h"` (17 occurrences)

**File:** `src/ui/simulator/application/main/help.cpp:28`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/application/main/events.file.cpp:32`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/application/main/menu.cpp:27`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:25`
**Line:** `#include "../../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

... and 12 more occurrences

### Pattern: `../../tools/yby-aggregator"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:154`
**Line:** `searchpaths.directories.push_back((s = root) << "/../../tools/yby-aggregator");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/tools/yby-aggregator"`

### Pattern: `../../windows/options/temp-folder/temp-folder.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:24`
**Line:** `#include "../../windows/options/temp-folder/temp-folder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/temp-folder/temp-folder.h`

### Pattern: `../../windows/options/temp-folder/temp-folder.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/options.cpp:24`
**Line:** `#include "../../windows/options/temp-folder/temp-folder.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/options/temp-folder/temp-folder.h"`

### Pattern: `../resources.h` (8 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:45`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:35`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/area.cpp:28`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 3 more occurrences

### Pattern: `../resources.h"` (8 occurrences)

**File:** `src/ui/simulator/toolbox/jobs/job.cpp:45`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/input/connection.cpp:35`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/input/area.cpp:28`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/input/renewable-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/toolbox/input/thermal-cluster.cpp:26`
**Line:** `#include "../resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

... and 3 more occurrences

### Pattern: `../share/antares/` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:163`
**Line:** `p.clear() << RootFolder << SEP << "../share/antares/" << ANTARES_VERSION << "/resources/";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/share/antares`

### Pattern: `../share/antares/"` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:163`
**Line:** `p.clear() << RootFolder << SEP << "../share/antares/" << ANTARES_VERSION << "/resources/";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/share/antares/"`

### Pattern: `../toolbox/resources.h` (7 occurrences)

**File:** `src/ui/simulator/application/application.cpp:33`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/application/menus.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/sets.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:37`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

**File:** `src/ui/simulator/windows/message.cpp:32`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h`

... and 2 more occurrences

### Pattern: `../toolbox/resources.h"` (7 occurrences)

**File:** `src/ui/simulator/application/application.cpp:33`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/application/menus.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/sets.cpp:26`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/studylogs.cpp:37`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

**File:** `src/ui/simulator/windows/message.cpp:32`
**Line:** `#include "../toolbox/resources.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/resources.h"`

... and 2 more occurrences

### Pattern: `..\\` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:465`
**Line:** `rootFolder += "..\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/..\\`

**File:** `src/ui/simulator/windows/exportmap.cpp:585`
**Line:** `rootFolder += "..\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/..\\`

### Pattern: `..\\";` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:465`
**Line:** `rootFolder += "..\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/..\\";`

**File:** `src/ui/simulator/windows/exportmap.cpp:585`
**Line:** `rootFolder += "..\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/..\\";`

### Pattern: `..\\..\\..\\solver\\Debug"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:59`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Debug"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\solver\\Debug"`

### Pattern: `..\\..\\..\\solver\\Release"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:57`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Release"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\solver\\Release"`

### Pattern: `..\\..\\constraints-builder"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:110`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\..\\constraints-builder"); // mingw`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\constraints-builder"`

### Pattern: `..\\..\\solver"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:62`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\..\\solver"); // mingw`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\solver"`

### Pattern: `..\\..\\tools\\yby-aggregator"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:149`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\..\\tools\\yby-aggregator"); // mingw`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\tools\\yby-aggregator"`

### Pattern: `..\\Debug\\..\\..\\..\\..\\resources\\examples";` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:115`
**Line:** `s.clear() << RootFolder << "\\..\\Debug\\..\\..\\..\\..\\resources\\examples";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/resources/..\\Debug\\..\\..\\..\\..\\resources\\examples";`

### Pattern: `..\\Debug\\..\\resources\\";` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:173`
**Line:** `p.clear() << RootFolder << "\\..\\Debug\\..\\resources\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/resources/..\\Debug\\..\\resources\\";`

### Pattern: `..\\Release\\..\\..\\..\\..\\resources\\examples";` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:124`
**Line:** `s.clear() << RootFolder << "\\..\\Release\\..\\..\\..\\..\\resources\\examples";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/resources/..\\Release\\..\\..\\..\\..\\resources\\examples";`

### Pattern: `..\\Release\\..\\resources\\";` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:178`
**Line:** `p.clear() << RootFolder << "\\..\\Release\\..\\resources\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/resources/..\\Release\\..\\resources\\";`

### Pattern: `..\\bin"` (3 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:54`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\bin");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\bin"`

**File:** `src/libs/antares/locator/locator.cpp:101`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\bin");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\bin"`

**File:** `src/libs/antares/locator/locator.cpp:140`
**Line:** `searchpaths.directories.push_back((s = root) << "\\..\\bin");`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\bin"`

### Pattern: `..\\folder1\\folder2"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/filename-manipulation.h:152`
**Line:** `** \bug The relative filenames like "C:..\\folder1\\folder2" are not handled properly`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/..\\folder1\\folder2"`

**File:** `src/ext/yuni/src/yuni/io/filename-manipulation.h:172`
**Line:** `** \bug The relative filenames like "C:..\\folder1\\folder2" are not handled properly`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/io/..\\folder1\\folder2"`

### Pattern: `..\\resources\\";` (1 occurrences)

**File:** `src/libs/antares/resources/resources.cpp:156`
**Line:** `p.clear() << RootFolder << "\\..\\resources\\";`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/resources/..\\resources\\";`


## Test Path (3 occurrences)
--------------------------------------------------
### Pattern: `../"` (1 occurrences)

**File:** `src/tests/src/libs/antares/test_utils.cpp:108`
**Line:** `helper(fs::path("a/.///b/../"));`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/tests/src/libs/"`

### Pattern: `../modeler/mockModelerObjects.h` (1 occurrences)

**File:** `src/tests/src/expressions/test_DeepWideTrees.cpp:32`
**Line:** `#include "../modeler/mockModelerObjects.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/tests/src/modeler/mockModelerObjects.h`

### Pattern: `../modeler/mockModelerObjects.h"` (1 occurrences)

**File:** `src/tests/src/expressions/test_DeepWideTrees.cpp:32`
**Line:** `#include "../modeler/mockModelerObjects.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/tests/src/modeler/mockModelerObjects.h"`


## Tool Path (532 occurrences)
--------------------------------------------------
### Pattern: `../../../../toolbox/components/refresh.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.cpp:24`
**Line:** `#include "../../../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../../../toolbox/components/refresh.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/area.cpp:24`
**Line:** `#include "../../../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

### Pattern: `../../../../toolbox/input/area.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../../toolbox/input/area.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/toolbox/components/datagrid/renderer/layers.h:26`
**Line:** `#include "../../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

### Pattern: `../../../toolbox/clipboard/clipboard.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:54`
**Line:** `#include "../../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../../toolbox/clipboard/clipboard.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:54`
**Line:** `#include "../../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h"`

### Pattern: `../../../toolbox/components/button.h` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:53`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/options/advanced/advanced.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

### Pattern: `../../../toolbox/components/button.h"` (3 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:53`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-options.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/options/advanced/advanced.h:24`
**Line:** `#include "../../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

### Pattern: `../../../toolbox/components/captionpanel.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:52`
**Line:** `#include "../../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../../toolbox/components/captionpanel.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:52`
**Line:** `#include "../../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

### Pattern: `../../../toolbox/components/datagrid/component.h` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.cpp:23`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

... and 1 more occurrences

### Pattern: `../../../toolbox/components/datagrid/component.h"` (6 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.cpp:23`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/output/panel/area-link.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/options/playlist/playlist.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

... and 1 more occurrences

### Pattern: `../../../toolbox/components/datagrid/renderer.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h`

### Pattern: `../../../toolbox/components/datagrid/renderer.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/area-link-renderer.h:24`
**Line:** `#include "../../../toolbox/components/datagrid/renderer.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/nodal-optimization.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/timeseries.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/timeseries.h"` (4 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/application/main/build/load.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/application/main/build/wind.cpp:26`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"` (3 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h"`

**File:** `src/ui/simulator/application/main/build/load.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h"`

**File:** `src/ui/simulator/application/main/build/wind.cpp:27`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-allareas.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h` (2 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:28`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:28`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.h:25`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/geographic-trimming-grid.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/mc-playlist.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/mc-playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/mc-playlist.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/mc-playlist.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/mc-playlist.h"`

### Pattern: `../../../toolbox/components/datagrid/renderer/select-variables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/select-variables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/select-variables.h`

### Pattern: `../../../toolbox/components/datagrid/renderer/select-variables.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:35`
**Line:** `#include "../../../toolbox/components/datagrid/renderer/select-variables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/select-variables.h"`

### Pattern: `../../../toolbox/components/notebook/notebook.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:24`
**Line:** `#include "../../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

### Pattern: `../../../toolbox/components/notebook/notebook.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:24`
**Line:** `#include "../../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

### Pattern: `../../../toolbox/components/wizardheader.h` (5 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:27`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:35`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../../../toolbox/components/wizardheader.h"` (5 occurrences)

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:27`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:35`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/options/adequacy-patch/adequacy-patch-areas.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/options/select-output/select-output.cpp:34`
**Line:** `#include "../../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

### Pattern: `../../../toolbox/create.h` (10 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:25`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:24`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:26`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:32`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:36`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 5 more occurrences

### Pattern: `../../../toolbox/create.h"` (10 occurrences)

**File:** `src/ui/simulator/application/main/build/standard-page.hxx:25`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:24`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/options/geographic-trimming/geographic-trimming.cpp:26`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/options/playlist/playlist.cpp:32`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/options/temp-folder/temp-folder.cpp:36`
**Line:** `#include "../../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

... and 5 more occurrences

### Pattern: `../../../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:26`
**Line:** `#include "../../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../../../toolbox/execute/execute.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/output/panel/panel.cpp:26`
**Line:** `#include "../../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h"`

### Pattern: `../../../toolbox/input/area.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/load.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:23`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/application/main/build/wind.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

### Pattern: `../../../toolbox/input/area.h"` (5 occurrences)

**File:** `src/ui/simulator/application/main/build/solar.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/application/main/build/economic-optimization.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/application/main/build/load.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/application/main/build/hydro.cpp:23`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/application/main/build/wind.cpp:24`
**Line:** `#include "../../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

### Pattern: `../../../toolbox/validator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:58`
**Line:** `#include "../../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../../../toolbox/validator.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/datagrid/component.cpp:58`
**Line:** `#include "../../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

### Pattern: `../../bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:16`
**Line:** `#include "../../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../../bind.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/process/program/program.h:16`
**Line:** `#include "../../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h"`

### Pattern: `../../core/bind.h` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:15`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:13`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../../core/bind.h"` (2 occurrences)

**File:** `src/ext/yuni/src/yuni/io/searchpath/searchpath.h:15`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h"`

**File:** `src/ext/yuni/src/yuni/io/directory/directory.h:13`
**Line:** `#include "../../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h"`

### Pattern: `../../toolbox/clipboard/clipboard.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:25`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

**File:** `src/ui/simulator/windows/inspector/inspector.cpp:32`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h`

### Pattern: `../../toolbox/clipboard/clipboard.h"` (2 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:25`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h"`

**File:** `src/ui/simulator/windows/inspector/inspector.cpp:32`
**Line:** `#include "../../toolbox/clipboard/clipboard.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/clipboard/clipboard.h"`

### Pattern: `../../toolbox/components/button.h` (14 occurrences)

**File:** `src/ui/simulator/application/main/internal-data.h:28`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/output/output.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/calendar/calendar.h:25`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:47`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

... and 9 more occurrences

### Pattern: `../../toolbox/components/button.h"` (14 occurrences)

**File:** `src/ui/simulator/application/main/internal-data.h:28`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/output/output.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/calendar/calendar.h:25`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:47`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:27`
**Line:** `#include "../../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

... and 9 more occurrences

### Pattern: `../../toolbox/components/captionpanel.h` (4 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:25`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:26`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:37`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

**File:** `src/ui/simulator/windows/scenario-builder/panel.cpp:29`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h`

### Pattern: `../../toolbox/components/captionpanel.h"` (4 occurrences)

**File:** `src/ui/simulator/windows/output/output.cpp:25`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:26`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

**File:** `src/ui/simulator/windows/simulation/panel.cpp:37`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

**File:** `src/ui/simulator/windows/scenario-builder/panel.cpp:29`
**Line:** `#include "../../toolbox/components/captionpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/captionpanel.h"`

### Pattern: `../../toolbox/components/datagrid/component.h` (19 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:34`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/xcast/xcast.h:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/renewables/panel.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:25`
**Line:** `// #include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

... and 14 more occurrences

### Pattern: `../../toolbox/components/datagrid/component.h"` (19 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:34`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/xcast/xcast.h:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/renewables/panel.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/renewables/cluster.h:25`
**Line:** `// #include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

... and 14 more occurrences

### Pattern: `../../toolbox/components/datagrid/gridhelper.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/application/main/main.cpp:35`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h`

### Pattern: `../../toolbox/components/datagrid/gridhelper.h"` (3 occurrences)

**File:** `src/ui/simulator/application/main/statusbar.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/application/main/main.cpp:35`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/gridhelper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/gridhelper.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/creditmodulations.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/creditmodulations.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/creditmodulations.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/creditmodulations.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/creditmodulations.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydro/allocation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/allocation.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/allocation.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydro/allocation.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydromonthlypower.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydroprepro.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/hydroprepro.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h"`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h"`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/hydroprepro.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/inflowpattern.h` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/inflowpattern.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/hydro/management.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h"`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h"`

**File:** `src/ui/simulator/windows/hydro/dailypower.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/inflowpattern.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h` (1 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/renewable.areasummary.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/renewable.areasummary.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/reservoirlevels.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/reservoirlevels.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/reservoirlevels.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/reservoirlevels.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/reservoirlevels.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/reservoirlevels.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermal.areasummary.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermal.areasummary.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalmodulation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalmodulation.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalprepro.h` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/thermalprepro.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/thermalprepro.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/thermalprepro.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/timeseries.h` (4 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/hydro/series.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/timeseries.h"` (4 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:24`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/windows/thermal/panel.cpp:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/windows/hydro/series.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

**File:** `src/ui/simulator/windows/hydro/localdatahydro.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/timeseries.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/watervalues.h` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/watervalues.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/watervalues.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.cpp:25`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/watervalues.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/watervalues.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-coefficients.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-coefficients.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-coefficients.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-conversion.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-conversion.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-conversion.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:29`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-conversion.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-conversion.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-k.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-k.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-k.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-k.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:26`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-k.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-k.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-translation.h` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-translation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-translation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/area/xcast-translation.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/area/xcast-translation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/area/xcast-translation.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/data.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:32`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/data.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h`

### Pattern: `../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.cpp:30`
**Line:** `#include "../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/bindingconstraint/weights.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h` (1 occurrences)

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.h:38`
**Line:** `#include "../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.h`

### Pattern: `../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.h:38`
**Line:** `#include "../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/constraintsbuilder/links.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/correlation.h` (2 occurrences)

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h`

**File:** `src/ui/simulator/windows/correlation/datasources.hxx:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h`

### Pattern: `../../toolbox/components/datagrid/renderer/correlation.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:31`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h"`

**File:** `src/ui/simulator/windows/correlation/datasources.hxx:23`
**Line:** `#include "../../toolbox/components/datagrid/renderer/correlation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/correlation.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:27`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h`

### Pattern: `../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/simulation/panel.cpp:28`
**Line:** `#include "../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"`

### Pattern: `../../toolbox/components/datagrid/selectionoperation.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:35`
**Line:** `#include "../../toolbox/components/datagrid/selectionoperation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/selectionoperation.h`

### Pattern: `../../toolbox/components/datagrid/selectionoperation.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:35`
**Line:** `#include "../../toolbox/components/datagrid/selectionoperation.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/selectionoperation.h"`

### Pattern: `../../toolbox/components/htmllistbox/item/error.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/logs.cpp:29`
**Line:** `#include "../../toolbox/components/htmllistbox/item/error.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/error.h`

### Pattern: `../../toolbox/components/htmllistbox/item/error.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/logs.cpp:29`
**Line:** `#include "../../toolbox/components/htmllistbox/item/error.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/error.h"`

### Pattern: `../../toolbox/components/mainpanel.h` (4 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:29`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/application/main/main.cpp:38`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:32`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:34`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

### Pattern: `../../toolbox/components/mainpanel.h"` (4 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:29`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h"`

**File:** `src/ui/simulator/application/main/main.cpp:38`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h"`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:32`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h"`

**File:** `src/ui/simulator/windows/inspector/property.update.cpp:34`
**Line:** `#include "../../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h"`

### Pattern: `../../toolbox/components/map/component.h` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:27`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/application/main/events.edit.cpp:26`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/application/main/main.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:29`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

... and 1 more occurrences

### Pattern: `../../toolbox/components/map/component.h"` (6 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:27`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

**File:** `src/ui/simulator/application/main/events.edit.cpp:26`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

**File:** `src/ui/simulator/application/main/main.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:29`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

**File:** `src/ui/simulator/windows/inspector/frame.cpp:31`
**Line:** `#include "../../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

... and 1 more occurrences

### Pattern: `../../toolbox/components/map/settings.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:36`
**Line:** `#include "../../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../../toolbox/components/map/settings.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.h:36`
**Line:** `#include "../../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

### Pattern: `../../toolbox/components/notebook/notebook.h` (15 occurrences)

**File:** `src/ui/simulator/application/main/main.h:37`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:27`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/xcast/xcast.h:24`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/renewables/panel.h:26`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:25`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h`

... and 10 more occurrences

### Pattern: `../../toolbox/components/notebook/notebook.h"` (15 occurrences)

**File:** `src/ui/simulator/application/main/main.h:37`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:27`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

**File:** `src/ui/simulator/windows/xcast/xcast.h:24`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

**File:** `src/ui/simulator/windows/renewables/panel.h:26`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:25`
**Line:** `#include "../../toolbox/components/notebook/notebook.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/notebook/notebook.h"`

... and 10 more occurrences

### Pattern: `../../toolbox/components/refresh.h` (3 occurrences)

**File:** `src/ui/simulator/application/main/refresh.cpp:27`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:26`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:33`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h`

### Pattern: `../../toolbox/components/refresh.h"` (3 occurrences)

**File:** `src/ui/simulator/application/main/refresh.cpp:27`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.cpp:26`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

**File:** `src/ui/simulator/windows/correlation/correlation.cpp:33`
**Line:** `#include "../../toolbox/components/refresh.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/refresh.h"`

### Pattern: `../../toolbox/components/wizardheader.h` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:30`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:25`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:44`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/simulation/run.cpp:49`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../../toolbox/components/wizardheader.h"` (4 occurrences)

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:30`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/textinput/input.cpp:25`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/constraints-builder/constraintsbuilder.cpp:44`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/simulation/run.cpp:49`
**Line:** `#include "../../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

### Pattern: `../../toolbox/create.h` (19 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:33`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/output/output.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/textinput/input.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 14 more occurrences

### Pattern: `../../toolbox/create.h"` (19 occurrences)

**File:** `src/ui/simulator/application/main/events.file.cpp:33`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/ext-source/window.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/toolbox/ext-source/performer.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/output/output.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/textinput/input.cpp:26`
**Line:** `#include "../../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

... and 14 more occurrences

### Pattern: `../../toolbox/dispatcher/study.h` (5 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/help.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/drag-drop.hxx:25`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:27`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

**File:** `src/ui/simulator/application/main/events.file.cpp:31`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

### Pattern: `../../toolbox/dispatcher/study.h"` (5 occurrences)

**File:** `src/ui/simulator/application/main/events.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

**File:** `src/ui/simulator/application/main/help.cpp:30`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

**File:** `src/ui/simulator/application/main/drag-drop.hxx:25`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

**File:** `src/ui/simulator/application/main/events.simulation.cpp:27`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

**File:** `src/ui/simulator/application/main/events.file.cpp:31`
**Line:** `#include "../../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

### Pattern: `../../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:24`
**Line:** `#include "../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../../toolbox/execute/execute.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:24`
**Line:** `#include "../../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h"`

### Pattern: `../../toolbox/ext-source/handler.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:27`
**Line:** `#include "../../toolbox/ext-source/handler.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/handler.h`

### Pattern: `../../toolbox/ext-source/handler.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/paste-from-clipboard.cpp:27`
**Line:** `#include "../../toolbox/ext-source/handler.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/ext-source/handler.h"`

### Pattern: `../../toolbox/input/area.h` (9 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.h:26`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/renewables/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/thermal/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

**File:** `src/ui/simulator/windows/hydro/management.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h`

... and 4 more occurrences

### Pattern: `../../toolbox/input/area.h"` (9 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.h:26`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/windows/renewables/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/windows/thermal/panel.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/windows/hydro/levelsandvalues.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

**File:** `src/ui/simulator/windows/hydro/management.h:25`
**Line:** `#include "../../toolbox/input/area.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/area.h"`

... and 4 more occurrences

### Pattern: `../../toolbox/input/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../../toolbox/input/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/bindingconstraint.h`

### Pattern: `../../toolbox/input/bindingconstraint.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/bindingconstraint/bindingconstraint.h:26`
**Line:** `#include "../../toolbox/input/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/bindingconstraint.h"`

### Pattern: `../../toolbox/input/renewable-cluster.h` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:26`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h`

### Pattern: `../../toolbox/input/renewable-cluster.h"` (3 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.cpp:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h"`

**File:** `src/ui/simulator/windows/renewables/cluster.h:26`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h"`

**File:** `src/ui/simulator/windows/renewables/cluster.h:30`
**Line:** `#include "../../toolbox/input/renewable-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/renewable-cluster.h"`

### Pattern: `../../toolbox/input/thermal-cluster.h` (2 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h`

### Pattern: `../../toolbox/input/thermal-cluster.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/thermal/panel.cpp:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.h:26`
**Line:** `#include "../../toolbox/input/thermal-cluster.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/input/thermal-cluster.h"`

### Pattern: `../../toolbox/jobs.h` (2 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:48`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:26`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../../toolbox/jobs.h"` (2 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:48`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h"`

**File:** `src/ui/simulator/application/main/constraintsbuilder.cpp:26`
**Line:** `#include "../../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h"`

### Pattern: `../../toolbox/system/diskfreespace.hxx` (2 occurrences)

**File:** `src/ui/simulator/windows/memorystatistics/memorystatistics.cpp:31`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx`

**File:** `src/ui/simulator/windows/simulation/run.cpp:51`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx`

### Pattern: `../../toolbox/system/diskfreespace.hxx"` (2 occurrences)

**File:** `src/ui/simulator/windows/memorystatistics/memorystatistics.cpp:31`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx"`

**File:** `src/ui/simulator/windows/simulation/run.cpp:51`
**Line:** `#include "../../toolbox/system/diskfreespace.hxx"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/system/diskfreespace.hxx"`

### Pattern: `../../toolbox/validator.h` (10 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:30`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/renewables/cluster.h:27`
**Line:** `// #include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/thermal/cluster.h:27`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/hydro/management.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

... and 5 more occurrences

### Pattern: `../../toolbox/validator.h"` (10 occurrences)

**File:** `src/ui/simulator/windows/xcast/xcast.hxx:30`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

**File:** `src/ui/simulator/windows/renewables/cluster.h:27`
**Line:** `// #include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

**File:** `src/ui/simulator/windows/thermal/cluster.h:27`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

**File:** `src/ui/simulator/windows/hydro/management.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

**File:** `src/ui/simulator/windows/hydro/prepro.cpp:28`
**Line:** `#include "../../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

... and 5 more occurrences

### Pattern: `../../toolbox/wx-wrapper.h` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.h:24`
**Line:** `// #include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/windows/thermal/panel.h:24`
**Line:** `#include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

### Pattern: `../../toolbox/wx-wrapper.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/renewables/panel.h:24`
**Line:** `// #include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

**File:** `src/ui/simulator/windows/thermal/panel.h:24`
**Line:** `#include "../../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

### Pattern: `../../windows/bindingconstraint/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:55`
**Line:** `#include "../../windows/bindingconstraint/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/bindingconstraint/bindingconstraint.h`

### Pattern: `../../windows/bindingconstraint/bindingconstraint.h"` (1 occurrences)

**File:** `src/ui/simulator/application/main/main.cpp:55`
**Line:** `#include "../../windows/bindingconstraint/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/windows/bindingconstraint/bindingconstraint.h"`

### Pattern: `../bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/event.h:13`
**Line:** `#include "../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../bind.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/core/event/event.h:13`
**Line:** `#include "../bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h"`

### Pattern: `../bindConstraints.h` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:27`
**Line:** `#include "../bindConstraints.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/bindConstraints.h`

### Pattern: `../bindConstraints.h"` (1 occurrences)

**File:** `src/solver/variable/include/antares/solver/variable/economy/all.h:27`
**Line:** `#include "../bindConstraints.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/solver/variable/include/antares/solver/variable/bindConstraints.h"`

### Pattern: `../core/bind.h` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:13`
**Line:** `#include "../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h`

### Pattern: `../core/bind.h"` (1 occurrences)

**File:** `src/ext/yuni/src/yuni/thread/utility.h:13`
**Line:** `#include "../core/bind.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ext/yuni/src/yuni/core/bind.h"`

### Pattern: `../item/bindingconstraint.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:24`
**Line:** `#include "../item/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/bindingconstraint.h`

### Pattern: `../item/bindingconstraint.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/htmllistbox/datasource/bindingconstraint.cpp:24`
**Line:** `#include "../item/bindingconstraint.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/htmllistbox/item/bindingconstraint.h"`

### Pattern: `../toolbox/components/button.h` (6 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/sets.cpp:27`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:39`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/startupwizard.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

**File:** `src/ui/simulator/windows/startupwizard.cpp:30`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h`

... and 1 more occurrences

### Pattern: `../toolbox/components/button.h"` (6 occurrences)

**File:** `src/ui/simulator/windows/link-property-buttons.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/sets.cpp:27`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/studylogs.cpp:39`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/startupwizard.h:26`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

**File:** `src/ui/simulator/windows/startupwizard.cpp:30`
**Line:** `#include "../toolbox/components/button.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/button.h"`

... and 1 more occurrences

### Pattern: `../toolbox/components/datagrid/component.h` (1 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:40`
**Line:** `#include "../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h`

### Pattern: `../toolbox/components/datagrid/component.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:40`
**Line:** `#include "../toolbox/components/datagrid/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/component.h"`

### Pattern: `../toolbox/components/datagrid/renderer/connection.h` (1 occurrences)

**File:** `src/ui/simulator/windows/connection.h:24`
**Line:** `#include "../toolbox/components/datagrid/renderer/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/connection.h`

### Pattern: `../toolbox/components/datagrid/renderer/connection.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/connection.h:24`
**Line:** `#include "../toolbox/components/datagrid/renderer/connection.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/connection.h"`

### Pattern: `../toolbox/components/datagrid/renderer/logfile.h` (2 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:41`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h`

**File:** `src/ui/simulator/windows/studylogs.h:35`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h`

### Pattern: `../toolbox/components/datagrid/renderer/logfile.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/studylogs.cpp:41`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h"`

**File:** `src/ui/simulator/windows/studylogs.h:35`
**Line:** `#include "../toolbox/components/datagrid/renderer/logfile.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/datagrid/renderer/logfile.h"`

### Pattern: `../toolbox/components/mainpanel.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:40`
**Line:** `#include "../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h`

### Pattern: `../toolbox/components/mainpanel.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:40`
**Line:** `#include "../toolbox/components/mainpanel.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/mainpanel.h"`

### Pattern: `../toolbox/components/map/component.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:41`
**Line:** `#include "../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h`

### Pattern: `../toolbox/components/map/component.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:41`
**Line:** `#include "../toolbox/components/map/component.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/component.h"`

### Pattern: `../toolbox/components/map/settings.h` (2 occurrences)

**File:** `src/ui/simulator/windows/exportmap.h:27`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:38`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h`

### Pattern: `../toolbox/components/map/settings.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/exportmap.h:27`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

**File:** `src/ui/simulator/windows/exportmap.cpp:38`
**Line:** `#include "../toolbox/components/map/settings.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/settings.h"`

### Pattern: `../toolbox/components/wizardheader.h` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h`

### Pattern: `../toolbox/components/wizardheader.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

**File:** `src/ui/simulator/windows/exportmap.cpp:31`
**Line:** `#include "../toolbox/components/wizardheader.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/wizardheader.h"`

### Pattern: `../toolbox/create.h` (9 occurrences)

**File:** `src/ui/simulator/windows/connection.cpp:23`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/saveas.cpp:33`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/sets.cpp:28`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/studylogs.cpp:38`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:26`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h`

... and 4 more occurrences

### Pattern: `../toolbox/create.h"` (9 occurrences)

**File:** `src/ui/simulator/windows/connection.cpp:23`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/saveas.cpp:33`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/sets.cpp:28`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/studylogs.cpp:38`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

**File:** `src/ui/simulator/windows/link-property-buttons.cpp:26`
**Line:** `#include "../toolbox/create.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/create.h"`

... and 4 more occurrences

### Pattern: `../toolbox/dispatcher/study.h` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:41`
**Line:** `#include "../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h`

### Pattern: `../toolbox/dispatcher/study.h"` (1 occurrences)

**File:** `src/ui/simulator/windows/startupwizard.cpp:41`
**Line:** `#include "../toolbox/dispatcher/study.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/dispatcher/study.h"`

### Pattern: `../toolbox/execute/execute.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:42`
**Line:** `#include "../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h`

### Pattern: `../toolbox/execute/execute.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:42`
**Line:** `#include "../toolbox/execute/execute.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/execute/execute.h"`

### Pattern: `../toolbox/jobs.h` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:43`
**Line:** `#include "../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h`

### Pattern: `../toolbox/jobs.h"` (1 occurrences)

**File:** `src/ui/simulator/application/study.cpp:43`
**Line:** `#include "../toolbox/jobs.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/jobs.h"`

### Pattern: `../toolbox/validator.h` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

**File:** `src/ui/simulator/windows/exportmap.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h`

### Pattern: `../toolbox/validator.h"` (2 occurrences)

**File:** `src/ui/simulator/windows/saveas.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

**File:** `src/ui/simulator/windows/exportmap.cpp:30`
**Line:** `#include "../toolbox/validator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/validator.h"`

### Pattern: `../toolbox/wx-wrapper.h` (4 occurrences)

**File:** `src/ui/simulator/application/application.h:24`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/study.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/menus.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

**File:** `src/ui/simulator/application/recentfiles.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h`

### Pattern: `../toolbox/wx-wrapper.h"` (4 occurrences)

**File:** `src/ui/simulator/application/application.h:24`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

**File:** `src/ui/simulator/application/study.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

**File:** `src/ui/simulator/application/menus.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

**File:** `src/ui/simulator/application/recentfiles.h:25`
**Line:** `#include "../toolbox/wx-wrapper.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/wx-wrapper.h"`

### Pattern: `../tools/connectioncreator.h` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:23`
**Line:** `#include "../tools/connectioncreator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/connectioncreator.h`

### Pattern: `../tools/connectioncreator.h"` (1 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:23`
**Line:** `#include "../tools/connectioncreator.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/connectioncreator.h"`

### Pattern: `../tools/remover.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:23`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:24`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h`

### Pattern: `../tools/remover.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.cpp:23`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h"`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.cpp:24`
**Line:** `#include "../tools/remover.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/remover.h"`

### Pattern: `../tools/tool.h` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.h:26`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:24`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h`

### Pattern: `../tools/tool.h"` (2 occurrences)

**File:** `src/ui/simulator/toolbox/components/map/nodes/connection.h:26`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h"`

**File:** `src/ui/simulator/toolbox/components/map/controls/addtools.h:24`
**Line:** `#include "../tools/tool.h"`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/ui/simulator/toolbox/components/map/tools/tool.h"`

### Pattern: `..\\..\\..\\tools\\yby-aggregator\\Debug"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:147`
**Line:** `<< "\\..\\..\\..\\tools\\yby-aggregator\\Debug"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\tools\\yby-aggregator\\Debug"`

### Pattern: `..\\..\\..\\tools\\yby-aggregator\\Release"` (1 occurrences)

**File:** `src/libs/antares/locator/locator.cpp:144`
**Line:** `(s = root) << "\\..\\..\\..\\tools\\yby-aggregator\\Release"); // msvc`
**Resolves to:** `/home/runner/work/Antares_Simulator/Antares_Simulator/src/libs/antares/locator/..\\..\\..\\tools\\yby-aggregator\\Release"`


## Directories with Most Relative Paths
--------------------------------------------------
- `src/ui/simulator/application/main`: 253 relative paths
- `src/ui/simulator/windows`: 151 relative paths
- `src/ui/simulator/windows/hydro`: 144 relative paths
- `src/ui/simulator/toolbox/components/datagrid/renderer/area`: 138 relative paths
- `src/ui/simulator/toolbox/input`: 114 relative paths
- `src/ui/simulator/application/main/build`: 111 relative paths
- `src/ui/simulator/toolbox/components/datagrid/renderer`: 111 relative paths
- `src/ext/yuni/src/yuni/thread`: 108 relative paths
- `src/ext/yuni/src/yuni/io/directory`: 90 relative paths
- `src/ui/simulator/windows/simulation`: 75 relative paths
- `src/ui/simulator/toolbox/components/map`: 60 relative paths
- `src/ui/simulator/windows/inspector`: 57 relative paths
- `src/ui/simulator/windows/thermal`: 57 relative paths
- `src/solver/variable/include/antares/solver/variable/economy`: 57 relative paths
- `src/ui/simulator/toolbox/components/datagrid`: 51 relative paths
- `src/ui/simulator/windows/bindingconstraint`: 51 relative paths
- `src/ext/yuni/src/yuni/io/file`: 51 relative paths
- `src/ext/yuni/src/tools/yuni-config`: 49 relative paths
- `src/ui/simulator/windows/options/adequacy-patch`: 48 relative paths
- `src/ext/yuni/src/yuni/core/system`: 48 relative paths
