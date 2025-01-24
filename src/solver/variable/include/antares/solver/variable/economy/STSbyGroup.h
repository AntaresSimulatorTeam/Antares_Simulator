/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "antares/solver/variable/variable.h"

namespace
{
inline std::vector<std::string> sortedUniqueGroups(
  const std::vector<Antares::Data::ShortTermStorage::STStorageCluster>& storages)
{
    std::set<std::string> names;
    for (const auto& cluster: storages)
    {
        names.insert(cluster.properties.groupName);
    }
    return {names.begin(), names.end()};
}

inline std::map<std::string, unsigned int> giveNumbersToGroups(
  const std::vector<std::string>& groupNames)
{
    unsigned int groupNumber{0};
    std::map<std::string, unsigned int> groupToNumbers;
    for (const auto& name: groupNames)
    {
        groupToNumbers[name] = groupNumber++;
    }
    return groupToNumbers;
}
} // namespace

namespace Antares::Solver::Variable::Economy
{
struct VCardSTSbyGroup
{
    //! Caption
    static std::string Caption()
    {
        return "STS by group";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "STS injections, withdrawals and levels";
    }

    //! The synhesis results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardSTSbyGroup VCardForSpatialAggregate;

    //! Data Level
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
    static constexpr uint8_t decimal = 0;
    // Nb of columns occupied by this variable in year-by-year results
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

}; // class VCard

/*!
** \brief Variables related to short term storage groups
*/
template<class NextT = Container::EndOfList>
class STSbyGroup: public Variable::IVariable<STSbyGroup<NextT>, NextT, VCardSTSbyGroup>
{
private:
    enum VariableType
    {
        injection = 0,
        withdrawal = 1,
        level = 2
    };

public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardSTSbyGroup VCardType;
    //! Ancestor
    typedef Variable::IVariable<STSbyGroup<NextT>, NextT, VCardType> AncestorType;

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

        // Building the vector of group names the clusters belong to.
        groupNames_ = sortedUniqueGroups(area->shortTermStorage.storagesByIndex);
        groupToNumbers_ = giveNumbersToGroups(groupNames_);

        nbColumns_ = groupNames_.size() * NB_COLS_PER_GROUP;

        if (nbColumns_)
        {
            AncestorType::pResults.resize(nbColumns_);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(nbColumns_);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != nbColumns_; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != nbColumns_; ++i)
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
        return nbColumns_ * ResultsType::count;
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != nbColumns_; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Here we perform time-aggregations :
        // ---------------------------------
        // For a given MC year, from hourly results we compute daily, weekly, monthly and annual
        // results by aggregation operations (averages or sums).
        // Caution :
        //  - level results are stored in columns of which indices satisfy : col_index % 3 == 2.
        //    They are time-aggregated by means of averages
        //  - injection and withdrawal results are stored in columns of which indices
        //    satisfy : col_index % 3 != 2.
        //    They are time-aggregated by means of sums.

        for (unsigned int column = 0; column < nbColumns_; column++)
        {
            switch (column % NB_COLS_PER_GROUP)
            {
            case VariableType::level:
                pValuesForTheCurrentYear[numSpace][column]
                  .computeAveragesForCurrentYearFromHourlyResults();
                break;
            case VariableType::injection:
            case VariableType::withdrawal:
                pValuesForTheCurrentYear[numSpace][column].computeStatisticsForTheCurrentYear();
                break;
            }
        }
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        // Here we compute synthesis :
        //  for each interval of any time period results (hourly, daily, weekly, ...),
        //  we compute the average over all MC years :
        //  For instance :
        //      - we compute the average of the results of the first hour over all MC years
        //      - or we compute the average of the results of the n-th day over all MC years
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                                 AncestorType::pResults,
                                                 numSpaceToYear[numSpace]);
        }

        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        using namespace Antares::Data::ShortTermStorage;
        const auto& shortTermStorage = state.area->shortTermStorage;

        uint clusterIndex = 0;
        for (const auto& cluster: shortTermStorage.storagesByIndex)
        {
            unsigned int groupNumber = groupToNumbers_[cluster.properties.groupName];
            const auto& result = state.hourlyResults->ShortTermStorage[state.hourInTheWeek];
            // Injection
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::injection][state.hourInTheYear]
              += result.injection[clusterIndex];

            // Withdrawal
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::withdrawal][state.hourInTheYear]
              += result.withdrawal[clusterIndex];

            // Levels
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::level][state.hourInTheYear]
              += result.level[clusterIndex];

            clusterIndex++;
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Ask to build the digest to the next variable
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    std::string caption(unsigned int column) const
    {
        static const std::vector<std::string> VAR_POSSIBLE_KINDS = {"INJECTION",
                                                                    "WITHDRAWAL",
                                                                    "LEVEL"};
        const std::string& groupName = groupNames_[column / NB_COLS_PER_GROUP];
        const std::string& variableKind = VAR_POSSIBLE_KINDS[column % NB_COLS_PER_GROUP];
        return groupName + "_" + variableKind;
    }

    std::string unit(unsigned int column) const
    {
        switch (column % NB_COLS_PER_GROUP)
        {
        case VariableType::level:
            return "MWh";
        case VariableType::injection:
        case VariableType::withdrawal:
            return "MW";
        default:
            return "error";
        }
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        for (unsigned int column = 0; column < nbColumns_; column++)
        {
            results.variableCaption = caption(column);
            results.variableUnit = unit(column);
            pValuesForTheCurrentYear[numSpace][column]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        // Building synthesis results
        // ------------------------------

        if (AncestorType::isPrinted[0])
        {
            // And only if we match the current data level _and_ precision level
            if ((dataLevel & VCardType::categoryDataLevel)
                && (fileLevel & VCardType::categoryFileLevel) && (precision & VCardType::precision))
            {
                results.isCurrentVarNA[0] = AncestorType::isNonApplicable[0];

                for (unsigned int column = 0; column < nbColumns_; column++)
                {
                    results.variableCaption = caption(column);
                    results.variableUnit = unit(column);
                    AncestorType::pResults[column]
                      .template buildSurveyReport<ResultsType, VCardType>(
                        results,
                        AncestorType::pResults[column],
                        dataLevel,
                        fileLevel,
                        precision);
                }
            }
        }
        // Ask to the next item in the static list to export its results as well
        NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t nbColumns_ = 0;
    std::vector<std::string> groupNames_; // Names of group containing the clusters of the area
    std::map<std::string, unsigned int> groupToNumbers_; // Gives to each group (of area) a number
    const int NB_COLS_PER_GROUP = 3; // Injection + withdrawal + levels = 3 variables
    unsigned int pNbYearsParallel;

}; // class STSbyGroup

} // End namespace Antares::Solver::Variable::Economy
