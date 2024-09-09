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
#ifndef __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__
#define __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__

#include "../../variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardFlowQuad
{
    //! Caption
    static std::string Caption()
    {
        return "FLOW QUAD.";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Flow (quad.)";
    }

    //! The expecte results
    typedef Results<R::AllYears::Raw< // Raw values
      >>
      ResultsType;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
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
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesType;

}; // class VCard

/*!
** \brief Marginal FlowQuad
*/
template<class NextT = Container::EndOfList>
class FlowQuad: public Variable::IVariable<FlowQuad<NextT>, NextT, VCardFlowQuad>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardFlowQuad VCardType;
    //! Ancestor
    typedef Variable::IVariable<FlowQuad<NextT>, NextT, VCardType> AncestorType;

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
    ~FlowQuad()
    {
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Average on all years
        pNbHours = study.runtime.rangeLimits.hour[Data::rangeEnd] + 1;
        AncestorType::pResults.initializeFromStudy(study);
        AncestorType::pResults.reset();

        // Intermediate values
        pValuesForTheCurrentYear.initializeFromStudy(study);

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
        assert(link && "invalid interconnection");
        pLinkGlobalIndex = link->index;
        // Next
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        pValuesForTheCurrentYear.reset();

        // Next
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        // Flow assessed over all MC years (linear)
        (void)::memcpy(pValuesForTheCurrentYear.hour,
                       transitMoyenInterconnexionsRecalculQuadratique[pLinkGlobalIndex].data(),
                       sizeof(double) * pNbHours);

        // Compute all statistics for the current year (daily,weekly,monthly)
        pValuesForTheCurrentYear.computeStatisticsForTheCurrentYear();
        // Merge all those values with the global results
        AncestorType::pResults.merge(0, pValuesForTheCurrentYear);

        // Next
        NextType::simulationEnd();
    }

    void yearBegin(uint year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(uint year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
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
        // Next item in the list
        NextType::hourForEachLink(state, numSpace);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        if (dataLevel & Category::DataLevel::link)
        {
            if (digestLevel & Category::digestFlowQuad)
            {
                results.data.matrix
                  .entry[results.data.link->from->index][results.data.link->with->index]
                  = AncestorType::pResults.rawdata.allYears;
                results.data.matrix
                  .entry[results.data.link->with->index][results.data.link->from->index]
                  = -AncestorType::pResults.rawdata.allYears;
            }
        }
        // Next
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint,
      uint) const
    {
        return pValuesForTheCurrentYear.hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            // Write the data for the current year
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear.template buildAnnualSurveyReport<VCardType>(results,
                                                                                 fileLevel,
                                                                                 precision);
        }
    }

private:
    uint pLinkGlobalIndex;
    uint pNbHours;
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;

}; // class FlowQuad

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__
