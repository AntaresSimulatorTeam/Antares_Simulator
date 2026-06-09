// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/misc/options.h"

#include <fstream>

#include <antares/exception/LoadingError.hpp>
#include "antares/config/config.h"
#include "antares/solver/utils/ortools_utils.h"
#include "antares/utils/utils.h"

namespace
{
namespace fs = std::filesystem;

void addSimulationOptions(Yuni::GetOpt::Parser& parser, Antares::Data::StudyLoadOptions& options)
{
    parser.addParagraph("Simulation");

    parser.addFlag(options.studyFolder, 'i', "input", "Study folder");
    parser.addFlag(options.forceExpansion,
                   ' ',
                   "expansion",
                   "Force the simulation in expansion mode");
    parser.addFlag(options.forceEconomy, ' ', "economy", "Force the simulation in economy mode");
    parser.addFlag(options.forceAdequacy, ' ', "adequacy", "Force the simulation in adequacy mode");
    parser.addFlag(options.enableParallel,
                   ' ',
                   "parallel",
                   "Enable the parallel computation of MC years");
    parser.add(options.maxNbYearsInParallel,
               ' ',
               "force-parallel",
               "Override the max number of years computed simultaneously");
}

void addParameterOptions(Yuni::GetOpt::Parser& parser,
                         Settings& settings,
                         Antares::Data::StudyLoadOptions& options)
{
    (void)options;

    parser.addParagraph("\nParameters");

    parser.add(settings.simulationName, 'n', "name", "Name of the current simulation");
    parser.addFlag(settings.tsGeneratorsOnly,
                   'g',
                   "generators-only",
                   "Run the time-series generators only");
    parser.add(settings.commentFile,
               'c',
               "comment-file",
               "Specify the file to copy as comments of the simulation");
    parser.addFlag(settings.ignoreLoadingErrors, 'f', "force", "Ignore all errors at loading");
    parser.addFlag(settings.noOutput,
                   ' ',
                   "no-output",
                   "Do not write the results in the output folder");
    parser.add(options.nbYears, 'y', "year", "Override the number of MC years");
    parser.addFlag(options.forceYearByYear,
                   ' ',
                   "year-by-year",
                   "Force the writing the result output for each year (economy only)");
    parser.addFlag(options.forceDerated, ' ', "derated", "Force the derated mode");
    parser.addFlag(settings.forceZipOutput,
                   'z',
                   "zip-output",
                   "Force the write output into a single zip archive");
    parser.addFlag(settings.parquetFmtForSimuTables,
                   ' ',
                   "parquet",
                   "Parquet format for simulation tables");
}

void addOptimizationOptions(Yuni::GetOpt::Parser& parser,
                            Settings& settings,
                            Antares::Data::StudyLoadOptions& options)
{
    parser.addParagraph("\nOptimization");

    parser.add(options.solverOptions.linearSolver,
               ' ',
               "linear-solver",
               "Solver used for linear optimizations during simulation. Use --list-solvers to get "
               "the avaible solver list");
    parser.add(options.solverOptions.linearSolver,
               ' ',
               "solver",
               "Deprecated, use --linear-solver instead.");
    parser.add(options.solverOptions.linearSolverParameters,
               ' ',
               "linear-solver-param",
               "Linear solver-specific parameters, for instance \"THREADS 1 "
               "PRESOLVE 1\""
               " for XPRESS or \"parallel/maxnthreads 1, lp/presolving TRUE\" for "
               "SCIP. Syntax is solver-dependent, and only supported for SCIP & XPRESS.");
    parser.add(options.solverOptions.linearSolverParameters,
               ' ',
               "solver-parameters",
               "Deprecated, use --linear-solver-param instead.");
    parser.add(options.solverOptions.lpSolverParamOptim1,
               ' ',
               "linear-solver-param-optim-1",
               "Linear solver-specific parameters for first optimization."
               " Only supported for SCIP & XPRESS.");
    parser.add(options.solverOptions.lpSolverParamOptim2,
               ' ',
               "linear-solver-param-optim-2",
               "Linear solver-specific parameters for second optimization."
               " Only supported for SCIP & XPRESS.");
    parser.addFlag(options.solverOptions.useOptim1BasisInNextWeek,
                   ' ',
                   "use-optim-1-basis-next-week",
                   "Use basis of first optimization in next week's first optimization");
    parser.addFlag(options.solverOptions.useOptim1BasisInOptim2,
                   ' ',
                   "use-optim-1-basis-optim-2",
                   "Use basis of first optimization in second optimization");
    parser.add(options.solverOptions.quadraticSolver,
               ' ',
               "quadratic-solver",
               "Solver used for quadratic optimizations during simulation. Use --list-solvers to "
               "get the avaible solver list");
    parser.add(options.solverOptions.quadraticSolverParameters,
               ' ',
               "quadratic-solver-param",
               "Quadratic solver-specific parameters, for instance \"THREADS 8\""
               " for XPRESS or \"parallel/maxnthreads 8\" for SCIP. "
               "Syntax is solver-dependent.");
    parser.addFlag(settings.simplexOptimRange,
                   ' ',
                   "optimization-range",
                   "Force the simplex optimization range ('day' or 'week')");
    parser.addFlag(settings.ignoreConstraints, ' ', "no-constraints", "Ignore all constraints");
    parser.addFlag(options.noTimeseriesImportIntoInput,
                   ' ',
                   "no-ts-import",
                   "Do not import timeseries into the input folder. This option might be useful "
                   "for running old studies without upgrading them");
    parser.addFlag(options.mpsToExport,
                   'm',
                   "mps-export",
                   "Export in the mps (anonymous) format the optimization problems (both optim).");
    parser.addFlag(options.namedProblems,
                   's',
                   "named-mps-problems",
                   "Export named constraints and variables in mps (both optim).");
    parser.addFlag(options.solverOptions.solverLogs, ' ', "solver-logs", "Print solver logs.");
}

void addMiscOptions(Yuni::GetOpt::Parser& parser,
                    Settings& settings,
                    Antares::Data::StudyLoadOptions& options)
{
    parser.addParagraph("\nMisc.");

    parser.add(settings.PID, 'p', "pid", "Specify the file where to write the process ID");
    parser.addFlag(options.listSolvers,
                   'l',
                   "list-solvers",
                   "List available OR-Tools solvers, then exit.");
    parser.addFlag(options.displayVersion,
                   'v',
                   "version",
                   "Print the version of antares-solver and exit");
}

void applySimplexOptimRange(const Settings& settings, Antares::Data::StudyLoadOptions& options)
{
    if (settings.simplexOptimRange.empty())
    {
        return;
    }

    auto range = settings.simplexOptimRange;
    range.trim(" \t");
    range.toLower();

    if (range == "week")
    {
        options.simplexOptimizationRange = Data::sorWeek;
        return;
    }

    if (range == "day")
    {
        options.simplexOptimizationRange = Data::sorDay;
        return;
    }

    throw Error::InvalidOptimizationRange();
}
} // namespace

std::unique_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings,
                                                   Antares::Data::StudyLoadOptions& options)
{
    settings.reset();

    auto parser = std::make_unique<Yuni::GetOpt::Parser>();

    parser->addParagraph(Yuni::String() << "Antares Solver v" << ANTARES_VERSION_PUB_STR << "\n");

    addSimulationOptions(*parser, options);
    addParameterOptions(*parser, settings, options);
    addOptimizationOptions(*parser, settings, options);
    addMiscOptions(*parser, settings, options);

    parser->remainingArguments(options.studyFolder);

    return parser;
}

void printPIDtoDisk(const Settings& settings)
{
    const auto& optPID = settings.PID;
    if (optPID.empty())
    {
        return;
    }

    if (std::ofstream pidfile(optPID); pidfile.is_open())
    {
        pidfile << getpid();
    }
    else
    {
        throw Error::WritingPID(optPID);
    }
}

void checkAndCorrectSettingsAndOptions(Settings& settings, Antares::Data::StudyLoadOptions& options)
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

    applySimplexOptimRange(settings, options);

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
    simplexOptimRange.clear();
    PID.clear();

    ignoreLoadingErrors = false;
    ignoreConstraints = false;
    tsGeneratorsOnly = false;
    noOutput = false;
    forceZipOutput = false;

    solverOptions = Antares::Solver::Optimization::CmdLineOptimOptions{};
}
