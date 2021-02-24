/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
#define __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "parameters.h"

#include <antares/study/OrtoolsSolver.hpp>

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
    //! Ignore all constraints
    bool ignoreConstraints;
    //! Simulation mode
    StudyMode forceMode;

    //! Enables the parallel computation of MC years
    bool enableParallel;

    //! Force a maximum number of MC years computed simultaneously
    bool forceParallel;
    uint maxNbYearsInParallel;

    //! A non-zero value if the data will be used for a simulation
    bool usedByTheSolver;

    //! Force ortools use
    bool ortoolsUsed;

    //! Ortool solver used for simulation
    OrtoolsSolver ortoolsEnumUsed;

    //! Temporary string for passing log message
    mutable Yuni::String logMessage;
    //! Porgression, tick count
    mutable uint progressTickCount;
    //! The current number of ticks
    mutable uint progressTicks;

}; // class StudyLoadOptions

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_SOLVER_LOAD_OPTIONS_H__
