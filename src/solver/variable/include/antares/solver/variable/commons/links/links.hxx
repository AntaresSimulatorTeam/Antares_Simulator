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
#ifndef __SOLVER_VARIABLE_INC_LINK_HXX__
#define __SOLVER_VARIABLE_INC_LINK_HXX__

#include <antares/study/filter.h>
#include "antares/solver/variable/endoflist.h" // For UNUSED_VARIABLE, should be somewhere else

namespace Antares
{
namespace Solver
{
namespace Variable
{

template<class VariablePerLink>
inline Links<VariablePerLink>::Links()
{
    // Do nothing
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::initializeFromStudy([[maybe_unused]] Data::Study& study)
{
    // Do nothing
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::initializeFromAreaLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::initializeFromThermalCluster(Data::Study*,
                                                                 Data::Area*,
                                                                 Data::ThermalCluster*)
{
    // This method should not be called at this stage
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::broadcastNonApplicability(bool applyNonApplicable)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].broadcastNonApplicability(applyNonApplicable);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::getPrintStatusFromStudy(Data::Study& study)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].getPrintStatusFromStudy(study);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::supplyMaxNumberOfColumns(Data::Study& study)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].supplyMaxNumberOfColumns(study);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::yearBegin(uint year, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearBegin(year, numSpace);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::yearEndBuildPrepareDataForEachThermalCluster(
  State& state,
  uint year,
  unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::yearEndBuildForEachThermalCluster(State& state,
                                                                      uint year,
                                                                      unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuildForEachThermalCluster(state, year, numSpace);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::yearEndBuild(State& state, uint year)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEndBuild(state, year);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].yearEnd(year, numSpace);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::computeSummary(
  std::map<unsigned int, unsigned int>& numSpaceToYear,
  unsigned int nbYearsForCurrentSummary)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::weekBegin(State& state)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].weekBegin(state);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::weekForEachArea(State&, uint numSpace)
{
    // do nothing
    UNUSED_VARIABLE(numSpace);
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::weekEnd(State& state)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].weekEnd(state);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].hourBegin(hourInTheYear);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::hourForEachArea(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].hourForEachArea(state, numSpace);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::hourForEachLink(State& state, unsigned int numSpace)
{
    pLinks[state.link->indexForArea].hourForEachLink(state, numSpace);
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].hourEnd(state, hourInTheYear);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::buildSurveyReport(SurveyResults& results,
                                                      int dataLevel,
                                                      int fileLevel,
                                                      int precision) const
{
    int count_int = count;
    bool link_dataLevel = (dataLevel & Category::DataLevel::link);
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
            {
                return;
            }
            break;
        case Category::daily:
            if (!(link.filterSynthesis & Data::filterDaily))
            {
                return;
            }
            break;
        case Category::weekly:
            if (!(link.filterSynthesis & Data::filterWeekly))
            {
                return;
            }
            break;
        case Category::monthly:
            if (!(link.filterSynthesis & Data::filterMonthly))
            {
                return;
            }
            break;
        case Category::annual:
            if (!(link.filterSynthesis & Data::filterAnnual))
            {
                return;
            }
            break;
        case Category::all:
            break;
        }
        auto& linkOutputData = pLinks[link.indexForArea];
        linkOutputData.buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
}

template<class VariablePerLink>
inline void Links<VariablePerLink>::buildAnnualSurveyReport(SurveyResults& results,
                                                            int dataLevel,
                                                            int fileLevel,
                                                            int precision,
                                                            uint numSpace) const
{
    int count_int = count;
    bool link_dataLevel = (dataLevel & Category::DataLevel::link);
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
            {
                return;
            }
            break;
        case Category::daily:
            if (not(link.filterYearByYear & Data::filterDaily))
            {
                return;
            }
            break;
        case Category::weekly:
            if (not(link.filterYearByYear & Data::filterWeekly))
            {
                return;
            }
            break;
        case Category::monthly:
            if (not(link.filterYearByYear & Data::filterMonthly))
            {
                return;
            }
            break;
        case Category::annual:
            if (not(link.filterYearByYear & Data::filterAnnual))
            {
                return;
            }
            break;
        case Category::all:
            break;
        }

        // Build the survey results for the given area
        auto& linkOutputData = pLinks[results.data.link->indexForArea];
        linkOutputData.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class VariablePerLink>
template<class I>
inline void Links<VariablePerLink>::provideInformations(I& infos)
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

template<class VariablePerLink>
template<class PredicateT>
void Links<VariablePerLink>::RetrieveVariableList(PredicateT& predicate)
{
    NextType::RetrieveVariableList(predicate);
}

template<class VariablePerLink>
inline uint64_t Links<VariablePerLink>::memoryUsage() const
{
    uint64_t result = 0;
    for (uint i = 0; i != pLinkCount; ++i)
    {
        result += sizeof(NextType) + sizeof(void*);
        result += pLinks[i].memoryUsage();
    }
    return result;
}

template<class VariablePerLink>
Links<VariablePerLink>::~Links()
{
    // Releasing the memory occupied by the areas
    delete[] pLinks;
}

template<class VariablePerLink>
void Links<VariablePerLink>::initializeFromArea(Data::Study* study, Data::Area* area)
{
    // Assert
    assert(study && "A study pointer must not be null");
    assert(area && "An area pointer must not be null");

    pLinkCount = (uint)area->links.size();
    if (pLinkCount)
    {
        pLinks = new NextType[pLinkCount];

        // For each link...
        uint lnkIndex = 0;
        auto end = area->links.end();
        for (auto i = area->links.begin(); i != end; ++i, ++lnkIndex)
        {
            // Instancing a new set of variables of the area
            NextType& n = pLinks[lnkIndex];

            // Initialize the variables
            // From the study
            n.initializeFromStudy(*study);
            // From the area
            n.initializeFromArea(study, area);
            // From the link
            n.initializeFromAreaLink(study, i->second);
        }
    }
    else
    {
        pLinks = nullptr;
    }
}

template<class VariablePerLink>
void Links<VariablePerLink>::simulationBegin()
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].simulationBegin();
    }
}

template<class VariablePerLink>
void Links<VariablePerLink>::simulationEnd()
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].simulationEnd();
    }
}

template<class VariablePerLink>
void Links<VariablePerLink>::buildDigest(SurveyResults& results,
                                         int digestLevel,
                                         int dataLevel) const
{
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::DataLevel::link;
    bool areaDataLevel = dataLevel & Category::DataLevel::area;
    if (count_int && (linkDataLevel || areaDataLevel))
    {
        if (not results.data.area->links.empty())
        {
            auto end = results.data.area->links.cend();
            for (auto i = results.data.area->links.cbegin(); i != end; ++i)
            {
                results.data.link = i->second;
                pLinks[results.data.link->indexForArea].buildDigest(results,
                                                                    digestLevel,
                                                                    Category::DataLevel::link);
            }
        }
    }
}

template<class VariablePerLink>
void Links<VariablePerLink>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].beforeYearByYearExport(year, numSpace);
    }
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_INC_LINK_HXX__
