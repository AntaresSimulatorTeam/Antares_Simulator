# Relative Paths Analysis and Resolution Tools

This directory contains tools to analyze and resolve relative paths in the Antares Simulator codebase.

## Files

- `resolve_paths.py` - Interactive tool for resolving and analyzing relative paths
- `analyze_relative_paths.py` - Comprehensive analysis script (in `/tmp/`) 

## Usage

### Resolve a specific relative path
```bash
python3 tools/resolve_paths.py --resolve "../path/to/file" --base src/component/file.cpp
```

### Analyze all relative paths in a file
```bash
python3 tools/resolve_paths.py --analyze src/component/file.cpp
```

### Check if all relative paths in a file exist
```bash
python3 tools/resolve_paths.py --check-file src/component/file.cpp
```

## Documentation

See `docs/developer-guide/RelativePathsDocumentation.md` for comprehensive documentation of relative paths in the codebase.

## Key Findings

The Antares Simulator codebase contains **3,899 relative paths** across:
- C/C++ include statements (1,261)
- Runtime resource paths (136) 
- Tool/binary paths (532)
- CMake build paths (77)
- Other usage (1,890+)

Most relative paths resolve correctly and serve important purposes in the modular architecture of the simulator.