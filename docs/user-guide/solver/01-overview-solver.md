# Overview

_**This section is under construction**_

The *Solver* is *Antares Simulator*'s main feature.  
It covers modelling & solving the adequacy optimization problem.

As of 2024, the modelling feature is being overhauled to allow more flexibility in the definition of physical models.

- The existing modeler will still be maintained for a few years, you can find its
  documentation under the ["static modeler" section](static-modeler/01-overview-static-modeler).
- The new modeler will be gradually enriched to cover all existing features, you can find its
  documentation under the ["dynamic modeler" section](dynamic-modeler/01-overview-dynamic-modeler).
- It will be possible, for a few transitional years, to define "hybrid" studies,
  mixing [static models](static-modeler/05-model.md) with [dynamic models](dynamic-modeler/05-model.md). This is
  documented [here](dynamic-modeler/08-hybrid-studies.md).

As a consequence, you will be able to use the solver with three types of studies: legacy studies, new studies, and
hybrid studies.  
All these possibilities are offered by the same "antares-solver" executable ; it is able to adapt to the different input
files. Its usage is documented [here](02-command-line.md).
