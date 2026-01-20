// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "watervalues.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
WaterValues::WaterValues(wxWindow* control, Toolbox::InputSelector::Area* notifier):
    MatrixAncestorType(control),
    Renderer::ARendererArea(control, notifier)
{
}

WaterValues::~WaterValues()
{
    destroyBoundEvents();
}

int WaterValues::width() const
{
    return 101;
}

int WaterValues::height() const
{
    return DAYS_PER_YEAR;
}

wxString WaterValues::columnCaption(int colIndx) const
{
    if (colIndx >= 0 && colIndx < 101)
    {
        wxString ret = "";
        ret << colIndx << " % ";
        return ret;
    }

    return wxEmptyString;
}

wxString WaterValues::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

double WaterValues::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

bool WaterValues::cellValue(int x, int y, const String& value)
{
    double v;
    if (!value.to(v))
    {
        return false;
    }

    return MatrixAncestorType::cellValue(x, y, value);
}

void WaterValues::internalAreaChanged(Antares::Data::Area* area)
{
    // FIXME for some reasons, the variable study here is not properly initialized
    if (area && !study)
    {
        study = GetCurrentStudy();
    }

    Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
    Renderer::ARendererArea::internalAreaChanged(area);
    if (pHydro)
    {
        MatrixAncestorType::matrix(&pHydro->waterValues);
    }
    else
    {
        MatrixAncestorType::matrix(nullptr);
    }
}

IRenderer::CellStyle WaterValues::cellStyle(int col, int row) const
{
    if (!pMatrix || (uint)Data::PreproHydro::hydroPreproMax > pMatrix->width
        || (uint)row >= pMatrix->height)
    {
        return IRenderer::cellStyleWithNumericCheck(col, row);
    }

    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxString WaterValues::rowCaption(int rowIndx) const
{
    if (!study || rowIndx >= study->calendar.maxDaysInYear)
    {
        return wxEmptyString;
    }
    return wxStringFromUTF8(study->calendar.text.daysYear[rowIndx]);
}

bool WaterValues::valid() const
{
    return MatrixAncestorType::valid();
}

/*bool WaterValues::circularShiftRowsUntilDate(MonthName month, uint daymonth)
{
        if (pArea)
                pArea->hydro.waterValues.circularShiftRows(month, daymonth);
        return MatrixAncestorType::circularShiftRowsUntilDate(month, daymonth);
}*/

void WaterValues::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void WaterValues::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Antares::Component::Datagrid::Renderer
