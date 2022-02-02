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
#include "../../ui/common/winmain.hxx"
#include <antares/utils.h>
#include <antares/version.h>
#include <antares/sys/policy.h>
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/process.h>
#include <yuni/io/directory/info.h>
#include <yuni/core/system/suspend.h>
#include <yuni/io/file.h>
#include <antares/memory/memory.h>
#include <antares/hostinfo.h>
#include <antares/locale.h>
#include <antares/emergency.h>
#include "../../config.h"

#include "output.h"
#include "datafile.h"
#include "studydata.h"
#include "job.h"
#include "progress.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

//! References to all outputs to aggregate
static Output::Vector AllOutputs;

/*!
** \brief Get the optimal number of jobs to run simultaneously
*/
static inline uint FindNbProcessors()
{
    // The value will be based on the number of virtual CPUs
    uint n = System::CPU::Count();
    // But we sould keep an idle cpu to avoid overload
    n = (n > 3) ? n - 1 : n;
    return (n > 4) ? 4 : n;
}

static bool DetermineOutputType(String& out, const String& original)
{
    out.clear() << original << SEP << "economy";
    if (IO::Directory::Exists(out))
        return true;

#ifndef YUNI_OS_WINDOWS
    // Before 3.8, the folder economy was named 'Economy' instead of 'economy'
    out.clear() << original << SEP << "Economy";
    if (IO::Directory::Exists(out))
        return true;
#endif

    out.clear() << original << SEP << "adequacy";
    if (IO::Directory::Exists(out))
        return true;
#ifndef YUNI_OS_WINDOWS
    out.clear() << original << SEP << "Adequacy"; // for compatibility reasons
    if (IO::Directory::Exists(out))
        return true;
#endif
    return false;
}

static bool FindOutputFolder(IO::Directory::Info& info)
{
    String original = info.directory();
    return DetermineOutputType(info.directory(), original);
}

static void ConvertVarNameToID(String& id, const String& name)
{
    id.clear();

    bool previousSpace = true;
    const String::const_utf8iterator end = name.utf8end();
    for (String::const_utf8iterator i = name.utf8begin(); i != end; ++i)
    {
        char c = String::ToLower((char)(*i));
        if (!c || (!String::IsDigit(c) && !String::IsAlpha(c)))
        {
            if (!previousSpace)
            {
                id += '-';
                previousSpace = true;
            }
        }
        else
        {
            id += c;
            previousSpace = false;
        }
    }

    id.trimRight('-');
}

static void PrepareTheWork(const String::Vector& outputs,
                           const DataFile::Vector& dataFiles,
                           const StudyData::Vector& studydata,
                           const String::Vector& columns)
{
    logs.checkpoint() << "Preparing the aggregation";
    progressBar.interval(1500 /*ms*/);
    progressBar.message("Reading the directory structure");
    progressBar.start();

    uint nbJobs = 0;
    String abspath;
    String path;
    IO::Directory::Info info;

    for (uint indx = 0; indx != (uint)outputs.size(); ++indx)
    {
        // The current study output
        IO::MakeAbsolute(abspath, outputs[indx]);
        IO::Normalize(info.directory(), abspath);
        logs.info() << "  reading " << info.directory();

        if (not info.exists())
        {
            logs.warning() << "The folder '" << info.directory() << "' does not exists";
            continue;
        }

        path.clear() << info.directory() << SEP << "info.antares-output";
        if (!IO::File::Exists(path))
        {
            logs.warning() << "Does not seem a valid study output: " << info.directory();
            continue;
        }
        path.clear() << info.directory() << SEP << "about-the-study";
        if (!IO::Directory::Exists(path))
        {
            logs.warning() << "Does not seem a valid study output: " << info.directory();
            continue;
        }

        // The new output
        auto output = std::make_shared<Output>(info.directory(), columns);
        if (!output)
            continue;

        if (not FindOutputFolder(info))
            continue;
        info.directory() << SEP << "mc-ind";
        if (not IO::Directory::Exists(info.directory()))
        {
            logs.warning() << "impossible to find data for individual years: " << info.directory();
            continue;
        }

        AnyString part;
        uint minYear = 9999999; // invalid
        uint maxYear = 0;
        Output::FolderName folderName;

        auto end = info.folder_end();
        for (auto i = info.folder_begin(); i != end; ++i)
        {
            ++Progress::Total;
            // if ((*i).size() < 5 || !(*i).startsWith("mc-i"))
            if ((*i).size() < 5)
                continue;

            folderName = *i;
            uint year;
            if (!folderName.to(year))
            {
                logs.warning() << "invalid MC year: " << i.filename();
                continue;
            }
            if (minYear > year)
                minYear = year;
            if (maxYear < year)
                maxYear = year;

            for (uint d = 0; d != dataFiles.size(); ++d)
            {
                const DataFile::Ptr& data = dataFiles[d];

                for (uint s = 0; s != studydata.size(); ++s)
                {
                    JobFileReader* job = new JobFileReader();
                    job->year = year - 1;
                    job->datafile = data;
                    job->output = output;
                    job->studydata = studydata[s];
                    job->path = i.filename();
                    // Adding the job
                    ++nbJobs;
                    queueService += job;
                }
            }
        }
        if (minYear > maxYear)
        {
            logs.warning() << info.directory() << ": invalid range for MC years";
            return;
        }
        uint nbYears = maxYear - minYear + 1;
        logs.debug() << "  " << info.directory() << " : from " << minYear << " to " << maxYear
                     << "  (total: " << nbYears << ")";

        output->minYear = minYear;
        output->maxYear = maxYear;
        output->nbYears = nbYears;

        // Adding the output
        AllOutputs.push_back(output);

        // Allocating the resources for the output
        logs.info() << "  allocating resources for " << info.directory();
        ResultsForAllStudyItems& results = output->results;
        for (uint s = 0; s != studydata.size(); ++s)
        {
            const StudyData::Ptr& sdata = studydata[s];
            ResultsForAllDataLevels& alldatalevels = results[sdata->name];

            for (uint d = 0; d != dataFiles.size(); ++d)
            {
                const DataFile::Ptr& data = dataFiles[d];
                ResultsForAllTimeLevels& alltimelevels = alldatalevels[data->dataLevel];
                ResultsAllVars& allvars = alltimelevels[data->timeLevel];
                allvars.resize(columns.size());
                for (uint v = 0; v != allvars.size(); ++v)
                {
                    ResultMatrix& mtrx = allvars[v];
                    mtrx.resize(maxYear);
                }
            }
        }
    } // each output

    logs.info() << "  added " << nbJobs << " jobs for " << info.directory();
    Progress::Total = nbJobs;
}

static void ReadCommandLineOptions(int argc, char** argv)
{
    String::Vector optOutputs;
    uint optJobs = FindNbProcessors();
    String optSwap;
    String::Vector optTimes;
    String::Vector optColumns;
    String::Vector optDatum;
    String::Vector optAreas;
    String::Vector optLinks;
    bool optForce = false;

    // Command Line options
    {
        // Parser
        GetOpt::Parser options;
        //
        options.addParagraph(Yuni::String()
                             << "Antares output aggregator " << VersionToCString() << "\n\nData");
        // Input
        options.remainingArguments(optOutputs);
        // Output
        options.add(optOutputs, 'i', "input", "one or more study outputs");
        options.add(optAreas, 'a', "area", "add an area");
        options.add(optLinks, 'l', "link", "add a link (format: '<area>,<area>')");
        options.add(optDatum, 'd', "data", "add a data type ('values', 'details', 'id')");
        options.add(optTimes,
                    't',
                    "time",
                    "add a time interval ('hourly', 'daily', 'weekly', 'monthly', 'annual')");
        options.add(optColumns, 'c', "column", "add a column to consider during the aggregation");
        options.addFlag(optForce, ' ', "force", "ignore warnings");

        options.addParagraph("\nResources");

        options.add(optJobs,
                    'j',
                    "jobs",
                    String() << "The number of jobs to run simultaneously (default: " << optJobs
                             << ", 4 should be a maxmimum due to i/o performance considerations)");
        // --swap
        options.add(optSwap,
                    ' ',
                    "swap-folder",
#ifdef ANTARES_SWAP_SUPPORT
                    String("Folder where the swap files will be written. (default: '")
                      << Antares::memory.cacheFolder() << "')"
#else
                    "Folder where the swap files will be written. This option has no effect"
#endif
        );

        options.addParagraph("\nMisc.");

        Yuni::String optPID;
        options.add(optPID, 'p', "pid", "Specify the file where to write the process ID");

        // Version
        bool optVersion = false;
        options.addFlag(optVersion, 'v', "version", "Print the version and exit");

        if (!options(argc, argv))
        {
            LocalPolicy::Close();
            AntaresSolverEmergencyShutdown(options.errors() ? 1 : 0);
        }

        if (optVersion)
        {
            PrintVersionToStdCout();
            LocalPolicy::Close();
            exit(0);
        }
        if (optOutputs.empty())
        {
            logs.error() << "Please provide at least one study output";
            LocalPolicy::Close();
            AntaresSolverEmergencyShutdown(1);
        }
        if (optAreas.empty() && optLinks.empty())
        {
            logs.error() << "Please provide at least one area or one link for the aggregation";
            LocalPolicy::Close();
            AntaresSolverEmergencyShutdown(1);
        }
        if (optColumns.empty())
        {
            logs.error() << "Please provide at least one column to find out";
            LocalPolicy::Close();
            AntaresSolverEmergencyShutdown(1);
        }

        // PID
        if (!optPID.empty())
        {
            IO::File::Stream pidfile(optPID, IO::OpenMode::write | IO::OpenMode::truncate);
            if (pidfile.opened())
                pidfile << Yuni::ProcessID();
        }

        if (optJobs < 1)
            optJobs = 1;
        queueService.maximumThreadCount(optJobs);
    }

#ifdef ANTARES_SWAP_SUPPORT
    // Changing the swap folder
    if (not optSwap.empty())
    {
        logs.info() << "  memory pool: scratch folder:" << optSwap;
        Antares::memory.cacheFolder(optSwap);
    }
    else
        logs.info() << "  memory pool: scratch folder:" << Antares::memory.cacheFolder();
#endif

    // Starting !
    {
        logs.checkpoint() << "Antares Study Output aggregator v" << ANTARES_VERSION_PUB_STR;
        WriteHostInfoIntoLogs();
        logs.info();
    }

    // Building the study data
    StudyData::Vector studydata;
    {
        String newname;
        uint index = 0;
        ;
        for (uint i = 0; i != optAreas.size(); ++i)
        {
            String& areaname = optAreas[i];
            areaname.trim(" \t\"'");
            if (!areaname)
                continue;

            if (System::windows)
                newname.clear() << "areas\\" << areaname;
            else
                newname.clear() << "areas/" << areaname;

            logs.info() << "registered " << newname;
            studydata.push_back(std::make_shared<StudyData>(newname, index));
            index++;
        }
        {
            String::Vector split;
            for (uint i = 0; i != optLinks.size(); ++i)
            {
                String& linkfullname = optLinks[i];
                linkfullname.trim(" \t\"'");
                if (!linkfullname)
                    continue;
                linkfullname.split(split, ",");
                if (split.size() != 2)
                {
                    logs.error() << "invalid link name: " << linkfullname;
                    continue;
                }
                newname.clear() << "links" << SEP;
                newname << split[0] << " - " << split[1];
                logs.info() << "registered " << newname;
                studydata.push_back(std::make_shared<StudyData>(newname, index));
                index++;
            }
        }
    }

    // Building the data files
    DataFile::Vector dataFiles;
    uint dataIndex = 0;
    for (uint i = 0; i != optTimes.size(); ++i)
    {
        for (uint j = 0; j != optDatum.size(); ++j)
        {
            dataFiles.push_back(std::make_shared<DataFile>(optDatum[j], optTimes[i], dataIndex));
            dataIndex++;
        }
    }
    optDatum.clear();
    optTimes.clear();
    if (dataFiles.empty())
        return;

    // lower case for all columns
    for (uint i = 0; i != optColumns.size(); ++i)
        optColumns[i].toLower();

    PrepareTheWork(optOutputs, dataFiles, studydata, optColumns);
}

static bool WriteAggregates()
{
    // Flush the whole content to disk
    memory.flushAll();

    const Output::Vector::iterator end = AllOutputs.end();
    for (Output::Vector::iterator i = AllOutputs.begin(); i != end; ++i)
    {
        // The current output
        Output::Ptr output = *i;
        {
            if (!output->canContinue())
                continue;
            const uint columnCount = (uint)output->columns.size();
            if (!columnCount)
                continue;
            if (columnCount > 1)
                logs.info() << "  checking " << output->path << "  (" << columnCount
                            << " variables)";
            else
                logs.info() << "  checking " << output->path << "  (1 variable)";
        }

        String mcvarfolder;
        if (!DetermineOutputType(mcvarfolder, output->path))
        {
            logs.error() << "impossible to find output folder in " << output->path;
            continue;
        }
        mcvarfolder << SEP << "mc-var";

        ResultsForAllStudyItems& results = output->results;
        const ResultsForAllStudyItems::iterator rend = results.end();
        ResultsForAllStudyItems::iterator r = results.begin();
        for (; r != rend; ++r)
        {
            const StudyData::ShortString512& studyItemName = r->first;
            ResultsForAllDataLevels& alldatalevels = r->second;
            if (alldatalevels.empty())
                continue;

            const ResultsForAllDataLevels::iterator tend = alldatalevels.end();
            ResultsForAllDataLevels::iterator t = alldatalevels.begin();
            for (; t != tend; ++t)
            {
                const DataFile::ShortString& dataLevelName = t->first;
                ResultsForAllTimeLevels& alltimelevels = t->second;
                if (alltimelevels.empty())
                    continue;

                const ResultsForAllTimeLevels::iterator avend = alltimelevels.end();
                ResultsForAllTimeLevels::iterator av = alltimelevels.begin();
                for (; av != avend; ++av)
                {
                    const DataFile::ShortString& timeLevelName = av->first;
                    ResultsAllVars& allvars = av->second;
                    if (allvars.empty())
                        continue;

                    if (output->columns.size() != allvars.size())
                    {
                        logs.error() << "array size does not match";
                        continue;
                    }

                    for (uint v = 0; v != allvars.size(); ++v)
                    {
                        progressBar.message(String()
                                            << "Writing results for '" << studyItemName << "' ("
                                            << dataLevelName << ", " << timeLevelName
                                            << ") : " << output->columns[v]);
                        Progress::Current = 0;

                        ResultMatrix& matrix = allvars[v];
                        if (!matrix.width)
                            continue;

                        uint requiredHeight = 0;
                        for (uint y = 0; y != matrix.width; ++y)
                        {
                            const CellColumnData& column = matrix.columns[y];
                            if (column.height)
                            {
                                if (!requiredHeight)
                                    requiredHeight = column.height;
                                else
                                {
                                    if (column.height != requiredHeight)
                                    {
                                        logs.error()
                                          << "All columns must have the same number of rows ("
                                          << column.height << " found, " << requiredHeight
                                          << " expected): " << output->path;
                                        output->incrementError();
                                        break;
                                    }
                                }
                            }
                        }

                        matrix.heightAfterAggregation = requiredHeight;

                        // DATA filename
                        String path;
                        path << mcvarfolder << SEP << studyItemName;

                        if (!IO::Directory::Create(path))
                        {
                            logs.error() << "impossible to create the directory " << path;
                            continue;
                        }
                        path << SEP;
                        path << dataLevelName << '-' << timeLevelName << '-';
                        String varNameID;
                        ConvertVarNameToID(varNameID, output->columns[v]);
                        path << varNameID << ".txt";

                        // Writing DATA
                        if (!requiredHeight)
                        {
                            logs.info()
                              << " No data for the variable " << studyItemName << '/'
                              << dataLevelName << '/' << timeLevelName << '/' << output->columns[v];
                            path << ".nodata";
                            if (!IO::File::CreateEmptyFile(path))
                                logs.error() << "I/O error: impossible to write " << path;
                        }
                        else
                        {
                            logs.info() << "    writing " << path;
                            logs.debug() << "    (" << matrix.width << 'x' << requiredHeight << ")";
                            if (!matrix.saveToCSVFile(path))
                                logs.error() << "impossible to write " << path;
                            // empty log entry
                            logs.info();
                        }
                    }
                }
            }
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    // locale
    InitializeDefaultLocale();

    // logs
    logs.applicationName("yby-aggregator");

    // Swap memory
    if (not memory.initialize())
        return EXIT_FAILURE;

    argv = AntaresGetUTF8Arguments(argc, argv);

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    ReadCommandLineOptions(argc, argv);

    if (Progress::Total > 0)
    {
        // Run all jobs
        progressBar.stop();
        logs.info();
        logs.checkpoint() << "Performing the aggregation";
        logs.info() << "Running...";
        logs.info() << "  using " << queueService.maximumThreadCount() << " worker(s)";
        progressBar.state = Progress::stJobs;
        queueService.start();
        progressBar.start();

        // wait for all queues
        queueService.wait(Yuni::qseIdle);
        while (JobFileReader::RemainJobsToExecute())
            SuspendMilliSeconds(170);
        progressBar.wait();

        if (progressBar.completed())
        {
            logs.info();
            logs.checkpoint() << "Writing the results...";
            progressBar.state = Progress::stWrite;
            progressBar.start();
            WriteAggregates();
            progressBar.stop();

            logs.info();
            logs.checkpoint() << "Aggregation";
            logs.info() << "The aggregation is complete";
            logs.debug() << "done.";
        }

        // early Release !
        AllOutputs.clear();
        // Removing all unused spwa files
        Antares::memory.removeAllUnusedSwapFiles();
    }
    else
    {
        // making sure that the progress bar is stopped
        progressBar.stop();
    }

    // Close the local policy
    LocalPolicy::Close();
    return 0;
}
