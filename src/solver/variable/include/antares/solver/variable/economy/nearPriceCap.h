// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{

constexpr double eps = 1e-2;
constexpr double margin = 5.;

struct VCardNearPriceCap
{
    //! Caption
    static std::string Caption()
    {
        return "NPCAP HOURS";
    }

    //! Unit
    static std::string Unit()
    {
        return "Hours";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Near Price Cap Hours";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardNearPriceCap VCardForSpatialAggregate;

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
    static constexpr uint8_t decimal = 4;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSumThen1IfPositive;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

}; // class VCardNearPriceCap

/*!
** \brief
*/
template<class NextT = Container::EndOfList>
class NearPriceCap: public Variable::IVariable<NearPriceCap<NextT>, NextT, VCardNearPriceCap>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardNearPriceCap VCardType;
    //! Ancestor
    typedef Variable::IVariable<NearPriceCap<NextT>, NextT, VCardType> AncestorType;

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
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }

        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        unsuppliedEnergyCost = area->thermal.unsuppliedEnergyCost;

        NextType::initializeFromArea(study, area);
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
        NextType::simulationBegin();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();

        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Time related aggregations to daily, weekly, monthly and annual results
        // are computed by summing hourly results
        pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();

        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        double mrgPrice = -state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek];
        if (mrgPrice > unsuppliedEnergyCost - margin + eps)
        {
            pValuesForTheCurrentYear[numSpace][state.hourInTheYear] = 1.;
        }
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            // Write the data for the current year
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;
    double unsuppliedEnergyCost = 0.;
}; // class NearPriceCap

} // namespace Antares::Solver::Variable::Economy
