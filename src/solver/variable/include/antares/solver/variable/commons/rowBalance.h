// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_RowBalance_H__
#define __SOLVER_VARIABLE_ECONOMY_RowBalance_H__

#include <antares/study/area/constants.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardRowBalance
{
    //! Caption
    static std::string Caption()
    {
        return "ROW BAL.";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Row Balance";
    }

    //! The expecte results
    using ResultsType = Results<std::tuple<R::AllYears::Raw>>;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardRowBalance VCardForSpatialAggregate;

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
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateOnce;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesType;
    typedef IntermediateValues IntermediateValuesBaseType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

}; // class VCard

/*!
** \brief Marginal RowBalance
*/
class RowBalance: public Variable::IVariable<RowBalance, VCardRowBalance>
{
public:
    //! VCard
    typedef VCardRowBalance VCardType;
    //! Ancestor
    typedef Variable::IVariable<RowBalance, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        count = 1,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? VCardType::columnCount * ResultsType::count
                       : 0),
        };
    };

public:
    ~RowBalance()
    {
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Average on all years
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear.initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Copy raw values
        unsigned int height = area->miscGen.height;
        (void)::memcpy(pValuesForTheCurrentYear.hour,
                       area->miscGen.entry[Data::fhhRowBalance],
                       sizeof(double) * height);

        if (study->parameters.mode == Data::SimulationMode::Adequacy)
        {
            for (unsigned int h = 0; h != height; ++h)
            {
                pValuesForTheCurrentYear.hour[h] -= area->reserves
                                                      .entry[Data::fhrPrimaryReserve][h];
            }
        }
        // Compute all statistics for the current year (daily,weekly,monthly)
        pValuesForTheCurrentYear.computeStatisticsForTheCurrentYear();

        // Merge all those values with the global results
        AncestorType::pResults.merge(0, pValuesForTheCurrentYear);
    }

    void simulationBegin()
    {
    }

    void simulationEnd()
    {
    }

    void yearBegin(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void yearEnd(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void computeSummary(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
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
                                      unsigned int) const
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
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;

}; // class RowBalance

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_RowBalance_H__
