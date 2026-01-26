// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
#define __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/optimization-options/options.h>

#include "parameters.h"

namespace Antares::Data
{
class StudyLoadOptions final
{
public:
    //! \name Constructor
    //@{
    //! Default constructor
    StudyLoadOptions();
    //@}

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

    //! All options related to linear & quadratic optimization
    Antares::Solver::Optimization::CmdLineOptimOptions solverOptions;

    //! Temporary string for passing log message
    mutable Yuni::String logMessage;

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

} // namespace Antares::Data

#endif // __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
