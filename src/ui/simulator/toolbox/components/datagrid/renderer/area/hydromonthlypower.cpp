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

#include "hydromonthlypower.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
HydroMonthlyHours::HydroMonthlyHours(wxWindow* control,
                                     Toolbox::InputSelector::Area* notifier,
                                     HoursType type) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier), hoursType(type)
{
}

HydroMonthlyHours::~HydroMonthlyHours()
{
    destroyBoundEvents();
}

wxString HydroMonthlyHours::columnCaption(int colIndx) const
{
    if (colIndx == 0 && hoursType == HoursType::Generation)
    {
        return wxT("  Generating Max Energy  \n   (Hours at Pmax)   ");
    }
    else if (colIndx == 0 && hoursType == HoursType::Pumping)
    {
        return wxT("  Pumping Max Energy  \n   (Hours at Pmax)   ");
    }
    else
    {
        return wxEmptyString;
    }
}

wxString HydroMonthlyHours::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

double HydroMonthlyHours::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

bool HydroMonthlyHours::cellValue(int x, int y, const String& value)
{
    double v;
    if (not value.to(v))
        return MatrixAncestorType::cellValue(x, y, "0");
    if (v < 0)
        return MatrixAncestorType::cellValue(x, y, "0");
    if (v > 1000000)
        return MatrixAncestorType::cellValue(x, y, "1000000");
    int round;
    if (x == 0 || x == 2)
        round = 0;
    else
        round = 2;

    return MatrixAncestorType::cellValue(x, y, String() << Math::Round(v, round));
}

void HydroMonthlyHours::internalAreaChanged(Antares::Data::Area* area)
{
    // FIXME for some reasons, the variable study here is not properly initialized
    if (area && !study)
        study = GetCurrentStudy();

    Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
    Renderer::ARendererArea::internalAreaChanged(area);
    if (pHydro && hoursType == HoursType::Generation)
        MatrixAncestorType::matrix(&pHydro->dailyNbHoursAtGenPmax);
    else if (pHydro && hoursType == HoursType::Pumping)
        MatrixAncestorType::matrix(&pHydro->dailyNbHoursAtPumpPmax);
    else
        MatrixAncestorType::matrix(nullptr);
}

IRenderer::CellStyle HydroMonthlyHours::cellStyle(int col, int row) const
{
    if (double MaxE = MatrixAncestorType::cellNumericValue(0, row);
        col == 0 && (MaxE < 0. || MaxE > 24.))
    {
        return IRenderer::cellStyleError;
    }
    else
    {
        return IRenderer::cellStyleWithNumericCheck(col, row);
    }
}

wxString HydroMonthlyHours::rowCaption(int row) const
{
    if (!study || row >= study->calendar.maxDaysInYear)
        return wxEmptyString;
    return wxStringFromUTF8(study->calendar.text.daysYear[row]);
}

void HydroMonthlyHours::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void HydroMonthlyHours::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

// Pump

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
