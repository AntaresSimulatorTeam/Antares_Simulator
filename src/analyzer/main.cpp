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

#include <yuni/yuni.h>
#include <yuni/core/getopt.h>

#include <antares/antares/version.h>
#include <antares/args/args_to_utf8.h>
#include <antares/inifile/inifile.h>
#include <antares/locale/locale.h>
#include <antares/logs/hostinfo.h>
#include <antares/logs/logs.h>
#include <antares/sys/policy.h>

#include "atsp/atsp.h"

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
    {
        nowstr[result] = '\0';
    }

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

    if (not memory.initializeTemporaryFolder())
    {
        return EXIT_FAILURE;
    }

    // locale
    InitializeDefaultLocale();

    logs.applicationName("analyzer");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();

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

        if (options(argc, argv) == GetOpt::ReturnCode::error)
        {
            return options.errors() ? 1 : 0;
        }

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
    {
        return 1;
    }

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
            {
                atsp.computeMonthlyCorrelations(); // monthly correlations
            }
        }
    }

    logs.info();
    logs.info() << "Done.";

    return 0;
}
