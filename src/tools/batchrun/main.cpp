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

#include <antares/antares.h>
#include <antares/logs.h>
#include <antares/solver.h>
#include <antares/resources/resources.h>
#include <yuni/core/nullable.h>
#include <map>
#include <iostream>
#include <yuni/core/string.h>
#include <yuni/core/getopt.h>
#include <antares/study/finder.h>
#include "../../ui/common/winmain.hxx"
#include <antares/version.h>
#include <antares/locale.h>
#ifdef YUNI_OS_WINDOWS
#include <process.h>
#endif

using namespace Yuni;
using namespace Antares;

namespace // anonymous
{
class MyStudyFinder final : public Data::StudyFinder
{
public:
    void onStudyFound(const String& folder, Data::Version) override
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
    argv = AntaresGetUTF8Arguments(argc, argv);

    // Initializing the toolbox
    Antares::Resources::Initialize(argc, argv, true);

    // options
    String optInput;
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
        options.addFlag(
          optForceExpansion, ' ', "economy", "Force the simulation(s) in expansion mode");
        options.addFlag(optForceEconomy, ' ', "economy", "Force the simulation(s) in economy mode");
        options.addFlag(
          optForceAdequacy, ' ', "adequacy", "Force the simulation(s) in adequacy mode");

        options.addParagraph("\nParameters");
        options.add(optName, 'n', "name", "Set the name of the new simulation outputs");
        options.add(optYears, 'y', "year", "Override the number of MC years");
        options.addFlag(optForce, 'f', "force", "Ignore all warnings at loading");
        options.addFlag(
          optNoOutput, ' ', "no-output", "Do not write the results in the output folder");
        options.addFlag(optYearByYear,
                        ' ',
                        "year-by-year",
                        "Force the writing of the result output for each year");

        options.addParagraph("\nOptimization");
        options.addFlag(
          optNoTSImport, ' ', "no-ts-import", "Do not import timeseries into the input folder.");
        options.addFlag(optIgnoreAllConstraints, ' ', "no-constraints", "Ignore all constraints");

        options.addParagraph("\nExtras");
        options.add(optSolver, ' ', "solver", "Specify the antares-solver location");
        options.addFlag(
          optParallel, 'p', "parallel", "Enable the parallel computation of MC years");
        options.add(optForceParallel,
                    ' ',
                    "force-parallel",
                    "Override the max number of years computed simultaneously");
        options.remainingArguments(optInput);
        // Version
        options.addParagraph("\nMisc.");
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");
        options.addFlag(optVerbose, ' ', "verbose", "Displays study runs outputs");

        if (options(argc, argv) == GetOpt::ReturnCode::ERROR)
            return options.errors() ? 1 : 0;

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
    }

    // Source Folder
    logs.debug() << "Folder : `" << optInput << '`';

    String solver;
    if (optSolver.empty())
    {
        Solver::Feature feature = Solver::standard;
        Solver::FindLocation(solver, (Data::Version)Data::versionLatest, feature);
        if (solver.empty())
        {
            logs.fatal() << "The solver has not been found";
            return EXIT_FAILURE;
        }
    }
    else
    {
        String tmp;
        IO::MakeAbsolute(tmp, *optSolver);
        IO::Normalize(solver, tmp);
        if (not IO::File::Exists(solver))
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
            logs.info() << "Found " << finder.list.size() << " studyies";
        else
            logs.info() << "Found 1 study";
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
        IO::ExtractFilePath(dirname, solver);

        String cmd;

        uint studyIndx = 0;
        foreach (auto& studypath, finder.list)
        {
            ++studyIndx;

            logs.info();
            if (optVerbose)
                logs.info();

            logs.checkpoint() << "Running simulation: `" << studypath << "` (" << studyIndx << '/'
                              << (uint)finder.list.size() << ')';
            if (optVerbose)
                logs.debug();

            cmd.clear();
            if (not System::windows)
                cmd << "nice ";
            else
                cmd << "call "; // why is it required for working ???
            cmd << "\"" << solver << "\"";
            if (optForce)
                cmd << " --force";
            if (optForceExpansion)
                cmd << " --economy";
            if (optForceEconomy)
                cmd << " --economy";
            if (optForceAdequacy)
                cmd << " --adequacy";
            if (!(!optName))
                cmd << " --name=\"" << *optName << "\"";
            if (!(!optYears))
                cmd << " --year=" << *optYears;
            if (optNoOutput)
                cmd << " --no-output";
            if (optYearByYear)
                cmd << " --year-by-year";
            if (optNoTSImport)
                cmd << " --no-ts-import";
            if (optIgnoreAllConstraints)
                cmd << " --no-constraints";
            if (optParallel)
                cmd << " --parallel";
            if (!(!optForceParallel))
                cmd << " --force-parallel=" << *optForceParallel;
            cmd << " \"" << studypath << "\"";
            if (!optVerbose)
                cmd << sendToNull();

            // Changing the current working directory
            IO::Directory::Current::Set(dirname);
            // Executing the converter
            if (optVerbose)
                logs.info() << "Executing " << cmd;

            // Execute the command
            int cmd_return_code = system(cmd.c_str());

            if (cmd_return_code != 0)
                logs.error() << "An error occured.";
            else
                logs.info() << "Success.";

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
