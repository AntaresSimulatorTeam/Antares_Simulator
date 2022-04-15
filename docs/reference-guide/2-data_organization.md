# Data organization

In Antares, all input and output data regarding a given study are located within a folder named after the study
and which should preferably be stored within a dedicated library of studies
(for instance: `C/.../A\_name\_for\_an\_Antares\_lib/Study-number-one`).

The software has been designed so that all input data may be handled (initialized, updated, deleted) through
the simulator's GUI. Likewise, all results in the output data can be displayed and analyzed within the simulator:
its standard GUI is actually meant to be able to provide, on a stand-alone basis, all the features required to access
input and output in a user-friendly way.

In addition to that, the Antares simulator may come [^3] with or without functional extensions that provide additional
ways to handle input and output data.

These extensions take the form of companion applications whose documentation is independent of that of the main simulator.
For information regarding these tools (*Antares Data Organizer*, *AntaresViz* R packages, …) please refer to
the specific relevant documents.

Besides, a point of notice is that most of Antares files belong to either ".txt" or ".csv" type:
as an alternative to the standard GUI, they can therefore be viewed and updated by many applications
(Windows Notepad, Excel, etc.). However, this is not recommended since handling data this way may result in fatal
data corruption (e.g. as a consequence of accidental insertion of special characters).

Direct access to input or output data files should therefore be reserved to experienced users.

The input data contained in the study folder describe the whole state of development of the interconnected power system
(namely: grid, load and generating plants of every kind) for a given future year.

As already stated, all of these data may be reviewed, updated, deleted through the GUI, whose commands and windows
are described in Sections 3 and 4.

Once the input data is ready for calculation purposes, an Antares session may start and involve any or all of
the following steps: historical time-series analysis, stochastic times-series generation, (draft) adequacy simulation,
(full) adequacy simulation and economic simulation.

The results of the session are stored within the output section of the study folder.
The results obtained in the different sessions are stored side by side and tagged.
The identification tag has two components: a user-defined session name and the time at which the session was launched.

Particular cases are:

1. The outputs of the Antares time-series analyzer are not printed in the general output files but kept within
the input files structure (the reason being that they are input data for the Antares time - series generators).
The associated data may nonetheless be accessed to be reviewed, updated and deleted at any time through the GUI.

2. Some specific input data may be located outside the study folder: this is the case for the historical times-series
to be processed by the time-series analyzer, which may be stored either within the "user" subfolder of the
study or anywhere else (for instance, on a remote "historical data" or "Meteorological data" server).

3. The study folder contains a specific subfolder named "user", whose status is particular: Antares is not allowed
to delete any files within it (yet files may be updated on the user's requirement). As a consequence,
the "user" subfolder is unaffected by the "clean study" command (see [Commands](#commands)).
This subfolder is therefore a
"private" user space, where all kinds of information can be stored without any kind of interference with Antares.
Note that on using the "save as" command (described further below), the choice is given to make or not a copy of
this subfolder.

4. The times-series analyzer requires the use of a temporary directory in which intermediate files are created
in the course of the analysis and deleted in the end, unless the user wishes to keep them for further examination.
Its location is user-defined and should usually be the "user" subfolder if files are to be kept, otherwise any
proper temporary space such as `C..../Temp`.

5. If the interconnected system to study is large and/or if the computer is low on RAM, it is possible to run
the Monte-Carlo adequacy simulator as well as the Monte-Carlo economic simulator in "Swap" mode.
Swap is not handled by the computer's OS but by an Antares specific swap manager, whose operation requires
the definition of a space where the software can store temporary files. This location is user-defined but
should never be chosen within the study folder. `C/.../Temp` may typically be used but an external drive may
be preferred if the computer is low on HDD.

6. The outputs of the Antares Kirchhoff's constraints generator are not printed in the general output files
but kept within the input files structure, the reason being that they are input data for the proper Antares simulation.
The associated data (so-called binding constraints bearing in their name the prefix "@UTO-") may nonetheless
be accessed to be reviewed, updated and deleted at any time through the GUI.


[^3]:
    Many various graphic extensions (under the form of programs written in the R language) are available at :

[https://cran.r-project.org/web/packages/antaresViz/index.html](https://cran.r-project.org/web/packages/antaresViz/index.html)
