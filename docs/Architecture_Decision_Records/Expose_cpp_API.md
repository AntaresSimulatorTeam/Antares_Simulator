# Expose public C++ API

## Status: Accepted [2024/02/26]

## Context

Several clients, such as Xpansion, express a preference for utilizing a library over a command-line interface (CLI) tool. Exposing a library necessitates the exposure of a public API.

This API can be presented in either pure C or C++. It's worth noting that a C API can always serve as a facade atop a C++ API.

In the case of a C++ API, concerns regarding ABI compatibility are alleviated, as the Simulator library is provided in a static form. Consequently, when a client upgrades, they would simply need to recompile and relink with the new version of the library.

## Decision

Expose C++ public API.

## Consequences

* This allow clients to bind to library directly and not use CLI client
* Allow data transmission between client and library without resorting to files
* Allow describing a pure C API in the future if the need comes
* Need to maintain API/ABI versioning and communicate changes/breaking changes
