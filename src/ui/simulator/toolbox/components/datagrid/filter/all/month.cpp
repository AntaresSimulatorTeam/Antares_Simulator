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

# include "month.h"
#include <antares/study/study.h>


namespace Antares
{
namespace Toolbox
{
namespace Filter
{


	bool Month::rowIsValid(int row) const
	{
		// TODO Do not use global study
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return false;
		auto& study = *studyptr;

		switch (pDataGridPrecision)
		{
			case Date::hourly:
				{
					if (row < study.calendar.maxHoursInYear)
					{
						uint calendarMonth = study.calendar.hours[row].month;
						uint realmonth     = study.calendar.months[calendarMonth].realmonth;
						return currentOperator->compute((int) realmonth);
					}
					break;
				}
			case Date::daily:
				{
					if (row < study.calendar.maxDaysInYear)
					{
						uint calendarMonth = study.calendar.days[row].month;
						uint realmonth     = study.calendar.months[calendarMonth].realmonth;
						return currentOperator->compute((int) realmonth);
					}
					break;
				}
			case Date::monthly:
				{
					if (row < 12 + 1)
					{
						//uint realmonth = study.calendar.months[row].realmonth;
						return currentOperator->compute((int) row);
					}
					break;
				}
			case Date::weekly:
			case Date::annual:
				break;
			case Date::stepAny:
			case Date::stepNone:
				break;
		}
		return false;
	}




} // namespace Filter
} // namespace Toolbox
} // namespace Antares
