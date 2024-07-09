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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "misc.h"
#include <yuni/core/math.h>
#include <antares/study/area/constants.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
Misc::Misc(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 Renderer::Matrix<>(control), Renderer::ARendererArea(control, notifier)
{
}

Misc::~Misc()
{
    destroyBoundEvents();
}

wxString Misc::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case Data::fhhCHP:
        return wxT("     CHP      ");
    case Data::fhhBioMass:
        return wxT("    Bio Mass  ");
    case Data::fhhBioGaz:
        return wxT("    Bio Gaz   ");
    case Data::fhhWaste:
        return wxT("     Waste    ");
    case Data::fhhGeoThermal:
        return wxT("   GeoThermal ");
    case Data::fhhOther:
        return wxT("     Other    ");
    case Data::fhhRowBalance:
        return wxT(" ROW Balance  ");
    case Data::fhhPSP:
        return wxT("      PSP     ");
    case Data::fhhMax:
        return wxT("       Total      ");
    }
    return wxEmptyString;
}

wxString Misc::cellValue(int x, int y) const
{
    if (!pMatrix || x != Data::fhhMax)
        return Renderer::Matrix<>::cellValue(x, y);

    double total = 0.;
    if (pMatrix->width && pMatrix->height)
    {
        for (uint i = 0; i != pMatrix->width; ++i)
            total += (*pMatrix)[i][y];
        return DoubleToWxString(Math::Round(total, 2));
    }
    return wxString(wxT("0.0"));
}

double Misc::cellNumericValue(int x, int y) const
{
    if (!pMatrix || x != Data::fhhMax)
        return Renderer::Matrix<>::cellNumericValue(x, y);

    double total = 0.;
    if (pMatrix->width && pMatrix->height)
    {
        for (uint i = 0; i != pMatrix->width; ++i)
            total += (*pMatrix)[i][y];
    }
    return total;
}

void Misc::internalAreaChanged(Antares::Data::Area* area)
{
    this->matrix((area) ? &(area->miscGen) : nullptr);
    Renderer::ARendererArea::internalAreaChanged(area);
}

IRenderer::CellStyle Misc::cellStyle(int col, int row) const
{
    if (col < Data::fhhPSP && cellNumericValue(col, row) < 0.)
        return IRenderer::cellStyleError;
    return (col >= Data::fhhMax) ? IRenderer::cellStyleDisabled
                                 : Renderer::Matrix<>::cellStyle(col, row);
}

wxColour Misc::horizontalBorderColor(int x, int y) const
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

wxColour Misc::verticalBorderColor(int x, int y) const
{
    return (x == Data::fhhRowBalance) ? Default::BorderHighlightColor()
                                      : IRenderer::verticalBorderColor(x, y);
}

void Misc::onStudyClosed()
{
    Renderer::Matrix<>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void Misc::onStudyLoaded()
{
    Renderer::Matrix<>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
