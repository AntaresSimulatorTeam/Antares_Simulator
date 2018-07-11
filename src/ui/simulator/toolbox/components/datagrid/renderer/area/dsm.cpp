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

#include "dsm.h"
#include <antares/study/area/constants.h>


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{



	DSM::DSM(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
		Renderer::Matrix<>(control), Renderer::ARendererArea(control, notifier)
	{}


	DSM::~DSM()
	{
		destroyBoundEvents();
	}


	wxString DSM::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
			case Data::fhrPrimaryReserve    :return wxT("  Primary Res. (draft)  ");
			case Data::fhrStrategicReserve  :return wxT("  Strategic Res. (draft) ");
			case Data::fhrDSM               :return wxT("       DSM       ");
			case Data::fhrDayBefore         :return wxT("   Day Ahead   ");
		}
		return wxEmptyString;
	}



	void DSM::internalAreaChanged(Antares::Data::Area* area)
	{
		this->matrix( (area) ? &(area->reserves) : NULL);
		Renderer::ARendererArea::internalAreaChanged(area);
	}


	IRenderer::CellStyle DSM::cellStyle(int col, int row) const
	{
		if (col != Data::fhrDSM && cellNumericValue(col, row) < 0.)
			return IRenderer::cellStyleError;
		return IRenderer::cellStyleWithNumericCheck(col, row);
	}



	wxColour DSM::horizontalBorderColor(int x, int y) const
	{
		// Getting informations about the next hour
		// (because the returned color is about the bottom border of the cell,
		// so the next hour for the user)
		if (!(!study) && y + 1 < Date::Calendar::maxHoursInYear)
		{
			auto& hourinfo = study->calendar.hours[y + 1];

			if (hourinfo.firstHourInMonth)
				return Default::BorderMonthSeparator();
			if (hourinfo.firstHourInDay)
				return Default::BorderDaySeparator();
		}
		return IRenderer::verticalBorderColor(x, y);
	}



	void DSM::onStudyClosed()
	{
		Renderer::Matrix<>::onStudyClosed();
		Renderer::ARendererArea::onStudyClosed();
	}


	void DSM::onStudyLoaded()
	{
		Renderer::Matrix<>::onStudyLoaded();
		Renderer::ARendererArea::onStudyLoaded();
	}






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

