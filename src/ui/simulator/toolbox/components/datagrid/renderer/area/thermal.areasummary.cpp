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

#include "thermal.areasummary.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
ThermalClusterSummarySingleArea::ThermalClusterSummarySingleArea(
    wxWindow* control,
    Toolbox::InputSelector::Area* notifier) : CommonClusterSummarySingleArea(control, notifier)
{
}

ThermalClusterSummarySingleArea::~ThermalClusterSummarySingleArea() {}

wxString ThermalClusterSummarySingleArea::rowCaption(int rowIndx) const
{
    if (pArea)
        return wxStringFromUTF8(pArea->thermal.list.byIndex[rowIndx]->name());
    return wxEmptyString;
}

wxString ThermalClusterSummarySingleArea::columnCaption(int colIndx) const
{
    // note: Those ugly white-spaces are required (for now)
    //  to fix the min width of the column. A method should be added for this purpose.
    static const wxChar* const rows[] = {
      wxT("          Group          "),
      wxT("  Enabled  "),
      wxT("  Unit count  "),
      wxT("   Nom. Capacity  \n        MW        "),
      wxT("    Must-run    "),
      wxT("  Min. Stable Power \n        MW        "),
      wxT("  Min. Up Time "),
      wxT("  Min. Down Time "),
      wxT("    Spinning    \n % "),
      wxT("   CO2   \n    Tons / MWh    "),
      wxT(" Marginal \n     \u20AC / MWh     "),
      wxT(" Fixed \n     \u20AC / hour     "),
      wxT(" Startup \n    \u20AC / startup    "),
      wxT(" Market Bid \n     \u20AC / MWh     "),
      wxT(" Spread \n     \u20AC / MWh     "),
    };
    return (colIndx < 15 and colIndx >= 0) ? rows[colIndx] : wxT("");
}

wxString ThermalClusterSummarySingleArea::cellValue(int x, int y) const
{
    Data::ThermalCluster* cluster = (pArea and (uint) y < pArea->thermal.list.size())
                                      ? pArea->thermal.list.byIndex[y]
                                      : nullptr;
    if (!cluster->enabled)
        return wxEmptyString;
    switch (x)
    {
    case 0:
        return wxStringFromUTF8(cluster->group());
    case 1:
        return cluster->enabled ? wxT("Yes") : wxT("no");
    case 2:
        return wxString() << cluster->unitCount;
    case 3:
        return DoubleToWxString(cluster->nominalCapacity);
    case 4:
        return cluster->mustrun ? wxT("must-run") : wxT("no");
    case 5:
        return DoubleToWxString(cluster->minStablePower);
    case 6:
        return wxString() << cluster->minUpTime;
    case 7:
        return wxString() << cluster->minDownTime;
    case 8:
        return DoubleToWxString(cluster->spinning);
    case 9:
        return DoubleToWxString(cluster->co2);
    case 10:
        return DoubleToWxString(Math::Round(cluster->marginalCost, 3));
    case 11:
        return DoubleToWxString(Math::Round(cluster->fixedCost, 3));
    case 12:
        return DoubleToWxString(Math::Round(cluster->startupCost, 3));
    case 13:
        return DoubleToWxString(Math::Round(cluster->marketBidCost, 3));
    case 14:
        return DoubleToWxString(cluster->spreadCost);
    }
    return wxEmptyString;
}

double ThermalClusterSummarySingleArea::cellNumericValue(int x, int y) const
{
    Data::ThermalCluster* cluster = (pArea and (uint) y < pArea->thermal.list.size())
                                      ? pArea->thermal.list.byIndex[y]
                                      : nullptr;
    if (!cluster->enabled)
        return 0.;
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
    case 4:
        return cluster->mustrun ? 1. : 0.;
    case 5:
        return cluster->minStablePower;
    case 6:
        return cluster->minUpTime;
    case 7:
        return cluster->minDownTime;
    case 8:
        return cluster->spinning;
    case 9:
        return cluster->co2;
    case 10:
        return cluster->marginalCost;
    case 11:
        return cluster->fixedCost;
    case 12:
        return cluster->startupCost;
    case 13:
        return cluster->marketBidCost;
    case 14:
        return cluster->spreadCost;
    }
    return 0.;
}

bool ThermalClusterSummarySingleArea::cellValue(int x, int y, const String& v)
{
    auto* cluster = (pArea and (uint) y < pArea->thermal.list.size())
                      ? pArea->thermal.list.byIndex[y]
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
            return Update<uint, CheckUnitCount, RefeshInspectorAndMarkAsModified>(cluster->unitCount, v);
        case 3:
            return Update<double, NoCheck, RefeshInspector>(cluster->nominalCapacity, v);
        case 4:
            return Update<bool, NoCheck, RefeshInspector>(cluster->mustrun, v);
        case 5:
            return Update<double, NoCheck, RefeshInspector>(cluster->minStablePower, v);
        case 6:
            return Update<uint, CheckMinUpDownTime, RefeshInspectorAndMarkAsModified>(cluster->minUpTime, v);
        case 7:
            return Update<uint, CheckMinUpDownTime, RefeshInspectorAndMarkAsModified>(cluster->minDownTime, v);
        case 8:
            return Update<double, NoCheck, RefeshInspector>(cluster->spinning, v);
        case 9:
            return Update<double, NoCheck, RefeshInspector>(cluster->co2, v);
        case 10:
            return Update<double, NoCheck, RefeshInspector>(cluster->marginalCost, v);
        case 11:
            return Update<double, NoCheck, RefeshInspector>(cluster->fixedCost, v);
        case 12:
            return Update<double, NoCheck, RefeshInspector>(cluster->startupCost, v);
        case 13:
            return Update<double, NoCheck, RefeshInspector>(cluster->marketBidCost, v);
        case 14:
            return Update<double, NoCheck, RefeshInspector>(cluster->spreadCost, v);
        }
    }
    return false;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
