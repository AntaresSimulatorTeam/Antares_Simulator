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
#ifndef __SOLVER_VARIABLE_STORAGE_OR_H__
# define __SOLVER_VARIABLE_STORAGE_OR_H__

# include "or-data.h"


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



	template<class NextT /*= Empty*/, int FileFilter /*= Variable::Category::allFile*/>
	struct Or : public NextT
	{
	public:
		//! Type of the net item in the list
		typedef NextT NextType;

		enum
		{
			//! The count if item in the list
			count = 1 + NextT::count,

			categoryFile = NextT::categoryFile | Variable::Category::allFile,
		};

		struct Data
		{
			double value;
			uint32_t indice;
		};

		//! Name of the filter
		static const char* Name() {return "raw";}

	public:
		Or()
		{
		}

		~Or()
		{
		}

	protected:
		void initializeFromStudy(Antares::Data::Study& study);

		inline void reset()
		{
			ordata.reset();
			// Next
			NextType::reset();
		}

		inline void merge(unsigned int year, const IntermediateValues& rhs)
		{
			ordata.merge(year, rhs);
			// Next
			NextType::merge(year, rhs);
		}

		inline void mergeHourInYear(unsigned int year, unsigned int hour, const double value)
		{
			// Next
			NextType::mergeHourInYear(year, hour, value);
		}


		template<class S, class VCardT>
		void buildSurveyReport(SurveyResults& report, const S& results, int dataLevel, int fileLevel, int precision) const
		{
			if (0 != (fileLevel & FileFilter))
			{
				switch (fileLevel)
				{
					case Category::mc:
						InternalExportValuesMC<1, VCardT>(precision, report, ordata.year);
						break;
					case Category::id:
						break;
					default:
						{
							switch (precision)
							{
								case Category::hourly:
									InternalExportValues<Category::hourly, maxHoursInAYear, VCardT>(report, ordata.hourly);
									break;
								case Category::daily:
									InternalExportValues<Category::daily, maxDaysInAYear, VCardT>(report, ordata.daily);
									break;
								case Category::weekly:
									InternalExportValues<Category::weekly, maxWeeksInAYear, VCardT>(report, ordata.weekly);
									break;
								case Category::monthly:
									InternalExportValues<Category::monthly, maxMonths, VCardT>(report, ordata.monthly);
									break;
								case Category::annual:
									InternalExportValues<Category::annual, 1, VCardT>(report, ordata.year);
									break;
							}
						}
				}
			}
			// Next
			NextType::template buildSurveyReport<S,VCardT>(report, results, dataLevel, fileLevel, precision);
		}


		template<class VCardT>
		void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
		{
			if ((dataLevel & Category::area || dataLevel & Category::setOfAreas)
				&& digestLevel & Category::digestAllYears)
			{
				if (report.data.study.parameters.mode != Antares::Data::stdmAdequacyDraft)
				{
					assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

					report.captions[0][report.data.columnIndex] = report.variableCaption;
					report.captions[1][report.data.columnIndex] = VCardT::Unit();
					report.captions[2][report.data.columnIndex] = "values";

					// Precision
					report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();
					// Value
					report.values[report.data.columnIndex][report.data.rowIndex] = ordata.allYears;
					// Non applicability
					report.digestNonApplicableStatus[report.data.rowIndex][report.data.columnIndex] = *report.isCurrentVarNA;

					++(report.data.columnIndex);
				}
			}
			// Next
			NextType::template buildDigest<VCardT>(report, digestLevel, dataLevel);
		}


		Yuni::uint64 memoryUsage() const
		{
			return
			# ifdef ANTARES_SWAP_SUPPORT
				0
			# else
				+ sizeof(double) * maxHoursInAYear
			# endif
				+ NextType::memoryUsage();
		}


		static void EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u)
		{
			u.requiredMemoryForOutput += sizeof(double) * u.years;
			Antares::Memory::EstimateMemoryUsage(sizeof(double), maxHoursInAYear, u, false);
			u.takeIntoConsiderationANewTimeserieForDiskOutput();
			NextType::EstimateMemoryUsage(u);
		}

		template<template<class,int> class DecoratorT>
		Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
		{
			if (Yuni::Static::Type::StrictlyEqual<DecoratorT<Empty,0>,Or<Empty,0> >::Yes)
				return ordata.hourly;
			return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
		}

	public:
		OrData ordata;

	private:
		template<int PrecisionT, unsigned int Size, class VCardT, class A>
		void InternalExportValues(SurveyResults& report, const A& array) const
		{
			assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

			// Caption
			report.captions[0][report.data.columnIndex] = report.variableCaption;
			report.captions[1][report.data.columnIndex] = VCardT::Unit();
			report.captions[2][report.data.columnIndex] = "values";
			// Precision
			report.precision[report.data.columnIndex] = Solver::Variable::PrecisionToPrintfFormat<VCardT::decimal>::Value();
			// Non applicability
			report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

			// Values
			if (PrecisionT == Category::annual)
			{
				ordata.allYears = 0.;
				for (unsigned int i = 0; i != ordata.nbYearsCapacity; ++i)
					ordata.allYears += array[i];
				*(report.values[report.data.columnIndex]) = ordata.allYears;
			}
			else
				(void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);

			// Next column index
			++report.data.columnIndex;
		}

		template<unsigned int Size, class VCardT>
		void InternalExportValuesMC(int precision, SurveyResults& report, const double* array) const
		{
			if (not (precision & Category::annual))
				return;
			assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

			// Caption
			report.captions[0][report.data.columnIndex] = report.variableCaption;
			report.captions[1][report.data.columnIndex] = VCardT::Unit();
			report.captions[2][report.data.columnIndex] = "values";
			// Precision
			report.precision[report.data.columnIndex] = Solver::Variable::PrecisionToPrintfFormat<VCardT::decimal>::Value();
			// Non applicability
			report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

			(void)::memcpy(report.data.matrix[report.data.columnIndex], array, report.data.nbYears * sizeof(double));

			// Next column index
			++report.data.columnIndex;
		}

	}; // class Or





} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

# include "or.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_OR_H__
