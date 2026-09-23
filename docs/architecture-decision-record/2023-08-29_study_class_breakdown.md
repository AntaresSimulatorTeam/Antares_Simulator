# Study class breakdown choices

## Status

Accepted: 2023 August 29

## Context

The class `Antares::Data::Study`, which is central in the codebase, violates the Single Responsibility Principle (SRP)
in multiple respects, which hurt the maintainability and evolutivity of the simulator. We discuss here some high-level
directions to improve this situation in the future.

Flaws we have discussed today are:

 - the `Study` carries multiple types of data, and methods, that do not belong together: 
    - the input network model (including areas, links, thermal clusters ...)
    - the input timeseries
    - some "business level" parameters (for example simulation mode: adequacy or economy)
    - some "technical" execution parameters (number of threads)
    - some GUI-related methods and data
    - some runtime methods and data associated to one particular simulation execution
      (`StudyRuntimeInfo`, `AreaScratchpad` ...)
 - `Study` carries its own unique serialization method, which does not separate responsibilities and
   makes it impossible to implement multiple serialization formats
 - `Study` and its children expose public fields and methods that makes it too easy to break its integrity.
   they are not responsible anymore for their own consistency.
 - `const Study&` or worse `Study&` is often used as function argument, in functions that only use
   a very small part of the study. Here again those functions either have too many responsibilities,
   or don't make their responsibility explicit by their signature.


## Decision

In order to improve on this, we agree on the following principles:

- Serialization methods will be extracted from the study and its parts to an interface with possibly
  multiple implementations in the future (~ hexagonal architecture)
- Mutable data used for the simulation should be extracted from the class `Study`, and be managed by
  the simulation implementation class instead
- Services used for the execution of the simulation should be extracted from the `Study`:
  service queue, result writer ...
- In new functions, we should only provide as argument the necessary subset of data for that processing,
  which will almost never be the whole `Study`
  When working on existing code, we should seize the opportunity to implement that principle there also.
- GUI related methods and data should be tagged deprecated, and possibly extracted from the class to the `ui` module
- When adding data or methods to the data model, we must take care of not making public data and methods
  which could break the model consistency


## Consequences

Overall, those choices will lead to increased respect of the Single Responsibility Principle,
with usual benefits of readability, maintainability, evolutivity, for a large part of the 
codebase.