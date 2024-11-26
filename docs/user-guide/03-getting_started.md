# Getting started

## Organizing your data

### General information

In Antares, all input and output data of a given study are located within a folder named after the study
and which should preferably be stored within a dedicated library of studies
(for instance: `C/.../A_name_for_an_Antares_lib/Study-number-one`).

Note that most of the *Antares* input and output files are either in ".txt" or ".csv" format. Although you may very well 
modify them in your favorite text editor, you are not advised to do so, since handling data this way may result in fatal
data corruption (e.g. as a consequence of accidental insertion of special characters). Only manipulate raw input and 
output data directly if you are an experienced *Antares* user.  
Instead, we advise you to use [Antares Web](https://antares-web.readthedocs.io) or [Antares Extensions](#using-extensions).

The input data contained in the study folder describe the whole state of development of the interconnected power system
(namely: grid, load and generating plants of every kind) for a given future year.

Once the input data is ready for calculation purposes, an Antares session may start and involve any or all of
the following steps: historical time-series analysis, stochastic times-series generation,
(full) adequacy simulation and economic simulation.

The results of the session are stored within the **input or the output** section of the study folder (depending on the session
type, refer to the documentation of the different features for more information).
The results obtained in the different sessions are stored side by side and tagged.
The identification tag has two components: a user-defined session name and the time at which the session was launched.

Particular cases:
- Some specific input data may be located outside the study folder (this is the case for some inputs of 
  the [time-series analyzer](other-features/analyzer.md), for example).
- If the study folder contains a specific subfolder named "user", then this folder will have a special status: *Antares* 
  will consider it as a "private" user space, and avoid deleting any files in it (but files may be updated if the user requires it). 
  As a consequence, the user is free to store any kind of information in this directory without the risk of it being 
  modified by *Antares*.

### Using extensions

The Antares simulator may be delivered with or without functional extensions that provide additional
ways to handle input and output data.  
Many various extensions (programs written in R language) are available:  

- [AntaresEditObject](https://cran.r-project.org/web/packages/antaresEditObject)
- [AntaresRead](https://cran.r-project.org/web/packages/antaresRead)
- [AntaresViz](https://cran.r-project.org/web/packages/antaresViz)

These extensions take the form of companion applications that are independent of the main simulator.
For information regarding these tools, please refer to the relevant documents.

## Usual workflow of an *Antares* session

A typical *Antares* session involves different steps that are usually run in sequence,
either automatically or with some degree of man-in-the-loop control, depending on the kind of study to perform.

These steps most often involve:

1. Initializing or updating the input data (time-series, grid topology, fleet description, etc.).  
   *In this step, the user is expected to provide all the input data they have, except the time-series that are 
   supposed to be [automatically generated](solver/static-modeler/04-parameters.md#generate) by *Antares* (see step (3)).*  
   *As stated above, it is highly recommended to use robust tools to produce input data, such as [Antares Web](https://antares-web.readthedocs.io) 
   or [Antares Extensions](#using-extensions).*   
2. Defining the simulation contexts (definition of the "Monte-Carlo years" to simulate)
3. *(Optional)* If some time-series are supposed to be [automatically generated](solver/static-modeler/04-parameters.md#generate), 
   running a simulation to produce actual numeric scenarios, following the directives defined in (2).  
   *In this step, the [ts-generator](ts-generator/01-overview-tsgenerator.md) tool should be used.*
4. Running the optimization, to solve all the optimization problems associated with each of the scenarios produced in (3).  
   *In this step, the main [solver](solver/01-overview-solver.md) tool should be used.*
5. Exploiting the detailed [results](solver/static-modeler/03-outputs.md) yielded by (4).  
   *In this step, we recommend using [Antares Web](https://antares-web.readthedocs.io) 
   or [Antares Extensions](#using-extensions).*

The scope of this user guide is to give a detailed description of the *Antares Simulator* software involved in
steps (1) to (5) mostly based on a functional approach, leaving thereby aside a significant
part of the mathematical content involved in several of these steps.  
The following picture gives a functional view of all that is involved in steps (1) to (5).

![Antares_Process](img/Antares_Process.jpg)
