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
#ifndef __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
# define __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__

# include <float.h>
# include <limits>
# include <yuni/core/math.h>


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



	template<class NextT = Empty, int FileFilter = Variable::Category::allFile>
	struct StdDeviation : public NextT
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
		static const char* Name() {return "std deviation";}

	public:
		StdDeviation()
		{
			using namespace Yuni;
			stdDeviationHourly = nullptr;
		}

		~StdDeviation()
		{
			Antares::Memory::Release(stdDeviationHourly);
		}

	protected:
		void initializeFromStudy(Antares::Data::Study& study)
		{
			Antares::Memory::Allocate<double>(stdDeviationHourly, maxHoursInAYear);
			// Next
			NextType::initializeFromStudy(study);

            yearsWeight     = study.parameters.getYearsWeight();
            yearsWeightSum  = study.parameters.getYearsWeightSum();
        }

		void reset()
		{
			// Reset
			(void)::memset(stdDeviationMonthly, 0, sizeof(double) * maxMonths);
			(void)::memset(stdDeviationWeekly,  0, sizeof(double) * maxWeeksInAYear);
			(void)::memset(stdDeviationDaily,   0, sizeof(double) * maxDaysInAYear);
			Antares::Memory::Zero(maxHoursInAYear, stdDeviationHourly);
			stdDeviationYear = 0.;
			// Next
			NextType::reset();
		}

		void merge(unsigned int year, const IntermediateValues& rhs)
		{
            //Ratio take into account MC year weight
            double pRatio = (double) yearsWeight[year] / (double) yearsWeightSum;

			unsigned int i;
			// StdDeviation value for each hour throughout all years
			for (i = 0; i != maxHoursInAYear; ++i)
				stdDeviationHourly[i] += rhs.hour[i] * rhs.hour[i] * pRatio;
			// StdDeviation value for each day throughout all years
			for (i = 0; i != maxDaysInAYear; ++i)
				stdDeviationDaily[i] += rhs.day[i] * rhs.day[i] * pRatio;
			// StdDeviation value for each week throughout all years
			for (i = 0; i != maxWeeksInAYear; ++i)
				stdDeviationWeekly[i] += rhs.week[i] * rhs.week[i] * pRatio;
			// StdDeviation value for each month throughout all years
			for (i = 0; i != maxMonths; ++i)
				stdDeviationMonthly[i] += rhs.month[i] * rhs.month[i] * pRatio;
			// StdDeviation value throughout all years
			stdDeviationYear += rhs.year * rhs.year * pRatio;

			// Next
			NextType::merge(year, rhs);
		}

		template<class S, class VCardT>
		void buildSurveyReport(SurveyResults& report, const S& results, int dataLevel, int fileLevel, int precision) const
		{
			switch (fileLevel)
			{
				case Category::id:
					break;
				case Category::mc:
					{
						switch (precision)
						{
							case Category::hourly:
								InternalExportValuesMC<S, maxHoursInAYear, VCardT, Category::hourly>(report, results, stdDeviationHourly);
								break;
							default:
								break;
						}
						break;
					}
				default:
					{
						switch (precision)
						{
							case Category::hourly:
								InternalExportValues<S, maxHoursInAYear, VCardT, Category::hourly>(report, results, Memory::RawPointer(stdDeviationHourly));
								break;
							case Category::daily:
								InternalExportValues<S, maxDaysInAYear, VCardT, Category::daily>(report, results, stdDeviationDaily);
								break;
							case Category::weekly:
								InternalExportValues<S, maxWeeksInAYear, VCardT, Category::weekly>(report, results, stdDeviationWeekly);
								break;
							case Category::monthly:
								InternalExportValues<S, maxMonths, VCardT, Category::monthly>(report, results, stdDeviationMonthly);
								break;
							case Category::annual:
								InternalExportValues<S, 1, VCardT, Category::annual>(report, results, &stdDeviationYear);
								break;
						}
					}
			}
			// Next
			NextType::template buildSurveyReport<S,VCardT>(report, results, dataLevel, fileLevel, precision);
		}


		Yuni::uint64 memoryUsage() const
		{
			return
			# ifdef ANTARES_SWAP_SUPPORT
			# else
				sizeof(double) * maxHoursInAYear
			# endif
				+ NextType::memoryUsage();
		}


		static void EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u)
		{
			Memory::EstimateMemoryUsage(sizeof(double), maxHoursInAYear, u, false);
			u.takeIntoConsiderationANewTimeserieForDiskOutput();
			NextType::EstimateMemoryUsage(u);
		}


		template<template<class,int> class DecoratorT>
		Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
		{
			if (Yuni::Static::Type::StrictlyEqual<DecoratorT<Empty,0>,StdDeviation<Empty,0> >::Yes)
				return stdDeviationHourly;
			return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
		}

	public:
		double stdDeviationMonthly[maxMonths];
		double stdDeviationWeekly[maxWeeksInAYear];
		double stdDeviationDaily[maxDaysInAYear];
		Antares::Memory::Stored<double>::Type stdDeviationHourly;
		double stdDeviationYear;

	private:
		template<class S, unsigned int Size, class VCardT, int PrecisionT, class A>
		void InternalExportValues(SurveyResults& report, const S& results, const A& array) const
		{
			assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

			// Caption
			report.captions[0][report.data.columnIndex] = report.variableCaption;
			report.captions[1][report.data.columnIndex] = VCardT::Unit();
			report.captions[2][report.data.columnIndex] = "std";

			// Precision
			report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();

			// Non applicability
			report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

			// Values
			double* target = report.values[report.data.columnIndex];
			// A mere copy

			switch (PrecisionT)
			{
				case Category::hourly:
					{
						for (unsigned int i = 0; i != Size; ++i)
							target[i] = Yuni::Math::SquareRoot(array[i] - results.avgdata.hourly[i] * results.avgdata.hourly[i]);
					}
					break;
				case Category::daily:
					{
						for (unsigned int i = 0; i != Size; ++i)
							target[i] = Yuni::Math::SquareRoot(array[i] - results.avgdata.daily[i] * results.avgdata.daily[i]);
					}
					break;
				case Category::weekly:
					{
						for (unsigned int i = 0; i != Size; ++i)
							target[i] = Yuni::Math::SquareRoot(array[i] - results.avgdata.weekly[i] * results.avgdata.weekly[i]);
					}
					break;
				case Category::monthly:
					{
						for (unsigned int i = 0; i != Size; ++i)
							target[i] = Yuni::Math::SquareRoot(array[i] - results.avgdata.monthly[i] * results.avgdata.monthly[i]);
					}
					break;
				case Category::annual:
					{
						const double d = *array - results.avgdata.allYears * results.avgdata.allYears;
						*target = Yuni::Math::SquareRoot(d);
					}
					break;
			}

			// Next column index
			++report.data.columnIndex;
		}

		template<class S, unsigned int Size, class VCardT, int PrecisionT, class A>
		void InternalExportValuesMC(SurveyResults& report, const S& /*results*/, const A& array) const
		{
			if (not (PrecisionT & Category::annual))
				return;
			assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

			// Caption
			report.captions[0][report.data.columnIndex] = report.variableCaption;
			report.captions[1][report.data.columnIndex] = VCardT::Unit();
			report.captions[2][report.data.columnIndex] = "std";

			// Precision
			report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();

			// Non applicability
			report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

			(void)::memcpy(report.data.matrix[report.data.columnIndex], array, Size * sizeof(double));

			// Next column index
			++report.data.columnIndex;
		}

	private:

        std::vector<float>    yearsWeight;
		float                 yearsWeightSum;
        
	}; // class StdDeviation






} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
