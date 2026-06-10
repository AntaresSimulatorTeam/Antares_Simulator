// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_MiscGenMinusRowPSP_H__
#define __SOLVER_VARIABLE_ECONOMY_MiscGenMinusRowPSP_H__

#include <antares/study/area/constants.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardMiscGenMinusRowPSP
{
    //! Caption
    static std::string Caption()
    {
        return "MISC. NDG";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Non-dispatchable generation (not including wind and run-of-the-river)";
    }

    //! The expecte results
    using ResultsType = Results<std::tuple<R::AllYears::Average>>;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardMiscGenMinusRowPSP VCardForSpatialAggregate;

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

}; // class VCard

/*!
** \brief Marginal MiscGenMinusRowPSP
*/
class MiscGenMinusRowPSP: public Variable::IVariable<MiscGenMinusRowPSP, VCardMiscGenMinusRowPSP>
{
public:
    //! VCard
    typedef VCardMiscGenMinusRowPSP VCardType;
    //! Ancestor
    typedef Variable::IVariable<MiscGenMinusRowPSP, VCardType> AncestorType;

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
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Average on all years
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
    }

    void initializeFromArea(Data::Study* /*study*/, Data::Area* area)
    {
        // Copy raw values
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            // Memcpy for the first value
            (void)::memcpy(pValuesForTheCurrentYear[numSpace].hour,
                           area->miscGen.entry[0],
                           sizeof(double) * area->miscGen.height);

            {
                // We must not include the CHP and the Solar. So we have to start from `2`
                for (unsigned int x = 1; x != (unsigned int)Data::fhhPSP; ++x)
                {
                    const Matrix<>::ColumnType& col = area->miscGen.entry[x];
                    for (unsigned int y = 0; y != area->miscGen.height; ++y)
                    {
                        pValuesForTheCurrentYear[numSpace].hour[y] += col[y];
                    }
                }
            }
        }
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void yearEnd(unsigned int /*year*/, unsigned int numSpace)
    {
        // Compute all statistics for the current year (daily,weekly,monthly)
        pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
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

}; // class MiscGenMinusRowPSP

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_MiscGenMinusRowPSP_H__
