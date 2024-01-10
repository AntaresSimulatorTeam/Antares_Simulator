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