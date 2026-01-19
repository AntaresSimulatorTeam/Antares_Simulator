// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/core/getopt.h>

#include <antares/antares/version.h>
#include <antares/args/args_to_utf8.h>
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/study/finder/finder.h>
#include <antares/utils/utils.h>

using namespace Yuni;
using namespace Antares;

class MyStudyFinder: public Data::StudyFinder
{
public:
    MyStudyFinder():
        extra(false),
        csv(false)
    {
    }

    virtual ~MyStudyFinder()
    {
    }

    void onStudyFound(const String& folder, const Data::StudyVersion& version)
    {
        std::cout << folder;
        if (extra)
        {
            if (csv)
            {
                std::cout << ";" << version.toString();
            }
            else
            {
                std::cout << " (" << version.toString() << ')';
            }
        }
        std::cout << '\n';
    }

public:
    //! Print extra informations, such as the study version
    bool extra;
    //! Print in CSV mode
    bool csv;
};

int main(int argc, const char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("finder");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
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

        if (options(argc, argv) == GetOpt::ReturnCode::error)
        {
            return options.errors() ? 1 : 0;
        }

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
