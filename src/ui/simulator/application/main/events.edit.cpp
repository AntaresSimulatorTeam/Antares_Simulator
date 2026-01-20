// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "main.h"
#include "internal-data.h"

// Map
#include "../../toolbox/components/map/component.h"
// inspector
#include "../../windows/inspector.h"
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::evtOnViewAllSystem(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("sys"));
    pMainMap->showLayerAll();
}

void ApplWnd::copyToClipboard()
{
    // Copy to the clipboard items present in the inspector
    uint count = Window::Inspector::CopyToClipboard();

    // Keeping the user informed about the operation
    switch (count)
    {
    case 0:
        SetStatusText(wxT("  Nothing has been copied"));
        break;
    case 1:
        SetStatusText(wxT("  1 item has been copied"));
        break;
    default:
        SetStatusText(wxString(wxT("  ")) << count << wxT(" items have been copied"));
    }
    // paste: applicate/main/paste-from-clipboard.cpp
}

void ApplWnd::evtOnEditCopy(wxCommandEvent&)
{
    if (GUIIsLock())
    {
        return;
    }
    copyToClipboard();
    // paste: applicate/main/paste-from-clipboard.cpp
}

void ApplWnd::evtOnEditMapSelectAll(wxCommandEvent&)
{
    if (GUIIsLock() || !pMainMap)
    {
        return;
    }
    pMainMap->selectAll();
}

void ApplWnd::evtOnEditMapUnselectAll(wxCommandEvent&)
{
    if (GUIIsLock())
    {
        return;
    }
    if (!pMainMap)
    {
        return;
    }
    pMainMap->unselectAll();
}

void ApplWnd::evtOnEditMapReverseSelection(wxCommandEvent&)
{
    if (GUIIsLock())
    {
        return;
    }
    if (!pMainMap)
    {
        return;
    }
    pMainMap->reverseSelection();
}

} // namespace Antares::Forms
