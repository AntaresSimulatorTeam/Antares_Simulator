# Expose public C++ API

## Status: Rejected [2024/04/11]

## Context

Originialy exists one install target to install the various antares executables. Most notably they're packaged in the realease
assets.
We may not want to produce a single package with both executables and libraries as they target different users much like 
with apt it is possible to install a binaries and a development (devel) packages.

## Decision

Split installation into two targets

Produce different assets/packages for each install targets.


## Consequences

* Mutltiplication of assets
* Two install targets to manage
* Users of executable not "polluted" by libraries

## Reason for rejection

The decision was rejected because it was deemed unnecessary to split the installation targets. It adds 
some difficulties for few benefits at the moment