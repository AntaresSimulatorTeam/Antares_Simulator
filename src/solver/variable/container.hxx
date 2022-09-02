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
#ifndef __SOLVER_CONTAINER_CONTAINER_HXX__
#define __SOLVER_CONTAINER_CONTAINER_HXX__

#include <yuni/yuni.h>
#include <yuni/core/static/types.h>

#include <memory>

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Container
{
template<class NextT>
inline List<NextT>::List()
{
}

template<class NextT>
inline List<NextT>::~List()
{
}

template<class NextT>
inline void List<NextT>::initializeFromStudy(Data::Study& study)
{
    // Store a pointer to the current study
    pStudy = &study;
    // Next
    NextType::initializeFromStudy(study);
}

template<class NextT>
inline void List<NextT>::initializeFromArea(Data::Study* study, Data::Area* area)
{
    // Ask to the first variable to do it
    NextType::initializeFromArea(study, area);
}

template<class NextT>
inline void List<NextT>::initializeFromLink(Data::Study* study, Data::AreaLink* link)
{
    // Next
    NextType::initializeFromAreaLink(study, link);
}

template<class NextT>
inline void List<NextT>::initializeFromThermalCluster(Data::Study* study,
                                                      Data::Area* area,
                                                      Data::ThermalCluster* cluster)
{
    // Next
    NextType::initializeFromThermalCluster(study, area, cluster);
}

template<class NextT>
inline void List<NextT>::simulationBegin()
{
    NextType::simulationBegin();
}

template<class NextT>
inline void List<NextT>::simulationEnd()
{
    NextType::simulationEnd();
}

template<class NextT>
inline void List<NextT>::yearBegin(unsigned int year, unsigned int numSpace)
{
    NextType::yearBegin(year, numSpace);
}

template<class NextT>
inline void List<NextT>::yearEnd(unsigned int year, unsigned int numSpace)
{
    NextType::yearEnd(year, numSpace);
}

template<class NextT>
inline void List<NextT>::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                                        unsigned int nbYearsForCurrentSummary)
{
    NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
}

template<class NextT>
template<class V>
inline void List<NextT>::yearEndSpatialAggregates(V& allVars,
                                                  unsigned int year,
                                                  unsigned int numSpace)
{
    // Next variable
    NextType::template yearEndSpatialAggregates(allVars, year, numSpace);
}

template<class NextT>
template<class V, class SetT>
inline void List<NextT>::yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set)
{
    // Next variable
    NextType::template yearEndSpatialAggregates(allVars, year, set);
}

template<class NextT>
template<class V>
inline void List<NextT>::computeSpatialAggregatesSummary(
  V& allVars,
  std::map<unsigned int, unsigned int>& numSpaceToYear,
  unsigned int nbYearsForCurrentSummary)
{
    // Next variable
    NextType::template computeSpatialAggregatesSummary(
      allVars, numSpaceToYear, nbYearsForCurrentSummary);
}

template<class NextT>
template<class V>
inline void List<NextT>::simulationEndSpatialAggregates(V& allVars)
{
    // Next variable
    NextType::template simulationEndSpatialAggregates(allVars);
}

template<class NextT>
template<class V, class SetT>
inline void List<NextT>::simulationEndSpatialAggregates(V& allVars, const SetT& set)
{
    // Next variable
    NextType::template simulationEndSpatialAggregates(allVars, set);
}

template<class NextT>
inline void List<NextT>::hourBegin(unsigned int hourInTheYear)
{
    NextType::hourBegin(hourInTheYear);
}

template<class NextT>
inline void List<NextT>::weekBegin(State& state)
{
    NextType::weekBegin(state);
}

template<class NextT>
inline void List<NextT>::weekEnd(State& state)
{
    NextType::weekEnd(state);
}

template<class NextT>
inline void List<NextT>::hourForEachArea(State& state, unsigned int numSpace)
{
    NextType::hourForEachArea(state, numSpace);
}

template<class NextT>
inline void List<NextT>::hourForEachThermalCluster(State& state)
{
    NextType::hourForEachThermalCluster(state);
}

template<class NextT>
inline void List<NextT>::hourForEachLink(State& state)
{
    NextType::hourForEachLink(state);
}

template<class NextT>
inline void List<NextT>::hourEnd(State& state, unsigned int hourInTheYear)
{
    NextType::hourEnd(state, hourInTheYear);
}

template<class NextT>
template<class SearchVCardT, class O>
inline void List<NextT>::computeSpatialAggregateWith(O& out)
{
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out);
}

template<class NextT>
template<class SearchVCardT, class O>
inline void List<NextT>::computeSpatialAggregateWith(O& out,
                                                     const Data::Area* area,
                                                     unsigned int numSpace)
{
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForArea(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    NextType::template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForThermalCluster(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    NextType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class NextT>
template<class VCardToFindT>
inline void List<NextT>::retrieveResultsForLink(
  typename Variable::Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    NextType::template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class NextT>
inline Yuni::uint64 List<NextT>::memoryUsage() const
{
    return sizeof(ListType) + NextType::memoryUsage();
}

template<class NextT>
void List<NextT>::buildSurveyReport(SurveyResults& results,
                                    int dataLevel,
                                    int fileLevel,
                                    int precision) const
{
    // We must not do something if mc and !annual
    if (fileLevel & Category::mc && !(precision & Category::annual))
        return;

    // Reset
    results.data.columnIndex = 0;

    // The new filename
    results.data.filename.clear();
    results.data.filename << results.data.output << '/';
    Category::FileLevelToStream(results.data.filename, fileLevel);
    results.data.filename << '-';
    Category::PrecisionLevelToStream(results.data.filename, precision);
    results.data.filename << ".txt";

    // Ask to all variables
    NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
        results.saveToFile(dataLevel, fileLevel, precision);
}

template<class NextT>
void List<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                          int dataLevel,
                                          int fileLevel,
                                          int precision,
                                          unsigned int numSpace) const
{
    // We must not do something if mc and !annual
    if (fileLevel & Category::mc && !(precision & Category::annual))
        return;

    // Reset
    results.data.columnIndex = 0;

    // The new filename
    results.data.filename.clear();
    results.data.filename << results.data.output << '/';
    Category::FileLevelToStream(results.data.filename, fileLevel);
    results.data.filename << '-';
    Category::PrecisionLevelToStream(results.data.filename, precision);
    results.data.filename.append(".txt", 4);

    // Ask to all variables
    NextType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);

    // If the column index is still equals to 0, that would mean we have nothing
    // to do (there is no data to write)
    if (results.data.columnIndex > 0)
        results.saveToFile(dataLevel, fileLevel, precision);
}

template<class NextT>
inline void List<NextT>::EstimateMemoryUsage(Data::StudyMemoryUsage& u)
{
    u.requiredMemoryForOutput += sizeof(ListType);
    u.requiredDiskSpaceForOutput += 20 * 1024; // 20Ko
    NextType::EstimateMemoryUsage(u);
}

template<class NextT>
void List<NextT>::exportSurveyResults(bool global,
                                      const Yuni::String& output,
                                      unsigned int numSpace)
{
    using namespace Antares;

    // Infos
    if (global)
    {
        logs.info(); // empty line
        logs.checkpoint() << "Exporting the survey results...";
    }
    else
        logs.info() << "Exporting the annual results";

    // Structure for the survey results
    // Getting the any report's max number of columns
    uint nbColumnsNeededForExportation = pStudy->parameters.variablesPrintInfo.getMaxColumnsCount();
    logs.debug() << "  (for " << nbColumnsNeededForExportation << " columns)";

    auto survey = std::make_shared<SurveyResults>(nbColumnsNeededForExportation, *pStudy, output);

    // Year by year ?
    survey->yearByYearResults = !global;

    if (global)
    {
        // alias to the type of the report builder
        using Builder = SurveyReportBuilder<true, ListType>;
        // Building the survey results for each possible state
        Builder::Run(*this, *survey);

        // Exporting the Grid (information about the study)
        survey->exportGridInfos();
    }
    else
    {
        // alias to the type of the report builder
        using Builder = SurveyReportBuilder<false, ListType>;
        // Building the survey results for each possible state
        Builder::Run(*this, *survey, numSpace);
    }
}

} // namespace Container
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_CONTAINER_CONTAINER_H__
