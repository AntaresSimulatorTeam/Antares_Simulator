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

#include "fswalker.h"
#include <antares/antares.h>
#include <stack>
#include <list>
#include <yuni/io/directory/info.h>
#include <yuni/core/noncopyable.h>
#include <yuni/job/job.h>
#include <yuni/job/queue/service.h>
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/suspend.h>
#include <algorithm>
#include "registry.inc.hxx"
#include "filejob.inc.hxx"
#include <antares/logs.h>

using namespace Yuni;
using namespace Antares;

namespace FSWalker
{
//! Mutex for the global queueservice
static Yuni::Mutex gsMutex;
static bool queueserviceInitialized = false;
//! Global Queue service
static Job::QueueService queueservice;

class WalkerThread final
{
public:
    enum
    {
        //! Maximum number of ticks between each thread state check
        maxTicks = 7000,
        //! Maximum number of files per job
        maxFilesPerJob = 100,
        //! Maximum number of jobs in queues, to avoid override
        maxJobsInQueue = 2048,
        //! The upper limit of jobs in queue, after the limit has been reached
        // This limit is lower than maxJobsInQueue to not restart too early
        maxJobsInQueueReducedPressure = 1024,
    };

public:
    WalkerThread(Statistics& stats);
    ~WalkerThread();

    void dispatchJob(IJob::Ptr job);

    void run();

public:
    //! Directories to walk through
    String directory;
    //! Statistics
    Statistics statistics;
    //! Events
    EventsRegistry events;
    //! Log prefix
    String logPrefix;

protected:
    bool onDirectoryEnter(const String& path);
    void onDirectoryLeave();

    void releaseResources();

    void walk(const String& path);

    bool triggerFileEvent(const String& filename,
                          const String& parent,
                          sint64 modified,
                          uint64 size);

    void waitForAllJobs();

private:
    class DirectoryContext final : private Yuni::NonCopyable<DirectoryContext>
    {
    public:
        typedef std::stack<DirectoryContext*> Stack;

        explicit DirectoryContext(const String& path) : info(path), cursor(info.begin())
        {
            // Currently bug
            // info.directory() = path;
            // cursor = info.begin();
        }

        IO::Directory::Info info;
        IO::Directory::Info::iterator cursor;
    };
    //! Directory stack
    std::list<String> pStack;
    //! Context
    DirectoryContext::Stack pContext;

    //! Current job in preparation for file inspection
    FileJob* pFileJob;

    //! Reference to the original statistics
    Statistics& pOriginalStatistics;

    bool pShouldStop;

    InternalJobCounter pJobCounter;

}; // class WalkerThread

WalkerThread::WalkerThread(Statistics& stats) : pFileJob(nullptr), pOriginalStatistics(stats)
{
    pJobCounter = new Atomic::Int<32>();
    pShouldStop = false;

    MutexLocker locker(gsMutex);
    if (not queueserviceInitialized)
    {
        queueserviceInitialized = true;
        // Trying to establish the best number of threads, only if
        // jobCount == 0
        uint core = System::CPU::Count();
        if (core < 3)
            core = 3;
        else if (core >= 16)
            core -= 4;
        else if (core > 4)
            core -= 1;
        logs.info() << "queueservice: " << core << " workers";
        queueservice.maximumThreadCount(core);

        // start the service
        queueservice.start();
    }
}

WalkerThread::~WalkerThread()
{
    // should not be needed. just in case, to avoid memory leak
    releaseResources();
}

void WalkerThread::releaseResources()
{
    // using the C++ idiom Clear-and-minimize to really deallocate the memory.
    if (not pStack.empty())
    {
        std::list<String> empty;
        pStack.swap(empty);
    }

    while (not pContext.empty())
    {
        delete pContext.top();
        pContext.pop();
    }

    delete pFileJob;
    pFileJob = nullptr;
}

bool WalkerThread::onDirectoryEnter(const String& path)
{
    // Context for this directory
    auto* context = new DirectoryContext(path);
    // Is the folder empty ?
    bool empty = not context->cursor.valid();

    // Checking if we have to analyze this folder
    for (uint i = 0; i != events.directory.enter.size(); ++i)
    {
        switch ((*events.directory.enter[i])(path, empty, events.directory.userdata[i]))
        {
        case flContinue:
            break;
        case flSkip:
        {
            delete context;
            return false;
        }
        case flAbort:
        {
            // Notifying the thread that we should stop as
            // soon as possible
            pShouldStop = true;
            delete context;
            return false;
        }
        }
    }

    // statistics
    ++statistics.folderCount;

    pStack.push_back(path);
    pContext.push(context);
    return true;
}

inline void WalkerThread::onDirectoryLeave()
{
    pStack.pop_back();
    delete pContext.top();
    pContext.pop();
}

void WalkerThread::run()
{
    // We will have to deal with files, preparing the future job
    if (not events.file.access.empty())
    {
        pFileJob = new FileJob(events);
        pFileJob->files.reserve(maxFilesPerJob);
        pFileJob->filessize.reserve(maxFilesPerJob);
        pFileJob->modified.reserve(maxFilesPerJob);
    }

    // Walk through all folders
    walk(directory);
    // Finalize all extensions
    events.finalize();
    // stopping the queue service
    waitForAllJobs();
    // Deallocate internal ressources
    releaseResources();
    // Update statistics
    pOriginalStatistics = statistics;
}

bool WalkerThread::triggerFileEvent(const String& filename,
                                    const String& parent,
                                    sint64 modified,
                                    uint64 size)
{
    assert(pFileJob != NULL);

    // Statistics
    ++statistics.fileCount;

    pFileJob->files.push_back(filename);
    pFileJob->parents.push_back(parent);
    pFileJob->filessize.push_back(size);
    pFileJob->modified.push_back(modified);
    if (pFileJob->files.size() == maxFilesPerJob)
    {
        // Dispatching the job
        if (queueservice.waitingJobsCount() >= maxJobsInQueue)
        {
            logs.debug() << logPrefix << "  :: waiting for dispatching job";
            do
            {
                enum
                {
                    // timeout in ms based on constants
                    timeout = maxFilesPerJob * maxJobsInQueueReducedPressure / 4,
                };
                SuspendMilliSeconds(timeout);
                if (pShouldStop)
                    return false;
            } while (queueservice.waitingJobsCount() > maxJobsInQueueReducedPressure);
            logs.debug() << logPrefix << "  :: resuming";
        }
        dispatchJob(pFileJob);
        // Allocating a new empty job
        pFileJob = new FileJob(events);
        pFileJob->files.reserve(maxFilesPerJob);
        pFileJob->filessize.reserve(maxFilesPerJob);
        pFileJob->modified.reserve(maxFilesPerJob);
    }
    return true;
}

void WalkerThread::walk(const String& path)
{
    if (not onDirectoryEnter(path)) // nothing to walk through
        return;

    // Should we take care of files ?
    bool handleSingleFiles = not events.file.access.empty();

    do
    {
        assert(pContext.top() != NULL);
        auto& context = *(pContext.top());

        if (pShouldStop)
            break;

        // Iterating all files
        {
            bool reloop = false;
            while (context.cursor.valid())
            {
                if (context.cursor.isFile())
                {
                    if (handleSingleFiles)
                    {
                        // notifying - the event will be triggered by another thread
                        auto& filename = context.cursor.filename();
                        auto& parent = context.info.directory();
                        auto size = context.cursor.size();
                        auto modified = context.cursor.modified();
                        if (not triggerFileEvent(filename, parent, modified, size))
                            return;
                    }

                    // next entry
                    ++context.cursor;
                }
                else
                {
                    // notifying
                    if (not onDirectoryEnter(context.cursor.filename()))
                    {
                        // The folder should be skipped, we should directly
                        // go the next item
                        ++context.cursor;

                        if (pShouldStop)
                            break;
                    }
                    else
                    {
                        // we will have to loop again to handle the new entry
                        reloop = true;
                        // next entry
                        ++context.cursor;
                        break;
                    }
                }
            }

            if (reloop)
                continue;
        }

        // Leaving directories
        do
        {
            onDirectoryLeave();
            if (pStack.empty() || pContext.top()->cursor.valid())
                break;
        } while (true);
    } while (not pStack.empty());

    // dispatching the last job
    if (pFileJob && not pFileJob->files.empty())
    {
        dispatchJob(pFileJob);
        pFileJob = nullptr;
    }

    // For the execution of all jobs
    waitForAllJobs();

    // the operation is complete
    statistics.aborted = false;
    logs.info() << logPrefix << "operation completed";
}

void WalkerThread::waitForAllJobs()
{
    // Previous number of jobs
    int lastremain = -1;
    // keep a reference to the jobCounter
    auto jobCounter = pJobCounter;
    if (!(!jobCounter)) // assigned
    {
        do
        {
            int count = *jobCounter;
            if (0 == count)
            {
                // ok, no more job. it is over
                break;
            }

            if (count != lastremain)
            {
                lastremain = count;
                switch (count)
                {
                case 0:
                    break;
                case 1:
                    logs.info() << logPrefix << "Waiting for 1 job";
                    break;
                default:
                    logs.info() << logPrefix << "Waiting for " << count << " jobs";
                }
            }

            // suspend the execution of the current thread for 2 seconds
            Suspend(2);
            // checking if we were asked to stop
            if (pShouldStop)
                break;
        } while (true);
    }
}

void WalkerThread::dispatchJob(IJob::Ptr job)
{
    // get the shared counter
    job->pJobCounter = pJobCounter;
    // incrementing the shared counter (from the job, to avoid race condition)
    ++(*job->pJobCounter);
    // adding the job to the queueservice
    queueservice += job;
}

Walker::Walker() : pJobCount(0)
{
}

Walker::Walker(const AnyString& logprefix) : pJobCount(0), pLogPrefix(logprefix)
{
}

Walker::~Walker()
{
}

void Walker::directory(const AnyString& path)
{
    if (not path.empty())
    {
        String normalized;
        if (IO::IsAbsolute(path))
        {
            IO::Normalize(pDirectory, path);
        }
        else
        {
            String absolute;
            IO::MakeAbsolute(absolute, path);

            IO::Normalize(pDirectory, absolute);
        }
    }
}

void Walker::run()
{
    // reset statistics
    pStats.reset();
    pStats.aborted = false;

    if (pDirectory.empty())
        return;

    // If there is no extensions, there is nothing to do
    if (pExtensions.empty())
        return;

    // Sorting extensions by they priority in the reverse order
    // higher priority must be executed first
    std::sort(pExtensions.begin(),
              pExtensions.end(),
              [](const IExtension::Ptr& a, const IExtension::Ptr& b) {
                  return b->priority() < a->priority();
              });

    // The process has aborted by default from now on
    pStats.aborted = true;

    // Re-creating the thread
    auto* thread = new WalkerThread(pStats);
    // The directory to scan
    thread->directory = pDirectory;
    thread->logPrefix = pLogPrefix;
    // Extensions
    DispatchJobEvent callback;
    callback.bind(thread, &WalkerThread::dispatchJob);
    thread->events.initialize(pExtensions, callback);

    // Starting the thread !
    thread->run();
    delete thread;
}

YString Walker::directory() const
{
    return pDirectory;
}

void Walker::retrieveStatistics(Statistics& out)
{
    out = pStats;
}

void Walker::add(IExtension::Ptr& extension)
{
    if (!(!extension))
        pExtensions.push_back(extension);
}

void Walker::add(IExtension* extension)
{
    if (extension)
        pExtensions.push_back(extension);
}

} // namespace FSWalker
