/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_VARIABLE_ECONOMY_thermalAirPollutantEmissions_H__
#define __SOLVER_VARIABLE_ECONOMY_thermalAirPollutantEmissions_H__

#include <antares/study/study.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardThermalAirPollutantEmissions
{
    //! Caption not used: several columns
    static std::string Caption()
    {
        return "";
    }

    //! Unit
    static std::string Unit()
    {
        return "Tons";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Overall pollutant emissions expected from all the thermal clusters";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardThermalAirPollutantEmissions VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = Antares::Data::Pollutant::POLLUTANT_MAX;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType[columnCount];
    typedef IntermediateValuesBaseType* IntermediateValuesType;

    typedef IntermediateValuesBaseType* IntermediateValuesTypeForSpatialAg;

    struct Multiple
    {
        static std::string Caption(const unsigned int indx)
        {
            if (indx < Antares::Data::Pollutant::POLLUTANT_MAX)
            {
                return Antares::Data::Pollutant::getPollutantName(indx).c_str();
            }

            return "<unknown>";
        }

        static std::string Unit([[maybe_unused]] const unsigned int indx)
        {
            return VCardThermalAirPollutantEmissions::Unit();
        }
    };
}; // class VCard

/*!
** \brief Marginal ThermalAirPollutantEmissions
*/
template<class NextT = Container::EndOfList>
class ThermalAirPollutantEmissions: public Variable::IVariable<ThermalAirPollutantEmissions<NextT>,
                                                               NextT,
                                                               VCardThermalAirPollutantEmissions>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardThermalAirPollutantEmissions VCardType;
    //! Ancestor
    typedef Variable::IVariable<ThermalAirPollutantEmissions<NextT>, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

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
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + VCardType::columnCount * ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ~ThermalAirPollutantEmissions()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
            {
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);
            }
        }

        // Next
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        for (unsigned int i = 0; i != VCardType::columnCount; ++i)
        {
            results[i].initializeFromStudy(study);
            results[i].reset();
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
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::template ComputeStatistics<VCardType>(
          pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                                 AncestorType::pResults,
                                                 numSpaceToYear[numSpace]);
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
        auto area = state.area;
        auto& thermal = state.thermal;
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            // Multiply every pollutant factor with production
            for (int pollutant = 0; pollutant < Antares::Data::Pollutant::POLLUTANT_MAX;
                 pollutant++)
            {
                pValuesForTheCurrentYear[numSpace][pollutant][state.hourInTheYear]
                  += cluster->emissions.factors[pollutant]
                     * thermal[state.area->index].thermalClustersProductions[cluster->enabledIndex];
            }
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // The current variable is actually a multiple-variable.
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            if (AncestorType::isPrinted[i])
            {
                // Write the data for the current year
                results.variableCaption = VCardType::Multiple::Caption(i);
                results.variableUnit = VCardType::Multiple::Unit(i);
                pValuesForTheCurrentYear[numSpace][i]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
            results.isCurrentVarNA++;
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;

}; // class ThermalAirPollutantEmissions

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_thermalAirPollutantEmissions_H__
