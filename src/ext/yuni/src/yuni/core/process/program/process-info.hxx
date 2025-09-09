/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "process-info.h"

namespace Yuni::Process
{
inline Program::ProcessSharedInfo::ProcessSharedInfo():
    running(false),
    processID(-1),
    processInput(-1),
    duration(0),
    durationPrecision(dpSeconds),
    timeout(),
    exitstatus(-1),
    redirectToConsole(false),
    timeoutThread(nullptr)
{
}

inline Program::ProcessSharedInfo::~ProcessSharedInfo()
{
    if (YUNI_UNLIKELY(timeoutThread))
    {
        // should never go in this section
        assert(false and "the thread for handling the timeout has not been properly stopped");
        timeoutThread->stop();
        delete timeoutThread;
    }
}

#ifdef YUNI_OS_WINDOWS

namespace // anonymous
{
void QuitProcess(DWORD processID)
{
    assert(processID != 0);
    // Loop on process windows
    for (HWND hwnd = ::GetTopWindow(nullptr); hwnd; hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT))
    {
        DWORD windowProcessID;
        DWORD threadID = ::GetWindowThreadProcessId(hwnd, &windowProcessID);
        if (windowProcessID == processID)
        {
            // Send WM_QUIT to the process thread
            ::PostThreadMessage(threadID, WM_QUIT, 0, 0);
        }
    }
}

} // namespace

#endif

template<bool WithLock>
inline bool Program::ProcessSharedInfo::sendSignal(int sigvalue)
{
    if (WithLock)
    {
        mutex.lock();
    }
    if (0 == running)
    {
        if (WithLock)
        {
            mutex.unlock();
        }
        return false;
    }

#ifndef YUNI_OS_WINDOWS
    {
        const pid_t pid = static_cast<pid_t>(processID);
        if (WithLock)
        {
            mutex.unlock();
        }
        if (pid > 0)
        {
            return (0 == ::kill(pid, sigvalue));
        }
    }
#else
    {
        switch (sigvalue)
        {
        // All signals are handled by force-quitting the child process' window threads.
        default:
            QuitProcess(processID);
            break;
        }

        if (WithLock)
        {
            mutex.unlock();
        }
    }
#endif

    return false;
}

namespace // anonymous
{
class TimeoutThread final: public Thread::IThread
{
public:
    TimeoutThread(int pid, uint timeout):
        timeout(timeout),
        pid(pid)
    {
        assert(pid > 0);
    }

    ~TimeoutThread()
    {
    }

protected:
    virtual bool onExecute() override
    {
        // wait for timeout... (note: the timeout is in seconds)
        if (not suspend(timeout * 1000))
        {
            // the timeout has been reached

#ifdef YUNI_OS_UNIX
            ::kill(pid, SIGKILL);
#else
            QuitProcess(pid);
#endif
        }
        return false; // stop the thread, does not suspend it
    }

private:
    //! Timeout in seconds
    uint timeout;
    //! PID of the sub process
    int pid;
};

} // anonymous namespace

inline void Program::ProcessSharedInfo::createThreadForTimeoutWL()
{
    delete timeoutThread; // for code safety

    if (processID > 0 and timeout > 0)
    {
        timeoutThread = new TimeoutThread(processID, timeout);
        timeoutThread->start();
    }
    else
    {
        // no valid pid, no thread required for a timeout
        timeoutThread = nullptr;
    }
}

} // namespace Yuni::Process
