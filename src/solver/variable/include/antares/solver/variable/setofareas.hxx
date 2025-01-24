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
#ifndef __SOLVER_VARIABLE_SET_OF_AREAS_HXX__
#define __SOLVER_VARIABLE_SET_OF_AREAS_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class NextT>
void SetsOfAreas<NextT>::initializeFromStudy(Data::Study& study)
{
    using namespace Antares;

    // The study
    pStudy = &study;
    // alias to the set of sets of areas
    auto& sets = study.setsOfAreas;
    // Reserving the memory
    pSetsOfAreas.reserve(sets.size());
    pOriginalSets.reserve(sets.size());

    // For each set...
    for (uint setIndex = 0; setIndex != sets.size(); ++setIndex)
    {
        if (!sets.hasOutput(setIndex))
        {
            continue;
        }
        // Name of the set
        const auto& setname = sets.caption(setIndex);

        // Useless if the result set is empty
        if (!sets.resultSize(setIndex))
        {
            logs.warning() << "The set of areas named '" << setname
                           << "' is empty. No output will be produced for this set.";
            continue;
        }

        auto n = std::make_unique<NextT>();

        // Initialize the variables
        // From the study
        n->initializeFromStudy(study);

        // Making specific variables non applicable in following output reports :
        // - annual district reports
        // - over all years district statistics reports
        n->broadcastNonApplicability(true);

        // For each current set's variable, getting the print status, that is :
        // is variable's column(s) printed in output (set of areas) reports ?
        n->getPrintStatusFromStudy(study);

        pSetsOfAreas.push_back(std::move(n));

        auto* originalSet = &sets[setIndex];
        assert(originalSet != NULL);
        assert(!originalSet->empty());
        pOriginalSets.push_back(originalSet);

        pNames.push_back(setname);
    }
}

template<class NextT>
inline void SetsOfAreas<NextT>::initializeFromArea(Data::Study*, Data::Area*)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::initializeFromAreaLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::initializeFromThermalCluster(Data::Study*,
                                                             Data::Area*,
                                                             Data::ThermalCluster*)
{
    // This method should not be called at this stage
}

template<class NextT>
inline void SetsOfAreas<NextT>::simulationBegin()
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::simulationEnd()
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::yearBegin(uint /*year*/, uint /* numSpace */)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::yearEndBuild(State& /*state*/, uint /*year*/, uint /*numSpace*/)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::yearEnd(uint /*year*/, uint /*numSpace*/)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::computeSummary(
  std::map<unsigned int, unsigned int>& /*numSpaceToYear*/,
  unsigned int /* nbYearsForCurrentSummary */)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::hourBegin(uint /*hourInTheYear*/)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::weekBegin(State&)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::weekForEachArea(State&, unsigned int /*numSpace*/)
{
    // Nothing to do here
}

template<class NextT>
inline void SetsOfAreas<NextT>::weekEnd(State&)
{
    // Nothing to do here
}

template<class NextT>
void SetsOfAreas<NextT>::hourForEachArea(State& state, unsigned int)
{
    (void)state;
}

template<class NextT>
inline void SetsOfAreas<NextT>::hourForEachLink(State& state)
{
    (void)state;
}

template<class NextT>
inline void SetsOfAreas<NextT>::hourEnd(State& state, uint hourInTheYear)
{
    (void)state;
    (void)hourInTheYear;
}

template<class NextT>
inline void SetsOfAreas<NextT>::buildSurveyReport(SurveyResults& results,
                                                  int dataLevel,
                                                  int fileLevel,
                                                  int precision) const
{
    int count_int = count;
    bool setOfAreasDataLevel = dataLevel & Category::DataLevel::setOfAreas;
    if (count_int && setOfAreasDataLevel)
    {
        pSetsOfAreas[results.data.setOfAreasIndex]->buildSurveyReport(results,
                                                                      dataLevel,
                                                                      fileLevel,
                                                                      precision);
    }
}

template<class NextT>
inline void SetsOfAreas<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                                        int dataLevel,
                                                        int fileLevel,
                                                        int precision,
                                                        uint numSpace) const
{
    int count_int = count;
    bool setOfAreasDataLevel = dataLevel & Category::DataLevel::setOfAreas;
    if (count_int && setOfAreasDataLevel)
    {
        pSetsOfAreas[results.data.setOfAreasIndex]->buildAnnualSurveyReport(results,
                                                                            dataLevel,
                                                                            fileLevel,
                                                                            precision,
                                                                            numSpace);
    }
}

template<class NextT>
void SetsOfAreas<NextT>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    int count_int = count;
    bool setOfAreasDataLevel = dataLevel & Category::DataLevel::setOfAreas;
    if (count_int && setOfAreasDataLevel)
    {
        // Reset
        results.data.rowCaptions.clear();
        results.data.rowCaptions.resize(pSetsOfAreas.size());
        results.data.area = nullptr;
        results.data.rowIndex = 0;

        for (auto& set: pSetsOfAreas)
        {
            results.data.columnIndex = 0;
            results.data.rowCaptions[results.data.rowIndex].clear()
              << "@ " << pNames[results.data.rowIndex];
            set->buildDigest(results, digestLevel, dataLevel);
            ++results.data.rowIndex;
        }
    }
}

template<class NextT>
template<class I>
inline void SetsOfAreas<NextT>::provideInformations(I& infos)
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

template<class NextT>
template<class V>
void SetsOfAreas<NextT>::yearEndSpatialAggregates(V& allVars, uint year, uint numSpace)
{
    for (uint setindex = 0; setindex != pSetsOfAreas.size(); ++setindex)
    {
        assert(setindex < pOriginalSets.size());
        pSetsOfAreas[setindex]->yearEndSpatialAggregates(allVars,
                                                         year,
                                                         *(pOriginalSets[setindex]),
                                                         numSpace);
    }
}

template<class NextT>
template<class V>
void SetsOfAreas<NextT>::computeSpatialAggregatesSummary(
  V& allVars,
  std::map<unsigned int, unsigned int>& numSpaceToYear,
  unsigned int nbYearsForCurrentSummary)
{
    for (uint setindex = 0; setindex != pSetsOfAreas.size(); ++setindex)
    {
        assert(setindex < pOriginalSets.size());
        pSetsOfAreas[setindex]->computeSpatialAggregatesSummary(allVars,
                                                                numSpaceToYear,
                                                                nbYearsForCurrentSummary);
    }
}

template<class NextT>
template<class V>
void SetsOfAreas<NextT>::simulationEndSpatialAggregates(V& allVars)
{
    for (uint i = 0; i != pSetsOfAreas.size(); ++i)
    {
        pSetsOfAreas[i]->simulationEndSpatialAggregates(allVars, *(pOriginalSets[i]));
    }
}

template<class NextT>
void SetsOfAreas<NextT>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pSetsOfAreas.size(); ++i)
    {
        pSetsOfAreas[i]->beforeYearByYearExport(year, numSpace);
    }
}

template<class NextT>
template<class SearchVCardT, class O>
inline void SetsOfAreas<NextT>::computeSpatialAggregateWith(O&)
{
    // Do nothing
}

template<class NextT>
template<class SearchVCardT, class O>
inline void SetsOfAreas<NextT>::computeSpatialAggregateWith(O& out,
                                                            const Data::Area* area,
                                                            uint numSpace)
{
    (void)out;
    (void)area;
    (void)numSpace;
    // pSetsOfAreas[area->index]->computeSpatialAggregateWith<SearchVCardT,O>(out);
}

template<class NextT>
template<class VCardToFindT>
inline const double* SetsOfAreas<NextT>::retrieveHourlyResultsForCurrentYear() const
{
    return nullptr;
}

template<class NextT>
template<class VCardToFindT>
inline void SetsOfAreas<NextT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    (void)result;
    (void)area;
}

template<class NextT>
template<class VCardToFindT>
inline void SetsOfAreas<NextT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    (void)result;
    (void)cluster;
}

template<class NextT>
template<class VCardToFindT>
inline void SetsOfAreas<NextT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    (void)result;
    (void)link;
}

template<class NextT>
template<class PredicateT>
inline void SetsOfAreas<NextT>::RetrieveVariableList(PredicateT& /*predicate*/)
{
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_SET_OF_AREAS_HXX__
