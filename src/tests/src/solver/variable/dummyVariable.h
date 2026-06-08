// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable
{
struct VCardDummyVariable
{
    //! Caption
    static std::string Caption()
    {
        return "Dummy Variable";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Value of all the dispatchable generation throughout all MC years";
    }

    //! The expecte results
    using ResultsType = StandardResults<>;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardDummyVariable VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;
};

/*!
** \brief Marginal DummyVariable
*/
class DummyVariable: public Variable::IVariable<DummyVariable, VCardDummyVariable>
{
public:
    //! VCard
    typedef VCardDummyVariable VCardType;
    //! Ancestor
    typedef Variable::IVariable<DummyVariable, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = ((VCardType::categoryDataLevel & CDataLevel
                                       && VCardType::categoryFileLevel & CFile)
                                        ? VCardType::columnCount * ResultsType::count
                                        : 0);
    };

public:
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Average thoughout all years
        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* /*study*/, Data::Area* /*area*/)
    {
    }

    void initializeFromLink(Data::Study* /*study*/, Data::AreaLink* /*link*/)
    {
    }

    void simulationBegin()
    {
    }

    void simulationEnd()
    {
    }

    virtual double hourlyValue(unsigned int year, unsigned int hour) = 0;

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();
        for (unsigned int h = 0; h < HOURS_PER_YEAR; ++h)
        {
            pValuesForTheCurrentYear[numSpace][h] = hourlyValue(year, h);
        }
    }

    void yearEnd(unsigned int /*year*/, unsigned int numSpace)
    {
        VariableAccessorType::template ComputeStatistics<VCardType>(
          pValuesForTheCurrentYear[numSpace]);
    }

    void computeSummary(unsigned int year, unsigned int /*nbYearsForCurrentSummary*/)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[year],
                                             AncestorType::pResults,
                                             year);
    }

    void hourBegin(unsigned int /*hourInTheYear*/)
    {
    }

    void hourForEachArea(State& /*state*/, unsigned int /*numSpace*/)
    {
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

}; // class DummyVariable
} // namespace Antares::Solver::Variable
