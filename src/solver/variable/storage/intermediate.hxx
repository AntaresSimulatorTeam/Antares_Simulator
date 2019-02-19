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
#ifndef __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__
# define __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__


namespace Antares
{
namespace Solver
{
namespace Variable
{


	inline IntermediateValues::~IntermediateValues()
	{
		Antares::Memory::Release(hour);
	}


	inline void IntermediateValues::flush() const
	{
		Antares::Memory::Flush(hour);
	}


	inline void IntermediateValues::reset()
	{
		Antares::Memory::Zero(maxHoursInAYear, hour);
		memset(month, 0, sizeof(month));
		memset(week,  0, sizeof(week));
		memset(day,   0, sizeof(day));
	}


	inline IntermediateValues::Type& IntermediateValues::operator [] (const unsigned int index)
	{
		return hour[index];
	}

	inline const IntermediateValues::Type& IntermediateValues::operator [] (const unsigned int index) const
	{
		return hour[index];
	}


	inline void
	IntermediateValues::EstimateMemoryUsage(Data::StudyMemoryUsage& u)
	{
		Antares::Memory::EstimateMemoryUsage(sizeof(double), maxHoursInAYear, u, true);
	}


	inline Yuni::uint64
	IntermediateValues::MemoryUsage()
	{
		return
		# ifdef ANTARES_SWAP_SUPPORT
			0
		# else
			+ sizeof(Type) * maxHoursInAYear
		# endif
			;
	}


	template<class VCardT>
	inline void
	IntermediateValues::buildAnnualSurveyReport(SurveyResults& report, int fileLevel, int precision) const
	{
		if (!(fileLevel & Category::id))
		{
			if (fileLevel & Category::mc)
			{
				// Do nothing
				// internalExportAnnualValuesMC<1, VCardT>(report, avgdata.year);
			}
			else
			{
				switch (precision)
				{
					case Category::hourly:
						internalExportAnnualValues<maxHoursInAYear, VCardT>(report, hour, false);
						break;
					case Category::daily:
						internalExportAnnualValues<maxDaysInAYear, VCardT>(report, day, false);
						break;
					case Category::weekly:
						internalExportAnnualValues<maxWeeksInAYear, VCardT>(report, week, false);
						break;
					case Category::monthly:
						internalExportAnnualValues<maxMonths, VCardT>(report, month, false);
						break;
					case Category::annual:
						internalExportAnnualValues<1, VCardT>(report, &year, true);
						break;
				}
			}
		}
	}



	template<unsigned int Size, class VCardT, class A>
	void
	IntermediateValues::internalExportAnnualValues(SurveyResults& report, const A& array, bool annual) const
	{
		using namespace Yuni;
		assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

		// Caption
		report.captions[0][report.data.columnIndex] = report.variableCaption;
		report.captions[1][report.data.columnIndex] = VCardT::Unit();
		report.captions[2][report.data.columnIndex] = nullptr;
		// Precision
		report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();
		// Non applicability
		report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

		// Values
		if (not annual)
		{
			(void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);
		}
		else
		{
			double& target = *(report.values[report.data.columnIndex]);
			target = year;
		}

		// Next column index
		++report.data.columnIndex;
	}




} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__
