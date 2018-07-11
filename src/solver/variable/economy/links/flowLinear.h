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
#ifndef __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__
# define __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__

# include "../../variable.h"


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{


	struct VCardFlowLinear
	{
		//! Caption
		static const char* Caption() {return "FLOW LIN.";}
		//! Unit
		static const char* Unit() {return "MWh";}

		//! The short description of the variable
		static const char* Description() {return "Flow assessed, over all MC years, through linear optimization";}

		//! The expecte results
		typedef Results<
			R::AllYears::Average<      // The average values throughout all years
			R::AllYears::StdDeviation< // The standard deviation values throughout all years
			R::AllYears::Min<          // The minimum values throughout all years
			R::AllYears::Max<          // The maximum values throughout all years
			> > > > >  ResultsType;

		enum
		{
			//! Data Level
			categoryDataLevel = Category::link,
			//! File level (provided by the type of the results)
			categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
			//! Precision (views)
			precision = Category::all,
			//! Indentation (GUI)
			nodeDepthForGUI = +0,
			//! Decimal precision
			decimal = 0,
			//! Number of columns used by the variable (One ResultsType per column)
			columnCount = 1,
			//! The Spatial aggregation
			spatialAggregate = Category::spatialAggregateSum,
			spatialAggregateMode = Category::spatialAggregateEachYear,
			spatialAggregatePostProcessing = 0,
			//! Intermediate values
			hasIntermediateValues = 1,
		};

		typedef IntermediateValues IntermediateValuesBaseType;
		typedef IntermediateValues * IntermediateValuesType;

	}; // class VCard


	/*!
	** \brief Marginal FlowLinear
	*/
	template<class NextT = Container::EndOfList>
	class FlowLinear : public Variable::IVariable<FlowLinear<NextT>, NextT, VCardFlowLinear>
	{
	public:
		//! Type of the next static variable
		typedef NextT NextType;
		//! VCard
		typedef VCardFlowLinear VCardType;
		//! Ancestor
		typedef Variable::IVariable<FlowLinear<NextT>, NextT, VCardType> AncestorType;

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
		~FlowLinear()
		{
			delete[] pValuesForTheCurrentYear;
		}

		void initializeFromStudy(Data::Study& study)
		{
			pNbYearsParallel = study.maxNbYearsInParallel;
			
			// Average on all years
			AncestorType::pResults.initializeFromStudy(study);
			AncestorType::pResults.reset();
			AncestorType::pResults.averageMaxValue(study.runtime->rangeLimits.year[Data::rangeCount]);
			AncestorType::pResults.stdDeviationMaxValue(study.runtime->rangeLimits.year[Data::rangeCount]);

			// Intermediate values
			pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
			for(unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
				pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);

			// Next
			NextType::initializeFromStudy(study);
		}

		void initializeFromArea(Data::Study* study, Data::Area* area)
		{
			// Next
			NextType::initializeFromArea(study, area);
		}

		void initializeFromAreaLink(Data::Study* study, Data::AreaLink* link)
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
			for(unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
				pValuesForTheCurrentYear[numSpace].reset();
			// Next
			NextType::simulationBegin();
		}

		void simulationEnd()
		{
			NextType::simulationEnd();
		}


		void yearBegin(uint year, unsigned int numSpace)
		{
			// Reset
			pValuesForTheCurrentYear[numSpace].reset();
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

		void yearEndBuild(State& state, unsigned int year)
		{
			
			// Next variable
			NextType::yearEndBuild(state, year);
		}

		void yearEnd(unsigned int year, unsigned int numSpace)
		{
			// Compute all statistics for the current year (daily,weekly,monthly)
			pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();

			// Next variable
			NextType::yearEnd(year, numSpace);
		}

		void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary)
		{
			for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
			{
				// Merge all those values with the global results
				AncestorType::pResults.merge(numSpaceToYear[numSpace], pValuesForTheCurrentYear[numSpace]);
			}
			
			// Next variable
			NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
		}

		void hourBegin(uint hourInTheYear)
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

		void hourForEachLink(State& state, unsigned int numSpace)
		{
			// Flow assessed over all MC years (linear)
			pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear]
				+= state.ntc->ValeurDuFlux[state.link->index];
			// Next item in the list
			NextType::hourForEachLink(state, numSpace);
		}

		void hourEnd(State& state, uint hourInTheYear)
		{
			// Next
			NextType::hourEnd(state, hourInTheYear);
		}

		void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
		{
			if (dataLevel & Category::link)
			{
				if (digestLevel & Category::digestFlowLinear)
				{
					results.data.matrix.entry[results.data.link->from->index][results.data.link->with->index] =
						AncestorType::pResults.avgdata.allYears;
					results.data.matrix.entry[results.data.link->with->index][results.data.link->from->index] =
						- AncestorType::pResults.avgdata.allYears;
				}
			}

			// Next
			NextType::buildDigest(results, digestLevel, dataLevel);
		}

		Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(uint, uint numSpace) const
		{
			return pValuesForTheCurrentYear[numSpace].hour;
		}

		void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision, uint numSpace) const
		{
			// Write the data for the current year
			results.variableCaption = VCardType::Caption();
			pValuesForTheCurrentYear[numSpace].template
				buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
		}


	private:
		//! Intermediate values for each year
		typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
		unsigned int pNbYearsParallel;

	}; // class FlowLinear






} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__
