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
#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__
#define __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__

#include <antares/study/area/constants.h>

#include "../../variable.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardCongestionProbability
{
    //! Caption
    static std::string Caption()
    {
        return "CONG. PROB. (+/-)";
    }

    //! Unit
    static std::string Unit()
    {
        return "%";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Probability for the line to be congested in the upstream-downstream way";
    }

    //! The expecte results
    typedef Results<R::AllYears::Raw< // The raw values throughout all years
      >>
      ResultsType;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 2;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 2;
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

    struct Multiple
    {
        static std::string Caption(uint indx)
        {
            switch (indx)
            {
            case 0:
                return "CONG. PROB +";
            case 1:
                return "CONG. PROB -";
            default:
                return "<unknown>";
            }
        }

        static std::string Unit([[maybe_unused]] const unsigned int indx)
        {
            return VCardCongestionProbability::Unit();
        }
    };

}; // class VCard

/*!
** \brief Marginal CongestionProbability
*/
template<class NextT = Container::EndOfList>
class CongestionProbability
    : public Variable::IVariable<CongestionProbability<NextT>, NextT, VCardCongestionProbability>
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
    ~CongestionProbability()
    {
        delete[] pValuesForTheCurrentYear;
        delete[] pValuesForYearLocalReport;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        yearsWeight = study.parameters.getYearsWeight();
        yearsWeightSum = study.parameters.getYearsWeightSum();

        // Average on all years
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(study);
            AncestorType::pResults[i].reset();
        }

        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
            {
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);
            }
        }

        pValuesForYearLocalReport = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
            {
                pValuesForYearLocalReport[numSpace][i].initializeFromStudy(study);
            }
        }

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

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
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

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (uint i = 0; i != VCardType::columnCount; ++i)
            {
                // Merge all those values with the global results
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
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

    void hourForEachLink(State& state, unsigned int numSpace)
    {
        // Ratio take into account MC year weight
        float ratio = yearsWeight[state.year] / yearsWeightSum;

        assert(state.link != NULL);
        const auto& linkDirectCapa = state.link->directCapacities;
        const auto& linkIndirectCapa = state.link->indirectCapacities;
        // CONG. PROB +
        if (state.ntc.ValeurDuFlux[state.link->index]
            > +linkDirectCapa.getCoefficient(state.year, state.hourInTheYear) - 10e-6)
        {
            pValuesForTheCurrentYear[numSpace][0].hour[state.hourInTheYear] += 100.0 * ratio;
        }
        // CONG. PROB -
        if (state.ntc.ValeurDuFlux[state.link->index]
            < -linkIndirectCapa.getCoefficient(state.year, state.hourInTheYear) + 10e-6)
        {
            pValuesForTheCurrentYear[numSpace][1].hour[state.hourInTheYear] += 100.0 * ratio;
        }

        // Next item in the list
        NextType::hourForEachLink(state, numSpace);
    }

    void beforeYearByYearExport(uint /*year*/, uint numSpace)
    {
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            for (uint h = 0; h != HOURS_PER_YEAR; ++h)
            {
                pValuesForYearLocalReport[numSpace][i].hour[h] = (pValuesForTheCurrentYear[numSpace]
                                                                                          [i]
                                                                                            .hour[h]
                                                                  > 0.)
                                                                   ? 100.
                                                                   : 0.;
            }

            for (uint d = 0; d != DAYS_PER_YEAR; ++d)
            {
                pValuesForYearLocalReport[numSpace][i].day[d] = (pValuesForTheCurrentYear[numSpace]
                                                                                         [i]
                                                                                           .day[d]
                                                                 > 0.)
                                                                  ? 100.
                                                                  : 0.;
            }

            for (uint w = 0; w != WEEKS_PER_YEAR; ++w)
            {
                pValuesForYearLocalReport[numSpace][i].week[w] = (pValuesForTheCurrentYear[numSpace]
                                                                                          [i]
                                                                                            .week[w]
                                                                  > 0.)
                                                                   ? 100.
                                                                   : 0.;
            }

            for (uint m = 0; m != MONTHS_PER_YEAR; ++m)
            {
                pValuesForYearLocalReport[numSpace][i].month[m] = (pValuesForTheCurrentYear
                                                                     [numSpace][i]
                                                                       .month[m]
                                                                   > 0.)
                                                                    ? 100.
                                                                    : 0.;
            }

            pValuesForYearLocalReport[numSpace][i].year = (pValuesForTheCurrentYear[numSpace][i]
                                                             .year
                                                           > 0.)
                                                            ? 100.
                                                            : 0.;
        }
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint column,
      uint numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
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
                pValuesForYearLocalReport[numSpace][i]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
            results.isCurrentVarNA++;
        }
    }

private:
    std::vector<float> yearsWeight;
    float yearsWeightSum;
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    typename VCardType::IntermediateValuesType pValuesForYearLocalReport;
    unsigned int pNbYearsParallel;

}; // class CongestionProbability

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__
