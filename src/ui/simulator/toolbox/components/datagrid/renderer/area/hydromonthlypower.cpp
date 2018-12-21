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

#include "hydromonthlypower.h"

using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	HydroMonthlyPower::HydroMonthlyPower(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
		MatrixAncestorType(control),
		Renderer::ARendererArea(control, notifier)
	{}


	HydroMonthlyPower::~HydroMonthlyPower()
	{
		destroyBoundEvents();
	}


	wxString HydroMonthlyPower::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
			case Data::PartHydro::genMaxP: return wxT("  Generating Max Power  \n   (MW)   ");
			case Data::PartHydro::genMaxE: return wxT("  Generating Max Energy  \n   (Hours at Pmax)   ");
			case Data::PartHydro::pumpMaxP: return wxT(" Pumping Max Power  \n   (MW)   ");
			case Data::PartHydro::pumpMaxE: return wxT("  Pumping Max Energy \n   (Hours at Pmax)   ");
			default : return wxEmptyString;
		}
		return wxEmptyString;
	}


	wxString HydroMonthlyPower::cellValue(int x, int y) const
	{
		return MatrixAncestorType::cellValue(x, y);
	}


	double HydroMonthlyPower::cellNumericValue(int x, int y) const
	{
		return MatrixAncestorType::cellNumericValue(x, y);
	}


	bool HydroMonthlyPower::cellValue(int x, int y, const String& value)
	{
		double v;
		if (not value.to(v))
			return MatrixAncestorType::cellValue(x, y, "0");
		if (v < 0)
			return MatrixAncestorType::cellValue(x, y, "0");
		if (v > 1000000)
			return MatrixAncestorType::cellValue(x, y, "1000000");
		int round;
		if (x == 0 || x == 2)
			round = 0;
		else
			round = 2;

		return MatrixAncestorType::cellValue(x, y, String() << Math::Round(v,round));
	}


	void HydroMonthlyPower::internalAreaChanged(Antares::Data::Area* area)
	{
		// FIXME for some reasons, the variable study here is not properly initialized
		if (area && !study)
			study = Data::Study::Current::Get();

		Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
		Renderer::ARendererArea::internalAreaChanged(area);
		if (pHydro)
			MatrixAncestorType::matrix(&pHydro->maxPower);
		else
			MatrixAncestorType::matrix(nullptr);
	}


	IRenderer::CellStyle HydroMonthlyPower::cellStyle(int col, int row) const
	{

		switch (col)
		{
			case 0:
				{
					double genMaxP = MatrixAncestorType::cellNumericValue(0, row);
					if (genMaxP<0.)
						return IRenderer::cellStyleError;
					break;
				}
			case 1: 
				{
					double genMaxE = MatrixAncestorType::cellNumericValue(1, row);
					if (genMaxE < 0. || genMaxE>24.)
						return IRenderer::cellStyleError;
					break;
				}
			case 2:
				{
					double PumpMaxP = MatrixAncestorType::cellNumericValue(2, row);
					if (PumpMaxP < 0.)
						return IRenderer::cellStyleError;
					break;
				}
			case 3:
			{
				double PumpMaxE = MatrixAncestorType::cellNumericValue(3, row);
				if (PumpMaxE < 0. || PumpMaxE >24.)
					return IRenderer::cellStyleError;
				break;
			}
		}
		return IRenderer::cellStyleWithNumericCheck(col, row);
	}



	wxString HydroMonthlyPower::rowCaption(int row) const
	{
		if (!study || row >= study->calendar.maxDaysInYear)
			return wxEmptyString;
		return wxStringFromUTF8(study->calendar.text.daysYear[row]);
	}


	void HydroMonthlyPower::onStudyClosed()
	{
		MatrixAncestorType::onStudyClosed();
		Renderer::ARendererArea::onStudyClosed();
	}


	void HydroMonthlyPower::onStudyLoaded()
	{
		MatrixAncestorType::onStudyLoaded();
		Renderer::ARendererArea::onStudyLoaded();
	}




} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

