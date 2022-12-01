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

#include <antares/emergency.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/exception/LoadingError.hpp>
#include <antares/Enum.hpp>
#include <antares/constants.h>

#include "utils/ortools_utils.h"

using namespace Yuni;
using namespace Antares;
using namespace Antares::Data;

static std::string availableOrToolsSolversString()
{
    const std::list<std::string> availableSolverList
      = OrtoolsUtils().getAvailableOrtoolsSolverName();
    std::string availableSolverListStr;
    for (auto it = availableSolverList.begin(); it != availableSolverList.end(); it++)
    {
        availableSolverListStr += *it + ";";
    }
    // Remove last semicolumn
    if (!availableSolverListStr.empty())
        availableSolverListStr.pop_back();
    return availableSolverListStr;
}

std::unique_ptr<GetOpt::Parser> CreateParser(Settings& settings,
                                             Antares::Data::StudyLoadOptions& options)
{
    settings.reset();

    auto parser = std::unique_ptr<GetOpt::Parser>(new GetOpt::Parser());

    parser->addParagraph(String() << "Antares Solver v" << ANTARES_VERSION_PUB_STR << "\n");

    // Simulation mode
    parser->addParagraph("Simulation");
    // --input
    parser->addFlag(options.studyFolder, 'i', "input", "Study folder");
    // --expansion
    parser->addFlag(
      options.forceExpansion, ' ', "expansion", "Force the simulation in expansion mode");
    // --economy
    parser->addFlag(options.forceEconomy, ' ', "economy", "Force the simulation in economy mode");
    // --adequacy
    parser->addFlag(
      options.forceAdequacy, ' ', "adequacy", "Force the simulation in adequacy mode");
    // --draft
    parser->addFlag(
      options.forceAdequacyDraft, ' ', "draft", "Force the simulation in adequacy-draft mode");
    // --parallel
    parser->addFlag(
      options.enableParallel, ' ', "parallel", "Enable the parallel computation of MC years");
    // --force-parallel
    parser->add(options.maxNbYearsInParallel,
                ' ',
                "force-parallel",
                "Override the max number of years computed simultaneously");

    // add option for ortools use
    // --use-ortools
    parser->addFlag(
      options.ortoolsUsed, ' ', "use-ortools", "Use ortools library to launch solver");

    //--ortools-solver
    parser->add(options.ortoolsSolver,
                ' ',
                "ortools-solver",
                "Ortools solver used for simulation (only available with use-ortools "
                "option)\nAvailable solver list : "
                  + availableOrToolsSolversString());

    parser->addParagraph("\nParameters");
    // --name
    parser->add(
      settings.simulationName, 'n', "name", "Set the name of the new simulation to VALUE");
    // --generators-only
    parser->addFlag(
      settings.tsGeneratorsOnly, 'g', "generators-only", "Run the time-series generators only");

    // --comment-file
    parser->add(settings.commentFile,
                'c',
                "comment-file",
                "Specify the file to copy as comments of the simulation");
    // --force
    parser->addFlag(settings.ignoreWarningsErrors, 'f', "force", "Ignore all warnings at loading");
    // --no-output
    parser->addFlag(
      settings.noOutput, ' ', "no-output", "Do not write the results in the output folder");
    // --year
    parser->add(options.nbYears, 'y', "year", "Override the number of MC years");
    // --year-by-year
    parser->addFlag(options.forceYearByYear,
                    ' ',
                    "year-by-year",
                    "Force the writing the result output for each year (economy only)");
    // --derated
    parser->addFlag(options.forceDerated, ' ', "derated", "Force the derated mode");

    // --output-force-zip
    parser->addFlag(settings.forceZipOutput,
                    'z',
                    "zip-output",
                    "Force the write output into a single zip archive");

    parser->addParagraph("\nOptimization");

    // --optimization-range
    parser->addFlag(settings.simplexOptimRange,
                    ' ',
                    "optimization-range",
                    "Force the simplex optimization range ('day' or 'week')");

    // --no-constraints
    parser->addFlag(settings.ignoreConstraints, ' ', "no-constraints", "Ignore all constraints");

    // --no-ts-import
    parser->addFlag(options.noTimeseriesImportIntoInput,
                    ' ',
                    "no-ts-import",
                    "Do not import timeseries into the input folder. This option might be useful "
                    "for running old studies without upgrading them");

    // --mps-export
    parser->addFlag(options.mpsToExport,
                    ' ',
                    "mps-export",
                    "Export in the mps format the optimization problems.");

    parser->addParagraph("\nMisc.");
    // --progress
    parser->addFlag(
      settings.displayProgression, ' ', "progress", "Display the progress of each task");

    // --pid
    parser->add(settings.PID, 'p', "pid", "Specify the file where to write the process ID");

    // --list-solvers
    parser->addFlag(
      options.listSolvers, 'l', "list-solvers", "List available OR-Tools solvers, then exit.");
    // --version

    parser->addFlag(
      options.displayVersion, 'v', "version", "Print the version of antares-solver and exit");

    // The last argument is the study folder.
    // Unlike all other arguments, it does not need to be given after a --flag.
    parser->remainingArguments(options.studyFolder);

    return parser;
}

void checkAndCorrectSettingsAndOptions(Settings& settings, Data::StudyLoadOptions& options)
{
    const auto& optPID = settings.PID;
    if (!optPID.empty())
    {
        IO::File::Stream pidfile;
        if (pidfile.openRW(optPID))
            pidfile << ProcessID();
        else
            throw Error::WritingPID(optPID);
    }

    // Simulation name
    if (!options.simulationName.empty())
        settings.simulationName = options.simulationName;

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

    if (!settings.simplexOptimRange.empty())
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

    options.checkForceSimulationMode();
    checkOrtoolsSolver(options);

    // PID
    if (!optPID.empty())
    {
        IO::File::Stream pidfile;
        if (pidfile.openRW(optPID))
            pidfile << ProcessID();
        else
            throw Error::WritingPID(optPID);
    }

    // no-output and force-zip-output
    if (settings.noOutput && settings.forceZipOutput)
    {
        throw Error::IncompatibleOutputOptions("no-output and zip-output options are incompatible");
    }
}

void checkOrtoolsSolver(Data::StudyLoadOptions& options)
{
    // ortools solver
    if (options.ortoolsUsed)
    {
        const std::list<std::string> availableSolverList
          = OrtoolsUtils().getAvailableOrtoolsSolverName();
        if (availableSolverList.empty())
        {
            throw Error::InvalidSolver(options.ortoolsSolver);
        }

        // Default is first available solver
        MPSolver::ParseSolverType(availableSolverList.front(), &options.ortoolsEnumUsed);

        // Check if solver is available
        bool found
          = (std::find(
               availableSolverList.begin(), availableSolverList.end(), options.ortoolsSolver)
             != availableSolverList.end());

        if (found)
        {
            MPSolver::ParseSolverType(options.ortoolsSolver, &options.ortoolsEnumUsed);
        }
        else
        {
            logs.warning() << "Invalid ortools-solver option. Got '" << options.ortoolsSolver
                           << "'. reset to " << (std::string)operations_research::ToString(options.ortoolsEnumUsed);
        }
    }
}

void Settings::checkAndSetStudyFolder(Yuni::String folder)
{
    // The study folder
    if (folder.empty())
        throw Error::NoStudyProvided();

    // Making the path absolute
    String abspath;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(folder, abspath);

    // Checking if the path exists
    if (!IO::Directory::Exists(folder))
    {
        throw Error::StudyFolderDoesNotExist(folder);
    }

    // Copying the result
    studyFolder = folder;
}

void Settings::reset()
{
    studyFolder.clear();
    simulationName.clear();
    commentFile.clear();
    ignoreWarningsErrors = 0;
    tsGeneratorsOnly = false;
    noOutput = false;
    displayProgression = false;
    ignoreConstraints = false;
    forceZipOutput = false;
}
