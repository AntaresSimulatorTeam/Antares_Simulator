/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve participation for all groups for all reserves of the area
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationBySTStorageGroup
    : public IVariable<ReserveParticipationBySTStorageGroup<NextT>,
                       NextT,
                       VCardReserveParticipationBySTStorageGroup>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationBySTStorageGroup VCardType;
    //! Ancestor
    typedef IVariable<ReserveParticipationBySTStorageGroup<NextT>, NextT, VCardType> AncestorType;

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
                          + static_cast<int>(VCardType::columnCount) * static_cast<int>(ResultsType::count))
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ReserveParticipationBySTStorageGroup() = default;

    void initializeFromArea(Study* study, Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        // Get the number of potential group reserve participation
        if (study->parameters.reservesEnabled)
        {
            for (auto& [resName, setGroups]: area->allCapacityReservations->reserveGroupPartSTS)
            {
                pSize += setGroups.size();
            }
        }
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
            }

            // Minimum power values of the cluster for the whole year - from the solver in the
            // accurate mode not to be displayed in the output \todo think of a better place like
            // the DispatchableMarginForAllAreas done at the beginning of the year

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

    size_t getMaxNumberColumns() const
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
        if (state.study.parameters.reservesEnabled)
        {
            auto& area = state.area;
            int column = 0;
            for (const auto& reserveName:
                 area->allCapacityReservations.value().areaCapacityReservations | std::views::keys)
            {
                if (area->allCapacityReservations->reserveGroupPartSTS.contains(reserveName))
                {
                    for (const auto& group:
                         area->allCapacityReservations->reserveGroupPartSTS.at(reserveName))
                    {
                        pValuesForTheCurrentYear[numSpace][column].hour[state.hourInTheYear]
                          += state.reserveParticipationPerGroupForYear[state.hourInTheYear]
                               .shortTermStorageGroupsReserveParticipation[group][reserveName];
                        column++;
                    }
                }
            }
        }
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
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

        if (AncestorType::isPrinted[0] && results.data.area->allCapacityReservations)
        {
            assert(NULL != results.data.area);
            // Write the data for the current year
            int column = 0;
            for (const auto& reserveName:
                 results.data.area->allCapacityReservations.value().areaCapacityReservations
                   | std::views::keys)
            {
                if (results.data.area->allCapacityReservations->reserveGroupPartSTS.contains(
                      reserveName))
                {
                    for (auto group = results.data.area->allCapacityReservations
                                        ->reserveGroupPartSTS.at(reserveName)
                                        .begin();
                         group
                         != results.data.area->allCapacityReservations->reserveGroupPartSTS
                              .at(reserveName)
                              .end();
                         group++)
                    {
                        // Write the data for the current year
                        std::string tmp = *group;
                        Yuni::String caption = reserveName;
                        caption << "_" << tmp;
                        results.variableCaption = caption; // VCardType::Caption();
                        results.variableUnit = VCardType::Unit();
                        pValuesForTheCurrentYear[numSpace][column]
                          .template buildAnnualSurveyReport<VCardType>(results,
                                                                       fileLevel,
                                                                       precision);
                        column++;
                    }
                }
            }
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationBySTStorageGroup

} // namespace Antares::Solver::Variable::Economy::Reserves
