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
#include "renewable-modulation.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
RenewableClusterCommonModulation::RenewableClusterCommonModulation(
  wxWindow* control,
  Toolbox::InputSelector::RenewableCluster* notifier) :
 Renderer::Matrix<>(control), pCluster(nullptr)
{
    if (notifier)
        notifier->onClusterChanged.connect(
          this, &RenewableClusterCommonModulation::internalRenewableClusterChanged);
}

RenewableClusterCommonModulation::~RenewableClusterCommonModulation()
{
    destroyBoundEvents();
}

wxString RenewableClusterCommonModulation::rowCaption(int row) const
{
    if (!study || row >= Date::Calendar::maxHoursInYear)
        return wxEmptyString;
    return wxStringFromUTF8(study->calendar.text.hours[row]);
}

wxString RenewableClusterCommonModulation::columnCaption(int column) const
{
    switch (column)
    {
    case Data::renewableModulationCost:
        return wxT(" Marginal Cost \n modulation ");
    case Data::renewableModulationCapacity:
        return wxT(" Capacity \n modulation ");
    case Data::renewableMinGenModulation:
        return wxT(" Min Gen \n modulation ");
    case (Data::renewableModulationCost + Data::renewableModulationMax):
        return wxT("Net\n Capacity ");
    case (Data::renewableModulationCapacity + Data::renewableModulationMax):
        return wxT("Net\n Min Gen ");
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

wxString RenewableClusterCommonModulation::cellValue(int x, int y) const
{
    if (pMatrix && (uint)y < pMatrix->height && pCluster)
    {
        switch (x)
        {
        case Data::renewableModulationCost:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::renewableModulationCost][y], 3));
        case Data::renewableModulationCapacity:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::renewableModulationCapacity][y], 3));
        case Data::renewableMinGenModulation:
            return DoubleToWxString(Math::Round((*pMatrix)[Data::renewableMinGenModulation][y], 3));
        case (Data::renewableModulationCost + Data::renewableModulationMax):
            // gp : "5" must be replaced with pCluster->unitCount when this last var is defined
            return DoubleToWxString(Math::Round(
               (*pMatrix)[Data::renewableModulationCapacity][y] * 5 * pCluster->nominalCapacity, 2));
        case (Data::renewableModulationCapacity + Data::renewableModulationMax):
            // gp : "5" must be replaced with pCluster->unitCount when this last var is defined
            return DoubleToWxString(Math::Round(
               (*pMatrix)[Data::renewableMinGenModulation][y] * 5 * pCluster->nominalCapacity, 2));
        }
    }
    return wxT("0");
}

double RenewableClusterCommonModulation::cellNumericValue(int x, int y) const
{
    if (pMatrix && (uint)y < pMatrix->height && pCluster)
    {
        assert(x < width());
        switch (x)
        {
        case Data::renewableModulationCost:
            return (*pMatrix)[Data::renewableModulationCost][y];
        case Data::renewableModulationCapacity:
            return (*pMatrix)[Data::renewableModulationCapacity][y];
        case Data::renewableMinGenModulation:
            return (*pMatrix)[Data::renewableMinGenModulation][y];
        case (Data::renewableModulationCost + Data::renewableModulationMax):
            // gp : "5" must be replaced with pCluster->unitCount when this last var is defined
            return (*pMatrix)[Data::renewableModulationCapacity][y] * 5 * pCluster->nominalCapacity;
        case (Data::renewableModulationCapacity + Data::renewableModulationMax):
            // gp : "5" must be replaced with pCluster->unitCount when this last var is defined
            return (*pMatrix)[Data::renewableMinGenModulation][y] * 5 * pCluster->nominalCapacity;
        }
    }
    return 0.;
}

void RenewableClusterCommonModulation::internalRenewableClusterChanged(
  Antares::Data::RenewableCluster* cluster)
{
    if (pCluster != cluster)
    {
        pCluster = cluster;
        if (cluster != nullptr)
            cluster->modulation.invalidate(true);
        matrix(cluster ? &cluster->modulation : nullptr);
    }
}

IRenderer::CellStyle RenewableClusterCommonModulation::cellStyle(int col, int row) const
{
    if (col > 2)
        return IRenderer::cellStyleDisabled;
    const double v = Renderer::Matrix<>::cellNumericValue(col, row);

    if (v < 0.)
        return IRenderer::cellStyleError;

    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxColour RenewableClusterCommonModulation::verticalBorderColor(int x, int y) const
{
    return (x == 2) ? Default::BorderHighlightColor() : IRenderer::verticalBorderColor(x, y);
}

wxColour RenewableClusterCommonModulation::horizontalBorderColor(int x, int y) const
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

bool RenewableClusterCommonModulation::cellValue(int x, int y, const String& value)
{
    /*
        User enters a value in the modulation grid
        - x : column number in the modulation grid
        - y : row number in the modulation grid
        - value : value given by user
    */
    if ((uint)x < 3u && pMatrix && (uint)y < pMatrix->height)
    {
        double v;
        if (!value.to(v))
            return false;
        if (v < 0. && x != Data::renewableMinGenModulation)
            v = 0.;
        switch (x)
        {
        case Data::renewableModulationCost:
        {
            (*pMatrix)[Data::renewableModulationCost][y] = v;
            pMatrix->markAsModified();
            return true;
        }

        case Data::renewableModulationCapacity:
        {
            (*pMatrix)[Data::renewableModulationCapacity][y] = v;
            pMatrix->markAsModified();
            return true;
        }
        case Data::renewableMinGenModulation:
        {
            (*pMatrix)[Data::renewableMinGenModulation][y] = v;
            pMatrix->markAsModified();
            return true;
        }
        }
    }

    return false;
}

void RenewableClusterCommonModulation::onStudyClosed()
{
    internalRenewableClusterChanged(nullptr);
    Renderer::Matrix<>::onStudyClosed();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
