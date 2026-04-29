// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/variable.h"

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

    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardSTStorageByClusterBase VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::de_sts);
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

}; // class VCardSTStorageByClusterBase

template<class Traits, class NextT = Container::EndOfList>
class STStorageByClusterBase: public Variable::IVariable<STStorageByClusterBase<Traits, NextT>,
                                                         NextT,
                                                         VCardSTStorageByClusterBase<Traits>>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardSTStorageByClusterBase<Traits> VCardType;
    //! Ancestor
    typedef Variable::IVariable<STStorageByClusterBase<Traits, NextT>, NextT, VCardType>
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
    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        // Get the area
        nbClusters_ = area->shortTermStorage.count();
        if (nbClusters_)
        {
            AncestorType::pResults.resize(nbClusters_);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(nbClusters_);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != nbClusters_; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != nbClusters_; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }
        // Next
        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return nbClusters_ * ResultsType::count;
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != nbClusters_; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int clusterIndex = 0; clusterIndex < nbClusters_; ++clusterIndex)
        {
            // Compute all statistics from hourly results for the current year (daily, weekly,
            // monthly, ...)
            Traits::computeStats(pValuesForTheCurrentYear[numSpace][clusterIndex]);
        }
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int clusterIndex = 0; clusterIndex < nbClusters_; ++clusterIndex)
        {
            // Merge all those values with the global results
            AncestorType::pResults[clusterIndex]
              .merge(year, pValuesForTheCurrentYear[numSpace][clusterIndex]);
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
        Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state);

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
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
            const auto& shortTermStorage = results.data.area->shortTermStorage;

            // Write the data for the current year
            uint clusterIndex = 0;
            for (const auto& sts: shortTermStorage.storagesByIndex)
            {
                // Write the data for the current year
                results.variableCaption = sts.properties.name;
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][clusterIndex]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);

                clusterIndex++;
            }
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t nbClusters_ = 0;
    unsigned int pNbYearsParallel = 0;

}; // class STStorageByClusterBase

} // namespace Antares::Solver::Variable::Economy
