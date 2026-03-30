// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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

template<typename Derived>
struct StandardVariableTraits
{
    static constexpr std::string_view kCaption = "VAR";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Variable";

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = 0;
    static constexpr uint8_t decimal = 0;
    static constexpr int columnCount = 1;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t hasIntermediateValues = 1;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;
    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

    static double getValue(const State&, unsigned int)
    {
        return 0.;
    }

    static void yearEndHook(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static void initializeAreaHook(Data::Study*, Data::Area*)
    {
    }
};

template<typename TraitsType>
struct VCardStandardVariable: public StandardVariableTraits<TraitsType>
{
    using BaseType = StandardVariableTraits<TraitsType>;

    inline static constexpr std::string_view kCaption = TraitsType::kCaption;
    inline static constexpr std::string_view kUnit = BaseType::kUnit;
    inline static constexpr std::string_view kDescription = TraitsType::kDescription;

    static std::string Caption()
    {
        return std::string(kCaption);
    }

    static std::string Unit()
    {
        return std::string(kUnit);
    }

    static std::string Description()
    {
        return std::string(kDescription);
    }

    using ResultsType = typename BaseType::ResultsType;
    using VCardForSpatialAggregate = VCardStandardVariable<TraitsType>;
    using IntermediateValuesBaseType = typename BaseType::IntermediateValuesBaseType;
    using IntermediateValuesType = typename BaseType::IntermediateValuesType;
    using IntermediateValuesTypeForSpatialAg = typename BaseType::
      IntermediateValuesTypeForSpatialAg;

    static constexpr uint8_t categoryDataLevel = BaseType::categoryDataLevel;
    static constexpr uint8_t categoryFileLevel = BaseType::categoryFileLevel;
    static constexpr uint8_t precision = BaseType::precision;
    static constexpr uint8_t nodeDepthForGUI = BaseType::nodeDepthForGUI;
    static constexpr uint8_t decimal = TraitsType::decimal;
    static constexpr int columnCount = BaseType::columnCount;
    static constexpr uint8_t spatialAggregate = TraitsType::spatialAggregate;
    static constexpr uint8_t spatialAggregateMode = BaseType::spatialAggregateMode;
    static constexpr uint8_t spatialAggregatePostProcessing = BaseType::
      spatialAggregatePostProcessing;
    static constexpr uint8_t hasIntermediateValues = BaseType::hasIntermediateValues;
    static constexpr uint8_t isPossiblyNonApplicable = TraitsType::isPossiblyNonApplicable;
};

namespace detail
{
template<class VCard, class Next, int CDataLevel, int CFile>
inline constexpr int StandardVariableStatisticsCount
  = ((VCard::categoryDataLevel & CDataLevel && VCard::categoryFileLevel & CFile)
       ? (Next::template Statistics<CDataLevel, CFile>::count
          + VCard::columnCount * VCard::ResultsType::count)
       : Next::template Statistics<CDataLevel, CFile>::count);
}

template<typename Derived, typename NextT, typename VCardType>
class StandardVariableBase: public Variable::IVariable<Derived, NextT, VCardType>
{
public:
    using NextType = NextT;
    using AncestorType = Variable::IVariable<Derived, NextT, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr int count = 1 + NextT::count;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = detail::
          StandardVariableStatisticsCount<VCardType, NextType, CDataLevel, CFile>;
    };

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
        pArea = area;
        VCardType::initializeAreaHook(study, area);
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace].reset();
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        VCardType::yearEndHook(pValuesForTheCurrentYear[numSpace]);
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace][state.hourInTheYear] = VCardType::getValue(state,
                                                                                      numSpace);
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
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

protected:
    Data::Area* pArea = nullptr;
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel = 0;
};

} // namespace Antares::Solver::Variable::Economy
