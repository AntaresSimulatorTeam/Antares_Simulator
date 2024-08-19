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

#include "application/main.h"

#include <yuni/yuni.h>
#include <yuni/core/getopt.h>

#include <antares/args/args_to_utf8.h>
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/resources/resources.h>
#include <antares/sys/policy.h>

#include "application/application.h"

#ifdef YUNI_OS_MSVC
// WxWidgets Stuff
IMPLEMENT_APP(Antares::Application)
#else

#include <antares/memory/memory.h>

// WxWidgets Stuff
IMPLEMENT_APP_NO_MAIN(Antares::Application)

using namespace Yuni;
using namespace Antares;

int main(int argc, const char* argv[])
{
    if (not memory.initializeTemporaryFolder())
    {
        return EXIT_FAILURE;
    }

    // We have one or several arguments
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);

    // locale
    InitializeDefaultLocale();

    if (argc > 1)
    {
        GetOpt::Parser options;
        //
        options.addParagraph(Yuni::String()
                             << "Antares Simulator v" << ANTARES_VERSION_PUB_STR << "\n");

        // Study
        options.add(Antares::Forms::StudyToLoadAtStartup, 'p', "path", "Path to the study to load");
        options.remainingArguments(Antares::Forms::StudyToLoadAtStartup);
        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        // An error has occured
        if (options(argc, argv) == GetOpt::ReturnCode::error)
        {
            return options.errors() ? 1 : 0;
        }

        // Version
        if (optVersion)
        {
            std::cout << ANTARES_VERSION_STR << "\n";
            return 0;
        }
    }

    // UNIX - Application name
    // For Windows, see application/application.cpp
    logs.applicationName("antares");

    // UNIX - Load the local policy settings
    // For Windows, see application/application.cpp
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    // Initialize resources
    Resources::Initialize(argc, argv, true);

    // Running the GUI
    const int ret = wxEntry(argc, argv);

    LocalPolicy::Close();
    return ret;
}

#endif // not YUNI_OS_MSVC
