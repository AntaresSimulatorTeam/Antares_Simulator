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
#include <yuni/job/job.h>
#include <yuni/job/queue/service.h>
#include <yuni/core/slist.h>
#include <yuni/core/system/cpu.h>
#include "action.h"
#include <antares/logs.h>
#include "settings.h"
#include <wx/frame.h>

using namespace Yuni;

namespace Antares
{
namespace Dispatcher
{
namespace // anonymous
{
//! The Dispatcher manager
using DispatcherServiceType = Yuni::Job::QueueService;

// The Dispatcher Service
static DispatcherServiceType gDispatcher;

static LinkedList<Yuni::Job::IJob::Ptr> gGUIDispatcherList;
static Yuni::Mutex gGUIMutex;

class JobDelay final : public Yuni::Job::IJob
{
public:
    JobDelay(const Job::IJob::Ptr& job, uint delay) : pJob(job), pDelay(delay)
    {
    }
    virtual ~JobDelay()
    {
    }

protected:
    virtual void onExecute() override
    {
        if (not suspend(pDelay))
            ::Antares::Dispatcher::GUI::Post(pJob);
    }

private:
    Job::IJob::Ptr pJob;
    uint pDelay;
};

} // anonymous namespace

namespace GUI
{
void Post(const Yuni::Bind<void()>& job)
{
    ::Antares::Dispatcher::GUI::Post(
      (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobSimpleDispatcher(job));
}

void Post(const Yuni::Job::IJob::Ptr& action, uint delay)
{
    ::Antares::Dispatcher::Post((const Yuni::Job::IJob::Ptr&)new JobDelay(action, delay));
}

void Post(const Yuni::Job::IJob::Ptr& action)
{
    if (Settings::WindowForPendingDispatchers)
    {
        gGUIMutex.lock();
        gGUIDispatcherList.push_back(action);
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, Settings::IDForWxDispatcher);
        Settings::WindowForPendingDispatchers->GetEventHandler()->AddPendingEvent(evt);
        gGUIMutex.unlock();
    }
    else
    {
        // logs.debug() << "impossible to post message. The window is invalid";
    }
}

} // namespace GUI

void Post(const Yuni::Job::IJob::Ptr& job)
{
    gDispatcher += job;
}

bool Start()
{
    // Looking for the maximum number of threads
    {
        uint cpus = System::CPU::Count();
        if (cpus > 1)
            --cpus;
        if (cpus < 3)
            cpus = 3; // a minimum is required for concurrent and blocking jobs
        if (cpus > 5)
            cpus = 5; // will be good enough
        gDispatcher.maximumThreadCount(cpus);
    }

    logs.info() << "starting worker pool (" << gDispatcher.maximumThreadCount() << " threads)";

    return gDispatcher.start();
}

void Stop(uint timeout)
{
    logs.info() << "stopping the worker pool";
    gDispatcher.stop(timeout);
}

void Wait()
{
    gDispatcher.wait(Yuni::qseIdle);
}

bool Empty()
{
    Yuni::MutexLocker locker(Antares::Dispatcher::gGUIMutex);
    return Antares::Dispatcher::gGUIDispatcherList.empty();
}

} // namespace Dispatcher
} // namespace Antares

namespace Antares
{
namespace Dispatcher
{
namespace Internal
{
void ExecuteQueueDispatcher()
{
    // Locking access to the shared resource
    Antares::Dispatcher::gGUIMutex.lock();
    // Retrieving the job to execute
    Job::IJob::Ptr job = Antares::Dispatcher::gGUIDispatcherList.front();
    // Removing it from the stack
    Antares::Dispatcher::gGUIDispatcherList.pop_front();
    // Unlock
    Antares::Dispatcher::gGUIMutex.unlock();

    // Execute the event
    if (!(!job))
        job->execute(nullptr);
}

} // namespace Internal
} // namespace Dispatcher
} // namespace Antares
