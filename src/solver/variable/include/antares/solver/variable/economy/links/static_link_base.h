// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "../economy_base.h"

namespace Antares::Solver::Variable::Economy
{

/*!
** \brief Base class for link variables computed once across all MC years.
**
** Unlike EconomyLink_Base, these variables hold a single IntermediateValues
** (not one per parallel year). Data is loaded during initializeFromStudy or
** initializeFromLink, and stats+merge happen once in simulationEnd.
**
** Traits hooks (all optional, dispatched via requires):
**   onInitializeFromStudy(iv, aux, study)        -- store study pointers / sizes
**   onInitializeFromAreaLink(iv, aux, study, link) -- load data from link
**   onSimulationBegin(iv, aux)                   -- reset iv if needed
**   loadDataForSimulationEnd(iv, aux)            -- copy data into iv before stats
**   buildDigest(results, digestLevel, dataLevel, pResults) -- fill digest matrix
*/
template<class Traits>
struct VCardStaticLinkBase
{
    static std::string Caption()
    {
        return Traits::Caption();
    }

    static std::string Unit()
    {
        return Traits::Unit();
    }

    static std::string Description()
    {
        return Traits::Description();
    }

    using ResultsType = typename Traits::ResultsProfile;
    using VCardForSpatialAggregate = VCardStaticLinkBase;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t decimal = 0;
    static constexpr int columnCount = 1;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesType = IntermediateValues;

}; // struct VCardStaticLinkBase

template<class Traits>
class StaticLinkBase
    : public Variable::IVariable<StaticLinkBase<Traits>, VCardStaticLinkBase<Traits>>
{
public:
    using VCardType = VCardStaticLinkBase<Traits>;
    using AncestorType = Variable::IVariable<StaticLinkBase<Traits>, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

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
        AncestorType::pResults.initializeFromStudy(study);
        AncestorType::pResults.reset();
        pValuesForTheCurrentYear.initializeFromStudy(study);

        if constexpr (requires {
                          Traits::onInitializeFromStudy(pValuesForTheCurrentYear, aux_, study);
                      })
        {
            Traits::onInitializeFromStudy(pValuesForTheCurrentYear, aux_, study);
        }
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        if constexpr (requires {
                          Traits::onInitializeFromAreaLink(pValuesForTheCurrentYear,
                                                           aux_,
                                                           study,
                                                           link);
                      })
        {
            Traits::onInitializeFromAreaLink(pValuesForTheCurrentYear, aux_, study, link);
        }
    }

    void simulationBegin()
    {
        if constexpr (requires { Traits::onSimulationBegin(pValuesForTheCurrentYear, aux_); })
        {
            Traits::onSimulationBegin(pValuesForTheCurrentYear, aux_);
        }
    }

    void simulationEnd()
    {
        if constexpr (requires {
                          Traits::loadDataForSimulationEnd(pValuesForTheCurrentYear, aux_);
                      })
        {
            Traits::loadDataForSimulationEnd(pValuesForTheCurrentYear, aux_);
        }
        pValuesForTheCurrentYear.computeStatisticsForTheCurrentYear();
        AncestorType::pResults.merge(0, pValuesForTheCurrentYear);
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
      uint) const
    {
        return pValuesForTheCurrentYear.hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear.template buildAnnualSurveyReport<VCardType>(results,
                                                                                 fileLevel,
                                                                                 precision);
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    AuxiliaryDataType aux_{};

}; // class StaticLinkBase

} // namespace Antares::Solver::Variable::Economy
