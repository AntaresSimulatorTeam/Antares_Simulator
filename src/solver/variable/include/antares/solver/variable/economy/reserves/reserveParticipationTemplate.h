// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

namespace Antares::Solver::Variable::Economy::Reserves
{

/**
 * @brief Base template for all ReserveParticipation variable classes
 *
 * This template provides common functionality for all reserve participation variables.
 * Derived classes only need to implement:
 * - getSizeFromArea(): Returns the count of reserve participations
 * - populateHourlyValues(): Populates values for current hour
 * - buildReportForIndex(): Generates report for a specific index
 *
 * @tparam DerivedType The derived class (CRTP pattern)
 * @tparam VCardType The VCard describing this variable
 * @tparam NextT The next variable in the chain
 */
template<typename DerivedType, typename VCardType, class NextT = Container::EndOfList>
class ReserveParticipationTemplate: public IVariable<DerivedType, NextT, VCardType>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardType VCardTypeClass;
    //! Ancestor
    typedef IVariable<DerivedType, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef VCardType::ResultsType ResultsType;

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
                          + static_cast<int>(VCardType::columnCount)
                              * static_cast<int>(ResultsType::count))
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ReserveParticipationTemplate() = default;

    void initializeFromArea(Study* study, Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        // Get the count from derived class
        auto* derived = static_cast<DerivedType*>(this);
        pSize = study->parameters.include.reserves ? derived->getSizeFromArea(study, area) : 0;

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

        // Next
        NextType::initializeFromArea(study, area);
    }

    [[nodiscard]] size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void initializeFromLink(Study* study, AreaLink* link)
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

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Merge all results
        for (unsigned int i = 0; i < pSize; ++i)
        {
            // Compute all statistics for the current year (daily, weekly, monthly)
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
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
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        // Call derived implementation
        auto* derived = static_cast<DerivedType*>(this);
        derived->populateHourlyValues(state, numSpace);

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    [[nodiscard]] Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
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
            assert(results.data.area != nullptr);
            const auto* derived = const_cast<const DerivedType*>(
              static_cast<DerivedType*>(const_cast<ReserveParticipationTemplate*>(this)));

            // Write the data for the current year
            for (uint i = 0; i < pSize; ++i)
            {
                if (derived->hasIndexMapping(results.data.study, results.data.area))
                {
                    derived->buildReportForIndex(results, i, fileLevel, precision, numSpace);
                }
            }
        }
    }

protected:
    //! Intermediate values for each year
    VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationTemplate

} // namespace Antares::Solver::Variable::Economy::Reserves
