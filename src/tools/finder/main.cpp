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

#include <antares/logs.h>
#include <antares/study/finder.h>
#include <yuni/core/getopt.h>
#include "../../ui/common/winmain.hxx"
#include <antares/utils.h>
#include <antares/version.h>
#include <antares/locale.h>

using namespace Yuni;
using namespace Antares;

class MyStudyFinder : public Data::StudyFinder
{
public:
    MyStudyFinder() : extra(false), csv(false)
    {
    }

    virtual ~MyStudyFinder()
    {
    }

    void onStudyFound(const String& folder, Data::Version version)
    {
        std::cout << folder;
        if (extra)
        {
            if (csv)
                std::cout << ";" << Data::VersionToCStr(version);
            else
                std::cout << " (" << Data::VersionToCStr(version) << ')';
        }
        std::cout << '\n';
    }

public:
    //! Print extra informations, such as the study version
    bool extra;
    //! Print in CSV mode
    bool csv;
};

int main(int argc, char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("finder");
    argv = AntaresGetUTF8Arguments(argc, argv);

    Yuni::String::Vector optInput;
    bool optExtra = false;
    bool optCSV = false;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(Yuni::String() << "Antares Finder v" << VersionToCString() << "\n");
        // Input
        options.remainingArguments(optInput);
        // Output
        options.add(optInput,
                    'i',
                    "input",
                    "Add an input folder where to look for studies. When no input folder is given, "
                    "the current one is used.");

        options.addFlag(optExtra, 'e', "extra", "Print the version of the study");
        options.addFlag(optCSV, ' ', "csv", "Print in a CSV format (semicolon)");

        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        if (options(argc, argv) == GetOpt::ReturnCode::ERROR)
            return options.errors() ? 1 : 0;

        if (optVersion)
        {
            PrintVersionToStdCout();
            return 0;
        }
    }

    MyStudyFinder finder;
    finder.extra = optExtra;
    finder.csv = optCSV;
    finder.lookup(optInput);
    finder.wait();
    return 0;
}
