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

#include "correlation.h"
#include "../../../../application/study.h"
#include "../component.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
wxString CorrelationMatrix::IDatasource::name(uint i) const
{
    auto* a = this->at(i);
    return (a) ? wxStringFromUTF8(a->name) : wxString();
}

CorrelationMatrix::CorrelationMatrix() : pMatrix(nullptr), pControl(nullptr)
{
}

CorrelationMatrix::~CorrelationMatrix()
{
    destroyBoundEvents();
}

void CorrelationMatrix::onUpdate()
{
}

int CorrelationMatrix::width() const
{
    return pSource ? pSource->size() : 0;
}

int CorrelationMatrix::height() const
{
    return pSource ? pSource->size() : 0;
}

bool CorrelationMatrix::valid() const
{
    return (pSource and pSource->size() != 0) and pMatrix and !pMatrix->empty();
}

wxString CorrelationMatrix::columnCaption(int colIndx) const
{
    return (pSource) ? pSource->name((uint)colIndx) : wxString();
}

wxString CorrelationMatrix::rowCaption(int rowIndx) const
{
    return (pSource) ? wxString() << pSource->name((uint)rowIndx) : wxString();
}

IRenderer::CellStyle CorrelationMatrix::cellStyle(int col, int row) const
{
    double d = cellNumericValue(col, row);
    if (d > 100. || d < -100.)
        return IRenderer::cellStyleError;

    if (pSource and pSource->cellStyle(col, row) == IRenderer::cellStyleCustom)
        return IRenderer::cellStyleCustom;
    if (col == row)
        return IRenderer::cellStyleDisabled;
    return (Yuni::Math::Zero(d))
             ? ((row > col) ? IRenderer::cellStyleDefaultAlternateDisabled
                            : IRenderer::cellStyleDefaultDisabled)
             : ((row > col) ? IRenderer::cellStyleDefaultAlternate : IRenderer::cellStyleDefault);
}

wxColour CorrelationMatrix::cellBackgroundColor(int col, int row) const
{
    return pSource ? pSource->cellBackgroundColor(col, row) : wxColor();
}

wxColour CorrelationMatrix::cellTextColor(int col, int row) const
{
    return (Math::Zero(cellNumericValue(col, row)))
             ? wxColour(100, 100, 100)
             : (pSource ? pSource->cellTextColor(col, row) : wxColor());
}

void CorrelationMatrix::datasource(const CorrelationMatrix::IDatasource::Ptr& s)
{
    pSource = s;
}

wxString CorrelationMatrix::cellValue(int x, int y) const
{
    if (x == y)
        return wxT("100.0");

    if (pMatrix and !(!study))
    {
        assert(pMatrix->width <= study->areas.size());
        assert(pMatrix->height <= study->areas.size());
        const uint nX = pSource->areaIndex(x);
        const uint nY = pSource->areaIndex(y);
        if (nX < pMatrix->width and nY < pMatrix->height)
            return DoubleToWxString(100 * pMatrix->entry[nX][nY]);
    }
    return wxEmptyString;
}

double CorrelationMatrix::cellNumericValue(int x, int y) const
{
    if (x == y)
        return 1.;
    if (pMatrix and !(!study))
    {
        assert(pMatrix->width <= study->areas.size());
        assert(pMatrix->height <= study->areas.size());
        const uint nX = pSource->areaIndex(x);
        const uint nY = pSource->areaIndex(y);
        if (nX < pMatrix->width and nY < pMatrix->height)
            return pMatrix->entry[nX][nY] * 100.0;
    }
    return 0.;
}

bool CorrelationMatrix::cellValue(int x, int y, const String& value)
{
    if (x == y || !pMatrix || !pSource)
        return false;
    double d;
    if (!value.to(d))
        return false;
    d /= 100.;

    uint ax = pSource->areaIndex(x);
    uint ay = pSource->areaIndex(y);

    if (ax < pMatrix->width and ay < pMatrix->height and not Math::Equals((*pMatrix)[ax][ay], d))
    {
        pMatrix->entry[ax][ay] = d;
        pMatrix->entry[ay][ax] = d;
        pMatrix->markAsModified();
        MarkTheStudyAsModified(study);
    }

    if (pControl)
        pControl->Refresh();
    return true;
}

void CorrelationMatrix::onStudyClosed()
{
    pMatrix = nullptr;
    pSource = nullptr;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
