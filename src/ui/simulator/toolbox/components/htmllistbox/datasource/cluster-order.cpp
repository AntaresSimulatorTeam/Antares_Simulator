/*
** Copyright 2007-2023 RTE
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

#include "cluster-order.h"
#include "../../../../application/study.h"
#include "../../../../application/main.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
wxString groupNameToGroupTitle(Data::Area* area, wxString& groupName)
{
    // Group title
    wxString groupTitle;

    groupTitle << wxStringFromUTF8(area->name);
    if (groupTitle.size() > 43)
    {
        groupTitle.resize(40);
        groupTitle += wxT("...");
    }

    if (groupName.empty())
        groupTitle << wxT(" / <i>* no group *</i>");
    else
        groupTitle << wxT(" / ") << groupName;

    return groupTitle;
}

ClustersByOrder::ClustersByOrder(HTMLListbox::Component& parent) :
 IDatasource(parent), pArea(nullptr), hasGroupJustChanged_(false)
{
    OnStudyAreasChanged.connect(this, &ClustersByOrder::onInvalidateAllAreas);
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this,
                                                          &ClustersByOrder::onInvalidateAllAreas);
}

//! Destructor
ClustersByOrder::~ClustersByOrder()
{
    destroyBoundEvents();
}

void ClustersByOrder::refresh(const wxString& search)
{
    if (hasGroupChanged())
    {
        // A cluster group just changed.
        // To keep the changed cluster selected, We have to reorder the items list.
        // Rebuilding the item list would re-initialize the cluster selection on the first cluster
        // of the list.
        reorderItemsList(search);
        hasGroupChanged(false);
    }
    else
        rebuildItemsList(search);
}

void ClustersByOrder::onAreaChanged(Data::Area* area)
{
    pArea = area;
}

void ClustersByOrder::onInvalidateAllAreas()
{
    pArea = nullptr;
}

void ClustersByOrder::hasGroupChanged(bool b)
{
    hasGroupJustChanged_ = b;
}

bool ClustersByOrder::hasGroupChanged() const
{
    return hasGroupJustChanged_;
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares