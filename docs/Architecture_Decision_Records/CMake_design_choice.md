
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
    ├── antares
    │   └── Foo
    │       └── foo.h
    ├── bar.cpp
    ├── bar.h
    ├── CMakeList.txt
    └── foo.cpp
    ```
* In the future all public headers will be grouped in a dedicated folder at the top level of the source directory to ease export and install

## Consequences

* Increase verbosity of the folder structures
* Better dependency management and visualisation between the different modules
* Prevent unwanted dependencies by design