// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_INC_LINK_HXX__
#define __SOLVER_VARIABLE_INC_LINK_HXX__

#include <antares/study/filter.h>

namespace Antares::Solver::Variable
{

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::initializeFromStudy([[maybe_unused]] Data::Study& study)
{
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::initializeFromLink(Data::Study*, Data::AreaLink*)
{
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::broadcastNonApplicability(bool applyNonApplicable)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].broadcastNonApplicability(applyNonApplicable);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::getPrintStatusFromStudy(Data::Study& study)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].getPrintStatusFromStudy(study);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::supplyMaxNumberOfColumns(Data::Study& study)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].supplyMaxNumberOfColumns(study);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::yearBegin(uint year, unsigned int numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].yearBegin(year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::yearEndBuildPrepareDataForEachThermalCluster(
  State& state,
  uint year,
  unsigned int numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::yearEndBuildForEachThermalCluster(State& state,
                                                                          uint year,
                                                                          unsigned int numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].yearEndBuildForEachThermalCluster(state, year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::buildThermalClusterYearEndResults(State& /*state*/,
                                                                          uint /*year*/,
                                                                          uint /*numSpace*/)
{
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].yearEnd(year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::computeSummary(unsigned int year, unsigned int numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].computeSummary(year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::weekBegin(State& state)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].weekBegin(state);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::weekForEachArea(State&, [[maybe_unused]] uint numSpace)
{
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].beforeYearByYearExport(year, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::weekEnd(State& state)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].weekEnd(state);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].hourBegin(hourInTheYear);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::hourForEachArea(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].hourForEachArea(state, numSpace);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::hourForEachLink(State& state, uint numSpace)
{
    pLinks[state.link->indexForArea].hourForEachLink(state, numSpace);
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].hourEnd(state, hourInTheYear);
    }
}

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::buildSurveyReport(SurveyResults& results,
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

        auto& link = *results.data.link;

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

template<class VariablePerLinkList>
inline void Links<VariablePerLinkList>::buildAnnualSurveyReport(SurveyResults& results,
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

        auto& linkOutputData = pLinks[results.data.link->indexForArea];
        linkOutputData.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class VariablePerLinkList>
void Links<VariablePerLinkList>::initializeFromArea(Data::Study* study, Data::Area* area)
{
    assert(study && "A study pointer must not be null");
    assert(area && "An area pointer must not be null");

    pLinks.resize(area->links.size());

    uint lnkIndex = 0;
    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i, ++lnkIndex)
    {
        VariablePerLink& n = pLinks[lnkIndex];

        n.initializeFromStudy(*study);
        n.initializeFromArea(study, area);
        n.initializeFromLink(study, i->second);
    }
}

template<class VariablePerLinkList>
void Links<VariablePerLinkList>::simulationBegin()
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].simulationBegin();
    }
}

template<class VariablePerLinkList>
void Links<VariablePerLinkList>::simulationEnd()
{
    for (uint i = 0; i != pLinks.size(); ++i)
    {
        pLinks[i].simulationEnd();
    }
}

template<class VariablePerLinkList>
void Links<VariablePerLinkList>::buildDigest(SurveyResults& results,
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

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_INC_LINK_HXX__
