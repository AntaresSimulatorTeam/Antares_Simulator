# Remove usage of fetch_content for runtime dependencies

## Status: Accepted [2024/04/08]

## Context

Efficiently publishing a library involves creating a CMake package for distribution. The recommended approach is to export the project targets.

When integrating dependencies through fetch_content, they are elevated to the status of first-party components within the project's ecosystem. This
means that targets of this dependency must also be exported. Consequently, it becomes necessary to export the targets associated with these dependencies. While this presents a challenge, it's manageable, though not without some complexity.

However, opting to utilize an installed package of a dependency rather than relying on fetch_content complicates matters further.
In such cases, the library is still considered a third-party entity, yet the targets associated must not be exported.
This discrepancy can lead to inconsistencies in behavior and export sets, necessitating the maintenance of two distinct approaches.

## Decision

Remove usage of fetch_content for runtime dependencies (e.g. ortools and minizip)

It can still be used for build or test dependencies (e.g. accepted methode for google test)

## Consequences

* or-tools and minizip must be provided and can't be downloaded automatically
* Developpers must install pre-compiled binairies of said dependencies or buil them from source.