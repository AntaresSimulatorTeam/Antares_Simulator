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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__
# define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_HXX__


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
		assert(col < (uint) indicesCols.size());
		return indicesCols[col];
	}


	inline int VGridHelper::realRow(uint row) const
	{
		assert(row < (uint) indicesRows.size());
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
