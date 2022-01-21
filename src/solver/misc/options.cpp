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

#include <yuni/yuni.h>
#include <yuni/core/system/process.h>
#include <yuni/core/getopt.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/logs.h>
#include <antares/study/study.h>
#include <cassert>
#include <string.h>
#include <limits>
#include <algorithm>

#include "options.h"
#include "../config.h"

#include "../../config.h"

#include <antares/memory/memory.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/exception/LoadingError.hpp>
#include <antares/Enum.hpp>
#include <antares/constants.h>

#include "utils/ortools_utils.h"

using namespace Yuni;
using namespace Antares;
using namespace Antares::Data;

void GrabOptionsFromCommandLine(int argc,
                                char* argv[],
                                Settings& settings,
                                Antares::Data::StudyLoadOptions& options)
{
    // Reset
    settings.studyFolder.clear();
    settings.simulationName.clear();
    settings.commentFile.clear();
    settings.ignoreWarningsErrors = 0;
    settings.tsGeneratorsOnly = false;
    settings.noOutput = false;
    settings.displayProgression = false;
    settings.ignoreConstraints = false;

    bool optForceExpansion = false;
    bool optForceEconomy = false;
    bool optForceAdequacy = false;
    bool optForceAdequacyDraft = false;

    String optStudyFolder;

    GetOpt::Parser parser;

    parser.addParagraph(String() << "Antares Solver v" << ANTARES_VERSION_PUB_STR << "\n");

    // Simulation mode
    parser.addParagraph("Simulation");
    // --input
    parser.addFlag(optStudyFolder, 'i', "input", "Study folder");
    // --expansion
    parser.addFlag(optForceExpansion, ' ', "expansion", "Force the simulation in expansion mode");
    // --economy
    parser.addFlag(optForceEconomy, ' ', "economy", "Force the simulation in economy mode");
    // --adequacy
    parser.addFlag(optForceAdequacy, ' ', "adequacy", "Force the simulation in adequacy mode");
    // --draft
    parser.addFlag(
      optForceAdequacyDraft, ' ', "draft", "Force the simulation in adequacy-draft mode");
    // --parallel
    parser.addFlag(
      options.enableParallel, ' ', "parallel", "Enable the parallel computation of MC years");
    // --force-parallel
    parser.add(options.maxNbYearsInParallel,
               ' ',
               "force-parallel",
               "Override the max number of years computed simultaneously");

    bool useOrtools = false;

    // add option for ortools use
    // --use-ortools
    parser.addFlag(useOrtools, ' ', "use-ortools", "Use ortools library to launch solver");

    // add option for ortools solver used
    std::string ortoolsSolver;

    // Define available solver list
    std::list<std::string> availableSolverList = OrtoolsUtils().getAvailableOrtoolsSolverName();
    std::string availableSolverListStr;
    for (auto it = availableSolverList.begin(); it != availableSolverList.end(); it++)
    {
        availableSolverListStr += *it + ";";
    }
    // Remove last semicolumn
    if (!availableSolverListStr.empty())
        availableSolverListStr.pop_back();

    //--ortools-solver
    parser.add(ortoolsSolver,
               ' ',
               "ortools-solver",
               "Ortools solver used for simulation (only available with use-ortools "
               "option)\nAvailable solver list : "
                 + availableSolverListStr);

    parser.addParagraph("\nParameters");
    // --name
    String optName;
    parser.add(optName, 'n', "name", "Set the name of the new simulation to VALUE");
    // --generators-only
    parser.addFlag(
      settings.tsGeneratorsOnly, 'g', "generators-only", "Run the time-series generators only");

    // --comment-file
    parser.add(settings.commentFile,
               'c',
               "comment-file",
               "Specify the file to copy as comments of the simulation");
    // --force
    parser.addFlag(settings.ignoreWarningsErrors, 'f', "force", "Ignore all warnings at loading");
    // --no-output
    parser.addFlag(
      settings.noOutput, ' ', "no-output", "Do not write the results in the output folder");
    // --year
    parser.add(options.nbYears, 'y', "year", "Override the number of MC years");
    // --year-by-year
    parser.addFlag(options.forceYearByYear,
                   ' ',
                   "year-by-year",
                   "Force the writing the result output for each year (economy only)");
    // --derated
    parser.addFlag(options.forceDerated, ' ', "derated", "Force the derated mode");

    parser.addParagraph("\nOptimization");

    // --optimization-range
    parser.addFlag(settings.simplexOptimRange,
                   ' ',
                   "optimization-range",
                   "Force the simplex optimization range ('day' or 'week')");

    // --no-constraints
    parser.addFlag(settings.ignoreConstraints, ' ', "no-constraints", "Ignore all constraints");

    // --no-ts-import
    parser.addFlag(options.noTimeseriesImportIntoInput,
                   ' ',
                   "no-ts-import",
                   "Do not import timeseries into the input folder. This option might be useful "
                   "for running old studies without upgrading them");

    // --mps-export
    parser.addFlag(options.mpsToExport,
                   ' ',
                   "mps-export",
                   "Export in the mps format the optimization problems.");

    parser.addParagraph("\nMisc.");
    // --progress
    parser.addFlag(
      settings.displayProgression, ' ', "progress", "Display the progress of each task");
    // --swap
    parser.add(settings.swap,
               ' ',
               "swap-folder",
#ifdef ANTARES_SWAP_SUPPORT
               String("Folder where the swap files will be written. (default: '")
                 << Antares::memory.cacheFolder() << "')"
#else
               "Folder where the swap files will be written. This option has no effect (swap files "
               "are only available for 'antares-solver-swap')"
#endif
    );

    // --pid
    String optPID;
    parser.add(optPID, 'p', "pid", "Specify the file where to write the process ID");

    // --version
    bool optVersion = false;
    parser.addFlag(optVersion, 'v', "version", "Print the version of the solver and exit");

    parser.remainingArguments(optStudyFolder);

    // Ask to parse the command line
    if (!parser(argc, argv))
        exit(parser.errors() ? 1 : 0);

    // Version
    if (optVersion)
    {
#ifdef GIT_SHA1_SHORT_STRING
        std::cout << ANTARES_VERSION_STR << " (revision " << GIT_SHA1_SHORT_STRING << ")"
                  << std::endl;
#else
        std::cout << ANTARES_VERSION_STR << std::endl;
#endif
        return;
    }

    // PID
    if (not optPID.empty())
    {
        IO::File::Stream pidfile;
        if (pidfile.openRW(optPID))
            pidfile << ProcessID();
        else
            throw Error::WritingPID(optPID);
    }

    // Simulation name
    if (not optName.empty())
        settings.simulationName = optName;

    if (options.nbYears > MAX_NB_MC_YEARS)
    {
        throw Error::InvalidNumberOfMCYears(options.nbYears);
    }

    if (options.maxNbYearsInParallel)
        options.forceParallel = true;

    if (options.enableParallel && options.forceParallel)
    {
        throw Error::IncompatibleParallelOptions();
    }

    if (not settings.simplexOptimRange.empty())
    {
        settings.simplexOptimRange.trim(" \t");
        settings.simplexOptimRange.toLower();
        if (settings.simplexOptimRange == "week")
            options.simplexOptimizationRange = Data::sorWeek;
        else
        {
            if (settings.simplexOptimRange == "day")
                options.simplexOptimizationRange = Data::sorDay;
            else
            {
                throw Error::InvalidOptimizationRange();
            }
        }
    }

    // Forcing simulation mode
    {
        uint number_of_enabled_force_options
          = optForceExpansion + optForceEconomy + optForceAdequacy + optForceAdequacyDraft;

        if (number_of_enabled_force_options > 1)
        {
            throw Error::InvalidSimulationMode();
        }
        if (optForceExpansion)
            options.forceMode = stdmExpansion;
        else if (optForceEconomy)
            options.forceMode = stdmEconomy;
        else if (optForceAdequacy)
            options.forceMode = stdmAdequacy;
        else if (optForceAdequacyDraft)
            options.forceMode = stdmAdequacyDraft;
    }

    // define ortools global values
    options.ortoolsUsed = useOrtools;

    // ortools solver
    if (useOrtools)
    {
        if (availableSolverList.empty())
        {
            throw Error::InvalidSolver(ortoolsSolver);
        }

        // Default is first available solver
        options.ortoolsEnumUsed = Antares::Data::Enum::fromString<Antares::Data::OrtoolsSolver>(
          availableSolverList.front());

        // Check if solver is available
        bool found
          = (std::find(availableSolverList.begin(), availableSolverList.end(), ortoolsSolver)
             != availableSolverList.end());

        if (found)
        {
            options.ortoolsEnumUsed
              = Antares::Data::Enum::fromString<Antares::Data::OrtoolsSolver>(ortoolsSolver);
        }
        else
        {
            logs.warning() << "Invalid ortools-solver option. Got '" << ortoolsSolver
                           << "'. reset to " << Enum::toString(options.ortoolsEnumUsed);
        }
    }

    // The study folder
    if (optStudyFolder.empty())
        throw Error::NoStudyProvided();

    // Making the path absolute
    String abspath;
    IO::MakeAbsolute(abspath, optStudyFolder);
    IO::Normalize(optStudyFolder, abspath);

    // Checking if the path exists
    if (not IO::Directory::Exists(optStudyFolder))
    {
        throw Error::StudyFolderDoesNotExist(optStudyFolder);
    }

    // Copying the result
    settings.studyFolder = optStudyFolder;
}
