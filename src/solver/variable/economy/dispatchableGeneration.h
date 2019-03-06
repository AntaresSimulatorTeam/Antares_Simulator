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
#ifndef __SOLVER_VARIABLE_ECONOMY_DispatchableGeneration_H__
# define __SOLVER_VARIABLE_ECONOMY_DispatchableGeneration_H__

# include "../variable.h"


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{


	struct VCardDispatchableGeneration
	{
		//! Caption
		static const char* Caption() {return "Dispatch. Gen.";}
		//! Unit
		static const char* Unit() {return "MWh";}

		//! The short description of the variable
		static const char* Description() {return "Value of all the dispatchable generation throughout all MC years";}

		//! The expecte results
		typedef Results<
			R::AllYears::Average<      // The average values throughout all years
			R::AllYears::StdDeviation< // The standard deviation values throughout all years
			R::AllYears::Min<          // The minimum values throughout all years
			R::AllYears::Max<          // The maximum values throughout all years
			> > > > >  ResultsType;

		//! The VCard to look for for calculating spatial aggregates
		typedef VCardDispatchableGeneration VCardForSpatialAggregate;

		enum
		{
			//! Data Level
			categoryDataLevel = Category::area,
			//! File level (provided by the type of the results)
			categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
			//! Precision (views)
			precision = Category::all,
			//! Indentation (GUI)
			nodeDepthForGUI = +0,
			//! Decimal precision
			decimal = 0,
			//! Number of columns used by the variable (One ResultsType per column)
			columnCount = 7,
			//! The Spatial aggregation
			spatialAggregate = Category::spatialAggregateSum,
			spatialAggregateMode = Category::spatialAggregateEachYear,
			spatialAggregatePostProcessing = 0,
			//! Intermediate values
			hasIntermediateValues = 1,
			//! Can this variable be non applicable (0 : no, 1 : yes)
			isPossiblyNonApplicable = 0,
		};

		typedef IntermediateValues IntermediateValuesBaseType[columnCount];
		typedef IntermediateValuesBaseType * IntermediateValuesType;

		typedef IntermediateValuesBaseType * IntermediateValuesTypeForSpatialAg;

		struct Multiple
		{
			static const char* Caption(const unsigned int indx)
			{
				switch (indx)
				{
					case 0: return "NUCLEAR";
					case 1: return "LIGNITE";
					case 2: return "COAL";
					case 3: return "GAS";
					case 4: return "OIL";
					case 5: return "MIX. FUEL";
					case 6: return "MISC. DTG";
					default: return "<unknown>";
				}
			}
		};
	}; // class VCard


	/*!
	** \brief Marginal DispatchableGeneration
	*/
	template<class NextT = Container::EndOfList>
	class DispatchableGeneration : public Variable::IVariable<DispatchableGeneration<NextT>, NextT, VCardDispatchableGeneration>
	{
	public:
		//! Type of the next static variable
		typedef NextT NextType;
		//! VCard
		typedef VCardDispatchableGeneration VCardType;
		//! Ancestor
		typedef Variable::IVariable<DispatchableGeneration<NextT>, NextT, VCardType> AncestorType;

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
		~DispatchableGeneration()
		{
			delete[] pValuesForTheCurrentYear;
			delete[] isPrinted;
		}

		void initializeFromStudy(Data::Study& study)
		{
			pNbYearsParallel = study.maxNbYearsInParallel;
			
			InitializeResultsFromStudy(AncestorType::pResults, study);

			// current variable output behavior container
			isPrinted = new bool[VCardType::columnCount];

			// Setting print info for current variable
			setPrintInfo(study);

			pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
			for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
				for (unsigned int i = 0; i != VCardType::columnCount; ++i)
					pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);


			// Next
			NextType::initializeFromStudy(study);
		}

		template<class R>
		static void InitializeResultsFromStudy(R& results, Data::Study& study)
		{
			const unsigned int years = study.runtime->rangeLimits.year[Data::rangeCount];
			for (unsigned int i = 0; i != VCardType::columnCount; ++i)
			{
				results[i].initializeFromStudy(study);
				results[i].reset();
				results[i].averageMaxValue(years);
				results[i].stdDeviationMaxValue(years);
			}
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

		bool* getPrintStatus() const { return isPrinted; }

		void setPrintInfo(Data::Study& study)
		{
			for (uint i = 0; i != VCardType::columnCount; ++i)
			{
				// Shifting (inside the variables print info collection) to the current variable print info
				study.parameters.variablesPrintInfo.find(VCardType::Multiple::Caption(i));
				// And then getting the print status
				isPrinted[i] = study.parameters.variablesPrintInfo.isPrinted();
			}
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
			// Reset the values for the current year
			for (unsigned int i = 0; i != VCardType::columnCount; ++i)
				pValuesForTheCurrentYear[numSpace][i].reset();
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

			VariableAccessorType::template ComputeStatistics<VCardType>(pValuesForTheCurrentYear[numSpace]);

			// Next variable
			NextType::yearEnd(year, numSpace);
		}

		void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary)
		{
			for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
				VariableAccessorType::ComputeSummary(	
														pValuesForTheCurrentYear[numSpace], 
														AncestorType::pResults, 
														numSpaceToYear[numSpace]
													);
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
			// Adding the dispatchable generation for the class_name fuel
			pValuesForTheCurrentYear[numSpace][state.cluster->groupID][state.hourInTheYear]
				+= state.thermalClusterProduction;
			// Next item in the list
			NextType::hourForEachThermalCluster(state, numSpace);
		}

		void hourEnd(State& state, unsigned int hourInTheYear)
		{
			NextType::hourEnd(state, hourInTheYear);
		}


		Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(unsigned int column, unsigned int numSpace) const
		{
			return pValuesForTheCurrentYear[numSpace][column].hour;
		}

		void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision, unsigned int numSpace) const
		{
			// The current variable is actually a multiple-variable.
			results.isCurrentVarNA = AncestorType::isNonApplicable;
			
			for (uint i = 0; i != VCardType::columnCount; ++i)
			{
				if (isPrinted[i])
				{
					// Write the data for the current year
					results.variableCaption = VCardType::Multiple::Caption(i);
					pValuesForTheCurrentYear[numSpace][i].template
						buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
				}
				results.isCurrentVarNA++;
			}
		}


	private:
		//! Intermediate values for each year
		typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
		unsigned int pNbYearsParallel;
		// Do we print results regarding internal variables in output files ? Or do we skip them ?
		bool* isPrinted;

	}; // class DispatchableGeneration






} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_DispatchableGeneration_H__
