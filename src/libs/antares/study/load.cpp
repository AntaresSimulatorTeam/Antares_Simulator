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
#include <fstream>
#include "study.h"
#include "../benchmarking.h"
#include "../array/correlation.h"
#include "../inifile/inifile.h"
#include "scenario-builder/sets.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{

constexpr int MATRIX_BUFFER_4MB = 4 * 1024 * 1024;

bool Study::internalLoadHeader(const String& path)
{
    // Header
    buffer.clear() << path << SEP << "study.antares";
    if (not header.loadFromFile(buffer))
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

bool Study::internalLoadFromFolder(const String& path, const StudyLoadOptions& options)
{
    // IO statistics
    Statistics::LogsDumper statisticsDumper;

    gotFatalError = false;

    // Check if the path is correct
    if (!IO::Directory::Exists(path))
    {
        logs.error()
          << path << ": The directory does not exist (or not enough privileges to read the folder)";
        return false;
    }

    if (not internalLoadHeader(path))
    {
        if (options.loadOnlyNeeded)
            return false;
    }

    // Initialize all internal paths
    relocate(path);

    // Compatibility - The extension according the study version
    inputExtensionCompatibility();

    // Reserving enough space in buffer to avoid several calls to realloc
    this->dataBuffer.reserve(MATRIX_BUFFER_4MB); // For matrices, reserving 4Mo
    this->bufferLoadingTS.reserve(2096);
    assert(this->bufferLoadingTS.capacity() > 0);

    // The simulation settings
    if (not simulation.loadFromFolder(options))
    {
        if (options.loadOnlyNeeded)
            return false;
    }
    // Load the general data
    buffer.clear() << folderSettings << SEP << "generaldata.ini";
    if (not parameters.loadFromFile(buffer, header.version, options))
    {
        if (options.loadOnlyNeeded)
            return false;
    }

    // Load the layer data
    buffer.clear() << path << SEP << "layers" << SEP << "layers.ini";
    loadLayers(buffer);


    // This settings can only be enabled from the solver
    // Prepare the output for the study
    if (not prepareOutput()) // will abort early if not usedByTheSolver
        return false;

    // -------------------------
    // Logical cores
    // -------------------------
    // Getting the number of logical cores to use before loading and creating the areas :
    // Areas need this number to be up-to-date at construction.
    computeNumberOfCores(options.forceParallel, options.maxNbYearsInParallel);

#ifdef ANTARES_SWAP_SUPPORT
    // In case of swap support, MC years parallel computing is forbidden.
    // Numbers of parallel years is set to 1.
    maxNbYearsInParallel = 1;
#endif

    // In case the study is run in the draft mode, only 1 core is allowed
    if (parameters.mode == Data::stdmAdequacyDraft)
        maxNbYearsInParallel = 1;

    // In case parallel mode was not chosen, only 1 core is allowed
    if (!options.enableParallel && !options.forceParallel)
        maxNbYearsInParallel = 1;

    // End logical core --------

    // Areas - Raw Data
    bool ret = areas.loadFromFolder(options);

    logs.info() << "Loading correlation matrices...";
    // Correlation matrices
    ret = internalLoadCorrelationMatrices(options) and ret;
    // Binding constraints
    ret = internalLoadBindingConstraints(options) and ret;
    // Sets of areas & links
    ret = internalLoadSets() and ret;

    // Scenario Rules sets, only available since v3.6
    // After two consecutive load, some scenario builder data
    // may still exist.
    scenarioRulesDestroy();

    if (JIT::usedFromGUI and uiinfo)
    {
        // Post-processing when loaded from the User-Interface
        uiinfo->reload();
        uiinfo->reloadBindingConstraints();
    }

    if (usedByTheSolver and options.prepareOutput)
    {
        // Write all available areas as a reminder
        {
            buffer.clear() << folderOutput << SEP << "about-the-study" << SEP << "areas.txt";
            IO::File::Stream file;
            if (file.openRW(buffer))
            {
                for (auto i = setsOfAreas.begin(); i != setsOfAreas.end(); ++i)
                {
                    if (setsOfAreas.hasOutput(i->first))
                        file << "@ " << i->first << "\r\n";
                }
                areas.each([&](const Data::Area& area) { file << area.name << "\r\n"; });
            }
            else
                logs.error() << "impossible to write " << buffer;
        }

        // Write all available links as a reminder
        buffer.clear() << folderOutput << SEP << "about-the-study" << SEP << "links.txt";
        if (not areas.saveLinkListToFile(buffer))
        {
            logs.error() << "impossible to write " << buffer;
            return false;
        }
    }

    return initializeInternalData(options);
}

void Study::inputExtensionCompatibility()
{
    // The extension to use according the version
    if (header.version < 310)
    {
        // Since v3.1, all extensions have been changed into .txt in the input folder.
        inputExtension = "csv";
        // In order to properly save the study from the interface, the Just-In-Time
        // mecanism must be temporary disabled
        JIT::enabled = 0;
    }
    else
        inputExtension = "txt";
}

bool Study::internalLoadCorrelationMatrices(const StudyLoadOptions& options)
{
    if ((uint)header.version > (uint)version320)
    {
        // Load
        if (!options.loadOnlyNeeded or timeSeriesLoad & parameters.timeSeriesToRefresh
            || timeSeriesLoad & parameters.timeSeriesToGenerate)
        {
            buffer.clear() << folderInput << SEP << "load" << SEP << "prepro" << SEP
                           << "correlation.ini";
            preproLoadCorrelation.loadFromFile(*this, buffer);
        }

        // Solar
        if (!options.loadOnlyNeeded or timeSeriesSolar & parameters.timeSeriesToRefresh
            || timeSeriesSolar & parameters.timeSeriesToGenerate)
        {
            buffer.clear() << folderInput << SEP << "solar" << SEP << "prepro" << SEP
                           << "correlation.ini";
            preproSolarCorrelation.loadFromFile(*this, buffer);
        }
    }
    else
    {
        preproLoadCorrelation.reset(*this);
        preproSolarCorrelation.reset(*this);
    }

    // Wind
    {
        if (!options.loadOnlyNeeded or timeSeriesWind & parameters.timeSeriesToRefresh
            || timeSeriesWind & parameters.timeSeriesToGenerate)
        {
            buffer.clear() << folderInput << SEP << "wind" << SEP << "prepro" << SEP
                           << "correlation.ini";
            preproWindCorrelation.loadFromFile(*this, buffer);
        }
    }

    // Hydro
    {
        if (not options.loadOnlyNeeded or (timeSeriesHydro & parameters.timeSeriesToRefresh)
            or (timeSeriesHydro & parameters.timeSeriesToGenerate))
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
    const bool r = bindingConstraints.loadFromFolder(*this, options, buffer);
    return (!r and options.loadOnlyNeeded) ? false : r;
}

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
        initializeSetsData();
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
        ret = area.load.prepro->loadFromFolder(*this, buffer) and ret;
        // Solar
        buffer.clear() << folderInput << SEP << "solar" << SEP << "prepro" << SEP << area.id;
        ret = area.solar.prepro->loadFromFolder(*this, buffer) and ret;
        // Wind
        buffer.clear() << folderInput << SEP << "wind" << SEP << "prepro" << SEP << area.id;
        ret = area.wind.prepro->loadFromFolder(*this, buffer) and ret;
    });
    return ret;
}

} // namespace Data
} // namespace Antares
