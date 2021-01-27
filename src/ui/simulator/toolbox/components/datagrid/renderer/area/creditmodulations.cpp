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

#include "creditmodulations.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
CreditModulations::CreditModulations(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

CreditModulations::~CreditModulations()
{
    destroyBoundEvents();
}

wxString CreditModulations::columnCaption(int colIndx) const
{
    return wxStringFromUTF8(colIndx);
}

wxString CreditModulations::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

double CreditModulations::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

bool CreditModulations::cellValue(int x, int y, const String& value)
{
    double v;
    if (not value.to(v))
        return MatrixAncestorType::cellValue(x, y, "0");
    if (v < 0)
        return MatrixAncestorType::cellValue(x, y, "0");
    if (v > 1000000)
        return MatrixAncestorType::cellValue(x, y, "1000000");

    return MatrixAncestorType::cellValue(x, y, String() << Math::Round(v, 2));
}

void CreditModulations::internalAreaChanged(Antares::Data::Area* area)
{
    // FIXME for some reasons, the variable study here is not properly initialized
    if (area && !study)
        study = Data::Study::Current::Get();

    Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
    Renderer::ARendererArea::internalAreaChanged(area);
    if (pHydro)
        MatrixAncestorType::matrix(&pHydro->creditModulation);
    else
        MatrixAncestorType::matrix(nullptr);
}

IRenderer::CellStyle CreditModulations::cellStyle(int col, int row) const
{
    auto& matrix = pArea->hydro.creditModulation;
    double d = matrix[col][row];
    if (d < 0)
        return IRenderer::cellStyleError;
    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxString CreditModulations::rowCaption(int row) const
{
    if (row == 0)
    {
        return wxT("Generating Power");
    }
    else
    {
        return wxT("Pumping Power");
    }
}

void CreditModulations::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void CreditModulations::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
