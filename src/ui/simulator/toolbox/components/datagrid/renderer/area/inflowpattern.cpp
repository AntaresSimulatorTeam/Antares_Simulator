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

#include "inflowpattern.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
InflowPattern::InflowPattern(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

InflowPattern::~InflowPattern()
{
    destroyBoundEvents();
}

int InflowPattern::width() const
{
    return 1;
}

int InflowPattern::height() const
{
    return DAYS_PER_YEAR;
}

wxString InflowPattern::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case 0:
        return wxT(" Inflow Pattern \n (X)");
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

wxString InflowPattern::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

double InflowPattern::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

bool InflowPattern::cellValue(int x, int y, const String& value)
{
    double v;
    if (!value.to(v))
        return false;
    if (v < 0)
        return false;

    return MatrixAncestorType::cellValue(x, y, value);
}

void InflowPattern::internalAreaChanged(Antares::Data::Area* area)
{
    // FIXME for some reasons, the variable study here is not properly initialized
    if (area && !study)
        study = GetCurrentStudy();

    Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
    Renderer::ARendererArea::internalAreaChanged(area);
    if (pHydro)
    {
        MatrixAncestorType::matrix(&pHydro->inflowPattern);
    }
    else
    {
        MatrixAncestorType::matrix(nullptr);
    }
}

IRenderer::CellStyle InflowPattern::cellStyle(int col, int row) const
{
    if (!pMatrix || (uint)Data::PreproHydro::hydroPreproMax > pMatrix->width
        || (uint)row >= pMatrix->height)
        return IRenderer::cellStyleWithNumericCheck(col, row);

    if (col == 0)
    {
        if ((*pMatrix)[Data::PreproHydro::expectation][row] < 0.)
            return IRenderer::cellStyleError;
    }
    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxString InflowPattern::rowCaption(int rowIndx) const
{
    if (!study || rowIndx >= study->calendar.maxDaysInYear)
        return wxEmptyString;
    return wxStringFromUTF8(study->calendar.text.daysYear[rowIndx]);
}

bool InflowPattern::valid() const
{
    return MatrixAncestorType::valid();
}

/*bool InflowPattern::circularShiftRowsUntilDate(MonthName month, uint daymonth)
{
        if (pArea)
                pArea->hydro.inflowPattern.circularShiftRows(month, daymonth);
        return MatrixAncestorType::circularShiftRowsUntilDate(month, daymonth);
}*/

void InflowPattern::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void InflowPattern::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
