// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <type_traits>

#include <antares/memory/memory.h>
#include <antares/solver/variable/categories.h>
#include <antares/solver/variable/container.h>
#include <antares/solver/variable/state.h>
#include <antares/solver/variable/storage/intermediate.h>
#include <antares/solver/variable/storage/results.h>
#include <antares/solver/variable/surveyresults.h>
#include <antares/solver/variable/variable.h>
#include <antares/study/area/area.h>
#include <antares/study/area/links.h>
#include <antares/study/study.h>

namespace Antares::Solver::Variable::Economy
{

// The `detail` namespace contains implementation details and helper
// utilities for the Economy variable system. Symbols placed in this
// namespace are internal implementation artifacts (traits, fallbacks,
// SFINAE helpers) and are not part of the public API. Using a dedicated
// `detail` namespace makes the intent explicit: these types help detect
// optional nested types or member functions in `Traits` (for example
// `AuxiliaryDataType`) and provide safe defaults when those optional
// members are absent. This enables generic code to compile whether or
// not a particular `Traits` class provides extra auxiliary data.
namespace detail
{
struct EmptyAuxiliaryData
{
};

template<class TraitsT, class = void>
struct AuxiliaryDataType
{
    using type = EmptyAuxiliaryData;
};

// Triggers static_assert only when a template fallback branch is actually instantiated.
template<class>
inline constexpr bool always_false_v = false;

template<class TraitsT>
struct AuxiliaryDataType<TraitsT, std::void_t<typename TraitsT::AuxiliaryDataType>>
{
    using type = typename TraitsT::AuxiliaryDataType;
};
} // namespace detail

template<class Traits>
struct VCard_Base
{
    //! Caption
    static std::string Caption()
    {
        return Traits::Caption();
    }

    //! Unit
    static std::string Unit()
    {
        return Traits::Unit();
    }

    //! The short description of the variable
    static std::string Description()
    {
        return Traits::Description();
    }

    //! The expecte results
    typedef typename Traits::ResultsType ResultsType;

    typedef VCard_Base VCardForSpatialAggregate;

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
    static constexpr uint8_t decimal = Traits::decimal;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Traits::spatialAggregate;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

}; // class VCard

/*!
** \brief Base class for economy variables like LOLP and LOLD
*/
template<class Traits, class NextT = Container::EndOfList>
class Economy_Base
    : public Variable::IVariable<Economy_Base<Traits, NextT>, NextT, VCard_Base<Traits>>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCard_Base<Traits> VCardType;
    //! Ancestor
    typedef Variable::IVariable<Economy_Base<Traits, NextT>, NextT, VCardType> AncestorType;

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
    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
        // Next
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        initializeFromAreaIfSupported(auxiliaryData_, study, area);

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
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
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

        yearBeginIfSupported(pValuesForTheCurrentYear[numSpace], auxiliaryData_, year, numSpace);

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
        // Compute all statistics for the current year (daily,weekly,monthly)
        Traits::computeStats(pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        setHourlyValueIfSupported(pValuesForTheCurrentYear[numSpace],
                                  auxiliaryData_,
                                  state,
                                  numSpace);

        // Next variable
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
    static void initializeFromAreaIfSupported(AuxiliaryDataType& auxiliaryData,
                                              Data::Study* study,
                                              Data::Area* area)
    {
        if constexpr (requires { Traits::initializeFromArea(auxiliaryData, study, area); })
        {
            Traits::initializeFromArea(auxiliaryData, study, area);
        }
    }

    static void yearBeginIfSupported(IntermediateValues& yearlyValues,
                                     AuxiliaryDataType& auxiliaryData,
                                     unsigned int year,
                                     unsigned int numSpace)
    {
        if constexpr (requires { Traits::yearBegin(yearlyValues, auxiliaryData, year, numSpace); })
        {
            Traits::yearBegin(yearlyValues, auxiliaryData, year, numSpace);
        }
    }

    static void setHourlyValueIfSupported(IntermediateValues& yearlyValues,
                                          AuxiliaryDataType& auxiliaryData,
                                          const State& state,
                                          unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::setHourlyValue(yearlyValues, auxiliaryData, state, numSpace);
                      })
        {
            Traits::setHourlyValue(yearlyValues, auxiliaryData, state, numSpace);
        }
        else if constexpr (requires { Traits::checkCondition(auxiliaryData, state); })
        {
            if (Traits::checkCondition(auxiliaryData, state))
            {
                yearlyValues[state.hourInTheYear] = Traits::value(auxiliaryData, state);
            }
        }
        else if constexpr (requires { Traits::checkCondition(state); })
        {
            if (Traits::checkCondition(state))
            {
                yearlyValues[state.hourInTheYear] = Traits::value(state);
            }
        }
        else
        {
            static_assert(detail::always_false_v<Traits>,
                          "Traits must provide either setHourlyValue(...), "
                          "checkCondition(auxiliaryData, state)+value(auxiliaryData, state), "
                          "or checkCondition(state)+value(state)");
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    AuxiliaryDataType auxiliaryData_{};
    unsigned int pNbYearsParallel = 0;

}; // class Economy_Base

} // namespace Antares::Solver::Variable::Economy
