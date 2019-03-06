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
#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__
# define __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__

# include "../../variable.h"
# include <antares/study/area/constants.h>


namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{


	struct VCardCongestionProbability
	{
		//! Caption
		static const char* Caption() {return "CONG. PROB. (+/-)";}
		//! Unit
		static const char* Unit() {return "%";}

		//! The short description of the variable
		static const char* Description() {return "Probability for the line to be congested in the upstream-downstream way";}

		//! The expecte results
		typedef Results<
			R::AllYears::Raw<      // The raw values throughout all years
			> >  ResultsType;

		enum
		{
			//! Data Level
			categoryDataLevel = Category::link,
			//! File level (provided by the type of the results)
			categoryFileLevel = ResultsType::categoryFile & (Category::va),
			//! Precision (views)
			precision = Category::all,
			//! Indentation (GUI)
			nodeDepthForGUI = +0,
			//! Decimal precision
			decimal = 2,
			//! Number of columns used by the variable (One ResultsType per column)
			columnCount = 2,
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

		struct Multiple
		{
			static const char* Caption(uint indx)
			{
				switch (indx)
				{
					case 0: return "CONG. PROB +";
					case 1: return "CONG. PROB -";
					default: return "<unknown>";
				}
			}
		};

	}; // class VCard


	/*!
	** \brief Marginal CongestionProbability
	*/
	template<class NextT = Container::EndOfList>
	class CongestionProbability : public Variable::IVariable<CongestionProbability<NextT>, NextT, VCardCongestionProbability>
	{
	public:
		//! Type of the next static variable
		typedef NextT NextType;
		//! VCard
		typedef VCardCongestionProbability VCardType;
		//! Ancestor
		typedef Variable::IVariable<CongestionProbability<NextT>, NextT, VCardType> AncestorType;

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
		~CongestionProbability()
		{
			delete[] pValuesForTheCurrentYear;
			delete[] pValuesForYearLocalReport;
			delete[] isPrinted;
		}

		void initializeFromStudy(Data::Study& study)
		{
			pNbYearsParallel = study.maxNbYearsInParallel;

			pRatio = 100. / (double) study.runtime->rangeLimits.year[Data::rangeCount];

			// Average on all years
			for (uint i = 0; i != VCardType::columnCount; ++i)
			{
				AncestorType::pResults[i].initializeFromStudy(study);
				AncestorType::pResults[i].reset();
			}

			// current variable output behavior container
			isPrinted = new bool[VCardType::columnCount];

			// Setting print info for current variable
			setPrintInfo(study);

			pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
			for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
				for (unsigned int i = 0; i != VCardType::columnCount; ++i)
					pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);

			pValuesForYearLocalReport = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
			for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
				for (unsigned int i = 0; i != VCardType::columnCount; ++i)
					pValuesForYearLocalReport[numSpace][i].initializeFromStudy(study);


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
			for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
			{
				pValuesForTheCurrentYear[numSpace][0].reset();
				pValuesForTheCurrentYear[numSpace][1].reset();
			}

			// Next
			NextType::simulationBegin();
		}

		void simulationEnd()
		{
			// Next variable
			NextType::simulationEnd();
		}


		void yearBegin(uint year, unsigned int numSpace)
		{
			// Reset
			pValuesForTheCurrentYear[numSpace][0].reset();
			pValuesForTheCurrentYear[numSpace][1].reset();

			pValuesForYearLocalReport[numSpace][0].reset();
			pValuesForYearLocalReport[numSpace][1].reset();	

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
		

		void yearEnd(uint year, uint numSpace)
		{
			for (uint i = 0; i != VCardType::columnCount; ++i)
			{
				// Compute all statistics for the current year (daily,weekly,monthly)
				pValuesForTheCurrentYear[numSpace][i].computeProbabilitiesForTheCurrentYear();
			}

			// Next variable
			NextType::yearEnd(year, numSpace);
		}

		void computeSummary(std::map<unsigned int, unsigned int> & numSpaceToYear, unsigned int nbYearsForCurrentSummary)
		{
			for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
			{
				for (uint i = 0; i != VCardType::columnCount; ++i)
				{
					// Merge all those values with the global results
					AncestorType::pResults[i].merge(numSpaceToYear[numSpace], pValuesForTheCurrentYear[numSpace][i]);
				}
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
			assert(state.link != NULL);
			auto& linkdata = state.link->data;
			// CONG. PROB +
			if (state.ntc->ValeurDuFlux[state.link->index] > + linkdata.entry[Data::fhlNTCDirect][state.hourInTheYear] - 10e-6)
				pValuesForTheCurrentYear[numSpace][0].hour[state.hourInTheYear] += pRatio;
			// CONG. PROB -
			if (state.ntc->ValeurDuFlux[state.link->index] < - linkdata.entry[Data::fhlNTCIndirect][state.hourInTheYear] + 10e-6)
				pValuesForTheCurrentYear[numSpace][1].hour[state.hourInTheYear] += pRatio;

			// Next item in the list
			NextType::hourForEachLink(state, numSpace);
		}

		void hourEnd(State& state, uint hourInTheYear)
		{
			// Next
			NextType::hourEnd(state, hourInTheYear);
		}

		void beforeYearByYearExport(uint /*year*/, uint numSpace)
		{
			for (uint i = 0; i != VCardType::columnCount; ++i)
			{
				for (uint h = 0; h != maxHoursInAYear; ++h)
					pValuesForYearLocalReport[numSpace][i].hour[h] = (pValuesForTheCurrentYear[numSpace][i].hour[h] > 0.) ? 100. : 0.;

				for (uint d = 0; d != maxDaysInAYear; ++d)
					pValuesForYearLocalReport[numSpace][i].day[d] = (pValuesForTheCurrentYear[numSpace][i].day[d] > 0.) ? 100. : 0.;

				for (uint w = 0; w != maxWeeksInAYear; ++w)
					pValuesForYearLocalReport[numSpace][i].week[w] = (pValuesForTheCurrentYear[numSpace][i].week[w] > 0.) ? 100. : 0.;

				for (uint m = 0; m != maxMonths; ++m)
					pValuesForYearLocalReport[numSpace][i].month[m] = (pValuesForTheCurrentYear[numSpace][i].month[m] > 0.) ? 100. : 0.;

				pValuesForYearLocalReport[numSpace][i].year = (pValuesForTheCurrentYear[numSpace][i].year > 0.) ? 100. : 0.;
			}


		}

		Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(uint column, uint numSpace) const
		{
			return pValuesForTheCurrentYear[numSpace][column].hour;
		}

		void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision, uint numSpace) const
		{
			// The current variable is actually a multiple-variable.
			results.isCurrentVarNA = AncestorType::isNonApplicable;

			for (uint i = 0; i != VCardType::columnCount; ++i)
			{				
				if(isPrinted[i])
				{
					// Write the data for the current year
					results.variableCaption = VCardType::Multiple::Caption(i);
					pValuesForYearLocalReport[numSpace][i].template
						buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
				}
				results.isCurrentVarNA++;
			}
		}


	private:
		double pRatio;
		//! Intermediate values for each year
		typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
		typename VCardType::IntermediateValuesType pValuesForYearLocalReport;
		unsigned int pNbYearsParallel;
		// Do we print results regarding internal variables in output files ? Or do we skip them ?
		bool* isPrinted;

	}; // class CongestionProbability





} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__
