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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__
# define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__

# include <antares/wx-wrapper.h>
# include "../filter.h"
# include <antares/date.h>
# include <antares/study/study.h>


namespace Antares
{
namespace Toolbox
{
namespace Filter
{



	class Weekday : public AFilterBase
	{
	public:
		static const wxChar* Name()    {return wxT("weekday");}
		static const wxChar* Caption() {return wxT("WeekDay");}
		static Date::Precision Precision() {return Date::daily;}

	public:
		Weekday(Input* parent) :
			AFilterBase(parent)
		{
			operators.addStdWeekday();
		}

		virtual ~Weekday() {}

		virtual Date::Precision precision() const {return Weekday::Precision();}

		virtual bool checkOnRowsLabels() const {return true;}

		virtual const wxChar* name() const {return Weekday::Name();}
		virtual const wxChar* caption() const {return Weekday::Caption();}


		virtual bool rowIsValid(int row) const
		{
			// TODO Do not use global study
			auto studyptr = Data::Study::Current::Get();
			if (!studyptr)
				return false;
			auto& study = *studyptr;
			auto& calendar = study.calendar;

			switch (pDataGridPrecision)
			{
				case Date::hourly:
					{
						if (row < study.calendar.maxHoursInYear)
						{
							uint w = calendar.hours[row].weekday;
							return currentOperator->compute((int) w);
						}
						break;
					}
				case Date::daily:
					{
						if (row < study.calendar.maxDaysInYear)
						{
							uint w = calendar.days[row].weekday;
							return currentOperator->compute((int) w);
						}
						break;
					}
				default:
					break;
			}
			return false;

		}

	}; // class HourYear





} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__
