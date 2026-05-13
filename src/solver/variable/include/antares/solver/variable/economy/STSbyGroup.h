// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/utils/utils.h>
#include "antares/solver/variable/economy/dynamic_multi_column_base.h"
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

        // Study-wide column set: every area produces identically-positioned columns,
        // so the shared captions array stays consistent with each area's rows.
        descriptors_ = buildColumnDescriptors(*study, area);

        std::set<std::string> names;
        study->areas.each(
          [&names](Data::Area& currentArea)
          {
              for (const auto& sts: currentArea.shortTermStorage.storagesByIndex)
              {
                  names.insert(sts.properties.groupName);
              }
          });
        groupNames_ = {names.begin(), names.end()};
        groupToNumbers_ = Utils::giveNumbersToStrings(groupNames_);

        nbColumns_ = descriptors_.size();

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

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    static std::vector<ColumnDescriptor> buildColumnDescriptors(const Data::Study& study,
                                                                const Data::Area* /*area*/)
    {
        std::set<std::string> uniqueGroups;
        study.areas.each(
          [&uniqueGroups](const Data::Area& currentArea)
          {
              for (const auto& sts: currentArea.shortTermStorage.storagesByIndex)
              {
                  uniqueGroups.insert(sts.properties.groupName);
              }
          });

        std::vector<ColumnDescriptor> descriptors;
        for (const auto& groupName: uniqueGroups)
        {
            descriptors.push_back({groupName + "_INJECTION", "MW"});
            descriptors.push_back({groupName + "_WITHDRAWAL", "MW"});
            descriptors.push_back({groupName + "_LEVEL", "MWh"});
        }
        return descriptors;
    }

    static std::vector<ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        std::set<std::string> uniqueGroups;
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            uniqueGroups.insert(sts.properties.groupName);
        }

        std::vector<ColumnDescriptor> descriptors;
        for (const auto& groupName: uniqueGroups)
        {
            descriptors.push_back({groupName + "_INJECTION", "MW"});
            descriptors.push_back({groupName + "_WITHDRAWAL", "MW"});
            descriptors.push_back({groupName + "_LEVEL", "MWh"});
        }
        return descriptors;
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        using namespace Antares::Data::ShortTermStorage;
        const auto& shortTermStorage = state.area->shortTermStorage;

        uint clusterIndex = 0;
        for (const auto& sts: shortTermStorage.storagesByIndex)
        {
            unsigned int groupNumber = groupToNumbers_[sts.properties.groupName];
            const auto& result = state.hourlyResults->ShortTermStorage[clusterIndex];
            // Injection
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::injection][state.hourInTheYear]
              += result.injection[state.hourInTheWeek];

            // Withdrawal
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::withdrawal][state.hourInTheYear]
              += result.withdrawal[state.hourInTheWeek];

            // Levels
            pValuesForTheCurrentYear[numSpace][NB_COLS_PER_GROUP * groupNumber
                                               + VariableType::level][state.hourInTheYear]
              += result.level[state.hourInTheWeek];

            clusterIndex++;
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Column layout is study-wide (see initializeFromArea), so every area writes the
        // same captions in the same indices — safe to contribute to the digest now.
        if (AncestorType::isPrinted[0]
            && (VCardType::categoryDataLevel
                & (Category::DataLevel::area | Category::DataLevel::setOfAreas)))
        {
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;

            for (size_t c = 0; c < nbColumns_; ++c)
            {
                results.variableCaption = descriptors_[c].caption;
                results.variableUnit = descriptors_[c].unit;
                AncestorType::pResults[c].template buildDigest<VCardType>(results,
                                                                          digestLevel,
                                                                          dataLevel);
            }
        }
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
            results.variableCaption = descriptors_[column].caption;
            results.variableUnit = descriptors_[column].unit;
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
    std::vector<ColumnDescriptor> descriptors_;
    const int NB_COLS_PER_GROUP = 3; // Injection + withdrawal + levels = 3 variables
    unsigned int pNbYearsParallel;

}; // class STSbyGroup

} // End namespace Antares::Solver::Variable::Economy
