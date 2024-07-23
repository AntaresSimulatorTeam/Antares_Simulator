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

#include "renewable.areasummary.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
RenewableClusterSummarySingleArea::RenewableClusterSummarySingleArea(
  wxWindow* control,
  Toolbox::InputSelector::Area* notifier) :
 CommonClusterSummarySingleArea(control, notifier)
{
}

RenewableClusterSummarySingleArea::~RenewableClusterSummarySingleArea()
{
}

wxString RenewableClusterSummarySingleArea::rowCaption(int rowIndx) const
{
    if (pArea)
        return wxStringFromUTF8(pArea->renewable.list[rowIndx]->name());
    return wxEmptyString;
}

wxString RenewableClusterSummarySingleArea::columnCaption(int colIndx) const
{
    // note: Those ugly white-spaces are required (for now)
    //  to fix the min width of the column. A method should be added for this purpose.
    static const wxChar* const rows[] = {
      wxT("          Group          "),
      wxT("  Enabled  "),
      wxT("  Unit count  "),
      wxT("   Nom. Capacity  \n        MW        "),
    };
    return (colIndx < 5 and colIndx >= 0) ? rows[colIndx] : wxT("");
}

wxString RenewableClusterSummarySingleArea::cellValue(int x, int y) const
{
    Data::RenewableCluster* cluster = (pArea and (uint) y < pArea->renewable.list.allClustersCount())
                                        ? pArea->renewable.list[y].get()
                                        : nullptr;
    switch (x)
    {
    case 0:
        return wxStringFromUTF8(cluster->group());
    case 1:
        return cluster->enabled ? wxT("Yes") : wxT("No");
    case 2:
        return wxString() << cluster->unitCount;
    case 3:
        return DoubleToWxString(cluster->nominalCapacity);
    }
    return wxEmptyString;
}

double RenewableClusterSummarySingleArea::cellNumericValue(int x, int y) const
{
    Data::RenewableCluster* cluster = (pArea and (uint) y < pArea->renewable.list.allClustersCount())
                                        ? pArea->renewable.list[y].get()
                                        : nullptr;
    // gp : do we wish to have the line empty if cluster disabled
    // if (!cluster->enabled)
    //     return 0.;
    switch (x)
    {
    case 0:
        return 0.;
    case 1:
        return cluster->enabled ? 1. : 0.;
    case 2:
        return cluster->unitCount;
    case 3:
        return cluster->nominalCapacity;
    }
    return 0.;
}

bool RenewableClusterSummarySingleArea::cellValue(int x, int y, const String& v)
{
    auto* cluster = (pArea and (uint) y < pArea->renewable.list.allClustersCount())
                      ? pArea->renewable.list[y].get()
                      : nullptr;

    if (cluster)
    {
        switch (x)
        {
        case 0:
        {
            if (cluster->group() != v)
            {
                cluster->setGroup(v);
                return true;
            }
            break;
        }
        case 1:
            return Update<bool, NoCheck, RefeshInspector>(cluster->enabled, v);
        case 2:
            return Update<uint, CheckUnitCount, RefeshInspectorAndMarkAsModified>(
              cluster->unitCount, v);
        case 3:
            return Update<double, NoCheck, RefeshInspector>(cluster->nominalCapacity, v);
        }
    }
    return false;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
