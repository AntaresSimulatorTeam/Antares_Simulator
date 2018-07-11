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
#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_H__
# define __SOLVER_VARIABLE_STORAGE_RESULTS_H__

# include <antares/study.h>
# include "intermediate.h"
# include "../categories.h"
# include "fwd.h"


namespace Antares
{
namespace Solver
{
namespace Variable
{

	template<
		class FirstDecoratorT = Empty,  // The first decorator for the results
		template<class,int> class DecoratorForSpatialAggregateT = R::AllYears::Raw
		>
	class Results ;

	/*!
	** \brief
	**
	** \tparam FirstDecoratorT The first decorator for the class. The common values
	**   are `Raw`, `Average`, `Min`, `Max`... This parameter is a static list.
	*/
	template<
		class FirstDecoratorT,
		template<class,int> class DecoratorForSpatialAggregateT
		>
	class Results : public FirstDecoratorT
	{
	public:
		//! Type of the first decorator
		typedef FirstDecoratorT DecoratorType;
		enum
		{
			//! The count if item in the list
			count = DecoratorType::count,
		};

		enum
		{
			categoryFile = FirstDecoratorT::categoryFile,
		};

	public:
		/*!
		** \brief Initialize result outputs from study
		*/
		void initializeFromStudy(Antares::Data::Study&);

		/*!
		** \brief Reset all values
		*/
		void reset();

		/*!
		** \brief Merge the intermediate values
		*/
		void merge(uint year, const IntermediateValues& data);


		template<class S, class VCardT>
		void buildSurveyReport(SurveyResults& report, const S& results, int dataLevel, int fileLevel, int precision) const;

		template<class VCardT>
		void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
		{
			// Next
			DecoratorType::template buildDigest<VCardT>(results, digestLevel, dataLevel);
		}

		Yuni::uint64 memoryUsage() const
		{
			return DecoratorType::memoryUsage();
		}

		static void EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u)
		{
			DecoratorType::EstimateMemoryUsage(u);
		}

		Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
		{
			return DecoratorType::template
				hourlyValuesForSpatialAggregate<DecoratorForSpatialAggregateT>();
		}

	}; // class Results






} // namespace Variable
} // namespace Solver
} // namespace Antares

# include "results.hxx"
# include "empty.h"
# include "raw.h"
# include "minmax.h"
# include "average.h"
# include "stdDeviation.h"
# include "and.h"
# include "or.h"

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_H__
