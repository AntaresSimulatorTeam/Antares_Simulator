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

#include "antares-study.h"
#include <antares/study/study.h>
#include <antares/logs.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/io/directory/info.h>
#include "io.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

enum
{
    traces = 0
};

class UserData : private Yuni::NonCopyable<UserData>
{
public:
    UserData() : bytesDeleted(0), filesDeleted(0), foldersDeleted(0)
    {
    }

public:
    sint64 dateLimit;
    FSWalker::DispatchJobEvent queue;

    Mutex mutex;
    uint64 bytesDeleted;
    uint64 filesDeleted;
    uint64 foldersDeleted;
};

class AntaresStudyAnalyzerJob : public FSWalker::IJob
{
public:
    AntaresStudyAnalyzerJob(const String& folder) : folder(folder), userdata(nullptr)
    {
    }
    virtual ~AntaresStudyAnalyzerJob()
    {
    }

public:
    String folder;
    sint64 dateLimit;
    UserData* userdata;

protected:
    virtual void onExecute();
    bool shouldBeDestroyed() const;

}; // class AntaresStudyAnalyzerJob

bool AntaresStudyAnalyzerJob::shouldBeDestroyed() const
{
    String filename;
    String text;
    filename << folder << SEP << "study.antares";

    // We will check the date of the last save, if recent enough,
    // there is no need to perform additional tests on the logs
    // or on the outputs
    Data::StudyHeader header;
    if (not header.loadFromFile(filename, false))
    {
        logs.info() << " delete study " << folder << " [invalid header]";
        return true;
    }

    if (traces)
        DateTime::TimestampToString(text, "%a, %d %b %Y", header.dateLastSave);
    if (header.dateLastSave > dateLimit)
    {
        // the study has been saved recently
        if (traces)
            logs.info() << "    - study status " << folder << ": modified recently " << text;
        return false;
    }
    if (traces)
        logs.info() << "    - study status " << folder << ": last save too old " << text;

    // The study has been modified a long time ago, we have to check
    // if it has been used recently. First of all, we will check the logs
    IO::Directory::Info info;

    info.directory().clear() << folder << SEP << "logs";
    for (auto i = info.file_begin(); i != info.file_end(); ++i)
    {
        // DateTime::TimestampToString(text, "%a, %d %b %Y", i.modified());
        // logs.info() << "  :: >> " << i.filename() << " : " << text;
        if (i.modified() > dateLimit)
        {
            // There is at least one recent logfile. aborting
            if (traces)
                logs.info() << "    - study status " << folder
                            << ": at least one recent simulation";
            return false;
        }
    }

    // The study really seems too old. However, we should perform a final
    // check : the outputs
    info.directory().clear() << folder << SEP << "output";
    for (auto i = info.folder_begin(); i != info.folder_end(); ++i)
    {
        // DateTime::TimestampToString(text, "%a, %d %b %Y", i.modified());
        // logs.info() << "  :: >> " << i.filename() << " : " << text;
        if (i.modified() > dateLimit)
        {
            // There is at least one recent logfile. aborting
            if (traces)
                logs.info() << "    - study status " << folder
                            << ": at least one recent simulation";
            return false;
        }
    }

    // sorry for the study...
    return true;
}

class FolderRemover : public FSWalker::IJob
{
public:
    typedef Yuni::Job::IJob::Ptr::Promote<FolderRemover>::Ptr Ptr;

public:
    FolderRemover(const String& folder) : folder(folder), userdata(nullptr), timeout(100)
    {
    }
    virtual ~FolderRemover()
    {
    }

public:
    //! Folder to remove
    String folder;
    //! userdata
    UserData* userdata;
    //! Dependencies
    std::vector<Ptr> dependencies;
    //! Timeout to wait before requeuing
    uint timeout;

protected:
    virtual void onExecute()
    {
        // Try to clean the folder
        perform();
        // We must decrement the counter by ourselves to avoid dead-lock
        decrementCounter();
    }

    void perform()
    {
        if (!userdata)
        {
            logs.error() << "internal error: userdata is null";
            return;
        }

        // Checking for other jobs dependencies, if any
        foreach (auto& job, dependencies)
        {
            if (not job->finished())
            {
                if (suspend(timeout))
                    return;
                // it seems that we should wait a bit more longer to let
                // other thread work
                timeout = 400;
                userdata->queue(new FolderRemover(*this));
                return;
            }
        }

        // ok delete !
        if (traces)
            logs.info() << "  :: starting removal of " << folder;

        String::Vector foldersToDelete;
        uint64 bytesDeleted = 0;
        uint64 filesDeleted = 0;

        IO::Directory::Info info(folder);
        auto end = info.recursive_end();
        for (auto i = info.recursive_begin(); i != end; ++i)
        {
            if (i.isFile())
            {
                // immediate removal
                bytesDeleted += i.size();
                ++filesDeleted;
                if (not RemoveFile(i.filename(), i.size()))
                    logs.error() << "impossible to delete " << i.filename();
            }
            else
            {
                // delayed removal
                foldersToDelete.push_back(i.filename());
            }
        }

        // Statistics update
        MutexLocker locker(userdata->mutex);
        userdata->bytesDeleted += bytesDeleted;
        userdata->filesDeleted += filesDeleted;
        userdata->foldersDeleted += foldersToDelete.size();

        for (auto i = foldersToDelete.rbegin(); i != foldersToDelete.rend(); ++i)
        {
            if (not RemoveDirectoryIfEmpty(*i))
                logs.error() << "impossible to delete directory " << *i;
        }

        // deletion of the root folder
        if (not RemoveDirectoryIfEmpty(folder))
            logs.error() << "impossible to delete directory " << folder;
    }

}; // class AntaresStudyAnalyzerJob

void AntaresStudyAnalyzerJob::onExecute()
{
    if (traces)
        logs.info() << "  :: analyzing study " << folder;
    if (shouldBeDestroyed())
    {
        logs.info() << "study delete " << folder;
        String path = folder;

        auto* folderRemovalJob = new FolderRemover(path);
        folderRemovalJob->userdata = userdata;

        // removing each output first
        IO::Directory::Info info;
        IO::Directory::Info subinfo;

        // OUTPUT folders
        info.directory().clear() << path << SEP << "output";
        for (auto i = info.folder_begin(); i != info.folder_end(); ++i)
        {
            subinfo.directory().clear() << i.filename() << SEP << "economy" << SEP << "mc-ind";
            for (auto j = subinfo.folder_begin(); j != subinfo.folder_end(); ++j)
            {
                auto* job = new FolderRemover(j.filename());
                folderRemovalJob->dependencies.push_back(job);
                job->userdata = userdata;
            }

            subinfo.directory().clear() << i.filename() << SEP << "economy" << SEP << "mc-all";
            for (auto j = subinfo.folder_begin(); j != subinfo.folder_end(); ++j)
            {
                auto* job = new FolderRemover(j.filename());
                folderRemovalJob->dependencies.push_back(job);
                job->userdata = userdata;
            }

            subinfo.directory().clear() << i.filename() << SEP << "economy" << SEP << "mc-var";
            for (auto j = subinfo.folder_begin(); j != subinfo.folder_end(); ++j)
            {
                auto* job = new FolderRemover(j.filename());
                folderRemovalJob->dependencies.push_back(job);
                job->userdata = userdata;
            }
        }

        // INPUT folders
        info.directory().clear() << path << SEP << "input";
        for (auto i = info.folder_begin(); i != info.folder_end(); ++i)
        {
            auto* job = new FolderRemover(i.filename());
            folderRemovalJob->dependencies.push_back(job);
            job->userdata = userdata;
        }

        // USER folders
        info.directory().clear() << path << SEP << "user";
        for (auto i = info.folder_begin(); i != info.folder_end(); ++i)
        {
            auto* job = new FolderRemover(i.filename());
            folderRemovalJob->dependencies.push_back(job);
            job->userdata = userdata;
        }

        // dispatch all jobs
        foreach (auto& job, folderRemovalJob->dependencies)
            userdata->queue(job);

        // put in queue the last master job
        if (!dry)
            userdata->queue(folderRemovalJob);
    }
}

static FSWalker::Flow OnDirectoryEvent(const String& path, bool empty, void* user)
{
    if (not empty)
    {
        if (Antares::Data::Study::IsRootStudy(path))
        {
            auto* job = new AntaresStudyAnalyzerJob(path);
            job->userdata = (UserData*)user;
            job->dateLimit = ((UserData*)user)->dateLimit;
            ((UserData*)user)->queue(job);
            return FSWalker::flSkip;
        }
    }
    return FSWalker::flContinue;
}

const char* AntaresStudy::caption() const
{
    return "antares study";
}

AntaresStudy::AntaresStudy(yint64 dateLimit) :
 bytesDeleted(), filesDeleted(), foldersDeleted(), pDateLimit(dateLimit)
{
}

FSWalker::OnDirectoryEvent AntaresStudy::directoryEvent()
{
    return &OnDirectoryEvent;
}

void* AntaresStudy::userdataCreate(FSWalker::DispatchJobEvent& queue)
{
    auto* user = new UserData();
    user->queue = queue;
    user->dateLimit = pDateLimit;
    return user;
}

void AntaresStudy::userdataDestroy(void* userdata)
{
    pMutex.lock();
    bytesDeleted += ((UserData*)userdata)->bytesDeleted;
    filesDeleted += ((UserData*)userdata)->filesDeleted;
    foldersDeleted += ((UserData*)userdata)->foldersDeleted;
    pMutex.unlock();
    delete (UserData*)userdata;
}
