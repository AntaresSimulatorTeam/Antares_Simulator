/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/misc/options.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <limits>
#include <string.h>

#include <yuni/yuni.h>

#include <antares/antares/constants.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include "antares/antares/Enum.hpp"
#include "antares/config/config.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;
using namespace Antares::Data;

std::unique_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings, StudyLoadOptions& options)
{
    settings.reset();

    auto parser = std::make_unique<Yuni::GetOpt::Parser>();

    parser->addParagraph(Yuni::String() << "Antares Solver v" << ANTARES_VERSION_PUB_STR << "\n");

    // Simulation mode
    parser->addParagraph("Simulation");
    // --input
    parser->addFlag(options.studyFolder, 'i', "input", "Study folder");
    // --expansion
    parser->addFlag(options.forceExpansion,
                    ' ',
                    "expansion",
                    "Force the simulation in expansion mode");
    // --economy
    parser->addFlag(options.forceEconomy, ' ', "economy", "Force the simulation in economy mode");
    // --adequacy
    parser->addFlag(options.forceAdequacy,
                    ' ',
                    "adequacy",
                    "Force the simulation in adequacy mode");
    // --parallel
    parser->addFlag(options.enableParallel,
                    ' ',
                    "parallel",
                    "Enable the parallel computation of MC years");
    // --force-parallel
    parser->add(options.maxNbYearsInParallel,
                ' ',
                "force-parallel",
                "Override the max number of years computed simultaneously");

    // add option for ortools use
    // --use-ortools
    parser->addFlag(options.optOptions.ortoolsUsed,
                    ' ',
                    "use-ortools",
                    "Use ortools library to launch solver");

    //--ortools-solver
    parser->add(options.optOptions.ortoolsSolver,
                ' ',
                "ortools-solver",
                "Ortools solver used for simulation (only available with use-ortools "
                "option)\nAvailable solver list : "
                  + availableOrToolsSolversString());

    //--xpress-parameters
    parser->add(
      options.optOptions.solverParameters,
      ' ',
      "solver-parameters",
      "Set xpress solver specific parameters. The specified string must be wrapped into quotes: "
      "--solver-parameters=\"param1 value1 param2 value2\". The syntax of parameters is solver "
      "specfic, examples are given in Antares-Simulator online documentation.");

    parser->addParagraph("\nParameters");
    // --name
    parser->add(settings.simulationName,
                'n',
                "name",
                "Set the name of the new simulation to VALUE");
    // --generators-only
    parser->addFlag(settings.tsGeneratorsOnly,
                    'g',
                    "generators-only",
                    "Run the time-series generators only");

    // --comment-file
    parser->add(settings.commentFile,
                'c',
                "comment-file",
                "Specify the file to copy as comments of the simulation");
    // --force
    parser->addFlag(settings.ignoreWarningsErrors, 'f', "force", "Ignore all warnings at loading");
    // --no-output
    parser->addFlag(settings.noOutput,
                    ' ',
                    "no-output",
                    "Do not write the results in the output folder");
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
                    'm',
                    "mps-export",
                    "Export in the mps (anonymous) format the optimization problems (both optim).");

    // --named-problems
    parser->addFlag(options.namedProblems,
                    's',
                    "named-mps-problems",
                    "Export named constraints and variables in mps (both optim).");

    // --solver-logs
    parser->addFlag(options.optOptions.solverLogs, ' ', "solver-logs", "Print solver logs.");

    parser->addParagraph("\nMisc.");
    // --progress
    parser->addFlag(settings.displayProgression,
                    ' ',
                    "progress",
                    "Display the progress of each task");

    // --pid
    parser->add(settings.PID, 'p', "pid", "Specify the file where to write the process ID");

    // --list-solvers
    parser->addFlag(options.listSolvers,
                    'l',
                    "list-solvers",
                    "List available OR-Tools solvers, then exit.");
    // --version

    parser->addFlag(options.displayVersion,
                    'v',
                    "version",
                    "Print the version of antares-solver and exit");

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
        if (std::ofstream pidfile(optPID); pidfile.is_open())
        {
            pidfile << getpid();
        }
        else
        {
            throw Error::WritingPID(optPID);
        }
    }

    // Simulation name
    if (!options.simulationName.empty())
    {
        settings.simulationName = options.simulationName;
    }

    if (options.maxNbYearsInParallel)
    {
        options.forceParallel = true;
    }

    if (options.enableParallel && options.forceParallel)
    {
        throw Error::IncompatibleParallelOptions();
    }

    if (!settings.simplexOptimRange.empty())
    {
        settings.simplexOptimRange.trim(" \t");
        settings.simplexOptimRange.toLower();
        if (settings.simplexOptimRange == "week")
        {
            options.simplexOptimizationRange = Data::sorWeek;
        }
        else
        {
            if (settings.simplexOptimRange == "day")
            {
                options.simplexOptimizationRange = Data::sorDay;
            }
            else
            {
                throw Error::InvalidOptimizationRange();
            }
        }
    }

    options.checkForceSimulationMode();
    checkOrtoolsSolver(options.optOptions);

    // no-output and force-zip-output
    if (settings.noOutput && settings.forceZipOutput)
    {
        throw Error::IncompatibleOutputOptions("no-output and zip-output options are incompatible");
    }
}

void checkOrtoolsSolver(const Antares::Solver::Optimization::OptimizationOptions& optOptions)
{
    if (optOptions.ortoolsUsed)
    {
        const std::string& solverName = optOptions.ortoolsSolver;
        const std::list<std::string> availableSolverList = getAvailableOrtoolsSolverName();

        // Check if solver is available
        bool found = (std::ranges::find(availableSolverList, solverName)
                      != availableSolverList.end());
        if (!found)
        {
            throw Error::InvalidSolver(optOptions.ortoolsSolver, availableOrToolsSolversString());
        }
    }
}

void Settings::checkAndSetStudyFolder(const std::string& folder)
{
    // The study folder
    if (folder.empty())
    {
        throw Error::NoStudyProvided();
    }

    // Making the path absolute
    std::filesystem::path abspath = std::filesystem::absolute(folder);
    abspath = abspath.lexically_normal();

    // Checking if the path exists
    if (!std::filesystem::exists(abspath))
    {
        throw Error::StudyFolderDoesNotExist(folder);
    }

    // Copying the result
    studyFolder = abspath.string();
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
