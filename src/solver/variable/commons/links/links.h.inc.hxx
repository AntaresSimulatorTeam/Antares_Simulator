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
#ifndef __SOLVER_VARIABLE_INC_LINK_H__
# define __SOLVER_VARIABLE_INC_LINK_H__

// NOTE : template includes are used here to reduce template instanciation
// which still seems to be really cpu/memory consuming

# include "../../state.h"
# include <vector>


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace LINK_NAMESPACE
{

	struct VCardAllLinks
	{
		//! Caption
		static const char* Caption() {return "Links";}
		//! Unit
		static const char* Unit() {return "";}
		//! The short description of the variable
		static const char* Description() {return "";}

		//! The expecte results
		typedef Results<>  ResultsType;

		enum
		{
			//! Data Level
			categoryDataLevel = Category::link,
			//! File level (provided by the type of the results)
			categoryFileLevel = ResultsType::categoryFile,
			//! Indentation (GUI)
			nodeDepthForGUI = +1,
			//! Number of columns used by the variable (One ResultsType per column)
			columnCount = 0,
			//! The Spatial aggregation
			spatialAggregate = Category::spatialAggregateSum,
			//! Intermediate values
			hasIntermediateValues = 0,
		};

	}; // class VCard



	class Links //: public Variable::IVariable<Links<NextT>, NextT, VCardAllLinks>
	{
	public:
		//! Type of the next static variable
		typedef VariablePerLink NextType;
		//! VCard
		typedef VCardAllLinks VCardType;
		//! Ancestor
		//typedef Variable::IVariable<Links<NextT>, NextT, VCardType> AncestorType;

		//! List of expected results
		typedef VCardType::ResultsType ResultsType;

		enum
		{
			//! How many items have we got
			count = NextType::count,
		};

		template<int CDataLevel, int CFile>
		struct Statistics
		{
			enum
			{
				count = NextType::template Statistics<CDataLevel, CFile>::count
			};
		};


	public:
		/*!
		** \brief Try to estimate the memory footprint that the solver will require to make a simulation
		*/
		static void EstimateMemoryUsage(Data::StudyMemoryUsage&);

		/*!
		** \brief Retrieve the list of all individual variables
		**
		** The predicate must implement the method `add(name, unit, comment)`.
		*/
		template<class PredicateT> static void RetrieveVariableList(PredicateT& predicate);

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Links();
		//! Destructor
		~Links();
		//@}

		void initializeFromStudy(Data::Study& study);
		void initializeFromArea(Data::Study*, Data::Area*);
		void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*);
		void initializeFromAreaLink(Data::Study*, Data::AreaLink*);


		void simulationBegin();
		void simulationEnd();

		void yearBegin(uint year, unsigned int numSpace);

		void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace);
		void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace);

		void yearEndBuild(State& state, uint year);

		void yearEnd(uint year, uint numSpace);

		void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary);

		void weekBegin(State& state);

		void weekForEachArea(State& state, uint numSpace);
		void weekEnd(State& state);

		void hourBegin(uint hourInTheYear);
		void hourForEachArea(State& state, uint numSpace);
		void hourForEachLink(State& state, uint numSpace);
		void hourForEachThermalCluster(State& state, uint numSpace);

		void hourEnd(State& state, uint hourInTheYear);

		void buildSurveyReport(SurveyResults& results, int dataLevel, int fileLevel, int precision) const;

		void buildAnnualSurveyReport(SurveyResults& results, int dataLevel, int fileLevel, int precision, uint numSpace) const;

		void beforeYearByYearExport(uint year, uint numSpace);

		Yuni::uint64 memoryUsage() const;


		void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;

		template<class I> static void provideInformations(I& infos);

		template<class VCardToFindT>
		inline const double* retrieveHourlyResultsForCurrentYear(uint) const
		{
			return nullptr;
		}

		template<class VCardToFindT>
		void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result, const Data::Area*)
		{
			*result = NULL;
		}

		template<class VCardToFindT>
		void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result, const Data::ThermalCluster*)
		{
			*result = NULL;
		}

		template<class VCardToFindT>
		void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result, const Data::AreaLink* link)
		{
			pLinks[link->indexForArea].template retrieveResultsForLink<VCardToFindT>(result, link);
		}

		template<class SearchVCardT, class O>
		void computeSpatialAggregateWith(O&, uint)
		{
			// Do nothing
		}

	public:
		//! Area list
		NextType* pLinks;
		//! The total number of links
		uint pLinkCount;

	}; // class Links





} // namespace LINK_NAMESPACE
} // namespace Variable
} // namespace Solver
} // namespace Antares

# include "links.hxx.inc.hxx"

#endif // __SOLVER_VARIABLE_INC_LINK_H__
