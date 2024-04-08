# Remove usage of fetch_content for runtime dependencies

## Status: Accepted [2024/04/08]

## Context

Trying to publish a library require publishing a cmake package. The proper way to do so is to export the project targets
When using fetch_content for a dependency, this dependency is promoted as a third party component of the project. This
means that targets of this dependency must also be exported. In itself it is already a issue but manageable albeit painfully.
However when a developer make the choice of using a installed package of a dependency instead of rellying on fetch_content, this
library is treated as third party, targets don't have to be exported, even worse won't be found elligible for export leading to maintain two behaviour 
and export sets.

## Decision

Remove usage of fetch_content for runtime dependencies (e.g. ortools and minizip)

It can still be used for build or test dependencies (e.g. accepted methode for google test)

## Consequences

* or-tools and minizip must be provided and can't be downloaded automatically
* Developpers must install pre-compiled binairies of said dependencies or buil them from source.