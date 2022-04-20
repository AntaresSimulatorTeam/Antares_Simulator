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
#include "thermalmodulation.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
ThermalClusterCommonModulation::ThermalClusterCommonModulation(
  wxWindow* control,
  Toolbox::InputSelector::ThermalCluster* notifier) :
 Renderer::Matrix<>(control), pCluster(nullptr)
{
    if (notifier)
        notifier->onThermalClusterChanged.connect(
          this, &ThermalClusterCommonModulation::internalThermalClusterChanged);
}

ThermalClusterCommonModulation::~ThermalClusterCommonModulation()
{
    destroyBoundEvents();
}

wxString ThermalClusterCommonModulation::rowCaption(int row) const
{
    if (!study || row >= Date::Calendar::maxHoursInYear)
        return wxEmptyString;
    return wxStringFromUTF8(study->calendar.text.hours[row]);
}

wxString ThermalClusterCommonModulation::columnCaption(int column) const
{
    switch (column)
    {
    case Data::thermalModulationCost:
        return wxT(" Marginal Cost \n modulation ");
    case Data::thermalModulationMarketBid:
        return wxT(" Market bid \n modulation ");
    case Data::thermalModulationCapacity:
        return wxT(" Capacity \n modulation ");
    case Data::thermalMinGenModulation:
        return wxT(" Min Gen \n modulation ");
    case (Data::thermalModulationCost + Data::thermalModulationMax):
        return wxT("Net\n Marginal Cost ");
    case (Data::thermalModulationMarketBid + Data::thermalModulationMax):
        return wxT("Net\n Market bid ");
    case (Data::thermalModulationCapacity + Data::thermalModulationMax):
        return wxT("Net\n Capacity ");
    case (Data::thermalMinGenModulation + Data::thermalModulationMax):
        return wxT("Net\n Min Gen ");
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

wxString ThermalClusterCommonModulation::cellValue(int x, int y) const
{
    if (pMatrix && (uint)y < pMatrix->height && pCluster)
    {
        switch (x)
        {
        case Data::thermalModulationCost:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::thermalModulationCost][y], 3));
        case Data::thermalModulationMarketBid:
            return DoubleToWxString(
              Math::Round((*pMatrix)[Data::thermalModulationMarketBid][y], 3));
        case Data::thermalModulationCapacity:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::thermalModulationCapacity][y], 3));
        case Data::thermalMinGenModulation:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::thermalMinGenModulation][y], 3));
        case (Data::thermalModulationCost + Data::thermalModulationMax):
            return DoubleToWxString(
              Math::Round((*pMatrix)[Data::thermalModulationCost][y] * pCluster->marginalCost, 2));
        case (Data::thermalModulationMarketBid + Data::thermalModulationMax):
            return DoubleToWxString(Math::Round(
              (*pMatrix)[Data::thermalModulationMarketBid][y] * pCluster->marketBidCost, 2));
        case (Data::thermalModulationCapacity + Data::thermalModulationMax):
            return DoubleToWxString(Math::Round(
              (*pMatrix)[Data::thermalModulationCapacity][y] * pCluster->nominalCapacity, 2));
        case (Data::thermalMinGenModulation + Data::thermalModulationMax):
            return DoubleToWxString(Math::Round((*pMatrix)[Data::thermalMinGenModulation][y]
                                                  * pCluster->unitCount * pCluster->nominalCapacity,
                                                2));
        }
    }
    return wxT("0");
}

double ThermalClusterCommonModulation::cellNumericValue(int x, int y) const
{
    if (pMatrix && (uint)y < pMatrix->height && pCluster)
    {
        assert((uint)x < static_cast<int>(width()));
        switch (x)
        {
        case Data::thermalModulationCost:
            return (*pMatrix)[Data::thermalModulationCost][y];
        case Data::thermalModulationMarketBid:
            return (*pMatrix)[Data::thermalModulationMarketBid][y];
        case Data::thermalModulationCapacity:
            return (*pMatrix)[Data::thermalModulationCapacity][y];
        case Data::thermalMinGenModulation:
            return (*pMatrix)[Data::thermalMinGenModulation][y];
        case (Data::thermalModulationCost + Data::thermalModulationMax):
            return (*pMatrix)[Data::thermalModulationCost][y] * pCluster->marginalCost;
        case (Data::thermalModulationMarketBid + Data::thermalModulationMax):
            return (*pMatrix)[Data::thermalModulationMarketBid][y] * pCluster->marketBidCost;
        case (Data::thermalModulationCapacity + Data::thermalModulationMax):
            return (*pMatrix)[Data::thermalModulationCapacity][y] * pCluster->nominalCapacity;
        case (Data::thermalMinGenModulation + Data::thermalModulationMax):
            return (*pMatrix)[Data::thermalMinGenModulation][y] * pCluster->unitCount
                   * pCluster->nominalCapacity;
        }
    }
    return 0.;
}

void ThermalClusterCommonModulation::internalThermalClusterChanged(
  Antares::Data::ThermalCluster* cluster)
{
    if (pCluster != cluster)
    {
        pCluster = cluster;
        if (cluster != nullptr)
            cluster->modulation.invalidate(true);
        matrix(cluster ? &cluster->modulation : nullptr);
    }
}

IRenderer::CellStyle ThermalClusterCommonModulation::cellStyle(int col, int row) const
{
    if (col > 3)
        return IRenderer::cellStyleDisabled;
    const double v = Renderer::Matrix<>::cellNumericValue(col, row);

    if (v < 0.)
        return IRenderer::cellStyleError;

    if (col == Data::thermalModulationCapacity)
    {
        if (pCluster->minDivModulation.isCalculated
            && v / ceil(v) < pCluster->minDivModulation.border
            && !pCluster->minDivModulation.isValidated)
            return IRenderer::cellStyleError;
    }

    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxColour ThermalClusterCommonModulation::verticalBorderColor(int x, int y) const
{
    return (x == 3) ? Default::BorderHighlightColor() : IRenderer::verticalBorderColor(x, y);
}

wxColour ThermalClusterCommonModulation::horizontalBorderColor(int x, int y) const
{
    // Getting informations about the next hour
    // (because the returned color is about the bottom border of the cell,
    // so the next hour for the user)
    if (!(!study) && y + 1 < Date::Calendar::maxHoursInYear)
    {
        auto& hourinfo = study->calendar.hours[y + 1];

        if (hourinfo.firstHourInMonth)
            return Default::BorderMonthSeparator();
        if (hourinfo.firstHourInDay)
            return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

bool ThermalClusterCommonModulation::cellValue(int x, int y, const String& value)
{
    if ((uint)x < 4u && pMatrix && (uint)y < pMatrix->height)
    {
        double v;
        if (!value.to(v))
            return false;
        if (v < 0.)
            v = 0.;
        switch (x)
        {
        case Data::thermalModulationCost:
        {
            (*pMatrix)[Data::thermalModulationCost][y] = v;
            pMatrix->markAsModified();
            return true;
        }
        case Data::thermalModulationMarketBid:
        {
            (*pMatrix)[Data::thermalModulationMarketBid][y] = v;
            pMatrix->markAsModified();
            return true;
        }
        case Data::thermalModulationCapacity:
        {
            (*pMatrix)[Data::thermalModulationCapacity][y] = v;
            bool old = pCluster->minDivModulation.isValidated;
            pCluster->checkMinStablePowerWithNewModulation(y, v);
            if (old != pCluster->minDivModulation.isValidated)
            {
                auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
                auto& panel = *mainFrm.mainPanel();
                OnInspectorRefresh(&panel);
            }

            pMatrix->markAsModified();
            return true;
        }
        case Data::thermalMinGenModulation:
        {
            (*pMatrix)[Data::thermalMinGenModulation][y] = v;
            pMatrix->markAsModified();
            return true;
        }
        }
    }

    return false;
}

void ThermalClusterCommonModulation::onStudyClosed()
{
    internalThermalClusterChanged(nullptr);
    Renderer::Matrix<>::onStudyClosed();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
