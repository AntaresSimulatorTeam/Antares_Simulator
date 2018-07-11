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
#ifndef __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__
# define __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__

# include <antares/wx-wrapper.h>
# include "../filter.h"



namespace Antares
{
namespace Toolbox
{
namespace Filter
{



	class HourYear : public AFilterBase
	{
	public:
		static const wxChar* Name()    {return wxT("houryear");}
		static const wxChar* Caption() {return wxT("Hour Year");}
		static Date::Precision Precision() {return Date::hourly;}

	public:
		HourYear(Input* parent)
			:AFilterBase(parent)
		{
			operators.addStdArithmetic();
		}

		virtual ~HourYear() {}

		virtual Date::Precision precision() const {return HourYear::Precision();}

		virtual bool checkOnRowsLabels() const {return true;}

		virtual const wxChar* name() const {return HourYear::Name();}
		virtual const wxChar* caption() const {return HourYear::Caption();}


		virtual bool rowIsValid(int row) const
		{
			return currentOperator->compute(row + 1);
		}

	}; // class HourYear





} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
