# Expose public C++ API

## Status: Rejected [2024/04/11]

## Context

Originally, there exists a single install target designated to deploy the diverse Antares executables, predominantly packaged within the release assets. However, there's a rationale behind avoiding the creation of a unified package containing both executables and libraries, given that they cater to distinct user demographics. This mirrors the flexibility offered by package managers such as apt, where users can opt to install binary packages and separate development (devel) packages according to their requirements.

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