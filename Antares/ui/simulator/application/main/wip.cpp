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
#include <wx/statusbr.h>
#include "../study.h"
#include <ui/common/lock.h>
#include <ui/common/dispatcher.h>
#include "internal-data.h"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
static uint stackCount = 0;

static void DelayedGUIEndUpdate()
{
    GUIEndUpdate();
}

void ApplWnd::showWIP()
{
    GUIBeginUpdate();
    MemoryFlushBeginUpdate();
    if (!stackCount++)
    {
        pData->wipEnabled = true;
        // The operation must not be delayed here
        if (pData->wipPanel)
            pData->wipPanel->Refresh();
    }
}

void ApplWnd::hideWIP()
{
    assert(stackCount > 0);

    if (stackCount and !--stackCount)
    {
        pData->wipEnabled = false;
        // The operation must not be delayed here
        if (pData->wipPanel)
            pData->wipPanel->Refresh();
    }
    MemoryFlushEndUpdate();

    if (GUILockRefCount() == 1)
    {
        Yuni::Bind<void()> callback;
        callback.bind(&DelayedGUIEndUpdate);
        Dispatcher::GUI::Post(callback, 10 /*ms, arbitrary*/);
    }
    else
        GUIEndUpdate();
}

} // namespace Forms
} // namespace Antares
