/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
#define __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "parameters.h"

namespace Antares
{
namespace Data
{
class StudyLoadOptions
{
public:
    //! \name Constructor
    //@{
    //! Default constructor
    StudyLoadOptions();
    //@}

    //! Push a new log entry according to the local progress data
    void pushProgressLogs() const;

    void checkForceSimulationMode();

public:
    //! The number of MC years (non-zero to force the value)
    uint nbYears;
    //! True to prepare the output folder
    bool prepareOutput;
    //! True to load only the strictly required data
    bool loadOnlyNeeded;
    //! Force the year-by-year flag
    bool forceYearByYear;
    //! Force the derated mode
    bool forceDerated;

    //! No Timeseries import in the input
    // This option might be useful for running old studies without upgrading
    bool noTimeseriesImportIntoInput;

    //! Simplex optimization range
    SimplexOptimization simplexOptimizationRange;
    //! Mps files export asked
    bool mpsToExport;
    //! named problems
    bool namedProblems = false;
    //! enable solver logs
    bool solverLogs = false;
    //! Ignore all constraints
    bool ignoreConstraints;
    //! Simulation mode
    SimulationMode forceMode;

    //! Enables the parallel computation of MC years
    bool enableParallel;

    //! Force a maximum number of MC years computed simultaneously
    bool forceParallel;
    uint maxNbYearsInParallel;

    //! A non-zero value if the data will be used for a simulation
    bool usedByTheSolver;

    //! Force ortools use
    bool ortoolsUsed;
    //! THe solver name, sirius is the default
    std::string ortoolsSolver = "sirius";

    //! Temporary string for passing log message
    mutable Yuni::String logMessage;
    //! Porgression, tick count
    mutable uint progressTickCount;
    //! The current number of ticks
    mutable uint progressTicks;

    //! Display version number and exit
    bool displayVersion = false;

    //! True => display the list of OR-Tools solvers and exit
    bool listSolvers = false;

    //! Simulation mode
    bool forceExpansion = false;
    bool forceEconomy = false;
    bool forceAdequacy = false;

    YString studyFolder;
    YString simulationName;
}; // class StudyLoadOptions

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
