// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_CONTAINER_CONTAINER_HXX__
#define __SOLVER_CONTAINER_CONTAINER_HXX__

#include <yuni/core/static/types.h>

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"
#include "antares/solver/variable/surveyresults/reportbuilder.hxx"

namespace Antares::Solver::Variable::Container
{
template<class NextT>
inline void List<NextT>::initializeFromStudy(Data::Study& study)
{
    // Store a pointer to the current study
    pStudy = &study;
    dynamicAggregationAllYears_ = std::make_unique<DynamicAggregationAllYears>(study);
    dynamicAggregationSingleYear_.reserve(study.maxNbYearsInParallel);
    for (size_t y = 0; y < study.maxNbYearsInParallel; ++y)
    {
        dynamicAggregationSingleYear_.emplace_back(study);
    }
    // Next
    NextT::initializeFromStudy(study);
}

template<class NextT>
inline void List<NextT>::initializeFromArea(Data::Study* study, Data::Area* area)
{
    // Ask to the first variable to do it
    NextT::initializeFromArea(study, area);
}

template<class NextT>
inline void List<NextT>::initializeFromLink(Data::Study* study, Data::AreaLink* link)
{
    // Next
    NextT::initializeFromAreaLink(study, link);
}

template<class NextT>
inline void List<NextT>::initializeFromThermalCluster(Data::Study* study,
                                                      Data::Area* area,
                                                      Data::ThermalCluster* cluster)
{
    // Next
    NextT::initializeFromThermalCluster(study, area, cluster);
}

template<class NextT>
inline void List<NextT>::simulationBegin()
{
    NextT::simulationBegin();
}

template<class NextT>
inline void List<NextT>::simulationEnd()
{
    NextT::simulationEnd();
}

template<class NextT>
inline void List<NextT>::yearBegin(unsigned int year, unsigned int numSpace)
{
    dynamicAggregationSingleYear_[numSpace].reset();
    NextT::yearBegin(year, numSpace);
}

template<class NextT>
inline void List<NextT>::yearEnd(unsigned int year, unsigned int numSpace)
{
    NextT::yearEnd(year, numSpace);
}

template<class NextT>
inline void List<NextT>::computeSummary(unsigned int year, unsigned int numSpace)
{
    NextT::computeSummary(year, numSpace);
}

template<class NextT>
template<class V>
inline void List<NextT>::yearEndSpatialAggregates(V& allVars,
                                                  unsigned int year,
                                                  unsigned int numSpace)
{
    // Next variable
    NextT::yearEndSpatialAggregates(allVars, year, numSpace);
}

template<class NextT>
template<class V, class SetT>
inline void List<NextT>::yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set)
{
    // Next variable
    NextT::yearEndSpatialAggregates(allVars, year, set);
}

template<class NextT>
template<class V>
inline void List<NextT>::computeSpatialAggregatesSummary(V& allVars,
                                                         unsigned int year,
                                                         unsigned int numSpace)
{
    dynamicAggregationAllYears_->merge(dynamicAggregationSingleYear_[numSpace], year);
    // Next variable
    NextT::computeSpatialAggregatesSummary(allVars, year, numSpace);
}

template<class NextT>
template<class V>
inline void List<NextT>::simulationEndSpatialAggregates(V& allVars)
{
    // Next variable
    NextT::simulationEndSpatialAggregates(allVars);
}

template<class NextT>
template<class V, class SetT>
inline void List<NextT>::simulationEndSpatialAggregates(V& allVars, const SetT& set)
{
    // Next variable
    NextT::simulationEndSpatialAggregates(allVars, set);
}

template<class NextT>
inline void List<NextT>::hourBegin(unsigned int hourInTheYear)
{
    NextT::hourBegin(hourInTheYear);
}

template<class NextT>
inline void List<NextT>::weekBegin(State& state)
{
    dynamicAggregationSingleYear_[state.numSpace].addResultsToSets(*state.problemeHebdo);
    NextT::weekBegin(state);
}

template<class NextT>
inline void List<NextT>::weekEnd(State& state)
{
    NextT::weekEnd(state);
}

template<class NextT>
inline void List<NextT>::hourForEachArea(State& state, unsigned int numSpace)
{
    NextT::hourForEachArea(state, numSpace);
}

template<class NextT>
inline void List<NextT>::hourForEachLink(State& state)
{
    NextT::hourForEachLink(state);
}

template<class NextT>
inline void List<NextT>::hourEnd(State& state, unsigned int hourInTheYear)
{
    NextT::hourEnd(state, hourInTheYear);
}

template<class NextT>
template<class SearchVCardT, class O>
inline void List<NextT>::computeSpatialAggregateWith(O& out)
{
    NextT::template computeSpatialAggregateWith<SearchVCardT, O>(out);
}

template<class NextT>
template<class SearchVCardT, class O>
inline void List<NextT>::computeSpatialAggregateWith(O& out,
                                                     const Data::Area* area,
                                                     unsigned int numSpace)
{
    NextT::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForArea(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    NextT::template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForThermalCluster(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    NextT::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForLink(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    NextT::template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class NextT>
void List<NextT>::buildSurveyReport(SurveyResults& results,
                                    int dataLevel,
                                    int fileLevel,
                                    int precision) const
{
    // Reset
    results.data.columnIndex = 0;

    // The new filename
    results.data.filename.clear();
    results.data.filename << std::filesystem::path(static_cast<std::string>(results.data.output))
                               / "";
    Category::FileLevelToStream(results.data.filename, fileLevel);
    results.data.filename << '-';
    Category::PrecisionLevelToStream(results.data.filename, precision);
    results.data.filename << ".txt";

    // Ask to all variables
    NextT::buildSurveyReport(results, dataLevel, fileLevel, precision);

    // Append dynamic aggregation columns for sets of areas, values files only
    if (dataLevel == Category::DataLevel::setOfAreas && fileLevel == Category::FileLevel::va
        && dynamicAggregationAllYears_)
    {
        const auto& setName = pStudy->setsOfAreas.nameByIndex(results.data.setOfAreasIndex);
        dynamicAggregationAllYears_
          ->appendToSurveyForSet(setName, results, static_cast<Category::Precision>(precision));
    }

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
    {
        results.saveToFile(dataLevel, fileLevel, precision);
    }
}

template<class NextT>
void List<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                          int dataLevel,
                                          int fileLevel,
                                          int precision,
                                          unsigned int numSpace) const
{
    // Reset
    results.data.columnIndex = 0;

    // The new filename
    results.data.filename.clear();
    results.data.filename << std::filesystem::path(static_cast<std::string>(results.data.output))
                               / "";
    Category::FileLevelToStream(results.data.filename, fileLevel);
    results.data.filename << '-';
    Category::PrecisionLevelToStream(results.data.filename, precision);
    results.data.filename.append(".txt", 4);

    // Ask to all variables
    NextT::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);

    // Append dynamic aggregation columns for sets of areas, values files only
    if (dataLevel == Category::DataLevel::setOfAreas && fileLevel == Category::FileLevel::va)
    {
        const auto& setName = pStudy->setsOfAreas.nameByIndex(results.data.setOfAreasIndex);
        dynamicAggregationSingleYear_[numSpace]
          .appendToSurveyForSet(setName, results, static_cast<Category::Precision>(precision));
    }

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
    {
        results.saveToFile(dataLevel, fileLevel, precision);
    }
}

template<class NextT>
void List<NextT>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    // Reset
    results.data.columnIndex = 0;
    results.data.thermalCluster = nullptr;
    results.data.area = nullptr;
    results.data.link = nullptr;
    results.variableCaption.clear();

    // Building the digest
    NextT::buildDigest(results, digestLevel, dataLevel);
}

template<class NextT>
void List<NextT>::exportSurveyResults(bool global,
                                      const Yuni::String& output,
                                      unsigned int numSpace,
                                      IResultWriter& writer)
{
    using namespace Antares;

    // Infos
    if (global)
    {
        logs.info(); // empty line
        logs.checkpoint() << "Exporting the survey results...";
    }
    else
    {
        logs.info() << "Exporting the annual results";
    }

    SurveyResults survey(*pStudy,
                         pStudy->parameters.variablesPrintInfo.getTotalMaxColumnsCount(),
                         output,
                         writer);

    // Year by year ?
    survey.yearByYearResults = !global;

    if (global)
    {
        // alias to the type of the report builder
        using Builder = SurveyReportBuilder<true, ListType>;
        // Building the survey results for each possible state
        Builder::Run(*this, survey);

        // Exporting the Grid (information about the study)
        survey.exportGridInfos();

        // Exporting the digest
        // The digest must be exported after the real report because some values
        // are computed at this moment.
        Builder::RunDigest(*this, survey, writer);
    }
    else
    {
        // alias to the type of the report builder
        using Builder = SurveyReportBuilder<false, ListType>;
        // Building the survey results for each possible state
        Builder::Run(*this, survey, numSpace);
    }
}

} // namespace Antares::Solver::Variable::Container

#endif // __SOLVER_CONTAINER_CONTAINER_H__
