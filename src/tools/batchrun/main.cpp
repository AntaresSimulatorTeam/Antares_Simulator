// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <map>
#include <string>

#include <yuni/core/getopt.h>
#include <yuni/core/string.h>

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

using namespace Antares;

namespace fs = std::filesystem;

namespace // anonymous
{
class MyStudyFinder final: public Data::StudyFinder
{
public:
    void onStudyFound(const Yuni::String& folder, const Data::StudyVersion&) override
    {
        logs.info() << "Found: " << folder;
        list.push_back(folder);
    }

public:
    Yuni::String::Vector list;
};

} // anonymous namespace

Yuni::String sendToNull()
{
#ifdef __linux__
    return " > /dev/null";
#elif _WIN32
    return " > nul";
#else
#endif
}

int main(int argc, const char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("batchrun");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
    // Initializing the toolbox
    Antares::Resources::Initialize(argc, argv, true);

    if (argc < 2)
    {
        logs.error() << "Usage " << argv[0] << " optInput [other options]";
        return EXIT_FAILURE;
    }

    const bool optVerbose = false;

    // Command Line options
    {
        const std::string optInput(argv[1]);
        if (optInput.empty())
        {
            logs.error() << "A folder input is required.";
            return EXIT_FAILURE;
        }

        // Source Folder
        logs.debug() << "Folder : `" << optInput << '`';

        Yuni::String solver;
        Solver::FindLocation(solver);
        if (solver.empty())
        {
            logs.fatal() << "The solver has not been found";
            return EXIT_FAILURE;
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
            logs.info() << "Number of studies found : " << finder.list.size();
            logs.info() << "Starting...";

            // The folder that contains the solver
            Yuni::String dirname;
            Yuni::IO::parent_path(dirname, solver);

            Yuni::String cmd;

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
                if (!Yuni::System::windows)
                {
                    cmd << "nice ";
                }
                else
                {
                    cmd << "call "; // why is it required for working ???
                }
                cmd << "\"" << solver << "\"";

                // argv[0] => executable name
                // argv[1] => directory name
                for (int ii = 2; ii < argc; ii++)
                {
                    const std::string arg(argv[ii]);
                    if (arg.find(" ") == std::string::npos)
                    {
                        cmd << " " << arg;
                    }
                    else
                    {
                        // Wrap spaces around quotes, if any
                        // example
                        // antares-batchrun directory --solver xpress
                        // --linear-solver-param "PRESOLVE 1"
                        cmd << " \"" << arg << "\"";
                    }
                }

                cmd << " \"" << studypath << "\"";
                if (!optVerbose)
                {
                    cmd << sendToNull();
                }

                // Changing the current working directory
                Yuni::IO::Directory::Current::Set(dirname);
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
