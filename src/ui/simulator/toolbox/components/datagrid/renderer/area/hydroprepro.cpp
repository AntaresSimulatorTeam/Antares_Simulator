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

#include "hydroprepro.h"


using namespace Yuni;



namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	HydroPrepro::HydroPrepro(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
		MatrixAncestorType(control),
		Renderer::ARendererArea(control, notifier)
	{}


	HydroPrepro::~HydroPrepro()
	{
		destroyBoundEvents();
	}


	int HydroPrepro::width() const
	{
		return Data::PreproHydro::hydroPreproMax/* + 3 reservoir levels*/;
	}


	int HydroPrepro::height() const
	{
		return 12;
	}


	wxString HydroPrepro::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
			case 0: return wxT(" Expectation \n(MWh)");
			case 1: return wxT(" Std Deviation \n(MWh)");
			case 2: return wxT("   Min.   \n(MWh)");
			case 3: return wxT("   Max.   \n(MWh)");
			case 4: return wxT(" ROR Share ");
			default : return wxEmptyString;
		}
		return wxEmptyString;
	}


	wxString HydroPrepro::cellValue(int x, int y) const
	{
	/*	if (x >= 5 && x < 8)
		{
			if (!pArea)
				return wxString();
			x -= 5;
			auto& matrix = pArea->hydro.reservoirLevel;
			return ((uint) x < matrix.width && (uint) y < matrix.height)
				? DoubleToWxString(100. * matrix[x][y])
				: wxString();
		}*/
		return MatrixAncestorType::cellValue(x, y);
	}


	double HydroPrepro::cellNumericValue(int x, int y) const
	{/*if (x >= 5 && x < 8)
		{
			if (!pArea)
				return 0.;
			x -= 5;
			auto& matrix = pArea->hydro.reservoirLevel;
			return ((uint) x < matrix.width && (uint) y < matrix.height)
				? matrix[x][y] * 100.
				: 0.;
		}*/
		return MatrixAncestorType::cellNumericValue(x, y);
	}


	bool HydroPrepro::cellValue(int x, int y, const String& value)
	{
		double v;
		if (!value.to(v))
			return false;

		// reservoir level
		/*if (x >= 5 && x < 8)
		{
			if (!pArea)
				return false;
			x -= 5;
			auto& matrix = pArea->hydro.reservoirLevel;
			if ((uint) x < matrix.width && (uint) y < matrix.height)
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
		}*/
		if (v < 0.)
			return MatrixAncestorType::cellValue(x, y, "0");
		return MatrixAncestorType::cellValue(x, y, value);
	}




	void HydroPrepro::internalAreaChanged(Antares::Data::Area* area)
	{
		// FIXME for some reasons, the variable study here is not properly initialized
		if (area && !study)
			study = Data::Study::Current::Get();

		auto* pPreproHydro = (area) ? area->hydro.prepro : nullptr;
		Renderer::ARendererArea::internalAreaChanged(area);
		if (pPreproHydro)
		{
			//area->hydro.reservoirLevel.invalidate(true);
			MatrixAncestorType::matrix(&pPreproHydro->data);
		}
		else
		{
			MatrixAncestorType::matrix(nullptr);
		}
	}



	IRenderer::CellStyle HydroPrepro::cellStyle(int col, int row) const
	{
		if (!pMatrix || (uint) Data::PreproHydro::hydroPreproMax > pMatrix->width || (uint) row >= pMatrix->height)
			return IRenderer::cellStyleWithNumericCheck(col, row);

		switch (col)
		{
			case 0:
				{
					if ((*pMatrix)[Data::PreproHydro::expectation][row] < 0.)
						return IRenderer::cellStyleError;
					break;
				}
			case 1:
				{
					if ((*pMatrix)[Data::PreproHydro::stdDeviation][row] < 0.)
						return IRenderer::cellStyleError;
					break;
				}

			case 2: // min >= 0
				{
					if ((*pMatrix)[Data::PreproHydro::minimumEnergy][row] < 0.)
						return IRenderer::cellStyleError;
					break;
				}
			case 3: // min <= max
				{
					if ((*pMatrix)[Data::PreproHydro::minimumEnergy][row] > pMatrix->entry[Data::PreproHydro::maximumEnergy][row])
						return IRenderer::cellStyleError;
					break;
				}
			case 4: // 0 <= ROR <= 1
				{
					double d = (*pMatrix)[Data::PreproHydro::powerOverWater][row];
					if (d < 0. || d > 1.)
						return IRenderer::cellStyleError;
					break;
				}
		}
		return IRenderer::cellStyleWithNumericCheck(col, row);
	}


	wxString HydroPrepro::rowCaption(int rowIndx) const
	{
		return wxStringFromUTF8(Date::MonthToString(rowIndx));
	}


	bool HydroPrepro::valid() const
	{
		return MatrixAncestorType::valid();
	}


	bool HydroPrepro::circularShiftRowsUntilDate(MonthName month, uint daymonth)
	{
		if (pArea)
			pArea->hydro.reservoirLevel.circularShiftRows(month);
		return MatrixAncestorType::circularShiftRowsUntilDate(month, daymonth);
	}


	void HydroPrepro::onStudyClosed()
	{
		MatrixAncestorType::onStudyClosed();
		Renderer::ARendererArea::onStudyClosed();
	}


	void HydroPrepro::onStudyLoaded()
	{
		MatrixAncestorType::onStudyLoaded();
		Renderer::ARendererArea::onStudyLoaded();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

