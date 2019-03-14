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
#ifndef __SOLVER_VARIABLE_STORAGE_AND_H__
# define __SOLVER_VARIABLE_STORAGE_AND_H__

# include <float.h>


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
	struct And : public NextT
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
		static const char* Name() {return "values";}

	public:
		And()
		{
			andHourly = nullptr;
		}

		~And()
		{
			Antares::Memory::Release(andHourly);
			delete[] andYear;
		}

	protected:
		void initializeFromStudy(Antares::Data::Study& study)
		{
			Antares::Memory::Allocate<double>(andHourly, maxHoursInAYear);
			Antares::Memory::Zero(maxHoursInAYear, andHourly);

			pNbYears = study.runtime->rangeLimits.year[Antares::Data::rangeCount];
			pNbYearsCapacity = pNbYears + study.runtime->rangeLimits.year[Antares::Data::rangeBegin] + 1;
			andYear = new double[pNbYearsCapacity];
			(void)::memset(andYear,    0, sizeof(double) * pNbYearsCapacity);

			// Next
			NextType::initializeFromStudy(study);
		}

		void reset()
		{
			// Next
			NextType::reset();
		}

		void merge(uint year, const IntermediateValues& rhs)
		{
			// Next
			NextType::merge(year, rhs);
		}

		void mergeHourInYear(uint year, uint hour, const double value)
		{
			// Next
			NextType::mergeHourInYear(year, hour, value);
		}


		template<class S, class VCardT>
		void buildSurveyReport(SurveyResults& report, const S& results, int dataLevel, int fileLevel, int precision) const
		{
			if (fileLevel & Category::mc)
			{
				InternalExportValuesMC<S, 1, VCardT>(precision, report, results, andYear);
			}
			else
			{
				if ((fileLevel & (Category::is | Category::cn)))
				{
					switch (precision)
					{
						case Category::hourly:
							InternalExportValues<Category::hourly, S, maxHoursInAYear, VCardT>(report, results, andHourly);
							break;
						case Category::annual:
							InternalExportValues<Category::annual, S, 1, VCardT>(report, results, andYear);
							break;
						default:
							break;
					}
				}
			}
			// Next
			NextType::template buildSurveyReport<S,VCardT>(report, results, dataLevel, fileLevel, precision);
		}

		template<class VCardT>
		void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
		{
			if (dataLevel & Category::area && digestLevel & Category::digestAllYears)
			{
				assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

				report.captions[0][report.data.columnIndex] = report.variableCaption;
				report.captions[1][report.data.columnIndex] = VCardT::Unit();
				report.captions[2][report.data.columnIndex] = "values";

				// Precision
				report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();
				// Value
				report.values[report.data.columnIndex][report.data.area->index] = andAllYears;
				// Non applicability
				report.digestNonApplicableStatus[report.data.rowIndex][report.data.columnIndex] = *report.isCurrentVarNA;

				++(report.data.columnIndex);
			}
			// Next
			NextType::template buildDigest<VCardT>(report, digestLevel, dataLevel);
		}



		static void EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u)
		{
			Antares::Memory::EstimateMemoryUsage(sizeof(double), maxHoursInAYear, u, false);
			u.takeIntoConsiderationANewTimeserieForDiskOutput();
			NextType::EstimateMemoryUsage(u);
		}


		Yuni::uint64 memoryUsage() const
		{
			return
			# ifdef ANTARES_SWAP_SUPPORT
				0
			# else
				sizeof(double) * maxHoursInAYear
			# endif
				+ NextType::memoryUsage();
		}

		template<template<class,int> class DecoratorT>
		Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
		{
			if (Yuni::Static::Type::StrictlyEqual<DecoratorT<Empty,0>,And<Empty,0> >::Yes)
				return andHourly;
			return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
		}

	public:
		double* andYear;
		mutable double andAllYears;
		Antares::Memory::Stored<double>::Type andHourly;

	protected:
		uint pNbYears;
		uint pNbYearsCapacity;

	private:
		template<int PrecisionT, class S, uint Size, class VCardT, class A>
		void InternalExportValues(SurveyResults& report, const S&, const A& array) const
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

			const double ratio = 1. / pNbYears;

			if (PrecisionT == Category::hourly)
			{
				for (uint i = 0; i != Size; ++i)
					report.values[report.data.columnIndex][i] = array[i] * ratio;
			}

			if (PrecisionT == Category::annual)
			{
				// Values
				andAllYears = 0.;
				for (uint i = 0; i != pNbYears; ++i)
					andAllYears += array[i];
				andAllYears *= ratio * 100.;
				*(report.values[report.data.columnIndex]) = andAllYears;
			}

			// Next column index
			++report.data.columnIndex;
		}

		template<class S, uint Size, class VCardT>
		void InternalExportValuesMC(int precision, SurveyResults& report, const S& /*results*/, const double* array) const
		{
			if (not (precision & Category::annual))
				return;
			assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

			// Caption
			report.captions[0][report.data.columnIndex] = report.variableCaption;
			report.captions[1][report.data.columnIndex] = VCardT::Unit();
			report.captions[2][report.data.columnIndex] = "values";

			// Precision
			report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();

			// Non applicability
			report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

			(void)::memcpy(report.data.matrix[report.data.columnIndex], array, pNbYears * sizeof(double));

			// Next column index
			++report.data.columnIndex;
		}


	}; // class Raw





} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_AND_H__
