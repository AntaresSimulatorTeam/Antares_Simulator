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
#include <antares/logs.h>
#include <yuni/core/getopt.h>
#include "../ui/common/winmain.hxx"
#include <antares/utils.h>
#include <antares/study/cleaner.h>
#include <antares/version.h>
#include "atsp/atsp.h"
#include <antares/hostinfo.h>
#include <antares/inifile.h>
#include <antares/sys/policy.h>
#include <antares/locale.h>
#include "../config.h"

using namespace Yuni;
using namespace Antares;

#define SEP Yuni::IO::Separator

static bool OpenLogFilename(const String& optSettings)
{
    IniFile ini;
    if (not ini.open(optSettings))
    {
        logs.fatal() << "impossible to open " << optSettings;
        return false;
    }
    auto* section = ini.find(".general");
    if (!section)
    {
        logs.fatal() << "invalid map structure (section not found): " << optSettings;
        return false;
    }
    auto* property = section->find("study");
    if (!property || !property->value)
    {
        logs.fatal() << "invalid map structure (no study specified): " << optSettings;
        return false;
    }
    String filename;
    filename << property->value << SEP << "logs";

    // Date/time
    char nowstr[128];
    time_t nowbin;
    struct tm nowstruct;

    time(&nowbin);
#ifdef YUNI_OS_MSVC
    localtime_s(&nowstruct, &nowbin);
#else
    nowstruct = *localtime(&nowbin);
#endif
    size_t result = strftime(nowstr, sizeof(nowstr), "%Y%m%d-%H%M%S", &nowstruct);
    if (result == 0)
    {
        logs.error() << "[output] Could not get string from strftime()";
        nowstr[0] = '\0';
    }
    else
        nowstr[result] = '\0';

    filename << SEP << "analyzer-" << (const char*)nowstr << ".log";

    // Assigning the log filename
    logs.logfile(filename);

    if (not logs.logfileIsOpened())
    {
        logs.error() << "Impossible to open " << filename;
        return false;
    }
    return true;
}

static void NotEnoughMemory()
{
    logs.fatal() << "Not enough memory. aborting.";
    exit(42);
}

int main(int argc, char* argv[])
{
    // Dealing with the lack of memory
    std::set_new_handler(&NotEnoughMemory);

    // Swap memory
    if (not memory.initialize())
        return EXIT_FAILURE;

    // locale
    InitializeDefaultLocale();

    logs.applicationName("analyzer");
    argv = AntaresGetUTF8Arguments(argc, argv);

    String optSettings;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(Yuni::String()
                             << "Antares Study Analyzer v" << Antares::VersionToCString() << "\n");
        // Input
        options.remainingArguments(optSettings);
        // Output
        options.add(optSettings, 'i', "input", "INI file which contains the settings");

        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        if (!options(argc, argv))
            return options.errors() ? 1 : 0;

        if (optVersion)
        {
            Antares::PrintVersionToStdCout();
            return 0;
        }
    }

    if (optSettings.empty())
    {
        logs.fatal() << "No settings file. Aborting.";
        return 1;
    }

    if (!OpenLogFilename(optSettings))
        return 1;

    // Starting !
    logs.checkpoint() << "Antares Analyzer v" << Antares::VersionToCString();
    WriteHostInfoIntoLogs();
#ifdef ANTARES_BENCHMARK
    logs.info() << " :: Built with benchmark support";
#endif
    logs.info();
    logs.info() << "Log filename: " << logs.logfile();

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    // ASTP
    // note : ASTP must be destroyed before the variable logs
    {
        ATSP atsp;
        // Load global settings + per area
        if (atsp.loadFromINIFile(optSettings))
        {
            // Print a summary of the global settings
            atsp.printSummary();
            // Prepare data
            if (atsp.preflight())
                atsp.computeMonthlyCorrelations(); // monthly correlations
        }
    }

    logs.info();
    logs.info() << "Done.";

    return 0;
}
