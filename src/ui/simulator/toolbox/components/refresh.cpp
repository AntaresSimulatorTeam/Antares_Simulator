// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "refresh.h"
#include "datagrid/component.h"
#include "notebook/notebook.h"
#include "../../application/study.h"
#include "../../../common/lock.h"

namespace Antares
{
static void RefreshAllControlsRecursive(wxWindow* comObj)
{
    auto* com = dynamic_cast<Component::Datagrid::Component*>(comObj);
    if (com)
    {
        if (com->IsShown())
        {
            com->InvalidateBestSize();
            com->ClearBackground();
            com->forceRefresh();
        }
    }
    else
    {
        auto* nbk = dynamic_cast<Component::Notebook*>(comObj);
        if (nbk)
        {
            auto* page = nbk->selected();
            if (page)
                RefreshAllControls(page->control());
        }
        else
        {
            const wxWindowList& list = comObj->GetChildren();
            const wxWindowList::const_iterator end = list.end();
            for (wxWindowList::const_iterator i = list.begin(); i != end; ++i)
                RefreshAllControlsRecursive(*i);
            comObj->Refresh();
        }
    }
}

static void EnableRefreshForAllDatagridRecursive(wxWindow* comObj, bool enabled)
{
    auto* com = dynamic_cast<Component::Datagrid::Component*>(comObj);
    if (com)
    {
        com->enableRefresh(enabled);
    }
    else
    {
        auto* nbk = dynamic_cast<Component::Notebook*>(comObj);
        if (nbk)
        {
            nbk->enableRefreshForAllDatagrid(enabled);
        }
        else
        {
            const wxWindowList& list = comObj->GetChildren();
            const wxWindowList::const_iterator end = list.end();
            for (wxWindowList::const_iterator i = list.begin(); i != end; ++i)
                EnableRefreshForAllDatagrid(*i, enabled);
        }
    }
}

void RefreshAllControls(wxWindow* comObj)
{
    if (comObj && !IsGUIAboutToQuit())
        RefreshAllControlsRecursive(comObj);
}

void EnableRefreshForAllDatagrid(wxWindow* comObj, bool enabled)
{
    if (comObj && !IsGUIAboutToQuit())
        EnableRefreshForAllDatagridRecursive(comObj, enabled);
}

} // namespace Antares
