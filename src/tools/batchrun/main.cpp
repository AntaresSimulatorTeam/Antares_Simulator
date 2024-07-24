/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <iostream>
#include <map>
#include <string>

#include <yuni/core/getopt.h>
#include <yuni/core/nullable.h>
#include <yuni/core/string.h>

#include <antares/antares/version.h>
#include <antares/args/args_to_utf8.h>
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/resources/resources.h>
#include <antares/solver/simulation/solver.h>
#include <antares/solver/utils/ortools_utils.h>
#include <antares/study/finder/finder.h>
#include "antares/antares/antares.h"
#include "antares/locator/locator.h"

#ifdef YUNI_OS_WINDOWS
#include <process.h>
#endif

using namespace Yuni;
using namespace Antares;

namespace fs = std::filesystem;

namespace // anonymous
{
class MyStudyFinder final: public Data::StudyFinder
{
public:
    void onStudyFound(const String& folder, const Data::StudyVersion&) override
    {
        logs.info() << "Found: " << folder;
        list.push_back(folder);
    }

public:
    String::Vector list;
};

} // anonymous namespace

String sendToNull()
{
#ifdef __linux__
    return " > /dev/null";
#elif _WIN32
    return " > nul";
#else
#endif
}

int main(int argc, char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("batchrun");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
    // Initializing the toolbox
    Antares::Resources::Initialize(argc, argv, true);

    // options
    std::string optInput;
    std::string ortoolsSolver;
    bool optNoTSImport = false;
    bool optIgnoreAllConstraints = false;
    bool optForceExpansion = false;
    bool optForceEconomy = false;
    bool optForceAdequacy = false;
    bool optForce = false;
    bool optYearByYear = false;
    bool optNoOutput = false;
    bool optParallel = false;
    bool optVerbose = false;
    bool ortoolsUsed = false;
    Nullable<uint> optYears;
    Nullable<String> optSolver;
    Nullable<String> optName;
    Nullable<uint> optForceParallel;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(String() << "Antares Batch Simulation Launcher v" << VersionToCString()
                                      << "\n");
        // Input
        options.addParagraph("Studies");
        options.add(optInput, 'i', "input", "The input folder, where to find some studies");

        // Simulation mode
        options.addParagraph("\nSimulation mode");
        options.addFlag(optForceExpansion,
                        ' ',
                        "economy",
                        "Force the simulation(s) in expansion mode");
        options.addFlag(optForceEconomy, ' ', "economy", "Force the simulation(s) in economy mode");
        options.addFlag(optForceAdequacy,
                        ' ',
                        "adequacy",
                        "Force the simulation(s) in adequacy mode");

        options.addParagraph("\nParameters");
        options.add(optName, 'n', "name", "Set the name of the new simulation outputs");
        options.add(optYears, 'y', "year", "Override the number of MC years");
        options.addFlag(optForce, 'f', "force", "Ignore all warnings at loading");
        options.addFlag(optNoOutput,
                        ' ',
                        "no-output",
                        "Do not write the results in the output folder");
        options.addFlag(optYearByYear,
                        ' ',
                        "year-by-year",
                        "Force the writing of the result output for each year");

        options.addParagraph("\nOptimization");
        options.addFlag(optNoTSImport,
                        ' ',
                        "no-ts-import",
                        "Do not import timeseries into the input folder.");
        options.addFlag(optIgnoreAllConstraints, ' ', "no-constraints", "Ignore all constraints");

        options.addParagraph("\nExtras");
        options.add(optSolver, ' ', "solver", "Specify the antares-solver location");
        options.addFlag(optParallel,
                        'p',
                        "parallel",
                        "Enable the parallel computation of MC years");
        options.add(optForceParallel,
                    ' ',
                    "force-parallel",
                    "Override the max number of years computed simultaneously");

        // add option for ortools use
        // --use-ortools
        options.addFlag(ortoolsUsed, ' ', "use-ortools", "Use ortools library to launch solver");

        //--ortools-solver
        options.add(ortoolsSolver,
                    ' ',
                    "ortools-solver",
                    "Ortools solver used for simulation (only available with use-ortools "
                    "option)\nAvailable solver list : "
                      + availableOrToolsSolversString());

        options.remainingArguments(optInput);
        // Version
        options.addParagraph("\nMisc.");
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");
        options.addFlag(optVerbose, ' ', "verbose", "Displays study runs outputs");

        if (options(argc, argv) == GetOpt::ReturnCode::error)
        {
            return options.errors() ? 1 : 0;
        }

        if (optVersion)
        {
            PrintVersionToStdCout();
            return 0;
        }

        if (optInput.empty())
        {
            logs.error() << "A folder input is required.";
            return EXIT_FAILURE;
        }

        if (optForceExpansion and optForceAdequacy)
        {
            logs.error() << "contradictory options: --expansion and --adequacy";
            return EXIT_FAILURE;
        }

        if (optForceEconomy and optForceAdequacy)
        {
            logs.error() << "contradictory options: --economy and --adequacy";
            return EXIT_FAILURE;
        }

        if (ortoolsUsed)
        {
            const auto availableSolvers = getAvailableOrtoolsSolverName();
            if (auto it = std::find(availableSolvers.begin(),
                                    availableSolvers.end(),
                                    ortoolsSolver);
                it == availableSolvers.end())
            {
                logs.error() << "Please specify a solver using --ortools-solver. Available solvers "
                             << availableOrToolsSolversString() << "";
                return EXIT_FAILURE;
            }
        }

        // Source Folder
        logs.debug() << "Folder : `" << optInput << '`';

        String solver;
        if (optSolver.empty())
        {
            Solver::FindLocation(solver);
            if (solver.empty())
            {
                logs.fatal() << "The solver has not been found";
                return EXIT_FAILURE;
            }
        }
        else
        {
            std::string tmp = *optSolver;
            fs::path solverPath = fs::absolute(tmp).lexically_normal();
            if (!fs::exists(solverPath))
            {
                logs.fatal() << "The solver has not been found. specify --solver=" << solver;
                return EXIT_FAILURE;
            }
        }

        logs.info() << "  Solver: '" << solver << "'";
        logs.info();
        logs.info();
        logs.info() << "Searching for studies...";
        logs.info();
        MyStudyFinder finder;
        finder.lookup(optInput);
        finder.wait();

        if (not finder.list.empty())
        {
            if (finder.list.size() > 1)
            {
                logs.info() << "Found " << finder.list.size() << " studyies";
            }
            else
            {
                logs.info() << "Found 1 study";
            }
            logs.info() << "Starting...";

            if (!(!optName))
            {
                String name;
                name = *optName;
                name.replace("\"", "\\\"");
                *optName = name;
            }

            // The folder that contains the solver
            String dirname;
            IO::parent_path(dirname, solver);

            String cmd;

            uint studyIndx = 0;
            foreach (auto& studypath, finder.list)
            {
                ++studyIndx;

                logs.info();
                if (optVerbose)
                {
                    logs.info();
                }

                logs.checkpoint() << "Running simulation: `" << studypath << "` (" << studyIndx
                                  << '/' << (uint)finder.list.size() << ')';
                if (optVerbose)
                {
                    logs.debug();
                }

                cmd.clear();
                if (not System::windows)
                {
                    cmd << "nice ";
                }
                else
                {
                    cmd << "call "; // why is it required for working ???
                }
                cmd << "\"" << solver << "\"";
                if (optForce)
                {
                    cmd << " --force";
                }
                if (optForceExpansion)
                {
                    cmd << " --economy";
                }
                if (optForceEconomy)
                {
                    cmd << " --economy";
                }
                if (optForceAdequacy)
                {
                    cmd << " --adequacy";
                }
                if (!(!optName))
                {
                    cmd << " --name=\"" << *optName << "\"";
                }
                if (!(!optYears))
                {
                    cmd << " --year=" << *optYears;
                }
                if (optNoOutput)
                {
                    cmd << " --no-output";
                }
                if (optYearByYear)
                {
                    cmd << " --year-by-year";
                }
                if (optNoTSImport)
                {
                    cmd << " --no-ts-import";
                }
                if (optIgnoreAllConstraints)
                {
                    cmd << " --no-constraints";
                }
                if (optParallel)
                {
                    cmd << " --parallel";
                }
                if (optForceParallel)
                {
                    cmd << " --force-parallel=" << *optForceParallel;
                }
                if (ortoolsUsed)
                {
                    cmd << " --use-ortools --ortools-solver=" << ortoolsSolver;
                }

                cmd << " \"" << studypath << "\"";
                if (!optVerbose)
                {
                    cmd << sendToNull();
                }

                // Changing the current working directory
                IO::Directory::Current::Set(dirname);
                // Executing the converter
                if (optVerbose)
                {
                    logs.info() << "Executing " << cmd;
                }

                // Execute the command
                int cmd_return_code = system(cmd.c_str());

                if (cmd_return_code != 0)
                {
                    logs.error() << "An error occured.";
                }
                else
                {
                    logs.info() << "Success.";
                }

                if (cmd_return_code == -1)
                {
#ifdef YUNI_OS_WINDOWS
                    switch (errno)
                    {
                    case E2BIG:
                        logs.error() << "Argument list (which is system dependent) is too big";
                        break;
                    case ENOENT:
                        logs.error() << "Command interpreter cannot be found";
                        break;
                    case ENOEXEC:
                        logs.error()
                          << "Command-interpreter file has invalid format and is not executable";
                        break;
                    case ENOMEM:
                        logs.error() << "Not enough memory is available to execute command";
                        break;
                    }
#endif
                }
            }

            logs.info() << "Done.";

            // Time interval
            if (optVerbose)
            {
                logs.debug();
                logs.debug();
            }
        }
        else
        {
            logs.fatal() << "No study has been found.";
            return 4;
        }

        return 0;
    }
}
