/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <fstream>

#include <antares/exception/LoadingError.hpp>
#include "antares/config/config.h"
#include "antares/solver/utils/ortools_utils.h"
#include "antares/utils/utils.h"

using namespace Antares;
using namespace Antares::Data;
namespace fs = std::filesystem;

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

    parser->addParagraph("\nParameters");
    // --name
    parser->add(settings.simulationName, 'n', "name", "Name of the current simulation");
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

    //--linear-solver
    parser->add(options.solverOptions.linearSolver,
                ' ',
                "linear-solver",
                "Solver used for linear optimizations during simulation. Use --list-solvers to get "
                "the avaible solver list");

    //--solver
    parser->add(options.solverOptions.linearSolver,
                ' ',
                "solver",
                "Deprecated, use --linear-solver instead.");

    //--linear-solver-param
    parser->add(options.solverOptions.linearSolverParameters,
                ' ',
                "linear-solver-param",
                "Linear solver-specific parameters, for instance \"THREADS 1 "
                "PRESOLVE 1\""
                " for XPRESS or \"parallel/maxnthreads 1, lp/presolving TRUE\" for "
                "SCIP. Syntax is solver-dependent, and only supported for SCIP & XPRESS.");

    //--solver-parameters
    parser->add(options.solverOptions.linearSolverParameters,
                ' ',
                "solver-parameters",
                "Deprecated, use --linear-solver-param instead.");

    // --linear-solver-param-optim-1
    parser->add(options.solverOptions.lpSolverParamOptim1,
                ' ',
                "linear-solver-param-optim-1",
                "Linear solver-specific parameters for first optimization."
                " Only supported for SCIP & XPRESS.");

    // --linear-solver-param-optim-2
    parser->add(options.solverOptions.lpSolverParamOptim2,
                ' ',
                "linear-solver-param-optim-2",
                "Linear solver-specific parameters for second optimization."
                " Only supported for SCIP & XPRESS.");

    // --use-optim-1-basis-next-week
    parser->addFlag(options.solverOptions.useOptim1BasisInNextWeek,
                    ' ',
                    "use-optim-1-basis-next-week",
                    "Use basis of first optimization in next week's first optimization");

    // --use-optim-1-basis-optim-2
    parser->addFlag(options.solverOptions.useOptim1BasisInOptim2,
                    ' ',
                    "use-optim-1-basis-optim-2",
                    "Use basis of first optimization in second optimization");

    //--quadratic-solver
    parser->add(options.solverOptions.quadraticSolver,
                ' ',
                "quadratic-solver",
                "Solver used for quadratic optimizations during simulation. Use --list-solvers to "
                "get the avaible solver list");

    //--quadratic-solver-param
    parser->add(options.solverOptions.quadraticSolverParameters,
                ' ',
                "quadratic-solver-param",
                "Quadratic solver-specific parameters, for instance \"THREADS 8\""
                " for XPRESS or \"parallel/maxnthreads 8\" for SCIP. "
                "Syntax is solver-dependent.");

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

void printPIDtoDisk(Settings& settings)
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
}

void checkAndCorrectSettingsAndOptions(Settings& settings, Data::StudyLoadOptions& options)
{
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

    if (settings.noOutput && settings.forceZipOutput)
    {
        throw Error::IncompatibleOutputOptions("no-output and zip-output options are incompatible");
    }
}

void checkStudyFolder(const std::string& studyFolder)
{
    if (studyFolder.empty())
    {
        throw Error::NoStudyProvided();
    }

    if (!Utils::isPathValid(studyFolder))
    {
        throw Error::StudyFolderContainsNonASCIIchars(studyFolder);
    }
}

std::string fixStudyFolder(const std::string& studyFolder)
{
    fs::path abspath = fs::absolute(studyFolder);
    abspath = abspath.lexically_normal();

    if (!fs::exists(abspath) || !fs::is_directory(abspath))
    {
        throw Error::StudyFolderDoesNotExist(studyFolder);
    }

    return abspath.string();
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
