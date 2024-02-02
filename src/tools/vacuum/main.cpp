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
#include <yuni/io/directory/info.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/job/job.h>
#include <yuni/job/queue/service.h>
#include <yuni/job/queue/q-event.h>
#include <antares/antares.h>
#include <fswalker/fswalker.h>
#include <antares/logs/logs.h>
#include <antares/args/args_to_utf8.h>
#include <antares/version.h>
#include <antares/locale.h>
#include "modified-inode.h"
#include "antares-study.h"
#include "io.h"

using namespace Yuni;
using namespace Antares;

static inline double Round2ndPlace(double v)
{
    v *= 100.;
    v = Math::Trunc(v);
    return v / 100.;
}

static void NotifyDuration(int64_t duration)
{
    switch (duration)
    {
    case 0:
        logs.info() << "Operation completed in 0 second";
        break;
    case 1:
        logs.info() << "Operation completed in 1 second";
        break;
    default:
        logs.info() << "Operation completed in " << duration << " seconds";
    }
}

static void NotifyBytesDeleted(uint64_t bytes)
{
    String message;

    if (0 == bytes)
    {
        message << "0 byte freed";
    }
    else if (bytes < 1024)
    {
        message << bytes << " bytes freed";
    }
    else if (bytes < 1024 * 1024)
    {
        double k = bytes / 1024.;
        message << Round2ndPlace(k);
        message.trimRight("0");
        message << " KiB freed";
    }
    else if (bytes < 1024. * 1024. * 1024.)
    {
        double k = bytes / (1024. * 1024.);
        message << Round2ndPlace(k);
        message.trimRight("0");
        message << " MiB freed";
    }
    else if (bytes < 1024. * 1024. * 1024. * 1024.)
    {
        double k = bytes / (1024. * 1024. * 1024.);
        message << Round2ndPlace(k);
        message.trimRight("0");
        message << " GiB freed";
    }

    else
    {
        double k = bytes / (1024. * 1024. * 1024. * 1024.);
        message << Round2ndPlace(k);
        message.trimRight("0");
        message << " PiB freed";
    }
    logs.info() << message;
}

static void NotifyFilesDeleted(uint64_t count)
{
    switch (count)
    {
    case 0:
        logs.info() << "no file deleted";
        break;
    case 1:
        logs.info() << "1 file was deleted";
        break;
    default:
        logs.info() << count << " files were deleted";
    }
}

static void NotifyFoldersDeleted(uint64_t count)
{
    switch (count)
    {
    case 0:
        logs.info() << "no folder deleted";
        break;
    case 1:
        logs.info() << "1 folder was deleted";
        break;
    default:
        logs.info() << count << " folders were deleted";
    }
}

static void NotifyStatistics(const String& logprefix,
                             const FSWalker::Statistics& stats,
                             uint64_t duration)
{
    String message;

    message << logprefix << "  :: ";
    switch (stats.fileCount)
    {
    case 0:
        message << "0 file";
        break;
    case 1:
        message << "1 file";
        break;
    default:
        message << stats.fileCount << " files";
    }
    message << " and ";
    switch (stats.folderCount)
    {
    case 0:
        message << "0 folder";
        break;
    case 1:
        message << "1 folder";
        break;
    default:
        message << stats.folderCount << " folders";
    }
    message << " analyzed in ";
    switch (duration)
    {
    case 0:
        message << "0 second";
        break;
    case 1:
        message << "1 second";
        break;
    default:
        message << duration << " seconds";
    }
    logs.info() << message;
}

namespace // anonymous
{
class DirectoryCleanerJob : public Job::IJob
{
public:
    DirectoryCleanerJob(const String& directory, int64_t dateLimit) :
     directory(directory), dateLimit(dateLimit)
    {
    }
    virtual ~DirectoryCleanerJob()
    {
    }

public:
    //! The directory to clean
    String directory;
    //! The lower date limit
    int64_t dateLimit;

protected:
    virtual void onExecute()
    {
        String prefix;
        prefix << '[' << directory << "] ";

        FSWalker::Walker walker(prefix);
        walker.add(std::make_shared<AntaresStudy>(dateLimit));
        walker.add(std::make_shared<ModifiedINode>(dateLimit));

        // start
        walker.directory(directory);

        auto starttime = DateTime::Now();

        walker.run();

        auto duration = DateTime::Now() - starttime;

        FSWalker::Statistics stats;
        walker.retrieveStatistics(stats);

        if (stats.aborted)
        {
            logs.error() << "operation aborted.";
            // return false;
        }

        NotifyStatistics(prefix, stats, duration);
        // return not stats.aborted;
    }

}; // class DirectoryCleanerJob

} // anonymous namespace

int main(int argc, char** argv)
{
    // locale
    InitializeDefaultLocale();

    logs.applicationName("vacuum");
    IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
    std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
    String::Vector optInput;
    String::Vector optEachFolderIn;
    uint optMaxDays = 90; // days
    dry = true;

    // Command Line options
    {
        String optLogs;

        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(Yuni::String() << "Antares Vacuum v" << VersionToCString() << "\n");
        // Input
        options.remainingArguments(optInput);
        // Output
        options.add(optInput, 'i', "input", "Add an folder to clean.");
        options.add(optEachFolderIn,
                    ' ',
                    "each-folder-in",
                    "Add all available folders to clean from a given directory");
        options.add(optMaxDays,
                    'm',
                    "max-days",
                    String("The maximum number of days without modification (default: ")
                      << optMaxDays << " days)");

        bool optDelete = false;
        options.addFlag(
          optDelete,
          'd',
          "delete",
          "Destroy all files and folders which are considered as too old (disabled by default)");

        options.add(optLogs, 'l', "log", "log file");

        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        if (options(argc, argv) == GetOpt::ReturnCode::error)
            return options.errors() ? 42 : 0;

        if (optVersion)
        {
            PrintVersionToStdCout();
            return 0;
        }

        if (not optLogs.empty())
            logs.logfile(optLogs);

        if (!optMaxDays)
        {
            logs.error() << "The parameter 'max-days' must not be null";
            return EXIT_FAILURE;
        }

        if (optDelete)
            dry = false;
    }

    logs.notice() << "Antares Vacuum v" << VersionToCString();

    auto starttime = DateTime::Now();
    auto dateLimit = starttime - optMaxDays * 24 * 3600;
    // display the date limit
    {
        String text;
        DateTime::TimestampToString(text, "%a, %d %b %Y", dateLimit);
        logs.info() << "date retention limit : " << text << "  (timestamp: " << dateLimit << ')';
    }

    if (dry)
        logs.info() << "dry run mode (nothing will de deleted)";

    foreach (auto& input, optInput)
        inputFolders.insert(input);

    foreach (auto& path, optEachFolderIn)
    {
        IO::Directory::Info info(path);
        for (auto i = info.folder_begin(); i.valid(); ++i)
            inputFolders.insert(i.filename());
    }
    optEachFolderIn.clear();

    foreach (auto& input, inputFolders)
        logs.info() << "added directory " << input;
    else logs.warning() << "no directory to analyze";

    // GO !
    {
        logs.info(); // for beauty
        logs.checkpoint() << "Cleaning up...";

        Job::QueueService queueservice;
        queueservice.maximumThreadCount(4);

        foreach (auto& input, inputFolders)
            queueservice += new DirectoryCleanerJob(input, dateLimit);

        queueservice.start();
        queueservice.wait(Yuni::qseIdle);
        queueservice.stop();
    }

    logs.info();
    logs.info();
    NotifyDuration(DateTime::Now() - starttime);
    NotifyBytesDeleted(IOBytesDeleted);
    NotifyFilesDeleted((uint64_t)IOFilesDeleted);
    NotifyFoldersDeleted((uint64_t)IOFoldersDeleted);

    logs.info();
    logs.info() << "exiting gracefully";
    return EXIT_SUCCESS;
}
