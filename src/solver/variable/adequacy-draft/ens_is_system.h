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
#ifndef __SOLVER_VARIABLE_ADEQUACY_ENS_IS_System_H__
# define __SOLVER_VARIABLE_ADEQUACY_ENS_IS_System_H__


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace AdequacyDraft
{


	struct VCardENS_IS_System
	{
		//! Caption
		static const char* Caption() {return "ENS IS";}
		//! Unit
		static const char* Unit() {return "MWh";}

		//! The short description of the variable
		static const char* Description() {return "Expectation of the Energy not served throughout all MC years";}

		//! The expecte results
		typedef Results<
			R::AllYears::Raw<          // The values throughout all years
			> >  ResultsType;

		enum
		{
			//! Data Level
			categoryDataLevel = Category::standard,
			//! File level (provided by the type of the results)
			categoryFileLevel = ResultsType::categoryFile & (Category::is),
			//! Precision (views)
			precision = Category::hourly | Category::annual,
			//! Indentation (GUI)
			nodeDepthForGUI = +0,
			//! Decimal precision
			decimal = 0,
			//! Number of columns used by the variable (One ResultsType per column)
			columnCount = 1,
			//! The Spatial aggregation
			spatialAggregate = Category::spatialAggregateSum,
			//! Intermediate values
			hasIntermediateValues = 0,
		};

	}; // class VCard


	/*!
	** \brief Marginal ENS_IS_System
	*/
	template<class NextT = Container::EndOfList>
	class ENS_IS_System : public Variable::IVariable<ENS_IS_System<NextT>, NextT, VCardENS_IS_System>
	{
	public:
		//! Type of the next static variable
		typedef NextT NextType;
		//! VCard
		typedef VCardENS_IS_System VCardType;
		//! Ancestor
		typedef Variable::IVariable<ENS_IS_System<NextT>, NextT, VCardType> AncestorType;

		//! List of expected results
		typedef typename VCardType::ResultsType ResultsType;

		typedef VariableAccessor<ResultsType, VCardType::columnCount>  VariableAccessorType;

		enum
		{
			//! How many items have we got
			count = 1 + NextT::count,
		};

		template<int CDataLevel, int CFile>
		struct Statistics
		{
			enum
			{
				count = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
					? (NextType::template Statistics<CDataLevel, CFile>::count + VCardType::columnCount * ResultsType::count)
					: NextType::template Statistics<CDataLevel, CFile>::count),
			};
		};

	public:
		~ENS_IS_System() {}

		void initializeFromStudy(Data::Study& study)
		{
			// Average on all years
			AncestorType::pResults.initializeFromStudy(study);
			AncestorType::pResults.reset();
			pRatio = 1. / study.runtime->rangeLimits.year[Data::rangeCount];

			// Next
			NextType::initializeFromStudy(study);
		}

		void initializeFromArea(Data::Study* study, Data::Area* area)
		{
			// Next
			NextType::initializeFromArea(study, area);
		}

		void initializeFromLink(Data::Study* study, Data::AreaLink* link)
		{
			// Next
			NextType::initializeFromAreaLink(study, link);
		}


		void initializeFromThermalCluster(Data::Study* study, Data::Area* area, Data::ThermalCluster* cluster)
		{
			// Next
			NextType::initializeFromThermalCluster(study, area, cluster);
		}

		void simulationBegin()
		{
			// Next
			NextType::simulationBegin();
		}

		void simulationEnd()
		{
			NextType::simulationEnd();
		}


		void yearBegin(unsigned int year, unsigned int numSpace)
		{
			// Next variable
			NextType::yearBegin(year, numSpace);
		}
		
		void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, unsigned int numSpace)
		{
			// Next variable
			NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
		}

		void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
		{
			// Next variable
			NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
		}

		void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
		{
			
			// Next variable
			NextType::yearEndBuild(state, year, numSpace);
		}
		
		void yearEnd(unsigned int year, unsigned int numSpace)
		{
			// Next variable
			NextType::yearEnd(year, numSpace);
		}

		void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary)
		{
			// Next variable
			NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
		}

		void hourBegin(unsigned int hourInTheYear)
		{
			// Next variable
			NextType::hourBegin(hourInTheYear);
		}

		void hourForEachArea(State& state, unsigned int numSpace)
		{
			// Next variable
			NextType::hourForEachArea(state, numSpace);
		}

		void hourForEachThermalCluster(State& state, unsigned int numSpace)
		{
			// Next item in the list
			NextType::hourForEachThermalCluster(state, numSpace);
		}

		void hourEnd(State& state, unsigned int hourInTheYear)
		{
			AncestorType::pResults.rawdata.hourly[hourInTheYear] += state.eensSystemIS * pRatio;
			AncestorType::pResults.rawdata.year[0] += state.eensSystemIS * pRatio;
			// Next
			NextType::hourEnd(state, hourInTheYear);
		}


		void localBuildAnnualSurveyReport(SurveyResults&, int, int, unsigned int) const
		{
			// do nothing
		}


	private:
		double pRatio;

	}; // class ENS_IS_System





} // namespace AdequacyDraft
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ADEQUACY_ENS_IS_System_H__
