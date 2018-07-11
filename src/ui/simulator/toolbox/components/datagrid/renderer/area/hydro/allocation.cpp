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

#include "allocation.h"
#include "../../../../../../application/study.h"
#include <yuni/core/math.h>

using namespace Yuni;



namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	HydroAllocation::HydroAllocation()
	{}


	HydroAllocation::~HydroAllocation()
	{
	}


	bool HydroAllocation::valid() const
	{
		return Data::Study::Current::Valid();
	}


	int HydroAllocation::width() const
	{
		auto study = Data::Study::Current::Get();
		return !(!study) ? study->areas.size() : 0;
	}


	int HydroAllocation::height() const
	{
		auto study = Data::Study::Current::Get();
		return !(!study) ? study->areas.size() : 0;
	}


	wxString HydroAllocation::columnCaption(int colIndx) const
	{
		auto study = Data::Study::Current::Get();
		return (study && (uint) colIndx < study->areas.size())
			? wxStringFromUTF8(" hydro \n " + study->areas[colIndx]->name) 
			: wxString(); // MBO 21/05/2014 - #22
	}


	wxString HydroAllocation::rowCaption(int rowIndx) const
	{
		auto study = Data::Study::Current::Get();
		return (study && (uint) rowIndx < study->areas.size())
			? wxStringFromUTF8("load " + study->areas[rowIndx]->name)
			: wxString();  // MBO 21/05/2014 - #22
	}


	wxString HydroAllocation::cellValue(int x, int y) const
	{
		auto study = Data::Study::Current::Get();
		if (!(!study))
		{
			auto colArea = (uint) x;
			auto rowArea = (uint) y;
			auto areaCount = study->areas.size();
			if (colArea < areaCount && rowArea < areaCount)
			{
				auto& area   = *(study->areas[colArea]);
				auto& target = *(study->areas[rowArea]);
				double v = area.hydro.allocation[target];
				if (!Math::Zero(v))
					return DoubleToWxString(v);
			}
		}
		static const wxString zero = wxT("0");
		return zero;
	}


	double HydroAllocation::cellNumericValue(int x, int y) const
	{
		auto study = Data::Study::Current::Get();
		if (!(!study))
		{
			auto colArea = (uint) x;
			auto rowArea = (uint) y;
			auto areaCount = study->areas.size();
			if (colArea < areaCount && rowArea < areaCount)
			{
				auto& area = *(study->areas[colArea]);
				auto& target = *(study->areas[rowArea]);
				return area.hydro.allocation[target];
			}
		}
		return 0;
	}


	bool HydroAllocation::cellValue(int x, int y, const String& value)
	{
		double v;
		if (!value.to(v))
			return false;

		auto study = Data::Study::Current::Get();
		if (!(!study))
		{
			auto colArea = (uint) x;
			auto rowArea = (uint) y;
			auto areaCount = study->areas.size();
			if (colArea < areaCount && rowArea < areaCount)
			{
				auto& area   = *(study->areas[colArea]);
				auto& target = *(study->areas[rowArea]);
				area.hydro.allocation.fromArea(target, v);
				return true;
			}
		}
		return false;
	}



	IRenderer::CellStyle HydroAllocation::cellStyle(int col, int row) const
	{
		if (Math::Zero(cellNumericValue(col, row)))
		{
			return ! (col % 2)
				? IRenderer::cellStyleDefaultCenterDisabled
				: IRenderer::cellStyleDefaultCenterAlternateDisabled;
		}
		else
		{
			return IRenderer::cellStyleConstraintWeight;
		}
	}






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

