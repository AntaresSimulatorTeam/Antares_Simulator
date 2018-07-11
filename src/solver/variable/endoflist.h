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
#ifndef __SOLVER_END_OF_LIST_END_OF_LIST_H__
# define __SOLVER_END_OF_LIST_END_OF_LIST_H__

# include <yuni/yuni.h>
# include <antares/study.h>
# include "state.h"
# include "surveyresults.h"


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Container
{


	class EndOfList
	{
	public:
		//! Type of the next static variable
		typedef void NextType;

		enum
		{
			//! How many items have we got
			count = 0,
		};

		template<int CategoryDataLevel, int CategoryFile>
		struct Statistics { enum { count = 0 }; };


	public:
		/*!
		** \brief Try to estimate the memory footprint that the solver will require to make a simulation
		*/
		static void EstimateMemoryUsage(Data::StudyMemoryUsage&) {}

		/*!
		** \brief Retrieve the list of all individual variables
		**
		** The predicate must implement the method `add(name, unit, comment)`.
		*/
		template<class PredicateT> static void RetrieveVariableList(PredicateT&) {}

	public:
		//! \name Constructor & Destructor
		//@{
		EndOfList() {}
		~EndOfList() {}
		//@}

		static void initializeFromStudy(Data::Study&) {}
		static void initializeFromArea(Data::Study*, Data::Area*) {}
		static void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*) {}
		static void initializeFromAreaLink(Data::Study*, Data::AreaLink*) {}

		static void simulationBegin() {}

		static void simulationEnd() {}

		static void yearBegin(unsigned int, unsigned int) {}

		static void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace) {}
		static void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace) {}
		static void yearEndBuild(State&, unsigned int) {}
		static void yearEnd(unsigned int, unsigned int) {}

		static void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, uint nbYearsForCurrentSummary) {};

		template<class V> void yearEndSpatialAggregates(V&, unsigned int, uint numSpace) {}

		template<class V, class SetT> void yearEndSpatialAggregates(V&, unsigned int, const SetT&, uint numSpace) {}

		template<class V> void computeSpatialAggregatesSummary(V&, std::map<unsigned int, unsigned int> &, unsigned int) {}

		template<class V> void simulationEndSpatialAggregates(V&) {}
		template<class V, class SetT> void simulationEndSpatialAggregates(V&, const SetT&) {}

		static void weekBegin(State&) {}

		static void weekForEachArea(State&, uint numSpace) {}

		static void weekEnd(State&) {}

		static void hourBegin(unsigned int) {}

		static void hourForEachArea(State&, uint numSpace) {}

		static void hourForEachLink(State&, uint numSpace) {}

		static void hourForEachThermalCluster(State&, uint numSpace) {}

		static void hourEnd(State&, unsigned int) {}

		static void buildSurveyReport(SurveyResults&, int, int, int) {}

		static void buildAnnualSurveyReport(SurveyResults&, int, int, int, uint) {}

		static void buildDigest(SurveyResults&, int, int) {}

		static void beforeYearByYearExport(uint /*year*/, uint) {}

		static Yuni::uint64 memoryUsage() {return 0;}

		template<class I> static void provideInformations(I&) {}

		template<class SearchVCardT, class O> static void computeSpatialAggregateWith(O&)
		{
			assert(false);
		}
		template<class SearchVCardT, class O> static void computeSpatialAggregateWith(O&, const Data::Area*)
		{
			assert(false);
		}


		template<class VCardToFindT>
		const double* retrieveHourlyResultsForCurrentYear(uint) const
		{
			return nullptr;
		}


		template<class VCardToFindT, class O>
		static void retrieveResultsForArea(O** /*result*/, const Data::Area*)
		{
			// Does nothing - this method may be called from a leaf
			// Consequently we can not throw an error from here if the variable `result`
			// is not initialized.
		}

		template<class VCardToFindT, class O>
		static void retrieveResultsForThermalCluster(O** /*result*/, const Data::ThermalCluster*)
		{
			// Does nothing - this method may be called from a leaf
			// Consequently we can not throw an error from here if the variable `result`
			// is not initialized.
		}

		template<class VCardToFindT, class O>
		static void retrieveResultsForLink(O** /*result*/, const Data::AreaLink*)
		{
			// Does nothing - this method may be called from a leaf
			// Consequently we can not throw an error from here if the variable `result`
			// is not initialized.
		}

		void localBuildAnnualSurveyReport(SurveyResults&, int, int, unsigned int) const
		{
			// Does nothing
		}

	}; // class EndOfList






} // namespace Container
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_END_OF_LIST_END_OF_LIST_H__
