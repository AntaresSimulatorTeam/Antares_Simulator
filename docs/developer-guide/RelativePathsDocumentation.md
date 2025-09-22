# Relative Paths in Antares Simulator Codebase

This document provides a comprehensive overview of relative paths used throughout the Antares Simulator codebase, their purposes, and how they resolve to absolute paths.

## Overview

The Antares Simulator codebase contains **3,899 relative paths** across various file types:

- **C/C++ Include Statements**: 1,261 occurrences
- **Runtime Resource Paths**: 136 occurrences  
- **Tool/Binary Paths**: 532 occurrences
- **CMake Build Paths**: 77 occurrences
- **Test Paths**: 3 occurrences
- **Other**: 1,890 occurrences

## Categories of Relative Paths

### 1. C/C++ Include Statements (1,261 occurrences)

These are `#include` directives using relative paths to reference header files. They follow standard C++ relative inclusion patterns.

**Common patterns:**
- `../header.h` - Parent directory
- `../../path/to/header.h` - Two levels up
- `../../../component/header.h` - Three levels up

**Examples:**
```cpp
// In src/tests/src/expressions/test_DeepWideTrees.cpp:32
#include "../modeler/mockModelerObjects.h"
// Resolves to: src/tests/src/modeler/mockModelerObjects.h

// In src/ui/common/component/frame/registry.cpp:26  
#include "../../dispatcher.h"
// Resolves to: src/ui/common/dispatcher.h
```

**Resolution strategy:** These paths are resolved by the C++ preprocessor relative to the including file's directory. The build system's include directories configuration ensures proper resolution.

### 2. Runtime Resource Paths (136 occurrences)

These are hardcoded relative paths used at runtime to locate resources, examples, and configuration files.

**Key files:**
- `src/libs/antares/resources/resources.cpp` - Resource discovery
- `src/libs/antares/locator/locator.cpp` - Tool location

**Examples:**
```cpp
// Windows resource path
s.clear() << RootFolder << "\\..\\resources\\";
// Resolves to: <RootFolder>/../resources/

// Unix example path  
s.clear() << RootFolder << SEP << ".." << SEP << "examples";
// Resolves to: <RootFolder>/../examples/

// MSVC debug resource path
p.clear() << RootFolder << "\\..\\Debug\\..\\resources\\";
// Resolves to: <RootFolder>/../Debug/../resources/
```

**Resolution strategy:** These paths are resolved at runtime using the `IO::Normalize()` function which handles path canonicalization.

### 3. Tool/Binary Paths (532 occurrences)

Relative paths used to locate executables and tools within the Antares ecosystem.

**Examples:**
```cpp
// Solver location (Windows)
searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Release");
// Resolves to: <root>/../../../solver/Release

// Constraints builder location (Unix)
searchpaths.directories.push_back((s = root) << "/../../solver");  
// Resolves to: <root>/../../solver
```

**Resolution strategy:** These use the Yuni library's `IO::SearchPath` mechanism to locate tools across multiple possible locations.

### 4. CMake Build Paths (77 occurrences)

Relative paths in CMake configuration files for build system setup.

**Examples:**
```cmake
# Include common settings
include(../../cmake/common-settings.cmake)
# Resolves to: src/cmake/common-settings.cmake

# Include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")
# Resolves to: Parent of current CMake source directory
```

**Resolution strategy:** CMake resolves these paths relative to `CMAKE_CURRENT_SOURCE_DIR` at configure time.

## Directory Structure Context

The relative paths reflect the modular structure of Antares Simulator:

```
src/
├── api/                    # API components
├── cmake/                  # Build system files  
├── ext/yuni/              # External Yuni framework
├── libs/antares/          # Core Antares libraries
├── solver/                # Simulation solver
├── tests/                 # Test suites
├── tools/                 # Utility tools
└── ui/                    # User interface (deprecated)
```

## Most Active Directories

Directories with the highest concentration of relative paths:

1. `src/ui/simulator/application/main`: 253 paths
2. `src/ui/simulator/windows`: 151 paths  
3. `src/ui/simulator/windows/hydro`: 144 paths
4. `src/ui/simulator/toolbox/components/datagrid/renderer/area`: 138 paths
5. `src/ui/simulator/toolbox/input`: 114 paths

> **Note:** The UI components contain many relative paths as they are deprecated and maintained for compatibility only.

## Platform-Specific Considerations

### Windows Paths
- Use backslash separators (`\\`)
- Handle both Debug and Release build configurations
- Account for MSVC-specific directory structures

### Unix/Linux Paths  
- Use forward slash separators (`/`)
- Follow standard Unix filesystem conventions
- Support system-wide installation paths (`/usr/share/antares/`)

## Best Practices for Developers

### For New Code
1. **Avoid relative paths in runtime code** - Use absolute paths or proper resource location APIs
2. **Use CMake variables** - Instead of hardcoded relative paths in CMake files
3. **Leverage include directories** - Rather than deep relative includes

### For Include Statements
- Prefer shorter relative paths when possible
- Use forward declarations to reduce include dependencies
- Consider if the dependency indicates a design issue

### For Resource Location
- Use the `Antares::Resources` API for finding resources
- Leverage the existing search path mechanisms
- Test resource location on both development and installed configurations

## Resolution Tools

The codebase provides several utilities for path resolution:

1. **Yuni::IO::Normalize()** - Canonicalizes paths and resolves `.` and `..` components
2. **Antares::Resources::FindFile()** - Locates resources using search paths
3. **IO::SearchPath** - Multi-directory search for executables
4. **CMake path resolution** - Built-in CMake variable expansion

## Migration Considerations

While most relative paths work correctly in the current system, future improvements could include:

1. **Centralized resource management** - Replace scattered resource paths with a unified system
2. **Build system modernization** - Use modern CMake practices to reduce relative path dependencies
3. **UI code cleanup** - Remove deprecated UI components and their relative path dependencies

## Conclusion

The extensive use of relative paths in Antares Simulator reflects its evolution as a complex, modular simulation platform. While these paths generally resolve correctly, understanding their patterns and resolution mechanisms is crucial for effective development and maintenance of the codebase.

For detailed analysis of all 3,899 relative paths, see the generated `relative_paths_analysis.md` file.