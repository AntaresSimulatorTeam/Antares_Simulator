/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#include <fstream>
#include "study.h"
#include "version.h"
#include <antares/benchmarking/DurationCollector.h>
#include "scenario-builder/sets.h"
#include "ui-runtimeinfos.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
bool Study::internalLoadHeader(const String& path)
{
    // Header
    buffer.clear() << path << SEP << "study.antares";
    if (!header.loadFromFile(buffer))
    {
        logs.error() << path << ": impossible to open the header file";
        return false;
    }

    // Informations about the study
    logs.info();
    logs.notice() << "Preparing " << header.caption << "...";
    logs.info() << "  detected version: " << header.version;
    logs.info() << "  from `" << path << '`';
    logs.info();
    return true;
}

bool Study::loadFromFolder(const AnyString& path, const StudyLoadOptions& options)
{
    String normPath;
    IO::Normalize(normPath, path);
    return internalLoadFromFolder(normPath, options);
}

bool Study::internalLoadIni(const String& path, const StudyLoadOptions& options)
{
    if (!internalLoadHeader(path))
    {
        if (options.loadOnlyNeeded)
            return false;
    }

    // The simulation settings
    if (!simulationComments.loadFromFolder(options))
    {
        if (options.loadOnlyNeeded)
            return false;
    }
    // Load the general data
    buffer.clear() << folderSettings << SEP << "generaldata.ini";
    if (!parameters.loadFromFile(buffer, header.version, options))
    {
        if (options.loadOnlyNeeded)
            return false;
    }

    // Load the layer data
    buffer.clear() << path << SEP << "layers" << SEP << "layers.ini";
    loadLayers(buffer);

    return true;
}

void Study::parameterFiller(const StudyLoadOptions& options)
{
    if (usedByTheSolver && !options.prepareOutput)
    {
        parameters.noOutput = true;
        parameters.yearByYear = false;
        parameters.timeSeriesToArchive = 0;
        parameters.storeTimeseriesNumbers = false;
        parameters.synthesis = false;
    }

    if (options.loadOnlyNeeded && !parameters.timeSeriesToGenerate)
        // Nothing to refresh
        parameters.timeSeriesToRefresh = 0;

    // We can not run the simulation if the study folder is not in the latest
    // version and that we would like to re-importe the generated timeseries
    if (usedByTheSolver)
    {
        // We have time-series to import
        StudyVersion studyVersion;
        if (parameters.exportTimeSeriesInInput && studyVersion.isStudyLatestVersion(folder.c_str()))
        {
            logs.info() << "Stochastic TS stored in input parametrized."
                           " Disabling Store in input because study is not at latest version"
                           "Prevents writing data in unsupported format at the study version";
            parameters.exportTimeSeriesInInput = 0;
        }
    }

    // This settings can only be enabled from the solver
    // Prepare the output for the study
    prepareOutput(); // will abort early if not usedByTheSolver

    // Scenario Rules sets, only available since v3.6
    // After two consecutive load, some scenario builder data
    // may still exist.
    scenarioRulesDestroy();

    if (JIT::usedFromGUI && uiinfo)
    {
        // Post-processing when loaded from the User-Interface
        uiinfo->reload();
        uiinfo->reloadBindingConstraints();
    }

    // calendar update
    if (usedByTheSolver)
        calendar.reset({parameters.dayOfThe1stJanuary, parameters.firstWeekday, parameters.firstMonthInYear, false});
    else
        calendar.reset({parameters.dayOfThe1stJanuary, parameters.firstWeekday, parameters.firstMonthInYear, parameters.leapYear});

    calendarOutput.reset({parameters.dayOfThe1stJanuary, parameters.firstWeekday, parameters.firstMonthInYear, parameters.leapYear});

    // In case hydro hot start is enabled, check all conditions are met.
    // (has to be called after areas load and calendar building)
    if (usedByTheSolver && !checkHydroHotStart())
        logs.error() << "hydro hot start is enabled, conditions are not met. Aborting";

    // Reducing memory footprint
    reduceMemoryUsage();
}

bool Study::internalLoadFromFolder(const String& path, const StudyLoadOptions& options)
{
    // IO statistics
    Statistics::LogsDumper statisticsDumper;

    // Check if the path is correct
    if (!IO::Directory::Exists(path))
    {
        logs.error()
          << path << ": The directory does not exist (or not enough privileges to read the folder)";
        return false;
    }

    // Initialize all internal paths
    relocate(path);

    // Reserving enough space in buffer to avoid several calls to realloc
    this->dataBuffer.reserve(4 * 1024 * 1024); // For matrices, reserving 4Mo
    this->bufferLoadingTS.reserve(2096);
    assert(this->bufferLoadingTS.capacity() > 0);

    if (!internalLoadIni(path, options))
    {
        return false;
    }

    // -------------------------
    // Logical cores
    // -------------------------
    // Getting the number of logical cores to use before loading and creating the areas :
    // Areas need this number to be up-to-date at construction.
    getNumberOfCores(options.forceParallel, options.maxNbYearsInParallel);

    // In case parallel mode was not chosen, only 1 core is allowed
    if (!options.enableParallel && !options.forceParallel)
        maxNbYearsInParallel = 1;

    // End logical core --------

    // Areas - Raw Data
    bool ret = areas.loadFromFolder(options);

    logs.info() << "Loading correlation matrices...";
    // Correlation matrices
    ret = internalLoadCorrelationMatrices(options) && ret;
    // Binding constraints
    ret = internalLoadBindingConstraints(options) && ret;
    // Sets of areas & links
    ret = internalLoadSets() && ret;

    parameterFiller(options);
    return ret;
}

bool Study::internalLoadCorrelationMatrices(const StudyLoadOptions& options)
{
    // Load
    if (!options.loadOnlyNeeded || timeSeriesLoad & parameters.timeSeriesToRefresh
            || timeSeriesLoad & parameters.timeSeriesToGenerate)
    {
        buffer.clear() << folderInput << SEP << "load" << SEP << "prepro" << SEP
            << "correlation.ini";
        preproLoadCorrelation.loadFromFile(*this, buffer);
    }

    // Solar
    if (!options.loadOnlyNeeded || timeSeriesSolar & parameters.timeSeriesToRefresh
            || timeSeriesSolar & parameters.timeSeriesToGenerate)
    {
        buffer.clear() << folderInput << SEP << "solar" << SEP << "prepro" << SEP
            << "correlation.ini";
        preproSolarCorrelation.loadFromFile(*this, buffer);
    }

    // Wind
    {
        if (!options.loadOnlyNeeded || timeSeriesWind & parameters.timeSeriesToRefresh
            || timeSeriesWind & parameters.timeSeriesToGenerate)
        {
            buffer.clear() << folderInput << SEP << "wind" << SEP << "prepro" << SEP
                           << "correlation.ini";
            preproWindCorrelation.loadFromFile(*this, buffer);
        }
    }

    // Hydro
    {
        if (!options.loadOnlyNeeded || (timeSeriesHydro & parameters.timeSeriesToRefresh)
            || (timeSeriesHydro & parameters.timeSeriesToGenerate))
        {
            buffer.clear() << folderInput << SEP << "hydro" << SEP << "prepro" << SEP
                           << "correlation.ini";
            preproHydroCorrelation.loadFromFile(*this, buffer);
        }
    }
    return true;
}

bool Study::internalLoadBindingConstraints(const StudyLoadOptions& options)
{
    // All checks are performed in 'loadFromFolder'
    // (actually internalLoadFromFolder)
    buffer.clear() << folderInput << SEP << "bindingconstraints";
    bool r = bindingConstraints.loadFromFolder(*this, options, buffer);
    if (r) {
        r &= bindingConstraintsGroups.buildFrom(bindingConstraints);
    }
    return (!r && options.loadOnlyNeeded) ? false : r;
}

class SetHandlerAreas
{
public:
    SetHandlerAreas(Study& study) : pStudy(study)
    {
    }

    void clear(Study::SingleSetOfAreas& set)
    {
        set.clear();
    }

    uint size(Study::SingleSetOfAreas& set)
    {
        return (uint)set.size();
    }

    bool add(Study::SingleSetOfAreas& set, const String& value)
    {
        Area* area = AreaListLFind(&pStudy.areas, value.c_str());
        if (area)
        {
            set.insert(area);
            return true;
        }
        return false;
    }

    bool add(Study::SingleSetOfAreas& set, const Study::SingleSetOfAreas& otherSet)
    {
        if (!otherSet.empty())
        {
            auto end = otherSet.end();
            for (auto i = otherSet.begin(); i != end; ++i)
                set.insert(*i);
        }
        return true;
    }

    bool remove(Study::SingleSetOfAreas& set, const String& value)
    {
        Area* area = AreaListLFind(&pStudy.areas, value.c_str());
        if (area)
        {
            set.erase(area);
            return true;
        }
        return false;
    }

    bool remove(Study::SingleSetOfAreas& set, const Study::SingleSetOfAreas& otherSet)
    {
        if (!otherSet.empty())
        {
            auto end = otherSet.end();
            for (auto i = otherSet.begin(); i != end; ++i)
            {
                set.erase(*i);
            }
        }
        return true;
    }

    bool applyFilter(Study::SingleSetOfAreas& set, const String& value)
    {
        if (value == "add-all")
        {
            auto end = pStudy.areas.end();
            for (auto i = pStudy.areas.begin(); i != end; ++i)
                set.insert(i->second);
            return true;
        }

        if (value == "remove-all")
        {
            set.clear();
            return true;
        }
        return false;
    }

private:
    Study& pStudy;

}; // class SetHandlerAreas

bool Study::internalLoadSets()
{
    // Set of areas
    logs.info();
    logs.info() << "Loading sets of areas...";

    // filename
    buffer.clear() << folderInput << SEP << "areas" << SEP << "sets.ini";

    // Load the rules
    if (setsOfAreas.loadFromFile(buffer))
    {
        // Apply the rules
        SetHandlerAreas handler(*this);
        setsOfAreas.rebuildAllFromRules(handler);
        // Write the results into the logs
        setsOfAreas.dumpToLogs(logs);
        return true;
    }

    logs.warning() << "Impossible to load the sets of areas";
    return false;
}

void Study::reloadCorrelation()
{
    StudyLoadOptions options;
    options.loadOnlyNeeded = false;
    internalLoadCorrelationMatrices(options);
}

bool Study::reloadXCastData()
{
    // if changes are required, please update AreaListLoadFromFolderSingleArea()
    bool ret = true;
    areas.each([&](Data::Area& area) {
        assert(area.load.prepro);
        assert(area.solar.prepro);
        assert(area.wind.prepro);

        // Load
        buffer.clear() << folderInput << SEP << "load" << SEP << "prepro" << SEP << area.id;
        ret = area.load.prepro->loadFromFolder(buffer) && ret;
        // Solar
        buffer.clear() << folderInput << SEP << "solar" << SEP << "prepro" << SEP << area.id;
        ret = area.solar.prepro->loadFromFolder(buffer) && ret;
        // Wind
        buffer.clear() << folderInput << SEP << "wind" << SEP << "prepro" << SEP << area.id;
        ret = area.wind.prepro->loadFromFolder(buffer) && ret;
    });
    return ret;
}

} // namespace Data
} // namespace Antares
