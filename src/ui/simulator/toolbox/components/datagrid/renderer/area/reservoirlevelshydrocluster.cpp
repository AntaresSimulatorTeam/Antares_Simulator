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

#include "reservoirlevelshydrocluster.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
ReservoirLevelsHydrocluster::ReservoirLevelsHydrocluster(wxWindow* control, 
Toolbox::InputSelector::HydroclusterCluster* notifier) :
 MatrixAncestorType(control)
{
    if (notifier)
        notifier->onClusterChanged.connect(
          this, &ReservoirLevelsHydrocluster::internalHydroclusterClusterChanged);    
}

ReservoirLevelsHydrocluster::~ReservoirLevelsHydrocluster()
{
    destroyBoundEvents();
}

int ReservoirLevelsHydrocluster::width() const
{
    return 3;
}

int ReservoirLevelsHydrocluster::height() const
{
    return DAYS_PER_YEAR;
}

wxString ReservoirLevelsHydrocluster::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case 0:
        return wxT(" Lev Low \n (%)");
    case 1:
        return wxT(" Lev Avg \n (%)");
    case 2:
        return wxT(" Lev High \n (%)");
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

wxString ReservoirLevelsHydrocluster::cellValue(int x, int y) const
{
    if (!pHydroclusterCluster)
        return wxString();
    auto& matrix = pHydroclusterCluster->reservoirLevel;
    return ((uint)x < matrix.width && (uint)y < matrix.height)
             ? DoubleToWxString(100. * matrix[x][y])
             : wxString();
}

double ReservoirLevelsHydrocluster::cellNumericValue(int x, int y) const
{
    if (!pHydroclusterCluster)
        return 0.;
    auto& matrix = pHydroclusterCluster->reservoirLevel;
    return ((uint)x < matrix.width && (uint)y < matrix.height) ? matrix[x][y] * 100. : 0.;
}

bool ReservoirLevelsHydrocluster::cellValue(int x, int y, const String& value)
{
    if (!pHydroclusterCluster)
        return false;
    auto& matrix = pHydroclusterCluster->reservoirLevel;
    if ((uint)x < matrix.width && (uint)y < matrix.height)
    {
        double v;
        if (value.to(v))
        {
            v = Math::Round(v / 100., 3);
            if (v < 0.)
                v = 0.;
            if (v > 1.)
                v = 1;
            matrix[x][y] = v;
            matrix.markAsModified();
            return true;
        }
    }
    return false;
}

void ReservoirLevelsHydrocluster::internalHydroclusterClusterChanged(Antares::Data::HydroclusterCluster* hydroclusterCluster)
{   
    if (hydroclusterCluster && !study)
        study = Data::Study::Current::Get();
    pHydroclusterCluster = hydroclusterCluster;
    // onClusterChanged(hydroclusterCluster);
    if (hydroclusterCluster)
    {
        MatrixAncestorType::matrix(&hydroclusterCluster->reservoirLevel);

    }
    else
    {
        MatrixAncestorType::matrix(nullptr);
    }
}

IRenderer::CellStyle ReservoirLevelsHydrocluster::cellStyle(int col, int row) const
{
    if (!pMatrix || 3 > pMatrix->width || (uint)row >= pMatrix->height)
        return IRenderer::cellStyleWithNumericCheck(col, row);

    switch (col)
    {
    case 0:
    case 1:
    case 2:
    {
        if (pHydroclusterCluster)
        {
            // auto& matrix = pArea->hydro.reservoirLevel;
            auto& matrix = pHydroclusterCluster->reservoirLevel;//&hydroclusterCluster->reservoirLevel
            if ((uint)row < matrix.height)
            {
                double d = matrix[col][row];
                double min = matrix[0][row];
                if (d < 0 || d > 1.)
                    return IRenderer::cellStyleError;
                if (d < min)
                    return IRenderer::cellStyleError;
                // We can use IRenderer::cellStyleWithNu... since this method
                // as no mean to know data from hydro.reservoirLevel
                return Math::Zero(d) ? ((row % 2) ? cellStyleDefaultAlternateDisabled
                                                  : cellStyleDefaultDisabled)
                                     : ((row % 2) ? cellStyleDefaultAlternate : cellStyleDefault);
            }
        }
        return ((row % 2) ? cellStyleDefaultAlternate : cellStyleDefault);
    }
    }
    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxString ReservoirLevelsHydrocluster::rowCaption(int rowIndx) const
{
    if (!study || rowIndx >= study->calendar.maxDaysInYear)
        return wxEmptyString;
    return wxStringFromUTF8(study->calendar.text.daysYear[rowIndx]);
}

bool ReservoirLevelsHydrocluster::valid() const
{
    return MatrixAncestorType::valid();
}

/*bool ReservoirLevels::circularShiftRowsUntilDate(MonthName month, uint daymonth)
{
        if (pHydroclusterCluster)
                pHydroclusterCluster->hydro.reservoirLevel.circularShiftRows(month, daymonth);
        return MatrixAncestorType::circularShiftRowsUntilDate(month, daymonth);
}
*/

void ReservoirLevelsHydrocluster::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    // Renderer::ARendererArea::onStudyClosed();
}

void ReservoirLevelsHydrocluster::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    // Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
