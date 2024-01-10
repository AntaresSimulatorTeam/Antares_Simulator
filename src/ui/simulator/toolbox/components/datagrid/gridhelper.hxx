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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__

namespace Antares
{
namespace Component
{
namespace Datagrid
{
inline int VGridHelper::GetNumberRows()
{
    return virtualSize.y;
}

inline int VGridHelper::GetNumberCols()
{
    return virtualSize.x;
}

inline double VGridHelper::GetValueAsDouble(int row, int col)
{
    return GetNumericValue(row, col);
}

inline bool VGridHelper::GetValueAsBool(int row, int col)
{
    return !Yuni::Math::Zero(GetValueAsDouble(row, col));
}

inline bool VGridHelper::IsEmptyCell(int, int)
{
    return false;
}

inline int VGridHelper::realCol(uint col) const
{
    assert(col < (uint)indicesCols.size());
    return indicesCols[col];
}

inline int VGridHelper::realRow(uint row) const
{
    assert(row < (uint)indicesRows.size());
    return indicesRows[row];
}

inline bool VGridHelper::CanValueAs(int, int, const wxString&)
{
    return true;
}

inline Renderer::IRenderer* VGridHelper::renderer() const
{
    return pRenderer;
}

} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__
