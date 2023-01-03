# Introduction

This document describes all the main features of the **Antares\_Simulator** package, version 8.3.

It gives useful general information regarding the way data are handled and processed,
as well as how the Graphic User Interface (GUI) works. To keep this documentation
as compact as possible, many redundant details (how to mouse-select, etc.) are omitted.

Real-life use of the software involves a learning curve process that cannot be supported by a
simple reference guide. In order to be able to address this basic issue, two kinds of resources may be used:

- The examples' library, which is meant as a self-teaching way to learn how to use the software.
It is enhanced in parallel to the development of new features.
The content of this library may depend on the type of installation package it comes from
(general public or members of the users' club).

- The [https://antares-simulator.org](https://antares-simulator.org/) website

If you notice an issue in the documentation, please report it on [github.com](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new/choose).

# General content of Antares\_Simulator sessions

A typical **Antares\_Simulator** [^1] session involves different steps that are usually run in sequence,
either automatically or with some degree of man-in-the-loop control, depending on the kind of study to perform.

These steps most often involve:

1. GUI session dedicated to the initialization or to the updating of various input data sections
(time-series of many kinds, grid topology, generating fleet description, etc.)

2. GUI session dedicated to the definition of simulation contexts
(definition of the number and consistency of the "Monte-Carlo years" to simulate)

3. Simulation session producing actual numeric scenarios following the directives defined in (2)

4. Optimization session aiming at solving all the optimization problems associated with
each of the scenarios produced in (3)

5. GUI session dedicated to the exploitation of the detailed results yielded by (4)

The scope of this document is to give a detailed description of the software involved in
step (1) to (5) mostly based on a functional approach, leaving thereby aside a significant
part of the mathematical content involved in several of these steps.
[^2] The following picture gives a functional view of all that is involved in steps (1) to (5).

![Antares_Process](Antares_Process.png)

The number and the size of the individual problems to solve (typically, a least-cost hydro-thermal power schedule and
unit commitment, with an hourly resolution and throughout a week, over a large interconnected system) make
optimization sessions often computer-intensive.

Depending on user-defined results accuracy requirements, various practical options allow to simplify either
the formulation of the problems or their resolution.

In terms of power studies, the different fields of application Antares has been designed for are the following:

- **Generation adequacy problems:**
- **Transmission project profitability:**

The common rationale of the modeling used in all of these studies is, whenever it is possible,
to decompose the general issue (representation of the system behavior throughout many years,
with a time step of one hour) into a series of standardized smaller problems.

In Antares, the "elementary" optimization problem resulting from this approach is that of the minimization of
the overall system operation cost over a week, taking into account all proportional and
non-proportional generation costs, as well as transmission charges and "external" costs such as
that of the unsupplied energy (generation shortage) or, conversely, that of the spilled energy (generation excess).
In this light, carrying out generation adequacy studies or transmission projects studies means formulating and
solving a series of a great many week-long operation problems (one for each week of each Monte-Carlo year),
assumed to be independent to some extent (note that, however, issues such as the management of hydro resources –or
possibly other kinds of energy storage facilities– may bring a significant coupling between the successive problems,
which needs to be addressed properly).

### Generation adequacy problems
Adequacy problems aim to study the need for new generating plants to keep the security of
supply above a given critical threshold.

What is most important in these studies is to survey a great number of scenarios that represent well enough
the random factors that may affect the balance between load and generation. Economic parameters do not play
as much a critical role as they do in the other kinds of studies since the stakes are mainly to know if and
when supply security is likely to be jeopardized (detailed costs incurred in more ordinary conditions are of
comparatively lower importance). In these studies, the default Antares option to use is the "Adequacy"
simulation mode, or the "Draft" simulation mode (which is extremely fast but which produces crude results).

### Transmission project profitability
Transmission project profitability studies the savings brought by a specific reinforcement of the grid,
in terms of decrease of the overall system generation cost (using an assumption of fair and perfect market) and/or
improvement of the security of supply (reduction of the loss-of-load expectation).

In these studies, economic parameters and the physical modeling of the dynamic constraints bearing on
the generating units are of paramount importance. Though a thorough survey of many "Monte-Carlo years"
is still required, the number of scenarios to simulate is not as large as in generation adequacy studies.
In these studies, the default Antares option to use is the "Economy" simulation mode.



[^1]: For simplicity's sake, the _**Antares\_Simulator**_ 8.3 application will as of now be simply denoted "Antares".

[^2]: A detailed expression of the basic mathematical problem solved in the red box of the following figure can be found in the document "Optimization problems formulation".
