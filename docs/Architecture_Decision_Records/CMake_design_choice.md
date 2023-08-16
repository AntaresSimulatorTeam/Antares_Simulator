
# CMake projects design choices

## Status

Accepted: 2023 August 12

## Context

The current Cmake project design generates some difficulties:
* One monolithic lib core with long compile time or useless recompilation
* One monolithic lib core meaning every thing can (sometimes do) depend on everything else leading to bad design choice by inadvertence
* Antares simulator or parts of it will be used as standalone libraries

## Decision

* libantarescore must be split
* Each "module" should provide a CMakeLists.txt file
* Each "module" should provide public headers accessible from a common "antares" ancestor. E.g `#include <antares/logs/log.h>`
  * Consequence: each module folder should look like the following representation where foo.h is a public header and bar.h a private implementation detail:
    ```
    Foo/
    ├── include
    │   └──antares
    │       └── Foo
    │           └── foo.h
    ├── bar.cpp
    ├── bar.h
    ├── CMakeList.txt
    └── foo.cpp
    ```
  * Public includes should be made public with: `target_include_directories(foo PUBLIC include)`

## Consequences

* Increase verbosity of the folder structures
* Better dependency management and visualisation between the different modules
* Prevent unwanted dependencies by design