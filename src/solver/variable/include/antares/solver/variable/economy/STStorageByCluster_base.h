// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file STStorageByCluster_base.h
**
** \brief Base class for short-term storage by cluster variables
**
** ## Traits Contract
**
** A valid STStorageByCluster Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - Results profile is provided by the base class (no trait alias required)
**   - \c clusterCount(const Data::Area*) -> size_t
**   - \c computeStats(IntermediateValues&) -> void
**   - \c buildSurveyReport(const std::vector<IntermediateValues>&, SurveyResults&, int, int) ->
*void
**
** - Optional hooks:
**   - \c setHourlyValue(std::vector<IntermediateValues>&, State&, uint numSpace) -> void
**   - Fallback (deprecated): \c setHourlyValue(std::vector<IntermediateValues>&, State&) -> void
*/

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

template<class Traits>
struct VCardSTStorageByClusterBase
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
    using ResultsType = Results<std::tuple<R::AllYears::Average>>;

    //! The VCard to look for for calculating spatial aggregates
    using VCardForSpatialAggregate = VCardSTStorageByClusterBase;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile & Traits::fileLevel;
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable
    static constexpr int columnCount = Category::dynamicColumns;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesDeepType = IntermediateValues;
    using IntermediateValuesBaseType = std::vector<IntermediateValues>;
    using IntermediateValuesType = std::vector<IntermediateValuesBaseType>;

}; // class VCardSTStorageByClusterBase

template<class Traits>
class STStorageByClusterBase: public Variable::IVariable<STStorageByClusterBase<Traits>,
                                                         VCardSTStorageByClusterBase<Traits>>
{
public:
    using VCardType = VCardSTStorageByClusterBase<Traits>;
    using AncestorType = Variable::IVariable<STStorageByClusterBase<Traits>, VCardType>;

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
    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        nbClusters_ = Traits::clusterCount(area);
        if (nbClusters_)
        {
            AncestorType::pResults.resize(nbClusters_);

            for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(nbClusters_);
            }

            for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (uint i = 0; i != nbClusters_; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (uint i = 0; i != nbClusters_; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }
    }

    size_t getMaxNumberColumns() const
    {
        return nbClusters_ * ResultsType::count;
    }

    void yearBegin(uint year, uint numSpace)
    {
        for (uint i = 0; i != nbClusters_; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
    }

    void yearEnd(uint year, uint numSpace)
    {
        for (uint clusterIndex = 0; clusterIndex < nbClusters_; ++clusterIndex)
        {
            Traits::computeStats(pValuesForTheCurrentYear[numSpace][clusterIndex]);
        }
    }

    void computeSummary(uint year, uint numSpace)
    {
        for (uint clusterIndex = 0; clusterIndex < nbClusters_; ++clusterIndex)
        {
            AncestorType::pResults[clusterIndex]
              .merge(year, pValuesForTheCurrentYear[numSpace][clusterIndex]);
        }
    }

    void hourForEachArea(State& state, uint numSpace)
    {
        if constexpr (requires {
                          Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace],
                                                 state,
                                                 numSpace);
                      })
        {
            Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state, numSpace);
        }
        else
        {
            Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state);
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
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            Traits::buildSurveyReport(pValuesForTheCurrentYear[numSpace],
                                      results,
                                      fileLevel,
                                      precision);
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t nbClusters_ = 0;
    uint pNbYearsParallel = 0;

}; // class STStorageByClusterBase

} // namespace Antares::Solver::Variable::Economy
