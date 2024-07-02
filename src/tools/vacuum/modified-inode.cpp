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

#include "modified-inode.h"

#include <unordered_set>

#include <yuni/core/noncopyable.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include "antares/antares/antares.h"

#include "io.h"

using namespace Yuni;
using namespace Antares;

namespace // anonymous
{
enum
{
    maxLogEntriesRetention = 1000,
};

class UserData: private Yuni::NonCopyable<UserData>
{
public:
    UserData():
        bytesDeleted(0),
        filesDeleted(0),
        foldersDeleted(0)
    {
    }

    void syncBeforeRelease();

    void pushToLogs();

public:
    int64_t dateLimit;

    Mutex mutex;
    uint64_t bytesDeleted;
    uint64_t filesDeleted;
    uint64_t foldersDeleted;
    std::unordered_set<String> pathsToDeleteIfEmpty;

    //! delayed Entries to push into logs
    //
    // To reduce lock contention from the logs, we will keep a temporary list
    // which will be displayed later
    String::List logsEntries;

}; // class UserData

inline void UserData::pushToLogs()
{
    if (logsEntries.size() >= maxLogEntriesRetention)
    {
        foreach (auto& entry, logsEntries)
        {
            logs.info() << entry;
        }
        logsEntries.clear();
    }
}

void UserData::syncBeforeRelease()
{
    // locking, just in case (it should not be needed)
    MutexLocker locker(mutex);

    // pushing logs
    foreach (auto& entry, logsEntries)
    {
        logs.info() << entry;
    }

    if (not dry && not pathsToDeleteIfEmpty.empty())
    {
        String folder;
        uint64_t folderRemovedCount = 0;
        foreach (auto& path, pathsToDeleteIfEmpty)
        {
            if (not RemoveDirectoryIfEmpty(path))
            {
                // the folder could not be removed. It is useles to try with its parents
                continue;
            }
            ++folderRemovedCount;
            logs.info() << "deleted empty folder " << path;

            auto offset = path.rfind(IO::Separator);
            while (offset < path.size())
            {
                if (!offset)
                {
                    break;
                }
                folder.assign(path, offset);
                if (folder.empty())
                {
                    break;
                }

                if (not RemoveDirectoryIfEmpty(folder))
                {
                    // the folder could not be removed. It is useles to try with its parents
                    break;
                }

                logs.info() << "deleted empty folder " << folder;
                ++folderRemovedCount;
                if (!offset)
                {
                    break;
                }
                offset = path.rfind(IO::Separator, offset - 1);
            }
        }

        switch (folderRemovedCount)
        {
        case 0:
            logs.info() << "  :: no empty folder found";
            break;
        case 1:
            logs.info() << "  :: 1 empty folder was removed";
            break;
        default:
            logs.info() << "  :: " << folderRemovedCount << " empty folders were removed";
        }
    }
}

} // anonymous namespace

static void OnFileEvent(const String& filename,
                        const String& parent,
                        int64_t modified,
                        uint64_t size,
                        void* user)
{
    if (not(modified < ((UserData*)user)->dateLimit))
    {
        return;
    }

    if (not RemoveFile(filename, size))
    {
        logs.error() << "impossible to delete " << filename;
        return;
    }

    String text;
    DateTime::TimestampToString(text, "%a, %d %b %Y", modified, false);
    text += ") ";

    auto* userdata = (UserData*)user;

    // mutex
    MutexLocker locker(userdata->mutex);

    userdata->bytesDeleted += size;
    ++userdata->filesDeleted;
    userdata->pathsToDeleteIfEmpty.insert(parent);

    // logs
    userdata->logsEntries.push_back(String());
    auto& logentry = userdata->logsEntries.back();
    logentry.append("delete file (", 13);
    DateTime::TimestampToString(logentry, "%a, %d %b %Y", modified, false);
    logentry.append(") ", 2);
    logentry << filename;
    if (size == 0)
    {
        logentry.append(" (empty)", 8);
    }
    else
    {
        logentry.append(" (", 2);
        logentry << size;
        logentry.append(" bytes)", 7);
    }

    userdata->pushToLogs();
}

static FSWalker::Flow OnDirectoryEvent(const String& path, bool empty, void* user)
{
    if (empty)
    {
        auto* userdata = (UserData*)user;
        MutexLocker locker(userdata->mutex);
        userdata->pathsToDeleteIfEmpty.insert(path);
    }
    return FSWalker::flContinue;
}

const char* ModifiedINode::caption() const
{
    return "last modification time";
}

FSWalker::OnFileEvent ModifiedINode::fileEvent()
{
    return &OnFileEvent;
}

FSWalker::OnDirectoryEvent ModifiedINode::directoryEvent()
{
    return &OnDirectoryEvent;
}

void* ModifiedINode::userdataCreate(FSWalker::DispatchJobEvent&)
{
    auto* user = new UserData();
    user->dateLimit = pDateLimit;
    return user;
}

void ModifiedINode::userdataDestroy(void* userdata)
{
    pQueue = []([[maybe_unused]] FSWalker::IJob::Ptr job) {};

    if (userdata)
    {
        {
            std::lock_guard lock(pMutex);
            bytesDeleted += ((UserData*)userdata)->bytesDeleted;
            filesDeleted += ((UserData*)userdata)->filesDeleted;
            foldersDeleted += ((UserData*)userdata)->foldersDeleted;
        }

        // destroying the user data
        ((UserData*)userdata)->syncBeforeRelease();
        delete (UserData*)userdata;
    }
}

ModifiedINode::ModifiedINode(int64_t dateLimit):
    bytesDeleted(),
    filesDeleted(),
    foldersDeleted(),
    pDateLimit(dateLimit),
    pQueue()
{
}
