// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <iostream>

#include <yuni/core/getopt.h>
#include <yuni/core/system/username.h>

#include <antares/antares/version.h>
#include <antares/args/args_to_utf8.h>
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/resources/resources.h>
#include <antares/sys/policy.h>

using namespace Yuni;
using namespace Antares;

int main(int argc, const char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("config");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
    // Initializing the toolbox
    Antares::Resources::Initialize(argc, argv, true);

    // Parser
    GetOpt::Parser options;
    //
    options.addParagraph(String() << "Antares Infos v" << VersionToCString() << "\n");

    bool optPrint = true;
    options.addFlag(optPrint, 'p', "print", "Print the current configuration");

    // Version
    bool optVersion = false;
    options.addFlag(optVersion, 'v', "version", "Print the version and exit");

    if (options(argc, argv) == GetOpt::ReturnCode::error)
    {
        return options.errors() ? EXIT_FAILURE : 0;
    }

    if (optVersion)
    {
        PrintVersionToStdCout();
        return 0;
    }

    if (optPrint || true)
    {
        // Load the local policy settings
        LocalPolicy::Open();
        LocalPolicy::CheckRootPrefix(argv[0]);

        Clob out;
        LocalPolicy::DumpToString(out);

        std::cout << out << std::flush;

        // release all resources held by the local policy engine
        LocalPolicy::Close();
    }
    return 0;
}
