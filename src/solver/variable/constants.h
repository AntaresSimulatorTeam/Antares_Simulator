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
#ifndef __SOLVER_VARIABLE_CONSTANT_H__
# define __SOLVER_VARIABLE_CONSTANT_H__


namespace Antares
{
namespace Solver
{
namespace Variable
{

	enum Constant
	{
		maxHoursInAYear = 8785,
		maxDaysInAYear  = 7 * 53 + 1, //366,
		maxWeeksInAYear = 53,
		maxHoursInADay  = 24,
		maxMonths       = 12,
		maxDaysInAWeek  = 7,
		maxHoursInAWeek = 24 * 7, //168,
	};


	template<int I> struct PrecisionToPrintfFormat    { static const char* Value() {return "%.6f";} };
	template<>      struct PrecisionToPrintfFormat<0> { static const char* Value() {return "%.0f";} };
	template<>      struct PrecisionToPrintfFormat<1> { static const char* Value() {return "%.1f";} };
	template<>      struct PrecisionToPrintfFormat<2> { static const char* Value() {return "%.2f";} };
	template<>      struct PrecisionToPrintfFormat<3> { static const char* Value() {return "%.3f";} };
	template<>      struct PrecisionToPrintfFormat<4> { static const char* Value() {return "%.4f";} };
	template<>      struct PrecisionToPrintfFormat<5> { static const char* Value() {return "%.5f";} };

	template<class StringT>
	static inline void AssignPrecisionToPrintfFormat(StringT& out, uint precision)
	{
		switch (precision)
		{
			case 0:  out.assign("%.0f", 4);break;
			case 1:  out.assign("%.1f", 4);break;
			case 2:  out.assign("%.2f", 4);break;
			case 3:  out.assign("%.3f", 4);break;
			case 4:  out.assign("%.4f", 4);break;
			case 5:  out.assign("%.5f", 4);break;
			default: out.assign("%.6f", 4);break;
		}
	}




} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_CONSTANT_H__
