# Overview

_**This section is under construction**_

The *Solver* is *Antares Simulator*'s main feature.

As of 2024, this feature is being overhauled to allow more flexibility in the definition of physical models.

- The existing solver will still be maintained for a few years, you can find its
  documentation under the ["legacy solver" section](legacy-solver/01-overview-legacy-solver.md).
- The new solver will be gradually enriched to cover all existing features, you can find its
  documentation under the ["new solver" section](new-solver/01-overview-new-solver.md).
- It will be possible, for a few transitional years, to define "hybrid" studies,
  mixing [legacy models](legacy-solver/05-model.md) with [new models](new-solver/05-model.md). This is
  documented [here](new-solver/07-hybrid-studies.md).

As a consequence, you will be able to use the solver with three types of studies: legacy studies, new studies, and
hybrid studies.  
All these possibilities are offered by the same "antares-solver" executable ; it is able to adapt to the different input
files. Its usage is documented [here](02-command-line.md).
