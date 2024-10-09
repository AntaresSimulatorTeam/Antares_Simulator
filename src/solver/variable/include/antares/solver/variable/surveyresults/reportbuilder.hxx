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
#ifndef __SOLVER_VARIABLE_SURVEYRESULTS_REPORT_BUILDER_HXX__
#define __SOLVER_VARIABLE_SURVEYRESULTS_REPORT_BUILDER_HXX__

#include <limits>

#include <yuni/yuni.h>
#include <yuni/core/static/types.h>
#include <yuni/core/string.h>
#include <yuni/io/directory.h>

#include <antares/logs/logs.h>
#include <antares/study/filter.h>

#include "../categories.h"
#include "../endoflist.h"
#include "../info.h"
#include "../surveyresults.h"

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Container
{
namespace // anonymous
{
template<class NextT, int CDataLevel, int CFile = 1>
struct VariablesStatsByDataLevel
{
    enum
    {
        nextFileLevel = (CFile * 2 > (int)Category::FileLevel::maxFileLevel) ? 1 : CFile * 2,
        currentVariableCount = NextT::template Statistics < CDataLevel,
        CFile > ::count,
        nextVariableCount = VariablesStatsByDataLevel < NextT,
        CDataLevel,
        nextFileLevel > ::count,

        //! Hpw many variables for this data level
        count = (int)currentVariableCount + (int)nextVariableCount,
    };
};

template<class NextT, int CDataLevel>
struct VariablesStatsByDataLevel<NextT, CDataLevel, Category::FileLevel::maxFileLevel>
{
    enum
    {
        count = 0,
    };
};

template<class NextT, int CDataLevel = 1, int CFile = 1>
struct BrowseAllVariables
{
    enum
    {
        nextFileLevel = (CFile * 2 > (int)Category::FileLevel::maxFileLevel) ? 1 : CFile * 2,
        nextDataLevel = (CDataLevel * 2 > (int)Category::DataLevel::maxDataLevel) ? 1
                                                                                  : CDataLevel * 2,
        currentValue = NextT::template Statistics < CDataLevel,
        CFile > ::count,
        nextValue = BrowseAllVariables < NextT,
        nextDataLevel,
        nextFileLevel > ::maxValue,

        maxValue = (currentValue > (int)nextValue) ? currentValue : (int)nextValue,
    };

    template<class L, class S>
    static void buildSurveyResults(const L& list, S& results)
    {
        // Exporting data for the current state
        list.template buildSurveyResults<S, CDataLevel, CFile>(results);
        // Go to the next status
        BrowseAllVariables<NextT, nextDataLevel, nextFileLevel>::template buildSurveyResults<L, S>(
          list,
          results);
    }
};

template<class NextT>
struct BrowseAllVariables<NextT,
                          Category::DataLevel::maxDataLevel,
                          Category::FileLevel::maxFileLevel>
{
    enum
    {
        maxValue = NextT::template Statistics < Category::DataLevel::maxDataLevel,
        Category::FileLevel::maxFileLevel > ::count
    };

    template<class L, class S>
    static void buildSurveyResults(const L& list, S& results)
    {
        // Exporting data for the current state
        list.template buildSurveyResults<S,
                                         Category::DataLevel::maxDataLevel,
                                         Category::FileLevel::maxFileLevel>(results);
        // This is the final available state
    }
};

template<bool GlobalT, class NextT, int CDataLevel, int CFile = 1>
class SurveyReportBuilderFile
{
public:
    using ListType = NextT;

    enum
    {
        //! A non-zero value to write down the results for the simulation
        globalResults = (GlobalT) ? 1 : 0,
        //! The next level
        nextFileLevel = CFile * 2,
    };

    static void Run(const ListType& list, SurveyResults& results, unsigned int numSpace)
    {
        if (globalResults)
        {
            RunGlobalResults(list, results);
        }
        else
        {
            RunAnnual(list, results, numSpace);
        }

        // The survey type
        using SurveyRBFileType = SurveyReportBuilderFile<GlobalT, NextT, CDataLevel, nextFileLevel>;
        // Go to the next data level
        SurveyRBFileType::Run(list, results, numSpace);
    }

private:
    static void RunGlobalResults(const ListType& list, SurveyResults& results)
    {
        // All hours
        list.buildSurveyReport(results, CDataLevel, CFile, Category::hourly);
        // All days
        list.buildSurveyReport(results, CDataLevel, CFile, Category::daily);
        // All weeks
        list.buildSurveyReport(results, CDataLevel, CFile, Category::weekly);
        // All months
        list.buildSurveyReport(results, CDataLevel, CFile, Category::monthly);
        // All years
        list.buildSurveyReport(results, CDataLevel, CFile, Category::annual);
    }

    static void RunAnnual(const ListType& list, SurveyResults& results, unsigned int numSpace)
    {
        // All hours
        list.buildAnnualSurveyReport(results, CDataLevel, CFile, Category::hourly, numSpace);
        // All days
        list.buildAnnualSurveyReport(results, CDataLevel, CFile, Category::daily, numSpace);
        // All weeks
        list.buildAnnualSurveyReport(results, CDataLevel, CFile, Category::weekly, numSpace);
        // All months
        list.buildAnnualSurveyReport(results, CDataLevel, CFile, Category::monthly, numSpace);
        // All years
        list.buildAnnualSurveyReport(results, CDataLevel, CFile, Category::annual, numSpace);
    }

}; // class SurveyReportBuilderFile

// Specialization for the final state (dummy)
template<bool GlobalT, class NextT, int N>
class SurveyReportBuilderFile<GlobalT, NextT, N, 2 * Category::FileLevel::maxFileLevel>
{
public:
    using ListType = NextT;

    // dead end
    static inline void Run(const ListType&, SurveyResults&, unsigned int)
    {
    }
};

template<bool GlobalT, class NextT, int CDataLevel = 1>
class SurveyReportBuilder
{
public:
    //! List
    using ListType = NextT;

    enum
    {
        nextDataLevel = CDataLevel * 2,
    };

    static void Run(const ListType& list, SurveyResults& results, unsigned int numSpace = 9999)
    {
        // Area - Thermal clusters - Links
        if (CDataLevel & Category::DataLevel::area || CDataLevel & Category::DataLevel::link
            || CDataLevel & Category::DataLevel::thermalAggregate)
        {
            RunForEachArea(list, results, numSpace);
        }

        // Set of Areas
        if (CDataLevel & Category::DataLevel::setOfAreas)
        {
            RunForEachSetOfAreas(list, results, numSpace);
        }

        // Binding constraints level
        if (CDataLevel & Category::DataLevel::bindingConstraint)
        {
            RunForEachBindingConstraint(list, results, numSpace);
        }

        // Go to the next data level
        SurveyReportBuilder<GlobalT, NextT, nextDataLevel>::Run(list, results, numSpace);
    }

    static void RunDigest(const ListType& list, SurveyResults& results, IResultWriter& writer)
    {
        logs.info() << "Exporting digest...";
        logs.debug() << " . Digest, truncating file";
        // Digest: Summary for All years
        logs.debug() << " . Digest, annual";

        // Digest file : areas part
        std::string digestBuffer;
        list.buildDigest(results, Category::digestAllYears, Category::DataLevel::area);
        results.exportDigestAllYears(digestBuffer);

        // Degest file : districts part
        list.buildDigest(results, Category::digestAllYears, Category::DataLevel::setOfAreas);
        results.exportDigestAllYears(digestBuffer);

        // Digest: Flow linear (only if selected by user)
        if (results.data.study.parameters.variablesPrintInfo.isPrinted("FLOW LIN."))
        {
            logs.debug() << " . Digest, flow linear";
            results.data.matrix.fill(std::numeric_limits<double>::quiet_NaN());
            list.buildDigest(results, Category::digestFlowLinear, Category::DataLevel::area);
            results.exportDigestMatrix("Links (FLOW LIN.)", digestBuffer);
        }

        // Digest: Flow Quad (only if selected by user)
        if (results.data.study.parameters.variablesPrintInfo.isPrinted("FLOW QUAD."))
        {
            logs.debug() << " . Digest, flow quad";
            results.data.matrix.fill(std::numeric_limits<double>::quiet_NaN());
            list.buildDigest(results, Category::digestFlowQuad, Category::DataLevel::area);
            results.exportDigestMatrix("Links (FLOW QUAD.)", digestBuffer);
        }
        // THIS FILE IS DEPRECATED !!!
        YString digestFileName;
        digestFileName << results.data.originalOutput << "/grid/digest.txt";
        writer.addEntryFromBuffer(digestFileName.c_str(), digestBuffer);
    }

private:
    static void RunForEachArea(const ListType& list, SurveyResults& results, unsigned int numSpace)
    {
        using namespace Yuni;

        // No need to do anything for any area here if no zonal variables were selected.
        uint selectedZonalVarsCount = results.data.study.parameters.variablesPrintInfo
                                        .getNbSelectedZonalVars();

        // All values related to an area
        // Note: A thermal cluster is attached to an area
        auto end = results.data.study.areas.end();
        for (auto i = results.data.study.areas.begin(); i != end; ++i)
        {
            auto& area = *(i->second);
            // Alias to the current area
            results.data.area = &area;
            // No thermal cluster for now
            results.data.thermalCluster = nullptr;
            // No link for now
            results.data.link = nullptr;

            // Skipping the creation of a result directory if it is meant to be empty.
            // ... Getting few indicators value before deciding if we skip the results directory
            // creation.
            bool printingSynthesis = GlobalT; // Are we printing synthesis or year-by-year results ?
            bool filterAllYearByYear = !(area.filterYearByYear & Data::filterAll);
            bool filterAllSynthesis = !(area.filterSynthesis & Data::filterAll);

            // ... Do we skip the current area's result directory creation because no results were
            // asked
            //	   in the inspector for the current area ?
            bool skipDirectory = (!printingSynthesis && filterAllYearByYear)
                                 || (printingSynthesis && filterAllSynthesis);

            // ... Or do we skip the current area's result directory creation because no zonal
            //	   variables were selected ?
            skipDirectory = skipDirectory || !selectedZonalVarsCount;

            // Generating the report for each area
            if (CDataLevel & Category::DataLevel::area && !skipDirectory)
            {
                logs.info() << "Exporting results : " << area.name;
                // The new output
                fs::path path = results.data.originalOutput;
                path /= "areas";
                path /= area.id.to<std::string>();

                results.data.output = path.string();
                SurveyReportBuilderFile<GlobalT, NextT, CDataLevel>::Run(list, results, numSpace);
            }

            // Thermal clusters for the current area
            if (CDataLevel & Category::DataLevel::thermalAggregate)
            {
                RunForEachThermalCluster(list, results, numSpace);
            }
            // Links
            if (CDataLevel & Category::DataLevel::link && !area.links.empty())
            {
                RunForEachLink(list, results, numSpace);
            }
        }
    }

    static void RunForEachThermalCluster(const ListType& list,
                                         SurveyResults& results,
                                         unsigned int numSpace)
    {
        // Only do something if there is at least one column to write somewhere
        // See below: if (CDataLevel & Category::DataLevel::thermalAggregate)
        if (VariablesStatsByDataLevel<NextT, Category::DataLevel::thermalAggregate>::count)
        {
            auto& area = *results.data.area;
            for (auto& cluster: area.thermal.list.each_enabled_and_not_mustrun())
            {
                results.data.thermalCluster = cluster.get();

                logs.info() << "Exporting results : " << area.name << " :: " << cluster->name();
                // The new output
                fs::path path = results.data.originalOutput;
                path /= fs::path("areas") / area.id.to<std::string>() / "thermal" / cluster->id();

                results.data.output = path.string();

                SurveyReportBuilderFile<GlobalT, NextT, CDataLevel>::Run(list, results, numSpace);
            }
        }
    }

    static void RunForEachLink(const ListType& list, SurveyResults& results, unsigned int numSpace)
    {
        using namespace Yuni;

        // No need to do anything for any link here if no link variables were selected.
        uint selectedLinkVarsCount = results.data.study.parameters.variablesPrintInfo
                                       .getNbSelectedLinkVars();
        if (!selectedLinkVarsCount)
        {
            return;
        }

        int count_int = VariablesStatsByDataLevel<NextT, Category::DataLevel::link>::count;
        if (count_int)
        {
            auto& area = *results.data.area;
            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
            {
                // The link
                auto& link = *(i->second);
                results.data.link = &link;

                // Skipping the creation of a result directory if it is meant to be empty.
                // ... Getting few indicators value before deciding if we skip the results directory
                // creation.
                bool printingSynthesis = GlobalT; // Are we printing synthesis or year-by-year
                                                  // results ?
                bool filterAllYearByYear = !(link.filterYearByYear & Data::filterAll);
                bool filterAllSynthesis = !(link.filterSynthesis & Data::filterAll);

                // ... Do we skip the current link's result directory creation because no results
                // were asked
                //	   in the inspector for the current link ?
                bool skipDirectory = (!printingSynthesis && filterAllYearByYear)
                                     || (printingSynthesis && filterAllSynthesis);

                if (!skipDirectory)
                {
                    Antares::logs.info() << "Exporting results : " << area.name << " - "
                                         << results.data.link->with->name;
                    // The new output
                    results.data.output.clear();
                    results.data.output << results.data.originalOutput << SEP << "links" << SEP
                                        << area.id << " - " << results.data.link->with->id;
                    SurveyReportBuilderFile<GlobalT, NextT, CDataLevel>::Run(list,
                                                                             results,
                                                                             numSpace);
                }
            }
        }
    }

    static void RunForEachSetOfAreas(const ListType& list,
                                     SurveyResults& results,
                                     unsigned int numSpace)
    {
        using namespace ::Antares;
        using namespace ::Yuni;

        // No need to do anything for any district (set of areas) here if no zonal variables were
        // selected.
        uint selectedZonalVarsCount = results.data.study.parameters.variablesPrintInfo
                                        .getNbSelectedZonalVars();
        if (!selectedZonalVarsCount)
        {
            return;
        }

        results.data.area = nullptr;
        results.data.thermalCluster = nullptr;
        results.data.link = nullptr;

        // alias to the set of sets of areas
        const Data::Study::SetsOfAreas& sets = results.data.study.setsOfAreas;

        unsigned int indx = 0;
        for (unsigned int i = 0; i != sets.size(); ++i)
        {
            if (!sets.hasOutput(i) || !sets.resultSize(i))
            {
                continue;
            }

            logs.info() << "Exporting results : " << sets.caption(i);
            // The new output
            results.data.output.clear();
            results.data.output << results.data.originalOutput << SEP << "areas" << SEP << "@ "
                                << sets.nameByIndex(i);

            results.data.setOfAreasIndex = indx++;
            SurveyReportBuilderFile<GlobalT, NextT, CDataLevel>::Run(list, results, numSpace);
        }
    }

    static void RunForEachBindingConstraint(const ListType& list,
                                            SurveyResults& results,
                                            unsigned int numSpace)
    {
        using namespace Yuni;

        // Generating the report for each binding constraint
        if (CDataLevel & Category::DataLevel::bindingConstraint)
        {
            logs.info() << "Exporting results : binding constraints";
            // The new output
            results.data.output.clear();
            results.data.output << results.data.originalOutput << SEP << "binding_constraints";
            SurveyReportBuilderFile<GlobalT, NextT, CDataLevel>::Run(list, results, numSpace);
        }
    }

}; // class SurveyReportBuilder

template<bool GlobalT, class NextT>
class SurveyReportBuilder<GlobalT, NextT, 2 * Category::DataLevel::maxDataLevel>
{
public:
    using ListType = NextT;

    // Dead end
    static void Run(const ListType&, SurveyResults&, unsigned int)
    {
    }
};

} // Anonymous namespace

} // namespace Container
} // namespace Variable
} // namespace Solver
} // namespace Antares

// cleanup
#undef SEP

#endif // __SOLVER_VARIABLE_SURVEYRESULTS_REPORT_BUILDER_HXX__
