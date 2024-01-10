/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
