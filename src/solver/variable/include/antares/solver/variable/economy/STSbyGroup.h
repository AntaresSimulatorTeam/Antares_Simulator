
#pragma once

#include "antares/solver/variable/variable.h"

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
    typedef Results<R::AllYears::Average<           // The average values throughout all years
                    R::AllYears::StdDeviation<      // The standard deviation values throughout all years
                    R::AllYears::Min<               // The minimum values throughout all years
                    R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
        ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardSTSbyGroup VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        // Nb of columns occupied by this variable in year-by-year results
        columnCount = Category::dynamicColumns,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;

}; // class VCard

/*!
** \brief Variables related to short term storage groups
*/
template<class NextT = Container::EndOfList>
class STSbyGroup : public Variable::IVariable<STSbyGroup<NextT>,
                                              NextT,
                                              VCardSTSbyGroup>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardSTSbyGroup VCardType;
    //! Ancestor
    typedef Variable::IVariable<STSbyGroup<NextT>, NextT, VCardType>
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
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    STSbyGroup() : pValuesForTheCurrentYear(nullptr)
    {}

    ~STSbyGroup()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];

        // Vector of group names the clusters of the are belong to.
        // In this Group names appear only once
        for (auto& cluster : area->shortTermStorage.storagesByIndex)
        {
            std::string groupName = cluster.properties.groupName;
            if (std::find(groupNames_.begin(), groupNames_.end(), groupName) == groupNames_.end())
                groupNames_.push_back(cluster.properties.groupName);
        }
        std::sort(groupNames_.begin(), groupNames_.end());

        // Giving a number to each group
        unsigned int groupNumber{0};
        for (auto name : groupNames_)
        {
            groupToNumbers_[name] = groupNumber;
            groupNumber++;
        }

        nbGroups_ = groupNames_.size();
        nbColumns_ = nbGroups_ * 3;


        if (nbColumns_)
        {
            AncestorType::pResults.resize(nbColumns_);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[nbColumns_];

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                for (unsigned int i = 0; i != nbColumns_; ++i)
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);

            for (unsigned int i = 0; i != nbColumns_; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace] = nullptr;
            }

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
            bool isAnInjectionColumn = (column % 3) == 0;
            bool isAnWithdrawalColumn = (column % 3) == 1;
            bool isALevelColumn = (column % 3) == 2;

            if (isALevelColumn)
                pValuesForTheCurrentYear[numSpace][column].computeAveragesForCurrentYearFromHourlyResults();
            if (isAnInjectionColumn || isAnWithdrawalColumn)
                pValuesForTheCurrentYear[numSpace][column].computeStatisticsForTheCurrentYear();
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
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        using namespace Antares::Data::ShortTermStorage;
        const auto& shortTermStorage = state.area->shortTermStorage;

        uint clusterIndex = 0;
        for (const auto& cluster : shortTermStorage.storagesByIndex)
        {
            unsigned int groupNumber = groupToNumbers_[cluster.properties.groupName];
            // Injection
            pValuesForTheCurrentYear[numSpace][3 * groupNumber][state.hourInTheYear]
                += state.hourlyResults->ShortTermStorage[state.hourInTheWeek].injection[clusterIndex];

            // Withdrawal
            pValuesForTheCurrentYear[numSpace][3 * groupNumber + 1][state.hourInTheYear]
                += state.hourlyResults->ShortTermStorage[state.hourInTheWeek].withdrawal[clusterIndex];

            // Levels
            pValuesForTheCurrentYear[numSpace][3 * groupNumber + 2][state.hourInTheYear]
                += state.hourlyResults->ShortTermStorage[state.hourInTheWeek].level[clusterIndex];

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

    inline uint64_t memoryUsage() const
    {
        uint64_t r = (sizeof(IntermediateValues) * nbColumns_ + IntermediateValues::MemoryUsage())
                         * pNbYearsParallel;
        r += sizeof(double) * nbColumns_ * maxHoursInAYear * pNbYearsParallel;
        r += AncestorType::memoryUsage();
        return r;
    }

    std::string caption(unsigned int column) const
    {
        std::string groupName = groupNames_[column / 3];
        std::string variableKind = VAR_POSSIBLE_KINDS[column % 3];
        return groupName + "_" + variableKind;
    }

    std::string unit(unsigned int column) const
    {
        return (column % 3 == 2) ? "MWh" : "MW"; // Level in MWh, others in "MW"
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (!AncestorType::isPrinted[0])
            return;

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
        // Building syntheses results
        // ------------------------------
        if (!AncestorType::isPrinted[0])
            return;

        // And only if we match the current data level _and_ precision level
        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            results.isCurrentVarNA[0] = AncestorType::isNonApplicable[0];

            for (unsigned int column = 0; column < nbColumns_; column++)
            {   
                results.variableCaption = caption(column);
                results.variableUnit = unit(column);
                AncestorType::pResults[column].template
                    buildSurveyReport<ResultsType, VCardType>(results,
                                                              AncestorType:: pResults[column],
                                                              dataLevel, 
                                                              fileLevel,
                                                              precision);
            }
        }

        // Ask to the next item in the static list to export its results as well
        NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }


private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t nbGroups_ = 0;
    size_t nbColumns_ = 0;
    std::vector<std::string> groupNames_; // Names of group containing the clusters of the area
    std::map<std::string, unsigned int> groupToNumbers_; // Gives to each group (of area) a number
    const std::vector<std::string> VAR_POSSIBLE_KINDS = { "injection", "withdrawal", "level" };
    unsigned int pNbYearsParallel;

}; // class STSbyGroup

} // End namespace Antares::Solver::Variable::Economy
