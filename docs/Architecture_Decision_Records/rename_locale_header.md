# Rename file "locale.h"

## Status: accepted

## Context

File locale.h found in antares source directory bear the same name as a standard header locale.h
It creates an issue where find one before the other may lead to missing definition errors.

## Decision

Rename antares locale.h file to a new name, e.g. antares_locale.h

## Consequences

File renamed
