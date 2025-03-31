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
#include <fstream>

#include <boost/algorithm/string/join.hpp>

#include <antares/exception/LoadingError.hpp>
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

    //--linear-solver
    parser->add(options.solverOptions.linearSolver,
                ' ',
                "linear-solver",
                "Solver used for linear optimizations during simulation\nAvailable solver list : "
                  + availableLinearSolversString());

    //--solver
    parser->add(options.solverOptions.linearSolver,
                ' ',
                "solver",
                "Deprecated, use linear-solver instead.");

    //--linear-solver-parameters
    parser->add(
      options.solverOptions.linearSolverParameters,
      ' ',
      "linear-solver-parameters",
      "Set linear solver-specific parameters, for instance --linear-solver-parameters=\"THREADS 1 "
      "PRESOLVE 1\""
      "for XPRESS or --linear-solver-parameters=\"parallel/maxnthreads 1, lp/presolving TRUE\" for "
      "SCIP."
      "Syntax is solver-dependent, and only supported for SCIP & XPRESS.");

    //--solver-parameters
    parser->add(options.solverOptions.linearSolverParameters,
                ' ',
                "solver-parameters",
                "Deprecated, use linear-solver-parameters instead.");

    //--quadratic-solver
    parser->add(
      options.solverOptions.quadraticSolver,
      ' ',
      "quadratic-solver",
      "Solver used for quadratic optimizations during simulation\nAvailable solver list : "
        + availableQuadraticSolversString());

    //--quadratic-solver-parameters
    parser->add(options.solverOptions.quadraticSolverParameters,
                ' ',
                "quadratic-solver-parameters",
                "Set quadratic solver-specific parameters, for instance "
                "--quadratic-solver-parameters=\"THREADS 8\""
                "for XPRESS or --quadratic-solver-parameters=\"parallel/maxnthreads 8\" for SCIP."
                "Syntax is solver-dependent.");

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
    parser->addFlag(options.solverOptions.solverLogs, ' ', "solver-logs", "Print solver logs.");

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
    checkForSolversExistence(options.solverOptions);

    // no-output and force-zip-output
    if (settings.noOutput && settings.forceZipOutput)
    {
        throw Error::IncompatibleOutputOptions("no-output and zip-output options are incompatible");
    }
}

void checkSolverExists(std::string solverName, const std::list<std::string> availableSolversList)
{
    // Check if solver is available
    bool found = std::ranges::find(availableSolversList, solverName) != availableSolversList.end();
    if (!found)
    {
        throw Error::InvalidSolver(solverName, boost::algorithm::join(availableSolversList, ","));
    }
}

void checkForSolversExistence(Solver::Optimization::OptimizationOptions& solverOptions)
{
    checkSolverExists(solverOptions.linearSolver, getAvailableLinearSolverNames());
    checkSolverExists(solverOptions.quadraticSolver, getAvailableQuadraticSolverNames());
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
