# ADR: Custom MPS Generator Preserving Model Order

**Status**: Accepted  
**Date**: 2026-01  
**Related PRs**: #3307, #3346

## Context

Antares Simulator and Antares Modeler export optimization problems in MPS format.

Previously, MPS generation relied on OR-Tools helpers or solver-specific writers.  
Although the output was deterministic, OR-Tools enforces its own ordering rules — notably printing **integer variables before continuous variables**, regardless of their creation order in the model.

This breaks the semantic order defined by Antares and makes MPS files less intuitive to read and compare.

## Decision

We implemented and adopted a **custom, solver-independent MPS generator** used by both:

- Antares Simulator
- Antares Modeler

This generator:

- Preserves the **exact variable creation order**
- Preserves constraint order
- Does not depend on the underlying solver
- Produces deterministic and stable output
- Is covered by dedicated tests

OR-Tools helper-based MPS generation is no longer used.

## Consequences

**Pros**
- MPS reflects the model as built
- Improved readability and debugging
- Solver independence
- Stable output across tools

**Cons**
- Antares maintains its own MPS writer
