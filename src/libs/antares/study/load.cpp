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
#include <fstream>

#include <antares/benchmarking/DurationCollector.h>
#include "antares/study/scenario-builder/sets.h"
#include "antares/study/study.h"
#include "antares/study/ui-runtimeinfos.h"
#include "antares/study/version.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace fs = std::filesystem;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
bool Study::internalLoadHeader(const fs::path& path)
{
    // Header
    auto headerPath = path / "study.antares";
    if (!header.loadFromFile(headerPath))
    {
        logs.error() << path << ": impossible to open the header file";
        return false;
    }

    // Informations about the study
    logs.info();
    logs.notice() << "Preparing " << header.caption << "...";
    logs.info() << "  detected study version: " << header.version.toString();
    logs.info() << "  from `" << path << '`';
    logs.info();
    return true;
}

bool Study::loadFromFolder(const std::string& path, const StudyLoadOptions& options)
{
    fs::path normPath = path;
    normPath = normPath.lexically_normal();
    return internalLoadFromFolder(normPath, options);
}

bool Study::internalLoadIni(const fs::path& path, const StudyLoadOptions& options)
{
    if (!internalLoadHeader(path))
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }

    // The simulation settings
    if (!simulationComments.loadFromFolder(options))
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }
    // Load the general data
    fs::path generalDataPath = folderSettings / "generaldata.ini";
    bool errorWhileLoading = !parameters.loadFromFile(generalDataPath, header.version);

    parameters.validateOptions(options);

    parameters.fixBadValues();

    if (errorWhileLoading)
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }

    // Load the layer data
    fs::path layersPath = path / "layers" / "layers.ini";
    loadLayers(layersPath);

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
    {
        // Nothing to refresh
        parameters.timeSeriesToRefresh = 0;
    }

    // We can not run the simulation if the study folder is not in the latest
    // version and that we would like to re-importe the generated timeseries
    if (usedByTheSolver)
    {
        // We have time-series to import
        if (parameters.exportTimeSeriesInInput && header.version != StudyVersion::latest())
        {
            logs.info() << "Stochastic TS stored in input parametrized."
                           " Disabling Store in input because study is not at latest version."
                           " This prevents writing data in unsupported format at the study version";
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
    {
        calendar.reset({parameters.dayOfThe1stJanuary,
                        parameters.firstWeekday,
                        parameters.firstMonthInYear,
                        false});
    }
    else
    {
        calendar.reset({parameters.dayOfThe1stJanuary,
                        parameters.firstWeekday,
                        parameters.firstMonthInYear,
                        parameters.leapYear});
    }

    calendarOutput.reset({parameters.dayOfThe1stJanuary,
                          parameters.firstWeekday,
                          parameters.firstMonthInYear,
                          parameters.leapYear});

    // Reducing memory footprint
    reduceMemoryUsage();
}

bool Study::internalLoadFromFolder(const fs::path& path, const StudyLoadOptions& options)
{
    // IO statistics
    Statistics::LogsDumper statisticsDumper;

    // Check if the path is correct
    if (!fs::exists(path))
    {
        logs.error()
          << path << ": The directory does not exist (or not enough privileges to read the folder)";
        return false;
    }

    // Initialize all internal paths
    relocate(path.string());

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
    {
        maxNbYearsInParallel = 1;
    }

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
        fs::path loadPath = folderInput / "load" / "prepro" / "correlation.ini";
        preproLoadCorrelation.loadFromFile(*this, loadPath.string());
    }

    // Solar
    if (!options.loadOnlyNeeded || timeSeriesSolar & parameters.timeSeriesToRefresh
        || timeSeriesSolar & parameters.timeSeriesToGenerate)
    {
        fs::path solarPath = folderInput / "solar" / "prepro" / "correlation.ini";
        preproSolarCorrelation.loadFromFile(*this, solarPath.string());
    }

    // Wind
    if (!options.loadOnlyNeeded || timeSeriesWind & parameters.timeSeriesToRefresh
        || timeSeriesWind & parameters.timeSeriesToGenerate)
    {
        fs::path windPath = folderInput / "wind" / "prepro" / "correlation.ini";
        preproWindCorrelation.loadFromFile(*this, windPath.string());
    }

    // Hydro
    if (!options.loadOnlyNeeded || (timeSeriesHydro & parameters.timeSeriesToRefresh)
        || (timeSeriesHydro & parameters.timeSeriesToGenerate))
    {
        fs::path hydroPath = folderInput / "hydro" / "prepro" / "correlation.ini";
        preproHydroCorrelation.loadFromFile(*this, hydroPath.string());
    }
    return true;
}

bool Study::internalLoadBindingConstraints(const StudyLoadOptions& options)
{
    // All checks are performed in 'loadFromFolder'
    // (actually internalLoadFromFolder)
    fs::path constraintPath = folderInput / "bindingconstraints";
    bool r = bindingConstraints.loadFromFolder(*this, options, buffer);
    if (r)
    {
        r &= bindingConstraintsGroups.buildFrom(bindingConstraints);
    }
    return (!r && options.loadOnlyNeeded) ? false : r;
}

bool Study::internalLoadSets()
{
    const fs::path path = fs::path(folderInput.c_str()) / "areas" / "sets.ini";
    // Set of areas
    logs.info();
    logs.info() << "Loading sets of areas...";

    // filename
    buffer.clear() << folderInput << SEP << "areas" << SEP << "sets.ini";

    // Load the rules
    if (setsOfAreas.loadFromFile(path))
    {
        // Apply the rules
        SetHandlerAreas handler(areas);
        setsOfAreas.rebuildAllFromRules(handler);
        // Write the results into the logs
        setsOfAreas.dumpToLogs();
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
    areas.each(
      [this, &ret](Area& area)
      {
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
