// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_CONTAINER_CONTAINER_HXX__
#define __SOLVER_CONTAINER_CONTAINER_HXX__

#include <yuni/core/static/types.h>

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"
#include "antares/solver/variable/surveyresults/reportbuilder.hxx"

namespace Antares::Solver::Variable::Container
{
template<class Inner>
inline void List<Inner>::initializeFromStudy(Data::Study& study)
{
    // Store a pointer to the current study
    pStudy = &study;
    dynamicAggregationAllYears_ = std::make_unique<DynamicAggregationAllYears>(study);
    dynamicAggregationSingleYear_.reserve(study.maxNbYearsInParallel);
    for (size_t y = 0; y < study.maxNbYearsInParallel; ++y)
    {
        dynamicAggregationSingleYear_.emplace_back(study);
    }
    next_.initializeFromStudy(study);
}

template<class Inner>
inline void List<Inner>::initializeFromArea(Data::Study* study, Data::Area* area)
{
    next_.initializeFromArea(study, area);
}

template<class Inner>
inline void List<Inner>::initializeFromLink(Data::Study* study, Data::AreaLink* link)
{
    next_.initializeFromLink(study, link);
}

template<class Inner>
inline void List<Inner>::simulationBegin()
{
    next_.simulationBegin();
}

template<class Inner>
inline void List<Inner>::simulationEnd()
{
    next_.simulationEnd();
}

template<class Inner>
inline void List<Inner>::yearBegin(unsigned int year, unsigned int numSpace)
{
    dynamicAggregationSingleYear_[numSpace].reset();
    next_.yearBegin(year, numSpace);
}

template<class Inner>
inline void List<Inner>::buildThermalClusterYearEndResults(State& state,
                                                           unsigned int year,
                                                           unsigned int numSpace)
{
    next_.buildThermalClusterYearEndResults(state, year, numSpace);
}

template<class Inner>
inline void List<Inner>::yearEnd(unsigned int year, unsigned int numSpace)
{
    next_.yearEnd(year, numSpace);
}

template<class Inner>
inline void List<Inner>::computeSummary(unsigned int year, unsigned int numSpace)
{
    next_.computeSummary(year, numSpace);
}

template<class Inner>
template<class V>
inline void List<Inner>::yearEndSpatialAggregates(V& allVars,
                                                  unsigned int year,
                                                  unsigned int numSpace)
{
    next_.yearEndSpatialAggregates(allVars, year, numSpace);
}

template<class Inner>
template<class V, class SetT>
inline void List<Inner>::yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set)
{
    next_.yearEndSpatialAggregates(allVars, year, set);
}

template<class Inner>
template<class V>
inline void List<Inner>::computeSpatialAggregatesSummary(V& allVars,
                                                         unsigned int year,
                                                         unsigned int numSpace)
{
    dynamicAggregationAllYears_->merge(dynamicAggregationSingleYear_[numSpace], year);
    next_.computeSpatialAggregatesSummary(allVars, year, numSpace);
}

template<class Inner>
template<class V>
inline void List<Inner>::simulationEndSpatialAggregates(V& allVars)
{
    next_.simulationEndSpatialAggregates(allVars);
}

template<class Inner>
template<class V, class SetT>
inline void List<Inner>::simulationEndSpatialAggregates(V& allVars, const SetT& set)
{
    next_.simulationEndSpatialAggregates(allVars, set);
}

template<class Inner>
inline void List<Inner>::beforeYearByYearExport(unsigned int year, unsigned int numSpace)
{
    next_.beforeYearByYearExport(year, numSpace);
}

template<class Inner>
inline void List<Inner>::hourBegin(unsigned int hourInTheYear)
{
    next_.hourBegin(hourInTheYear);
}

template<class Inner>
inline void List<Inner>::weekBegin(State& state)
{
    dynamicAggregationSingleYear_[state.numSpace].addResultsToSets(*state.problemeHebdo);
    next_.weekBegin(state);
}

template<class Inner>
inline void List<Inner>::weekForEachArea(State& state, unsigned int numSpace)
{
    next_.weekForEachArea(state, numSpace);
}

template<class Inner>
inline void List<Inner>::weekEnd(State& state)
{
    next_.weekEnd(state);
}

template<class Inner>
inline void List<Inner>::hourForEachArea(State& state, unsigned int numSpace)
{
    next_.hourForEachArea(state, numSpace);
}

template<class Inner>
inline void List<Inner>::hourForEachLink(State& state)
{
    next_.hourForEachLink(state);
}

template<class Inner>
inline void List<Inner>::hourEnd(State& state, unsigned int hourInTheYear)
{
    next_.hourEnd(state, hourInTheYear);
}

template<class Inner>
template<class SearchVCardT, class O>
inline void List<Inner>::computeSpatialAggregateWith(O& out)
{
    next_.template computeSpatialAggregateWith<SearchVCardT, O>(out);
}

template<class Inner>
template<class SearchVCardT, class O>
inline void List<Inner>::computeSpatialAggregateWith(O& out,
                                                     const Data::Area* area,
                                                     unsigned int numSpace)
{
    next_.template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
}

template<class Inner>
template<class VCardToFindT>
inline void List<Inner>::retrieveResultsForArea(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    next_.template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class Inner>
template<class VCardToFindT>
inline void List<Inner>::retrieveResultsForThermalCluster(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    next_.template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class Inner>
template<class VCardToFindT>
inline void List<Inner>::retrieveResultsForLink(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    next_.template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class Inner>
void List<Inner>::buildSurveyReport(SurveyResults& results,
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
    next_.buildSurveyReport(results, dataLevel, fileLevel, precision);

    // Append dynamic aggregation columns for sets of areas, values files only
    if (dataLevel == Category::DataLevel::setOfAreas && fileLevel == Category::FileLevel::va
        && dynamicAggregationAllYears_)
    {
        dynamicAggregationAllYears_->appendToSurveyForSet(results.data.setOfAreasName,
                                                          results,
                                                          static_cast<Category::Precision>(
                                                            precision));
    }

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
    {
        results.saveToFile(dataLevel, fileLevel, precision);
    }
}

template<class Inner>
void List<Inner>::buildAnnualSurveyReport(SurveyResults& results,
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
    next_.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);

    // Append dynamic aggregation columns for sets of areas, values files only
    if (dataLevel == Category::DataLevel::setOfAreas && fileLevel == Category::FileLevel::va)
    {
        dynamicAggregationSingleYear_[numSpace].appendToSurveyForSet(
          results.data.setOfAreasName,
          results,
          static_cast<Category::Precision>(precision));
    }

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
    {
        results.saveToFile(dataLevel, fileLevel, precision);
    }
}

template<class Inner>
void List<Inner>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    // Reset
    results.data.columnIndex = 0;
    results.data.thermalCluster = nullptr;
    results.data.area = nullptr;
    results.data.link = nullptr;
    results.variableCaption.clear();

    // Building the digest
    next_.buildDigest(results, digestLevel, dataLevel);
}

template<class Inner>
void List<Inner>::exportSurveyResults(bool global,
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
