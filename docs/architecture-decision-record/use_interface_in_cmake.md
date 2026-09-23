# Use $<BUILD_INTERFACE> in CMakeLists

## Status: accepted

## Context
Using BUILD_INTERFACE is required to allow build system to find include at the right place during compilation on a local
machine while still maintaining the possibility to resolve include path once installed.

INSTALL_INTERFACE can be used when different path is required between build and installation

## Decision

Accepted

## Consequences

Use BUILD_INTERFACE in CMakeLists
E.g : Add a public include as $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
