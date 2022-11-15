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
#include <antares/resources/resources.h>
#include <map>
#include <iostream>
#include <yuni/core/string.h>
#include <yuni/core/string/wstring.h>
#include <yuni/core/getopt.h>
#include "../../ui/common/winmain.hxx"
#include <antares/version.h>
#include <antares/sys/appdata.h>
#include <yuni/core/system/environment.h>
#include <yuni/core/system/process.h>
#include <yuni/core/system/username.h>
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/memory.h>
#include <antares/sys/hostname.hxx>
#include <antares/locale.h>
#include "../../../config.h"
#include <antares/sys/policy.h>

using namespace Yuni;
using namespace Antares;

int main(int argc, char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("config");
    argv = AntaresGetUTF8Arguments(argc, argv);

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

    if (options(argc, argv) == GetOpt::ReturnCode::_ERROR)
        return options.errors() ? EXIT_FAILURE : 0;

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
