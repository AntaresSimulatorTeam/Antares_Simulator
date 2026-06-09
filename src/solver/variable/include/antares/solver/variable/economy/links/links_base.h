// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file links_base.h
**
** \brief Base class for link-level economy variables
**
** ## Traits Contract
**
** A valid Links variable Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsProfile : type alias for results template
**   - \c decimal : uint8_t
**   - \c spatialAggregate : uint8_t
**   - \c computeStats(IntermediateValues&) -> void
**
** - Optional hooks (dispatched via \c if constexpr):
**   - \c hourForEachLink(IntermediateValues&, State&) -> void
**   - \c hourValue(State&) -> double (accumulates to hourly values)
**   - \c hourValue(State&, double upstreamPrice, double downstreamPrice) -> double (computes link
*value)
**   - \c buildDigest(SurveyResults&, int digestLevel, int dataLevel, AncestorType&) -> void
**
** ## Hook execution order
** - initializeFromStudy() / initializeFromLink()
** - simulationBegin() / simulationEnd()
** - yearBegin() / yearEnd()
** - computeSummary() / hourBegin() -> hourForEachLink() -> [hourValue hook]
** - buildDigest() / localBuildAnnualSurveyReport()
*/

#include <antares/solver/variable/economy/economy_base.h>

namespace Antares::Solver::Variable::Economy
{

template<class Traits>
struct VCard_LinkBase
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

    //! The expected results
    using ResultsType = typename Traits::ResultsProfile;

    using VCardForSpatialAggregate = VCard_LinkBase;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Decimal precision
    static constexpr uint8_t decimal = Traits::decimal;
    //! Number of columns used by the variable (one results profile per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;

}; // class VCard

template<class Traits>
class EconomyLink_Base: public Variable::IVariable<EconomyLink_Base<Traits>, VCard_LinkBase<Traits>>
{
public:
    //! VCard
    using VCardType = VCard_LinkBase<Traits>;
    //! Ancestor
    using AncestorType = Variable::IVariable<EconomyLink_Base<Traits>, VCardType>;

    //! List of expected results
    using ResultsType = typename VCardType::ResultsType;

    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = detail::
          statisticsCount<VCardType, ResultsType, CDataLevel, CFile>;
    };

public:
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        AncestorType::pResults.initializeFromStudy(study);
        AncestorType::pResults.reset();

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
    }

    void simulationBegin()
    {
        for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
    }

    void yearBegin([[maybe_unused]] uint year, uint numSpace)
    {
        pValuesForTheCurrentYear[numSpace].reset();
    }

    void yearEnd([[maybe_unused]] uint year, uint numSpace)
    {
        Traits::computeStats(pValuesForTheCurrentYear[numSpace]);
    }

    void computeSummary(uint year, uint numSpace)
    {
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
    }

    void hourForEachLink(State& state, uint numSpace)
    {
        if constexpr (requires {
                          Traits::hourForEachLink(pValuesForTheCurrentYear[numSpace], state);
                      })
        {
            Traits::hourForEachLink(pValuesForTheCurrentYear[numSpace], state);
        }
        else if constexpr (requires { Traits::hourValue(state); })
        {
            pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear] += Traits::hourValue(
              state);
        }
        else if constexpr (requires { Traits::hourValue(state, 0.0, 0.0); })
        {
            const double upstreamPrice = state.problemeHebdo
                                           ->ResultatsHoraires[state.link->from->index]
                                           .CoutsMarginauxHoraires[state.hourInTheWeek];
            const double downstreamPrice = state.problemeHebdo
                                             ->ResultatsHoraires[state.link->with->index]
                                             .CoutsMarginauxHoraires[state.hourInTheWeek];
            pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear] = Traits::hourValue(
              state,
              upstreamPrice,
              downstreamPrice);
        }
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        if constexpr (requires {
                          Traits::buildDigest(results,
                                              digestLevel,
                                              dataLevel,
                                              AncestorType::pResults);
                      })
        {
            Traits::buildDigest(results, digestLevel, dataLevel, AncestorType::pResults);
        }
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint,
      uint numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
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

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    uint pNbYearsParallel;

}; // class EconomyLink_Base

} // namespace Antares::Solver::Variable::Economy
