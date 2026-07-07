// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_SET_OF_AREAS_HXX__
#define __SOLVER_VARIABLE_SET_OF_AREAS_HXX__

namespace Antares::Solver::Variable
{
template<class VariableList>
void SetsOfAreas<VariableList>::initializeFromStudy(Data::Study& study)
{
    using namespace Antares;

    // The study
    pStudy = &study;
    // alias to the set of sets of areas
    auto& sets = study.setsOfAreas;
    // Reserving the memory
    pSetsOfAreas.reserve(sets.size());
    pOriginalSets.reserve(sets.size());
    pIds.reserve(sets.size());

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

        auto n = std::make_unique<VariableList>();

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

        pIds.push_back(sets.nameByIndex(setIndex));
        pNames.push_back(setname);
    }
}

template<class VariableList>
inline void SetsOfAreas<VariableList>::buildSurveyReport(SurveyResults& results,
                                                         int dataLevel,
                                                         int fileLevel,
                                                         int precision) const
{
    int count_int = count;
    bool setOfAreasDataLevel = dataLevel & Category::DataLevel::setOfAreas;
    if (count_int && setOfAreasDataLevel)
    {
        if (const auto* set = findSetById(results.data.setOfAreasName))
        {
            set->buildSurveyReport(results, dataLevel, fileLevel, precision);
        }
    }
}

template<class VariableList>
inline void SetsOfAreas<VariableList>::buildAnnualSurveyReport(SurveyResults& results,
                                                               int dataLevel,
                                                               int fileLevel,
                                                               int precision,
                                                               uint numSpace) const
{
    int count_int = count;
    bool setOfAreasDataLevel = dataLevel & Category::DataLevel::setOfAreas;
    if (count_int && setOfAreasDataLevel)
    {
        if (const auto* set = findSetById(results.data.setOfAreasName))
        {
            set->buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
        }
    }
}

template<class VariableList>
const VariableList* SetsOfAreas<VariableList>::findSetById(
  const Data::Study::SetsOfAreas::IDType& setId) const
{
    for (uint i = 0; i != pNames.size(); ++i)
    {
        if (pIds[i] == setId)
        {
            return pSetsOfAreas[i].get();
        }
    }
    return nullptr;
}

template<class VariableList>
void SetsOfAreas<VariableList>::buildDigest(SurveyResults& results,
                                            int digestLevel,
                                            int dataLevel) const
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

template<class VariableList>
template<class V>
void SetsOfAreas<VariableList>::yearEndSpatialAggregates(V& allVars, uint year, uint numSpace)
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

template<class VariableList>
template<class V>
void SetsOfAreas<VariableList>::computeSpatialAggregatesSummary(V& allVars,
                                                                unsigned int year,
                                                                unsigned int numSpace)
{
    for (uint setindex = 0; setindex != pSetsOfAreas.size(); ++setindex)
    {
        assert(setindex < pOriginalSets.size());
        pSetsOfAreas[setindex]->computeSpatialAggregatesSummary(allVars, year, numSpace);
    }
}

template<class VariableList>
template<class V>
void SetsOfAreas<VariableList>::simulationEndSpatialAggregates(V& allVars)
{
    for (uint i = 0; i != pSetsOfAreas.size(); ++i)
    {
        pSetsOfAreas[i]->simulationEndSpatialAggregates(allVars, *(pOriginalSets[i]));
    }
}

template<class VariableList>
void SetsOfAreas<VariableList>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pSetsOfAreas.size(); ++i)
    {
        pSetsOfAreas[i]->beforeYearByYearExport(year, numSpace);
    }
}

template<class VariableList>
template<class SearchVCardT, class O>
inline void SetsOfAreas<VariableList>::computeSpatialAggregateWith(O&)
{
    // Do nothing
}

template<class VariableList>
template<class SearchVCardT, class O>
inline void SetsOfAreas<VariableList>::computeSpatialAggregateWith(O& out,
                                                                   const Data::Area* area,
                                                                   uint numSpace)
{
    (void)out;
    (void)area;
    (void)numSpace;
    // pSetsOfAreas[area->index]->computeSpatialAggregateWith<SearchVCardT,O>(out);
}

template<class VariableList>
template<class VCardToFindT>
inline void SetsOfAreas<VariableList>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    (void)result;
    (void)area;
}

template<class VariableList>
template<class VCardToFindT>
inline void SetsOfAreas<VariableList>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    (void)result;
    (void)cluster;
}

template<class VariableList>
template<class VCardToFindT>
inline void SetsOfAreas<VariableList>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    (void)result;
    (void)link;
}

template<class VariableList>
template<class PredicateT>
inline void SetsOfAreas<VariableList>::RetrieveVariableList(PredicateT& /*predicate*/)
{
}

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_SET_OF_AREAS_HXX__
