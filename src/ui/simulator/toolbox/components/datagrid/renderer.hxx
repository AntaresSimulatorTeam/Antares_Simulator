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
#include "renderer.h"
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HXX__

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
inline int IRenderer::internalWidth() const
{
    return width();
}

inline int IRenderer::internalHeight() const
{
    return height();
}

inline bool IRenderer::ensureDataAreLoaded()
{
    return false;
}

inline IRenderer::CellStyle IRenderer::cellStyleWithNumericCheck(int x, int y) const
{
    return Yuni::Math::Zero(cellNumericValue(x, y))
             ? ((y % 2) ? cellStyleDefaultAlternateDisabled : cellStyleDefaultDisabled)
             : ((y % 2) ? cellStyleDefaultAlternate : cellStyleDefault);
}

inline IRenderer::CellStyle IRenderer::cellStyle(int, int y) const
{
    return ((y % 2) ? cellStyleDefaultAlternate : cellStyleDefault);
}

inline wxColour IRenderer::cellBackgroundColor(int, int) const
{
    return wxColour(255, 255, 255);
}

inline wxColour IRenderer::cellTextColor(int, int) const
{
    return wxColour(255, 0, 0);
}

inline bool IRenderer::onMatrixResize(uint, uint, uint&, uint&)
{
    return true;
}

inline wxColour IRenderer::verticalBorderColor(int, int) const
{
    return Default::BorderColor();
}

inline wxColour IRenderer::horizontalBorderColor(int, int) const
{
    return Default::BorderColor();
}

inline uint IRenderer::maxWidthResize() const
{
    return 0;
}

inline uint IRenderer::maxHeightResize() const
{
    return 0;
}

inline int IRenderer::cellAlignment(int, int) const
{
    return 0;
}

inline int IRenderer::columnWidthCustom(int) const
{
    return 0;
}

inline void IRenderer::hintForColumnWidth(int, wxString& out) const
{
    out.Clear();
}

inline wxString IRenderer::asString(int x, int y) const
{
    return cellValue(x, y);
}

inline double IRenderer::asDouble(int x, int y) const
{
    return cellNumericValue(x, y);
}

inline Date::Precision IRenderer::precision()
{
    return Date::stepAny;
}

inline bool IRenderer::circularShiftRowsUntilDate(MonthName, uint)
{
    return false; // not implemented
}

inline void IRenderer::applyLayerFiltering(size_t /* layerID */, VGridHelper* /* gridHelper */)
{
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HXX__
