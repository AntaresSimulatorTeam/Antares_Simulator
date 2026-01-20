// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__

namespace Antares::Component::Datagrid
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

} // namespace Antares::Component::Datagrid

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__
