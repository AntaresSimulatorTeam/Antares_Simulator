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
#ifndef __SOLVER_VARIABLE_ECONOMY_Overflow_H__
# define __SOLVER_VARIABLE_ECONOMY_Overflow_H__

# include "../variable.h"


namespace Antares
{
	namespace Solver
	{
		namespace Variable
		{
			namespace Economy
			{

				struct VCardOverflow
				{
					//! Caption
					static const char* Caption() { return "H. OVFL"; }
					//! Unit
					static const char* Unit() { return "%"; }

					//! The short description of the variable
					static const char* Description() { return "Hydro overflow"; }

					//! The expecte results
					typedef Results<
						R::AllYears::Average<      // The average values throughout all years
						R::AllYears::StdDeviation< // The standard deviation values throughout all years
						R::AllYears::Min<          // The minimum values throughout all years
						R::AllYears::Max<          // The maximum values throughout all years
						> > > > >  ResultsType;

					//! The VCard to look for for calculating spatial aggregates
					typedef VCardOverflow VCardForSpatialAggregate;

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
						columnCount = 1,
						//! The Spatial aggregation
						spatialAggregate = Category::noSpatialAggregate,
						spatialAggregateMode = Category::spatialAggregateEachYear,
						spatialAggregatePostProcessing = 0,
						//! Intermediate values
						hasIntermediateValues = 1,
					};

					typedef IntermediateValues IntermediateValuesBaseType;
					typedef IntermediateValues * IntermediateValuesType;

					typedef IntermediateValuesBaseType * IntermediateValuesTypeForSpatialAg;

				}; // class VCard


				   /*!
				   ** \brief Reservoir level
				   */
				template<class NextT = Container::EndOfList>
				class Overflows : public Variable::IVariable<Overflows<NextT>, NextT, VCardOverflow>
				{
				public:
					//! Type of the next static variable
					typedef NextT NextType;
					//! VCard
					typedef VCardOverflow VCardType;
					//! Ancestor
					typedef Variable::IVariable<Overflows<NextT>, NextT, VCardType> AncestorType;

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
					~Overflows()
					{
						delete[] pValuesForTheCurrentYear;
					}

					void initializeFromStudy(Data::Study& study)
					{
						pNbYearsParallel = study.maxNbYearsInParallel;

						InitializeResultsFromStudy(AncestorType::pResults, study);

						pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
						for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
							pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);

						// Next
						NextType::initializeFromStudy(study);
					}

					template<class R>
					static void InitializeResultsFromStudy(R& results, Data::Study& study)
					{
						VariableAccessorType::InitializeAndReset(results, study);
						results.averageMaxValue(study.runtime->rangeLimits.year[Data::rangeCount]);
						results.stdDeviationMaxValue(study.runtime->rangeLimits.year[Data::rangeCount]);
					}


					void initializeFromArea(Data::Study* study, Data::Area* area)
					{
						// For this variable (if no revervoir management), "N/A" is printed in output files 
						pIsNotApplicable = not area->hydro.reservoirManagement;

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
						for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
							pValuesForTheCurrentYear[numSpace].reset();
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
						// Compute all statistics for the current year (daily, weekly, monthly, annual).
						pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();

						// Next variable
						NextType::yearEnd(year, numSpace);
					}

					void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary)
					{
						for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
						{
							// Merge all those values with the global results
							AncestorType::pResults.merge(numSpaceToYear[numSpace] /*year*/, pValuesForTheCurrentYear[numSpace]);
						}

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
						// Retrieving hourly reservoir levels of week simulation
						pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear] = state.hourlyResults->debordementsHoraires[state.hourInTheWeek];

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
						// Next variable
						NextType::hourEnd(state, hourInTheYear);
					}

					Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(unsigned int, unsigned int numSpace) const
					{
						return pValuesForTheCurrentYear[numSpace].hour;
					}

					void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision, unsigned int numSpace) const
					{
						if (pIsNotApplicable && !(fileLevel & Category::id))
							results.data.nonApplicableColIdx.push_back(results.data.columnIndex);

						// Write the data for the current year
						results.variableCaption = VCardType::Caption();
						pValuesForTheCurrentYear[numSpace].template
							buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
					}


					void buildSurveyReport(SurveyResults& results, int dataLevel, int fileLevel, int precision) const
					{
						// Generating value for the area
						// Only if there are some results to export...
						if (0 != ResultsType::count)
						{
							// And only if we match the current data level _and_ precision level
							if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
								&& (precision & VCardType::precision))
							{
								if (pIsNotApplicable)
								{
									// Only min and max have to be printed in id-{precision}.txt
									if (fileLevel & Category::id)
										for (int i = 0; i < 2; i++)
											results.data.nonApplicableColIdx.push_back(results.data.columnIndex + i);
									// All variable results are printed (see ResultsType) in other output files
									else
										for (int i = 0; i < ResultsType::count; i++)
											results.data.nonApplicableColIdx.push_back(results.data.columnIndex + i);
								}

								VariableAccessorType::template
									BuildSurveyReport<VCardType>(results, AncestorType::pResults, dataLevel, fileLevel, precision);
							}

						}
						// Ask to the next item in the static list to export
						// its results as well
						NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
					}


				private:
					//! Intermediate values for each year
					typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
					unsigned int pNbYearsParallel;

				}; // class HydroLevel





			} // namespace Economy
		} // namespace Variable
	} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_Overflow_H__

