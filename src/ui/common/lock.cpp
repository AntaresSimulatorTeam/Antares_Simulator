// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "lock.h"
#include <cassert>
#include <atomic>

namespace Antares
{
static std::atomic<int> gGUIFlushRefCount = 0;

static std::atomic<int> GUIIsAboutToQuitFlag = false;

bool IsGUIAboutToQuit()
{
    return (0 != GUIIsAboutToQuitFlag);
}

void GUIIsAboutToQuit()
{
    GUIIsAboutToQuitFlag = 1;
}

void GUIIsNoLongerQuitting()
{
    GUIIsAboutToQuitFlag = 0;
}

void GUIBeginUpdate()
{
    ++gGUIFlushRefCount;
}

void GUIEndUpdate()
{
    assert(gGUIFlushRefCount > 0);
    gGUIFlushRefCount += -1;
}

bool GUIIsLock()
{
    return (0 != gGUIFlushRefCount || GUIIsAboutToQuitFlag);
}

uint GUILockRefCount()
{
    return (uint)gGUIFlushRefCount;
}

} // namespace Antares
