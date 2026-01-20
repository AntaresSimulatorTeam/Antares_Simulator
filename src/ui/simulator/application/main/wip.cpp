// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "main.h"
#include <wx/statusbr.h>
#include "antares/study/study.h"
#include <ui/common/lock.h>
#include <ui/common/dispatcher.h>
#include "internal-data.h"
#include "application/study.h"

using namespace Yuni;

namespace Antares::Forms
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
        {
            pData->wipPanel->Refresh();
        }
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
        {
            pData->wipPanel->Refresh();
        }
    }
    MemoryFlushEndUpdate();

    if (GUILockRefCount() == 1)
    {
        Yuni::Bind<void()> callback;
        callback.bind(&DelayedGUIEndUpdate);
        Dispatcher::GUI::Post(callback, 10 /*ms, arbitrary*/);
    }
    else
    {
        GUIEndUpdate();
    }
}

} // namespace Antares::Forms
