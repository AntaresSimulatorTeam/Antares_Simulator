// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__
#define __SOLVER_VARIABLE_ECONOMY_CongestionProbability_H__

#include <array>
#include <vector>

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

    //! Expected results configuration
    using ResultsType = Results<std::tuple<R::AllYears::Raw>>;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Decimal precision
    static constexpr uint8_t decimal = 2;
    //! Number of columns used by the variable (one results configuration per column)
    static constexpr int columnCount = 2;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = std::array<IntermediateValues, columnCount>;
    using IntermediateValuesType = std::vector<IntermediateValuesBaseType>;

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
class CongestionProbability
    : public Variable::IVariable<CongestionProbability, VCardCongestionProbability>
{
public:
    //! VCard
    typedef VCardCongestionProbability VCardType;
    //! Ancestor
    typedef Variable::IVariable<CongestionProbability, VCardType> AncestorType;

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

        yearsWeight = study.parameters.getYearsWeight();
        yearsWeightSum = study.parameters.getYearsWeightSum();

        // Average on all years
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(study);
            AncestorType::pResults[i].reset();
        }

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        pValuesForYearLocalReport.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
            {
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);
                pValuesForYearLocalReport[numSpace][i].initializeFromStudy(study);
            }
        }
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            pValuesForTheCurrentYear[numSpace][0].reset();
            pValuesForTheCurrentYear[numSpace][1].reset();
        }
    }

    void simulationEnd()
    {
    }

    void yearBegin(uint /*year*/, unsigned int numSpace)
    {
        // Reset
        pValuesForTheCurrentYear[numSpace][0].reset();
        pValuesForTheCurrentYear[numSpace][1].reset();

        pValuesForYearLocalReport[numSpace][0].reset();
        pValuesForYearLocalReport[numSpace][1].reset();
    }

    void yearEnd(uint /*year*/, uint numSpace)
    {
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            // Compute all statistics for the current year (daily,weekly,monthly)
            pValuesForTheCurrentYear[numSpace][i].computeProbabilitiesForTheCurrentYear();
        }
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            // Merge all those values with the global results
            AncestorType::pResults[i].merge(year, pValuesForTheCurrentYear[numSpace][i]);
        }
    }

    void hourBegin(uint /*hourInTheYear*/)
    {
    }

    void hourForEachArea(State& /*state*/, unsigned int /*numSpace*/)
    {
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
    }

    void beforeYearByYearExport(uint /*year*/, uint numSpace)
    {
        auto binarize = [](double v) { return v > 0. ? 100. : 0.; };
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            const auto& src = pValuesForTheCurrentYear[numSpace][i];
            auto& dst = pValuesForYearLocalReport[numSpace][i];
            for (uint h = 0; h != HOURS_PER_YEAR; ++h)
            {
                dst.hour[h] = binarize(src.hour[h]);
            }
            for (uint d = 0; d != DAYS_PER_YEAR; ++d)
            {
                dst.day[d] = binarize(src.day[d]);
            }
            for (uint w = 0; w != WEEKS_PER_YEAR; ++w)
            {
                dst.week[w] = binarize(src.week[w]);
            }
            for (uint m = 0; m != MONTHS_PER_YEAR; ++m)
            {
                dst.month[m] = binarize(src.month[m]);
            }
            dst.year = binarize(src.year);
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
