// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "thermal.areasummary.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
ThermalClusterSummarySingleArea::ThermalClusterSummarySingleArea(
  wxWindow* control,
  Toolbox::InputSelector::Area* notifier):
    CommonClusterSummarySingleArea(control, notifier)
{
}

ThermalClusterSummarySingleArea::~ThermalClusterSummarySingleArea()
{
}

wxString ThermalClusterSummarySingleArea::rowCaption(int rowIndx) const
{
    if (pArea)
    {
        return wxStringFromUTF8(pArea->thermal.list[rowIndx]->name());
    }
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
    Data::ThermalCluster* cluster = (pArea and (uint) y < pArea->thermal.list.allClustersCount())
                                      ? pArea->thermal.list[y].get()
                                      : nullptr;
    if (!cluster->enabled)
    {
        return wxEmptyString;
    }
    switch (x)
    {
    case 0:
        return wxStringFromUTF8(cluster->getGroup());
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
        return DoubleToWxString(cluster->emissions.factors[Antares::Data::Pollutant::CO2]);
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
    Data::ThermalCluster* cluster = (pArea and (uint) y < pArea->thermal.list.allClustersCount())
                                      ? pArea->thermal.list[y].get()
                                      : nullptr;
    if (!cluster->enabled)
    {
        return 0.;
    }
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
        return cluster->emissions.factors[Antares::Data::Pollutant::CO2];
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
    auto* cluster = (pArea and (uint) y < pArea->thermal.list.allClustersCount())
                      ? pArea->thermal.list[y].get()
                      : nullptr;

    if (cluster)
    {
        switch (x)
        {
        case 0:
        {
            if (cluster->getGroup() != v)
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
              cluster->unitCount,
              v);
        case 3:
            return Update<double, NoCheck, RefeshInspector>(cluster->nominalCapacity, v);
        case 4:
            return Update<bool, NoCheck, RefeshInspector>(cluster->mustrun, v);
        case 5:
            return Update<double, NoCheck, RefeshInspector>(cluster->minStablePower, v);
        case 6:
            return Update<uint, CheckMinUpDownTime, RefeshInspectorAndMarkAsModified>(
              cluster->minUpTime,
              v);
        case 7:
            return Update<uint, CheckMinUpDownTime, RefeshInspectorAndMarkAsModified>(
              cluster->minDownTime,
              v);
        case 8:
            return Update<double, NoCheck, RefeshInspector>(cluster->spinning, v);
        case 9:
            return Update<double, NoCheck, RefeshInspector>(
              cluster->emissions.factors[Antares::Data::Pollutant::CO2],
              v);
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

} // namespace Antares::Component::Datagrid::Renderer
