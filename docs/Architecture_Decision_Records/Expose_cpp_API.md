# Expose public C++ API

## Status: Accepted [2024/02/26]

## Context

Some clients (e.g. Xpansion) would prefer to use a library than a CLI tool. Exposing a library require exposing a public API.
The API can be exposed either as pure C or as C++.
A C API can always be developped as a facade on top of a C++ API.
C++ API will not have to care about ABI compatibility because Simulator lib will be provided as static, meaning a client
upgrading would need to be recompiled and relink with the new lib version.

## Decision

Expose C++ public API.

## Consequences

* This allow clients to bind to library directly and not use CLI client
* Allow data transmission between client and library without resorting to files
* Allow describing a pure C API in the future if the need comes
* Need to maintain API/ABI versioning
