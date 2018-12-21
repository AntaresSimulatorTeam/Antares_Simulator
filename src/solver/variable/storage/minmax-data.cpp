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

#include <yuni/yuni.h>
#include <antares/study/memory-usage.h>
#include "intermediate.h"
#include "minmax-data.h"
#include <float.h>


using namespace Yuni;



namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace R
{
namespace AllYears
{

	namespace // anonymous
	{
		double eps = 1.e-7;

		template<uint Size, bool OpInferior>
		struct ArrayInitializer
		{
			static void Init(Antares::Memory::Array<MinMaxData::Data>& array)
			{
				for (uint i = 0; i != Size; ++i)
				{
					MinMaxData::Data& data = array[i];
					data.value  = DBL_MAX;             // +inf
					data.indice = (Yuni::uint32)(-1); // invalid indice
				}
			}

			static void Init(MinMaxData::Data* array)
			{
				for (uint i = 0; i != Size; ++i)
				{
					MinMaxData::Data& data = array[i];
					data.value  = DBL_MAX;             // +inf
					data.indice = (Yuni::uint32)(-1); // invalid indice
				}
			}

		}; // class ArrayInitializer


		template<uint Size>
		struct ArrayInitializer<Size, false>
		{
			static void Init(Antares::Memory::Array<MinMaxData::Data>& array)
			{
				for (uint i = 0; i != Size; ++i)
				{
					// Contrary to what we could guess, DBL_MIN is not the smallest number
					// you can hold in a double, but the smallest positive number you can
					// hold in a double
					MinMaxData::Data& data = array[i];
					data.value  = - DBL_MAX;           // -inf
					data.indice = (Yuni::uint32)(-1); // invalid indice
				}
			}

			static void Init(MinMaxData::Data* array)
			{
				for (uint i = 0; i != Size; ++i)
				{
					// Contrary to what we could guess, DBL_MIN is not the smallest number
					// you can hold in a double, but the smallest positive number you can
					// hold in a double
					MinMaxData::Data& data = array[i];
					data.value  = - DBL_MAX;           // -inf
					data.indice = (Yuni::uint32)(-1); // invalid indice
				}
			}

		}; // class ArrayInitializer


		template<bool OpInferior, uint Size>
		struct MergeArray
		{
			template<class U>
			static void Do(const uint year, Antares::Memory::Array<MinMaxData::Data>& results, const U& values)
			{
				for (uint i = 0; i != Size; ++i)
				{
					MinMaxData::Data& data = results[i];
					if (values[i] < data.value - eps)
					{
						data.value  = values[i];
						data.indice = year + 1; // The year is zero-based
					}
				}
			}

			template<class U>
			static void Do(const uint year, MinMaxData::Data* results, const U& values)
			{
				for (uint i = 0; i != Size; ++i)
				{
					if (values[i] < results[i].value - eps)
					{
						results[i].value  = values[i];
						results[i].indice = year + 1; // The year is zero-based
					}
				}
			}

		}; // class MergeArray


		template<uint Size>
		struct MergeArray<0, Size>
		{
			template<class U>
			static void Do(const uint year, Antares::Memory::Array<MinMaxData::Data>& results, const U& values)
			{
				for (uint i = 0; i != Size; ++i)
				{
					MinMaxData::Data& data = results[i];
					if (values[i] > data.value + eps)
					{
						data.value = values[i];
						data.indice = year + 1; // The year is zero-based
					}
				}
			}

			template<class U>
			static void Do(const uint year, MinMaxData::Data* results, const U& values)
			{
				for (uint i = 0; i != Size; ++i)
				{
					MinMaxData::Data& data = results[i];
					if (values[i] > data.value + eps)
					{
						data.value = values[i];
						data.indice = year + 1; // The year is zero-based
					}
				}
			}

		}; // class MergeArray


	} // anonymous namespace




	MinMaxData::MinMaxData()
		:hourly(nullptr)
	{
	}


	MinMaxData::~MinMaxData()
	{
		Antares::Memory::Release(hourly);
	}


	void MinMaxData::resetInf()
	{
		ArrayInitializer<1,               true>::Init(&annual);
		ArrayInitializer<maxMonths,       true>::Init(monthly);
		ArrayInitializer<maxWeeksInAYear, true>::Init(weekly);
		ArrayInitializer<maxDaysInAYear,  true>::Init(daily);
		ArrayInitializer<maxHoursInAYear, true>::Init(hourly);
	}

	void MinMaxData::resetSup()
	{
		ArrayInitializer<1,               false>::Init(&annual);
		ArrayInitializer<maxMonths,       false>::Init(monthly);
		ArrayInitializer<maxWeeksInAYear, false>::Init(weekly);
		ArrayInitializer<maxDaysInAYear,  false>::Init(daily);
		ArrayInitializer<maxHoursInAYear, false>::Init(hourly);
	}



	void MinMaxData::initialize()
	{
		Antares::Memory::Allocate(hourly, maxHoursInAYear);
	}


	void MinMaxData::mergeInf(uint year, const IntermediateValues& rhs)
	{
		MergeArray<true, maxMonths       >::Do (year, monthly, rhs.month);
		MergeArray<true, maxWeeksInAYear >::Do (year, weekly,  rhs.week);
		MergeArray<true, maxDaysInAYear  >::Do (year, daily,   rhs.day);
		MergeArray<true, maxHoursInAYear >::Do (year, hourly,  rhs.hour);
		MergeArray<true, 1               >::Do (year, &annual, &rhs.year);
	}


	void MinMaxData::mergeSup(uint year, const IntermediateValues& rhs)
	{
		MergeArray<false, maxMonths       >::Do (year, monthly, rhs.month);
		MergeArray<false, maxWeeksInAYear >::Do (year, weekly,  rhs.week);
		MergeArray<false, maxDaysInAYear  >::Do (year, daily,   rhs.day);
		MergeArray<false, maxHoursInAYear >::Do (year, hourly,  rhs.hour);
		MergeArray<false, 1               >::Do (year, &annual, &rhs.year);
	}





} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

