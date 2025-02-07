# Overview

This user guide describes all the main features of the *Antares Simulator*[^1] package.
[^1]: For simplicity's sake, the *Antares Simulator* application will be simply denoted *Antares*.

It gives useful general information regarding the way data are handled and processed.  
Since the *Antares* GUI support has been dropped (in favor of [Antares Web](https://antares-web.readthedocs.io)), 
all its documentation has been removed. If you are still using an old version of *Antares* GUI, you can still find 
its documentation in the assets of the release, or by browsing older versions of the documentation website.

Real-life use of the software involves a learning curve process that cannot be supported by a
simple user guide. In order to be able to address this basic issue, two kinds of resources may be used:

- The ["examples"](https://github.com/AntaresSimulatorTeam/Antares_Simulator_Examples) library, which is meant 
  as a self-teaching way to learn how to use the software. It is regularly enriched with the software's new features.
  The contents of this library depend on the installation package it comes from (public version vs. users' club version).
- The [https://antares-simulator.org](https://antares-simulator.org/) website

If you notice an issue in the documentation, please report it on [github.com](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new/choose).

## Applications

In terms of power studies, the fields of application *Antares* has been designed for are:  

- **Generation adequacy problems**
- **Transmission project profitability**  

### Generation adequacy problems
**Adequacy** problems aim to study the need for new generating plants to keep the security of
supply above a given critical threshold.

What is most important in these studies is to survey a great number of scenarios that represent well enough
the random factors that may affect the balance between load and generation. Economic parameters do not play
as much a critical role as they do in the other kinds of studies since the stakes are mainly to know if and
when supply security is likely to be jeopardized (detailed costs incurred in more ordinary conditions are of
comparatively lower importance). In these studies, the default *Antares* option to use is the
[`adequacy`](solver/static-modeler/04-parameters.md#mode) simulation mode.

### Transmission project profitability
[//]: # (TODO: explain what "fair and perfect market" means)
**Transmission project profitability** studies the savings brought by a specific reinforcement of the grid,
in terms of decrease of the overall system generation cost (using an assumption of a fair and perfect market)
and/or improvement of the security of supply (reduction of the loss-of-load expectation).

In these studies, economic parameters and the physical modeling of the dynamic constraints bearing on
the generating units are of paramount importance. Though a thorough survey of many "Monte-Carlo years"
is still required, the number of scenarios to simulate is not as large as in generation adequacy studies.
In these studies, the default *Antares* option to use is the [`economy`](solver/static-modeler/04-parameters.md#mode) simulation mode.

## Performance considerations
Typically, *Antares* has to solve a least-cost hydro-thermal power schedule and unit commitment problem, with an hourly 
resolution and throughout a week, over a large interconnected system.  
The large number and the size of the individual problems to solve often make optimization sessions computer-intensive.

Depending on user-defined results accuracy requirements, various practical options[^2] allow to simplify either
the formulation of the problems, or their resolution.
[^2]: See [hydro-pricing-mode](solver/static-modeler/04-parameters.md#hydro-pricing-mode), [unit-commitment-mode](solver/static-modeler/04-parameters.md#unit-commitment-mode)

[//]: # (TODO: list in [^2] the other parameters that have impact on performance)

*Antares* has been designed to handle [adequacy and profitability problems](#applications). 

The common rationale of the modeling used in all of these studies is, whenever it is possible,
to divide the overall problem (representation of the system behavior throughout many years,
with a time step of one hour) into a series of standardized, smaller problems.

In *Antares*, the "elementary" optimization problem resulting from this approach is that of the minimization of
the **whole power system**'s operational cost over **one week**, taking into account all proportional and
non-proportional generation costs, as well as transmission charges and "external" costs such as
that of the un-supplied energy (generation shortage) or, conversely, that of the spilled energy (generation excess).  
In other words, adequacy and profitability studies are carried out by solving a series of a large number of week-long 
operation problems (one for each week of each Monte-Carlo year), assumed to be independent to some extent.  
Note that, however, dependency issues such as the management of hydro stock (or any other kind of energy storage
facility) may bring a significant coupling between the successive problems, which needs to be addressed properly[^3].

[^3]: See how *Antares* addresses stock dependency between successive problems [here](solver/static-modeler/06-hydro-heuristics.md#seasonal-hydro-pre-allocation).