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
#ifndef __SOLVER_VARIABLE_ECONOMY_ReserveParticipationBySTStorageGroup_H__
#define __SOLVER_VARIABLE_ECONOMY_ReserveParticipationBySTStorageGroup_H__

#include "../variable.h"
#include "./vCardReserveParticipationBySTStorageGroup.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

/*!
** \brief Reserve participation for all groups for all reserves of the area
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationBySTStorageGroup
 : public Variable::IVariable<ReserveParticipationBySTStorageGroup<NextT>,
                              NextT,
                              VCardReserveParticipationBySTStorageGroup>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationBySTStorageGroup VCardType;
    //! Ancestor
    typedef Variable::IVariable<ReserveParticipationBySTStorageGroup<NextT>, NextT, VCardType>
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
    ReserveParticipationBySTStorageGroup() : pValuesForTheCurrentYear(NULL), pSize(0)
    {
    }

    ~ReserveParticipationBySTStorageGroup()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];

        // Get the area
        pSize = 0;
        for (auto res : area->allCapacityReservations.areaCapacityReservationsUp)
        {
            pSize += Antares::Data::ShortTermStorage::Group::groupMax;
        }
        for (auto res : area->allCapacityReservations.areaCapacityReservationsDown)
        {
            pSize += Antares::Data::ShortTermStorage::Group::groupMax;
        }
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[pSize];

            // Minimum power values of the cluster for the whole year - from the solver in the
            // accurate mode not to be displayed in the output \todo think of a better place like
            // the DispatchableMarginForAllAreas done at the beginning of the year

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                for (unsigned int i = 0; i != pSize; ++i)
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);

            for (unsigned int i = 0; i != pSize; ++i)
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
            pValuesForTheCurrentYear[numSpace][i].reset();

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

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Merge all those values with the global results
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
            }
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
        auto& area = state.area;
        int column = 0;
        for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
        {
            for (int indexGroup = 0; indexGroup < Antares::Data::ShortTermStorage::groupMax;
                 indexGroup++)
            {
                pValuesForTheCurrentYear[numSpace][column].hour[state.hourInTheYear]
                  += state.reserveParticipationForYear[state.hourInTheYear]
                       .shortTermStorageGroupsReserveParticipation[static_cast<Antares::Data::ShortTermStorage::Group>(
                         indexGroup)][reserveName];
                column++;
            }
        }
        for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsDown)
        {
            for (int indexGroup = 0; indexGroup < Antares::Data::ShortTermStorage::groupMax;
                 indexGroup++)
            {
                pValuesForTheCurrentYear[numSpace][column].hour[state.hourInTheYear]
                  += state.reserveParticipationForYear[state.hourInTheYear]
                       .shortTermStorageGroupsReserveParticipation[static_cast<Antares::Data::ShortTermStorage::Group>(
                         indexGroup)][reserveName];
                column++;
            }
        }
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace]->hour;
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

            // Write the data for the current year
            int column = 0;
            for (auto& res : results.data.area->allCapacityReservations.areaCapacityReservationsUp)
            {
                for (int indexGroup = 0;
                     indexGroup < Antares::Data::ShortTermStorage::Group::groupMax;
                     indexGroup++)
                {
                    // Write the data for the current year
                    Yuni::String caption = res.first;
                    caption << "_"
                            << Antares::Data::ShortTermStorage::STStorageCluster::GroupName(
                                 static_cast<Antares::Data::ShortTermStorage::Group>(indexGroup));
                    results.variableCaption = caption; // VCardType::Caption();
                    results.variableUnit = VCardType::Unit();
                    pValuesForTheCurrentYear[numSpace][column]
                      .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
                    column++;
                }
            }
            for (auto& res :
                 results.data.area->allCapacityReservations.areaCapacityReservationsDown)
            {
                for (int indexGroup = 0;
                     indexGroup < Antares::Data::ShortTermStorage::Group::groupMax;
                     indexGroup++)
                {
                    // Write the data for the current year
                    Yuni::String caption = res.first;
                    caption << "_"
                            << Antares::Data::ShortTermStorage::STStorageCluster::GroupName(
                                 static_cast<Antares::Data::ShortTermStorage::Group>(indexGroup));
                    results.variableCaption = caption; // VCardType::Caption();
                    results.variableUnit = VCardType::Unit();
                    pValuesForTheCurrentYear[numSpace][column]
                      .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
                    column++;
                }
            }
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize;
    unsigned int pNbYearsParallel;

}; // class ReserveParticipationBySTStorageGroup

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_ReserveParticipationBySTStorageGroup_H__