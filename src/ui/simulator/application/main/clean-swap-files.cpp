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

#include "main.h"
#include <antares/memory/memory.h>
#include <antares/logs.h>
#include <yuni/thread/timer.h>

using namespace Yuni;

namespace Antares
{
namespace Forms
{
namespace
{
class SwapFileCleanerTimer final : public Yuni::Thread::Timer
{
public:
    SwapFileCleanerTimer(uint timeout) : Yuni::Thread::Timer(timeout)
    {
    }

    virtual ~SwapFileCleanerTimer()
    {
        stop();
    }

protected:
    virtual bool onInterval(uint) override
    {
        logs.info() << "memory pool: looking for orphan swap files...";
        Antares::memory.cleanupCacheFolder();
        logs.debug() << "memory pool: cleanup is done";
        return false;
    }
};

SwapFileCleanerTimer* cleanTimer = nullptr;

} // anonymous namespace

void ApplWnd::timerCleanSwapFiles(uint timeout)
{
    if (not cleanTimer)
    {
        cleanTimer = new SwapFileCleanerTimer(timeout);
        cleanTimer->start();
    }
    else
    {
        cleanTimer->stop();
        cleanTimer->interval(timeout);
        cleanTimer->start();
    }
}

void ApplWnd::timerCleanSwapFilesDestroy()
{
    if (cleanTimer)
    {
        delete cleanTimer;
        cleanTimer = nullptr;
    }
}

} // namespace Forms
} // namespace Antares
