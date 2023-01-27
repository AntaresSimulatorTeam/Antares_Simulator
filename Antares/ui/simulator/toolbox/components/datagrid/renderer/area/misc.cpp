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
    this->matrix((area) ? &(area->miscGen) : NULL);
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
