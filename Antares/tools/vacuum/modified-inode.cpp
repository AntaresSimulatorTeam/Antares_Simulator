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
#include "modified-inode.h"
#include <yuni/datetime/timestamp.h>
#include <yuni/io/file.h>
#include <antares/logs.h>
#include <unordered_set>
#include <yuni/core/noncopyable.h>
#include "io.h"

using namespace Yuni;
using namespace Antares;

namespace // anonymous
{
enum
{
    maxLogEntriesRetention = 1000,
};

class UserData : private Yuni::NonCopyable<UserData>
{
public:
    UserData() : bytesDeleted(0), filesDeleted(0), foldersDeleted(0)
    {
    }

    void syncBeforeRelease();

    void pushToLogs();

public:
    sint64 dateLimit;

    Mutex mutex;
    uint64 bytesDeleted;
    uint64 filesDeleted;
    uint64 foldersDeleted;
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
            logs.info() << entry;
        logsEntries.clear();
    }
}

void UserData::syncBeforeRelease()
{
    // locking, just in case (it should not be needed)
    MutexLocker locker(mutex);

    // pushing logs
    foreach (auto& entry, logsEntries)
        logs.info() << entry;

    if (not dry && not pathsToDeleteIfEmpty.empty())
    {
        String folder;
        uint64 folderRemovedCount = 0;
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
                    break;
                folder.assign(path, offset);
                if (folder.empty())
                    break;

                if (not RemoveDirectoryIfEmpty(folder))
                {
                    // the folder could not be removed. It is useles to try with its parents
                    break;
                }

                logs.info() << "deleted empty folder " << folder;
                ++folderRemovedCount;
                if (!offset)
                    break;
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
                        sint64 modified,
                        uint64 size,
                        void* user)
{
    if (not(modified < ((UserData*)user)->dateLimit))
        return;

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
        logentry.append(" (empty)", 8);
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
    pQueue.unbind();

    if (userdata)
    {
        pMutex.lock();
        bytesDeleted += ((UserData*)userdata)->bytesDeleted;
        filesDeleted += ((UserData*)userdata)->filesDeleted;
        foldersDeleted += ((UserData*)userdata)->foldersDeleted;
        pMutex.unlock();

        // destroying the user data
        ((UserData*)userdata)->syncBeforeRelease();
        delete (UserData*)userdata;
    }
}

ModifiedINode::ModifiedINode(yint64 dateLimit) :
 bytesDeleted(), filesDeleted(), foldersDeleted(), pDateLimit(dateLimit), pQueue()
{
}
