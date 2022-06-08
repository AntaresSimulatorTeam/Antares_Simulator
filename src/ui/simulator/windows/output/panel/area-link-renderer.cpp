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

#include "area-link-renderer.h"
#include "../../../toolbox/components/datagrid/component.h"

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
// convenient alias
using IRenderer = Antares::Component::Datagrid::Renderer::IRenderer;

AreaLinkRenderer::AreaLinkRenderer(OutputViewerComponent* component,
                                   const Antares::Matrix<CellType>* matrix) :
 pComponent(component),
 pGrid(nullptr),
 pMatrix(matrix),
 pSelectionCellX((uint)-1),
 pSelectionCellY((uint)-1)
{
    assert(pMatrix && "invalid matrix");

    if (!pMatrix) // just in case
    {
        pWidth = 0;
        pHeight = 0;
    }
    else
    {
        MemoryFlushLocker memoryLocker;

        pWidth = pMatrix->width;
        pHeight = pMatrix->height;
        if (pWidth > 2)
            pWidth -= 2;
        if (pHeight > 4 + 3)
            pHeight -= 4 + 3;
        else
            pHeight = 0;

        if (pWidth && pHeight)
        {
            pColNames.resize(pWidth);
            pColToUseForRowCaption.resize(pWidth + 2);

            if (pMatrix->height >= 7)
            {
                for (uint x = 0; x != pWidth; ++x)
                {
                    if (x + 2 >= pMatrix->width)
                        continue;
                    wxString& caption = pColNames[x];
                    uint countNonEmpty = 0;

                    // 1srt line
                    {
                        auto& cell = (*pMatrix)[x + 2][4];
                        if (not cell.empty())
                        {
                            caption << wxT("     ") << wxStringFromUTF8(cell) << wxT("    \n");
                            ++countNonEmpty;
                        }
                    }
                    // 2nd line
                    {
                        const MatrixType::Type& cell = (*pMatrix)[x + 2][5];
                        if (not cell.empty())
                        {
                            caption << wxStringFromUTF8(cell) << wxT('\n');
                            ++countNonEmpty;
                        }
                    }
                    // 3nd line
                    {
                        const MatrixType::Type& cell = (*pMatrix)[x + 2][6];
                        if (not cell.empty())
                        {
                            ++countNonEmpty;
                            if (cell != "EXP")
                                caption << wxStringFromUTF8(cell);
                            else
                                caption << wxT("exp");
                        }
                    }

                    // The check on "year" is required for the view variable-per-variable
                    pColToUseForRowCaption[x + 2] = (countNonEmpty <= 1);
                }
            }

            // It may happen that we have a width greater than the width that
            // really interests us. The header always has 4 columns, but we may have
            // only 3 useful columns for displaying data
            // shrinking :
            if (pMatrix->height > 4)
            {
                MatrixType::Type copy;
                uint rW = pMatrix->width;
                for (uint x = rW - 1; x > 0; --x)
                {
                    copy = (*pMatrix)[x][4];
                    copy.trim();
                    if (!copy)
                        --rW;
                    else
                        break;
                }
                if (rW != pMatrix->width)
                {
                    const uint diff = pMatrix->width - rW;
                    if (pWidth >= diff)
                        pWidth -= diff;
                    else
                        pWidth = 0;
                }
            }
        }

        logs.debug() << "got matrix " << pMatrix->width << 'x' << pMatrix->height
                     << ", shrinked to " << pWidth << 'x' << pHeight;

        // early cleanup
        if (!pWidth || !pHeight)
        {
            pWidth = 0;
            pHeight = 0;
            delete pMatrix;
            pMatrix = nullptr;
            pColNames.clear();
            pColToUseForRowCaption.clear();
        }

    } // if pMatrix
}

AreaLinkRenderer::~AreaLinkRenderer()
{
    destroyBoundEvents();
    delete pMatrix;
}

int AreaLinkRenderer::width() const
{
    return pWidth;
}

int AreaLinkRenderer::height() const
{
    return pHeight;
}

bool AreaLinkRenderer::valid() const
{
    return (NULL != pMatrix);
}

wxString AreaLinkRenderer::columnCaption(int colIndx) const
{
    if (static_cast<uint>(colIndx) < pColNames.size())
        return pColNames[colIndx];
    return wxEmptyString;
}

wxString AreaLinkRenderer::rowCaption(int y) const
{
    y += 4 + 3;
    if (!pMatrix || (uint)y >= pMatrix->height || pMatrix->width < 2)
        return wxEmptyString;
    wxString s;
    for (uint i = 2; i < pColToUseForRowCaption.size(); ++i)
    {
        assert(i < pMatrix->width);
        if (!pColToUseForRowCaption[i])
            break;
        s << wxStringFromUTF8((*pMatrix)[i][y]) << wxT(' ');
    }
    s << wxT("  ") << wxStringFromUTF8((*pMatrix)[1][y]);
    return s;
}

IRenderer::CellStyle AreaLinkRenderer::cellStyle(int, int) const
{
    return IRenderer::cellStyleCustom;
}

wxColour AreaLinkRenderer::cellBackgroundColor(int x, int y) const
{
    if (!pMatrix || ((uint)x == pSelectionCellX && (uint)y == pSelectionCellY))
        return wxColour(229, 206, 206);

    x += 2;
    y += 4 + 3;
    if ((uint)x < pMatrix->width && (uint)y < pMatrix->height)
    {
        auto& text = (*pMatrix)[x][y];
        if (not text.empty() && text[0] == '-' && not text.startsWith("-0"))
            return wxColour(235, 235, 245);
    }

    return wxColour(245, 245, 245);
}

wxColour AreaLinkRenderer::cellTextColor(int x, int y) const
{
    if (!pMatrix || ((uint)x == pSelectionCellX && (uint)y == pSelectionCellY))
        return wxColour(0, 0, 0);

    x += 2;
    y += 4 + 3;
    if ((uint)x < pMatrix->width && (uint)y < pMatrix->height)
    {
        auto& text = (*pMatrix)[x][y];

        if (text == "NaN" || text == "+inf" || text == "-inf")
            return wxColour(245, 240, 240);

        if (not text.empty())
        {
            switch (text[0])
            {
            case '0':
                return wxColour(164, 167, 183);
            case '-':
                return wxColour(82, 84, 100);
            }
        }
    }
    return wxColour(49, 49, 49);
}

wxColour AreaLinkRenderer::verticalBorderColor(int x, int y) const
{
    if ((uint)x == pSelectionCellX && (uint)y == pSelectionCellY)
        return wxColour(206, 185, 185);
    return IRenderer::verticalBorderColor(x, y);
}

wxString AreaLinkRenderer::cellValue(int x, int y) const
{
    if (!pMatrix)
        return wxEmptyString;

    // recompute the good offset
    x += 2;
    y += 4 + 3;
    if ((uint)x >= pMatrix->width || (uint)y >= pMatrix->height)
        return wxEmptyString;

    auto& text = (*pMatrix)[x][y];
    if (!text)
        return wxEmptyString;
    return wxStringFromUTF8(text);
}

int AreaLinkRenderer::cellAlignment(int, int) const
{
    return 1;
}

double AreaLinkRenderer::cellNumericValue(int x, int y) const
{
    if (!pMatrix)
        return 0.;
    // recompute the good offset
    x += 2;
    y += 4 + 3;
    if ((uint)x >= pMatrix->width || (uint)y >= pMatrix->height)
        return 0.;

    auto& text = (*pMatrix)[x][y];
    if (!text)
        return 0.;

    double d;
    if (text.to(d))
        return d;
    return std::numeric_limits<double>::quiet_NaN();
}

bool AreaLinkRenderer::cellValue(int /*x*/, int /*y*/, const String& /*value*/)
{
    return false;
}

void AreaLinkRenderer::onScroll()
{
    if (pComponent && pGrid)
        pComponent->scrollAllPanels(pGrid->gridAsScrolledWindow());
}

void AreaLinkRenderer::onSelectCell(unsigned x, unsigned y)
{
    if (pComponent)
        pComponent->selectCellAllPanels(x, y);
}

void AreaLinkRenderer::onExternalSelectCell(unsigned x, unsigned y)
{
    pSelectionCellX = x;
    pSelectionCellY = y;
    if (pGrid)
        pGrid->Refresh();
}

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares
