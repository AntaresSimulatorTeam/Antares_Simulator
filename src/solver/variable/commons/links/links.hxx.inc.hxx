/*
** Copyright 2007-2018 RTE
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
#ifndef __SOLVER_VARIABLE_INC_LINK_HXX__
#define __SOLVER_VARIABLE_INC_LINK_HXX__

#include <antares/study/filter.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace LINK_NAMESPACE
{
inline Links::Links()
{
    // Do nothing
}

inline void Links::initializeFromStudy(Data::Study& study)
{
    // Do nothing but resetting the pointer on print info collection
    study.parameters.variablesPrintInfo.resetInfoIterator();
}

inline void Links::initializeFromAreaLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

inline void Links::initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*)
{
    // This method should not be called at this stage
}

inline void Links::broadcastNonApplicability(bool applyNonApplicable)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].broadcastNonApplicability(applyNonApplicable);
    // Flush all memory into the swap files
    // (only if the support is available)
    if (Antares::Memory::swapSupport)
        Antares::memory.flushAll();
}

inline void Links::getPrintStatusFromStudy(Data::Study& study)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].getPrintStatusFromStudy(study);
    // Flush all memory into the swap files
    // (only if the support is available)
    if (Antares::Memory::swapSupport)
        Antares::memory.flushAll();
}

inline void Links::yearBegin(uint year, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].yearBegin(year, numSpace);

    // Flush all memory into the swap files
    // (only if the support is available)
    if (Antares::Memory::swapSupport)
        Antares::memory.flushAll();
}

inline void Links::yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                                uint year,
                                                                unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);

        // Flush all memory into the swap files
        // (only if the support is available)
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

inline void Links::yearEndBuildPrepareDataForEachRenewableCluster(State& state,
                                                                uint year,
                                                                unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuildPrepareDataForEachRenewableCluster(state, year, numSpace);

        // Flush all memory into the swap files
        // (only if the support is available)
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}


inline void Links::yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuildForEachThermalCluster(state, year, numSpace);

        // Flush all memory into the swap files
        // (only if the support is available)
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

inline void Links::yearEndBuild(State& state, uint year)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuild(state, year);

        // Flush all memory into the swap files
        // (only if the support is available)
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

inline void Links::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEnd(year, numSpace);

        // Flush all memory into the swap files
        // (only if the support is available)
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

inline void Links::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                                  unsigned int nbYearsForCurrentSummary)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
}

inline void Links::weekBegin(State& state)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].weekBegin(state);
}

inline void Links::weekForEachArea(State&, uint numSpace)
{
    // do nothing
    UNUSED_VARIABLE(numSpace);
}

inline void Links::weekEnd(State& state)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].weekEnd(state);
}

inline void Links::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].hourBegin(hourInTheYear);
}

inline void Links::hourForEachArea(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].hourForEachArea(state, numSpace);
}

inline void Links::hourForEachLink(State& state, unsigned int numSpace)
{
    pLinks[state.link->indexForArea].hourForEachLink(state, numSpace);
}

inline void Links::hourForEachThermalCluster(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].hourForEachThermalCluster(state, numSpace);
}

inline void Links::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].hourEnd(state, hourInTheYear);
}

inline void Links::buildSurveyReport(SurveyResults& results,
                                     int dataLevel,
                                     int fileLevel,
                                     int precision) const
{
    int count_int = count;
    bool link_dataLevel = (dataLevel & Category::link);
    if (count_int && link_dataLevel)
    {
        assert(results.data.link != NULL
               && "The link must not be null to generate a survey report");

        // Build the survey results for the given area
        auto& link = *results.data.link;

        // Filtering
        switch (precision)
        {
        case Category::hourly:
            if (!(link.filterSynthesis & Data::filterHourly))
                return;
            break;
        case Category::daily:
            if (!(link.filterSynthesis & Data::filterDaily))
                return;
            break;
        case Category::weekly:
            if (!(link.filterSynthesis & Data::filterWeekly))
                return;
            break;
        case Category::monthly:
            if (!(link.filterSynthesis & Data::filterMonthly))
                return;
            break;
        case Category::annual:
            if (!(link.filterSynthesis & Data::filterAnnual))
                return;
            break;
        case Category::all:
            break;
        }
        auto& linkOutputData = pLinks[link.indexForArea];
        linkOutputData.buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
}

inline void Links::buildAnnualSurveyReport(SurveyResults& results,
                                           int dataLevel,
                                           int fileLevel,
                                           int precision,
                                           uint numSpace) const
{
    int count_int = count;
    bool link_dataLevel = (dataLevel & Category::link);
    if (count_int && link_dataLevel)
    {
        assert(results.data.link != NULL
               && "The link must not be null to generate a survey report");

        auto& link = *results.data.link;

        // Filtering
        switch (precision)
        {
        case Category::hourly:
            if (not(link.filterYearByYear & Data::filterHourly))
                return;
            break;
        case Category::daily:
            if (not(link.filterYearByYear & Data::filterDaily))
                return;
            break;
        case Category::weekly:
            if (not(link.filterYearByYear & Data::filterWeekly))
                return;
            break;
        case Category::monthly:
            if (not(link.filterYearByYear & Data::filterMonthly))
                return;
            break;
        case Category::annual:
            if (not(link.filterYearByYear & Data::filterAnnual))
                return;
            break;
        case Category::all:
            break;
        }

        // Build the survey results for the given area
        auto& linkOutputData = pLinks[results.data.link->indexForArea];
        linkOutputData.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class I>
inline void Links::provideInformations(I& infos)
{
    // Begining of the node
    if (VCardType::nodeDepthForGUI)
    {
        infos.template beginNode<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
        // End of the node
        infos.endNode();
    }
    else
    {
        // Giving our VCard
        infos.template addVCard<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
    }
}

template<class PredicateT>
void Links::RetrieveVariableList(PredicateT& predicate)
{
    NextType::RetrieveVariableList(predicate);
}

inline Yuni::uint64 Links::memoryUsage() const
{
    Yuni::uint64 result = 0;
    for (uint i = 0; i != pLinkCount; ++i)
    {
        result += sizeof(NextType) + sizeof(void*);
        result += pLinks[i].memoryUsage();
    }
    return result;
}

inline void Links::EstimateMemoryUsage(Data::StudyMemoryUsage& u)
{
    if (!u.area)
        return;

    // The total number of items
    auto count = (uint)u.area->links.size();

    for (uint i = 0; i != count; ++i)
    {
        u.requiredMemoryForOutput += sizeof(NextType) + sizeof(void*);
        u.overheadDiskSpaceForSingleAreaOrLink();

        // year-by-year
        if (!u.gatheringInformationsForInput)
        {
            if (u.study.parameters.yearByYear && u.mode != Data::stdmAdequacyDraft)
            {
                for (uint i = 0; i != u.years; ++i)
                    u.overheadDiskSpaceForSingleAreaOrLink();
            }
        }

        // Append the amount of data consummed by the varaibles related to
        // the links
        NextType::EstimateMemoryUsage(u);
    }
}

} // namespace LINK_NAMESPACE
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_INC_LINK_HXX__
