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
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/study/cleaner.h>
#include <antares/study/finder/finder.h>
#include <antares/sys/policy.h>
#include <antares/utils/utils.h>

using namespace Yuni;
using namespace Antares;

static bool onProgress(uint)
{
    return true;
}

class MyStudyFinder final: public Data::StudyFinder
{
public:
    void onStudyFound(const String& folder, const Data::StudyVersion& version) override
    {
        if (version == Data::StudyVersion::unknown() || version > Data::StudyVersion::latest())
        {
            logs.info() << folder << " : version of study is too new or unknown";
            return;
        }

        if (listOnly)
        {
            logs.info() << folder << " : nothing done (currently dry mode is enabled).";
            return;
        }

        if (Data::StudyVersion::latest() == version && (!removeUselessTimeseries))
        {
            logs.info() << folder << " : is up-to-date";
        }
        else
        {
            logs.notice() << "Upgrading " << folder << "  (v" << version.toString() << " to "
                          << Data::StudyVersion::latest().toString() << ")";

            auto study = std::make_unique<Data::Study>();

            // It is important to enabled those feature to have the entire set of data
            // loaded and written
            JIT::enabled = true;
            JIT::usedFromGUI = true;

            Data::StudyLoadOptions options;
            options.loadOnlyNeeded = false;

            // disabling all useless logs
            logs.verbosityLevel = Logs::Verbosity::Warning::level;

            if (study->loadFromFolder(folder, options))
            {
                if (removeUselessTimeseries)
                {
                    study->removeTimeseriesIfTSGeneratorEnabled();
                }

                if (forceReadonly)
                {
                    study->parameters.readonly = true;
                }

                logs.info() << "Saving...";
                study->saveToFolder(folder);
            }
            else
            {
                logs.error() << "Could not load the study correctly";
                return;
            }

            // restoring logs
            logs.verbosityLevel = Logs::Verbosity::Debug::level;
        }

        if (cleanup)
        {
            auto cleaner = std::make_unique<Data::StudyCleaningInfos>(folder.c_str());
            cleaner->onProgress = &onProgress;
            if (cleaner->analyze())
            {
                cleaner->performCleanup();
            }
        }

        logs.info();
    }

public:
    bool cleanup;
    bool listOnly;
    bool removeUselessTimeseries;
    bool forceReadonly;

}; // class MyStudyFinder

// Useful for information addition in help content
// ... Extracting base name from full path
const char* ExtractFilenameOnly(const char* argv)
{
    const char* result = argv;
    while ('\0' != *argv)
    {
        if ('\\' == *argv or '/' == *argv)
        {
            result = argv;
            ++result;
        }
        ++argv;
    }
    return result;
}

// ... Getting the size of a char* (ending with '\0')
int getFilenameSize(const char* filename)
{
    int count = 0;
    while ('\0' != *filename)
    {
        filename++;
        count++;
    }
    return count;
}

// ... Building a left margin with as many spaces as the int in parameter
std::string getMargin(int size)
{
    std::string margin;
    for (int i = 0; i < size; i++)
    {
        margin += " ";
    }
    margin += " ";
    return margin;
}

int main(int argc, const char* argv[])
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("updater");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();

    String::Vector optInput;
    bool optCleanup = false;
    bool optPrintOnly = false;
    bool optRemoveUselessTimeseries = false;
    bool optForceReadonly = false;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;

        // General information
        std::string updaterComment = "\nFound studies are upgraded, unless dry run is enabled (-d "
                                     "| --dry).\n";
        updaterComment += "Following options make other options useless when used in the same run "
                          ":\n";
        updaterComment += "(-h|--help), (-v|--version) and (-d | --dry), in that order of "
                          "priority.\n";
        updaterComment += "Thus they should be used alone.\n";
        updaterComment += "If previous options are not used, other options have an action in "
                          "addition to upgrade.\n";
        updaterComment += "Note that (-i|--input) is required for upgrade.\n";
        options.addParagraph(Yuni::String() << updaterComment);

        // Adding usage information to help content
        const char* updaterFilename = ExtractFilenameOnly(argv[0]);
        std::string tabs = getMargin(getFilenameSize(updaterFilename));
        std::string updaterUsage = "Updater usage:\n";
        updaterUsage += updaterFilename;
        updaterUsage += " -i <Directory/study path> [-c|--clean]\n";
        updaterUsage += tabs + "[--remove-generated-timeseries]\n";
        updaterUsage += tabs + "[--force-readonly] [-d|--dry]\n";
        updaterUsage += tabs + "[-v|--version] [-h|--help]\n";
        updaterUsage += updaterFilename;
        updaterUsage += " -d|--dry\n";
        updaterUsage += updaterFilename;
        updaterUsage += " -v|--version\n";
        updaterUsage += updaterFilename;
        updaterUsage += " -h|--help\n";
        options.addParagraph(Yuni::String() << updaterUsage << "\n");

        // Adding a line on Antares version to help usage.
        options.addParagraph(Yuni::String()
                             << "Antares Study Updater v" << VersionToCString() << "\n");

        // Input
        options.remainingArguments(optInput);
        std::string inputComment = "Add an input folder where to look for studies or give a study "
                                   "folder to process the study.\n";
        inputComment += "This argument is mandatory.";
        options.add(optInput, 'i', "input", inputComment);

        // Options
        options.addFlag(optCleanup, 'c', "clean", "Clean all studies found");
        options.addFlag(optRemoveUselessTimeseries,
                        ' ',
                        "remove-generated-timeseries",
                        "Remove timeseries which will be regenerated by the ts-generators");
        options.addFlag(optPrintOnly,
                        'd',
                        "dry",
                        "Only Lists the study folders which would be upgraded but do nothing");
        options.addFlag(optForceReadonly,
                        ' ',
                        "force-readonly",
                        "Force read-only mode for all studies found");

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

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    if (optInput.empty())
    {
        logs.error() << "No directory to look for studies is given. Please use -i option.";
        LocalPolicy::Close();
        return 0;
    }

    MyStudyFinder updater;
    updater.listOnly = optPrintOnly;
    updater.cleanup = optCleanup;
    updater.removeUselessTimeseries = optRemoveUselessTimeseries;
    updater.forceReadonly = optForceReadonly;
    updater.lookup(optInput);
    updater.wait();

    LocalPolicy::Close();
    return 0;
}
