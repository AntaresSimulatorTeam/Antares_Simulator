// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardDispatchablePlantByClusterBase VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::de);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable
    static constexpr int columnCount = Category::dynamicColumns;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

}; // class VCardDispatchablePlantByClusterBase

template<class Traits, class NextT = Container::EndOfList>
class DispatchablePlantByClusterBase
    : public Variable::IVariable<DispatchablePlantByClusterBase<Traits, NextT>,
                                 NextT,
                                 VCardDispatchablePlantByClusterBase<Traits>>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardDispatchablePlantByClusterBase<Traits> VCardType;
    //! Ancestor
    typedef Variable::IVariable<DispatchablePlantByClusterBase<Traits, NextT>, NextT, VCardType>
      AncestorType;

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
    void initializeFromStudy(Data::Study& study)
    {
        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        // Get the area
        pSize = area->thermal.list.enabledCount();
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != pSize; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != pSize; ++i)
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

        // Next
        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        // Next
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
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
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }

        yearBeginIfSupported(auxiliaryData_, year, numSpace, pSize);

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                      uint year,
                                                      unsigned int numSpace)
    {
        yearEndBuildPrepareDataForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                                auxiliaryData_,
                                                                state,
                                                                year,
                                                                numSpace);

        // Next variable
        NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        yearEndBuildForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                     state,
                                                     year,
                                                     numSpace);

        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Merge all results for all thermal clusters
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Compute all statistics for the current year (daily,weekly,monthly)
                pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
            }
        }
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            // Merge all those values with the global results
            AncestorType::pResults[i].merge(year, pValuesForTheCurrentYear[numSpace][i]);
        }

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
        setHourlyValuesForCurrentYearIfSupported(pValuesForTheCurrentYear[numSpace],
                                                 auxiliaryData_,
                                                 state,
                                                 numSpace);

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
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
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            const auto& thermal = results.data.area->thermal;

            // Write the data for the current year
            for (auto& cluster: thermal.list.each_enabled())
            {
                // Write the data for the current year
                results.variableCaption = cluster->name(); // VCardType::Caption();
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][cluster->enabledIndex]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
        }
    }

protected:
    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    static void setHourlyValuesForCurrentYearIfSupported(
      std::vector<IntermediateValues>& clusterValues,
      AuxiliaryDataType& auxiliaryData,
      State& state,
      unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::setHourlyValuesForCurrentYear(clusterValues,
                                                                auxiliaryData,
                                                                state,
                                                                numSpace);
                      })
        {
            Traits::setHourlyValuesForCurrentYear(clusterValues, auxiliaryData, state, numSpace);
        }
        else if constexpr (requires {
                               Traits::setHourlyValuesForCurrentYear(clusterValues, state);
                           })
        {
            Traits::setHourlyValuesForCurrentYear(clusterValues, state);
        }
    }

    static void initializeAuxiliaryDataIfSupported(AuxiliaryDataType& auxiliaryData,
                                                   Data::Study* study,
                                                   unsigned int nbYearsParallel,
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
                                     unsigned int year,
                                     unsigned int numSpace,
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
      unsigned int numSpace)
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
      unsigned int numSpace)
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
    unsigned int pNbYearsParallel = 0;

}; // class DispatchablePlantByClusterBase

} // namespace Antares::Solver::Variable::Economy
