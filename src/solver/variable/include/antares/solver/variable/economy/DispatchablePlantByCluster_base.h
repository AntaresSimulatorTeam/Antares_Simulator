// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file DispatchablePlantByCluster_base.h
**
** \brief Base class for dispatchable plant by cluster variables
**
** ## Traits Contract
**
** A valid DispatchablePlantByCluster Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - Results profile is provided by the base class (no trait alias required)
**
** - Optional hooks (dispatched via \c if constexpr):
**   - \c AuxiliaryDataType : type alias
**   - \c initializeAuxiliaryData(AuxiliaryDataType&, Data::Study*, uint, size_t) -> void
**   - \c yearBegin(AuxiliaryDataType&, uint, uint, size_t) -> void
**   - \c setHourlyValue(std::vector<IntermediateValues>&, AuxiliaryDataType&, State&, uint)
*-> void
**   - Fallback: \c setHourlyValue(std::vector<IntermediateValues>&, State&, uint) -> void
**   - Fallback: \c setHourlyValue(std::vector<IntermediateValues>&, State&) -> void
**   - \c
*yearEndBuildPrepareDataForEachThermalCluster(std::vector<std::vector<IntermediateValues>>&,
*AuxiliaryDataType&, State&, uint, uint) -> void
**   - \c yearEndBuildForEachThermalCluster(std::vector<std::vector<IntermediateValues>>&, State&,
*uint, uint) -> void
*/

#include <type_traits>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

template<class Traits>
struct VCardDispatchablePlantByClusterBase
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
    using VCardForSpatialAggregate = VCardDispatchablePlantByClusterBase;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::de);
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

}; // class VCardDispatchablePlantByClusterBase

template<class Traits>
class DispatchablePlantByClusterBase
    : public Variable::IVariable<DispatchablePlantByClusterBase<Traits>,
                                 VCardDispatchablePlantByClusterBase<Traits>>
{
public:
    using VCardType = VCardDispatchablePlantByClusterBase<Traits>;
    using AncestorType = Variable::IVariable<DispatchablePlantByClusterBase<Traits>, VCardType>;

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

        pSize = area->thermal.list.enabledCount();
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
            }

            for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (uint i = 0; i != pSize; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (uint i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }

        initializeAuxiliaryDataIfSupported(auxiliaryData_, study, pNbYearsParallel, pSize);
    }

    size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void yearBegin(uint year, uint numSpace)
    {
        for (uint i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }

        yearBeginIfSupported(auxiliaryData_, year, numSpace, pSize);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace)
    {
        yearEndBuildPrepareDataForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                                auxiliaryData_,
                                                                state,
                                                                year,
                                                                numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace)
    {
        yearEndBuildForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                     state,
                                                     year,
                                                     numSpace);
    }

    void yearEnd([[maybe_unused]] uint year, uint numSpace)
    {
        for (uint i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
    }

    void computeSummary(uint year, uint numSpace)
    {
        for (uint i = 0; i < pSize; ++i)
        {
            AncestorType::pResults[i].merge(year, pValuesForTheCurrentYear[numSpace][i]);
        }
    }

    void hourForEachArea(State& state, uint numSpace)
    {
        setHourlyValueIfSupported(pValuesForTheCurrentYear[numSpace],
                                  auxiliaryData_,
                                  state,
                                  numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint column,
      uint numSpace) const
    {
        if constexpr (requires {
                          Traits::retrieveRawHourlyValuesForCurrentYear(pValuesForTheCurrentYear,
                                                                        column,
                                                                        numSpace);
                      })
        {
            return Traits::retrieveRawHourlyValuesForCurrentYear(pValuesForTheCurrentYear,
                                                                 column,
                                                                 numSpace);
        }

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
            const auto& thermal = results.data.area->thermal;

            for (auto& cluster: thermal.list.each_enabled())
            {
                results.variableCaption = cluster->name();
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][cluster->enabledIndex]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
        }
    }

protected:
    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    static void setHourlyValueIfSupported(std::vector<IntermediateValues>& clusterValues,
                                          AuxiliaryDataType& auxiliaryData,
                                          State& state,
                                          uint numSpace)
    {
        if constexpr (requires {
                          Traits::setHourlyValue(clusterValues, auxiliaryData, state, numSpace);
                      })
        {
            Traits::setHourlyValue(clusterValues, auxiliaryData, state, numSpace);
        }
        else if constexpr (requires { Traits::setHourlyValue(clusterValues, state, numSpace); })
        {
            Traits::setHourlyValue(clusterValues, state, numSpace);
        }
    }

    static void initializeAuxiliaryDataIfSupported(AuxiliaryDataType& auxiliaryData,
                                                   Data::Study* study,
                                                   uint nbYearsParallel,
                                                   size_t nbClusters)
    {
        if constexpr (requires {
                          Traits::initializeAuxiliaryData(auxiliaryData,
                                                          study,
                                                          nbYearsParallel,
                                                          nbClusters);
                      })
        {
            Traits::initializeAuxiliaryData(auxiliaryData, study, nbYearsParallel, nbClusters);
        }
    }

    static void yearBeginIfSupported(AuxiliaryDataType& auxiliaryData,
                                     uint year,
                                     uint numSpace,
                                     size_t nbClusters)
    {
        if constexpr (requires { Traits::yearBegin(auxiliaryData, year, numSpace, nbClusters); })
        {
            Traits::yearBegin(auxiliaryData, year, numSpace, nbClusters);
        }
    }

    static void yearEndBuildPrepareDataForEachThermalClusterIfSupported(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      AuxiliaryDataType& auxiliaryData,
      State& state,
      uint year,
      uint numSpace)
    {
        if constexpr (requires {
                          Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                               auxiliaryData,
                                                                               state,
                                                                               year,
                                                                               numSpace);
                      })
        {
            Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                 auxiliaryData,
                                                                 state,
                                                                 year,
                                                                 numSpace);
        }
        else if constexpr (requires {
                               Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                                    state,
                                                                                    year,
                                                                                    numSpace);
                           })
        {
            Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                 state,
                                                                 year,
                                                                 numSpace);
        }
    }

    static void yearEndBuildForEachThermalClusterIfSupported(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint year,
      uint numSpace)
    {
        if constexpr (requires {
                          Traits::yearEndBuildForEachThermalCluster(yearlyValues,
                                                                    state,
                                                                    year,
                                                                    numSpace);
                      })
        {
            Traits::yearEndBuildForEachThermalCluster(yearlyValues, state, year, numSpace);
        }
    }

    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    AuxiliaryDataType auxiliaryData_;
    size_t pSize = 0;
    uint pNbYearsParallel = 0;

}; // class DispatchablePlantByClusterBase

} // namespace Antares::Solver::Variable::Economy
