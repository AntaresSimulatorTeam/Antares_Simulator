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
#ifndef __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__
# define __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include "../constants.h"
# include <antares/study.h>
# include "../categories.h"
# include "../surveyresults.h"


namespace Antares
{
namespace Solver
{
namespace Variable
{

	/*!
	** \brief Temporary buffer for allocating results for a single year
	**
	** This class is mostly used by economic variables for storing
	** their data for a single year.
	*/
	class IntermediateValues final
	{
	public:
		//! Basic type
		typedef double Type;

	public:
		//! Try to estimate the amount of memory that will be required for a simulation
		static void EstimateMemoryUsage(Data::StudyMemoryUsage& u);
		//! Get the dynamic amount of memory consummed by a simulation
		// \note This method assumes that you've already have gathered the size
		//   of this class
		static Yuni::uint64 MemoryUsage();

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		IntermediateValues();
		//! Destructor
		~IntermediateValues();
		//@}

		void initializeFromStudy(Data::Study& study);

		/*!
		** \brief Reset all values
		*/
		void reset();

		/*!
		** \brief Flush the memory
		*/
		void flush() const;

		/*!
		** \brief Compute statistics for the current year
		*/
		void computeStatisticsForTheCurrentYear();

		/*!
		** \brief Compute statistics for the current year
		*/
		void computeStatisticsOrForTheCurrentYear();

		/*!
		** \brief Compute statistics for the current year
		*/
		void computeStatisticsAdequacyForTheCurrentYear();

		/*!
		** \brief Compute daily, weekly, monthly and yearly averages for the current year
		*/
		void computeAVGstatisticsForCurrentYear();

		/*!
		** \brief Compute probabilities for the current year
		*/
		void computeProbabilitiesForTheCurrentYear();

		/*!
		** \brief Make another calculations when values are related to a price
		*/
		void adjustValuesWhenRelatedToAPrice();

		/*!
		** \brief Make another calculations when values are related to a price
		*/
		void adjustValuesAdequacyWhenRelatedToAPrice();


		//! \name User reports
		//@{
		template<class VCardT>
		void buildAnnualSurveyReport(SurveyResults& report, int fileLevel, int precision) const;
		//@}


		//! \name Operators
		//@{
		/*!
		** \brief Vector alias for an hour in the year
		*/
		Type& operator [] (const uint index);
		const Type& operator [] (const uint index) const;
		//@}


	private:
		template<uint Size, class VCardT, class A>
		void internalExportAnnualValues(SurveyResults& report, const A& array, bool annual) const;

	public:
		//! Range
		Antares::Data::StudyRangeLimits* pRange;
		//! Calendar, from the study, but dedicated to the output (with leap year)
		Antares::Date::Calendar* calendar;
		//! Range
		Antares::Data::StudyRuntimeInfos* pRuntimeInfo;

		//! Values for each month
		Type month[maxMonths];
		//! Values for each week
		Type week[maxWeeksInAYear];
		//! Values for each day in the year
		Type day[maxDaysInAYear];
		//! Values for each hour in the year
		mutable Antares::Memory::Stored<Type>::Type hour;
		//! Year
		Type year;

	private:
		// non copyable
		IntermediateValues(const IntermediateValues&) {}
		IntermediateValues& operator = (const IntermediateValues&) {return *this;}

	}; // class IntermediateValues





} // namespace Variable
} // namespace Solver
} // namespace Antares

# include "intermediate.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__
