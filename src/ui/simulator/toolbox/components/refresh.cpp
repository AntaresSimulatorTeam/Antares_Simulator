/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

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
