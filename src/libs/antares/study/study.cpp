/*
** Copyright 2007-2023 RTE
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

#include "../antares.h"
#include <yuni/yuni.h>
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/string.h>
#include <yuni/io/file.h>
#include <yuni/datetime/timestamp.h>

#include <sstream> // std::ostringstream
#include <cassert>
#include <climits>
#include <optional>

#include "study.h"
#include "runtime.h"
#include "scenario-builder/sets.h"
#include "correlation-updater.hxx"
#include "scenario-builder/updater.hxx"
#include "area/constants.h"

#include <yuni/core/system/cpu.h> // For use of Yuni::System::CPU::Count()
#include <cmath>                 // For use of floor(...) and ceil(...)
#include <antares/writer/writer_factory.h>
#include "ui-runtimeinfos.h"

using namespace Yuni;


namespace Antares::Data
{
//! Clear then shrink a string
template<class StringT>
static inline void ClearAndShrink(StringT& string)
{
    string.clear();
    string.shrink();
}

template<class T>
static inline void FreeAndNil(T*& pointer)
{
    delete pointer;
    pointer = nullptr;
}

Study::Study(bool forTheSolver) :
 LayerData(0, true),
 simulationComments(*this),
 areas(*this),
 pQueueService(std::make_shared<Yuni::Job::QueueService>()),
 usedByTheSolver(forTheSolver)
{
    // TS generators
    for (uint i = 0; i != timeSeriesCount; ++i)
        cacheTSGenerator[i] = nullptr;

    // Correlation names
    preproLoadCorrelation.correlationName = "Correlation: Load";
    preproSolarCorrelation.correlationName = "Correlation: Solar";
    preproWindCorrelation.correlationName = "Correlation: Wind";
    preproHydroCorrelation.correlationName = "Correlation: Hydro";
    // TS
    preproLoadCorrelation.timeSeries = timeSeriesLoad;
    preproSolarCorrelation.timeSeries = timeSeriesSolar;
    preproWindCorrelation.timeSeries = timeSeriesWind;
    preproHydroCorrelation.timeSeries = timeSeriesHydro;

    // Data related to the GUI
    if (JIT::usedFromGUI)
    {
        uiinfo = new UIRuntimeInfo(*this);
        uiinfo->reloadAll();
    }
}

Study::~Study()
{
    clear();
}

void Study::clear()
{
    // Releasing runtime infos
    FreeAndNil(runtime);
    FreeAndNil(scenarioRules);
    FreeAndNil(uiinfo);

    // areas
    setsOfAreas.clear();
    setsOfLinks.clear();

    preproLoadCorrelation.clear();
    preproSolarCorrelation.clear();
    preproWindCorrelation.clear();
    preproHydroCorrelation.clear();

    bindingConstraints.clear();
    bindingConstraintsGroups.clear();
    areas.clear();

    // maintenance Group-s
    maintenanceGroups.clear();

    // no folder
    ClearAndShrink(header.caption);
    ClearAndShrink(header.author);
    ClearAndShrink(folder);
    ClearAndShrink(folderInput);
    ClearAndShrink(folderOutput);
    ClearAndShrink(folderSettings);
    inputExtension.clear();

    gotFatalError = false;
}

void Study::createAsNew()
{
    inputExtension = "txt";
    // Folders
    folder.clear();
    folderInput.clear();
    folderOutput.clear();
    folderSettings.clear();

    // Simulations
    parameters.reset();
    // ... At study creation, renewable cluster is the default mode for RES (Renewable Energy
    // Source)
    parameters.renewableGeneration.rgModelling = Antares::Data::rgClusters;
    // default mode for maintenancePlanning is Randomized 
    parameters.maintenancePlanning.clear();

    parameters.yearsFilter = std::vector<bool>(1, true);

    // Sets
    setsOfAreas.defaultForAreas();
    setsOfLinks.clear();
    setsOfAreas.markAsModified();
    setsOfLinks.markAsModified();

    // Binding constraints
    bindingConstraints.clear();

    // maintenance Group-s
    maintenanceGroups.clear();

    // Areas
    areas.clear();

    // Correlation
    preproLoadCorrelation.reset(*this);
    preproSolarCorrelation.reset(*this);
    preproWindCorrelation.reset(*this);
    preproHydroCorrelation.reset(*this);

    // Scenario Builder
    scenarioRulesDestroy();

    // Cache
    if (JIT::usedFromGUI)
    {
        if (not uiinfo)
            uiinfo = new UIRuntimeInfo(*this);
        uiinfo->reloadAll();
    }
    // Reduce memory footprint
    reduceMemoryUsage();
}

void Study::reduceMemoryUsage()
{
    ClearAndShrink(buffer);
    ClearAndShrink(dataBuffer);
    ClearAndShrink(bufferLoadingTS);
}

uint64_t Study::memoryUsage() const
{
    return folder.capacity()
           // Folders paths
           + folderInput.capacity() + folderOutput.capacity() + folderSettings.capacity()
           + buffer.capacity() + dataBuffer.capacity()
           + bufferLoadingTS.capacity()
           // Simulation
           + simulationComments.memoryUsage()
           // parameters
           + parameters.memoryUsage()
           // Areas
           + areas.memoryUsage()
           // Binding constraints
           + bindingConstraints.memoryUsage()
           // maintenance Group-s
           + maintenanceGroups.memoryUsage()
           // Correlations matrices
           + preproLoadCorrelation.memoryUsage() + preproSolarCorrelation.memoryUsage()
           + preproHydroCorrelation.memoryUsage() + preproWindCorrelation.memoryUsage()
           + (uiinfo ? uiinfo->memoryUsage() : 0);
}

std::map<std::string, uint> Study::getRawNumberCoresPerLevel()
{
    std::map<std::string, uint> table;

    uint nbLogicalCores = Yuni::System::CPU::Count();
    if (!nbLogicalCores)
        logs.fatal() << "Number of logical cores available is 0.";

    switch (nbLogicalCores)
    {
    case 1:
        table["min"] = 1;
        table["low"] = 1;
        table["med"] = 1;
        table["high"] = 1;
        table["max"] = 1;
        break;
    case 2:
        table["min"] = 1;
        table["low"] = 1;
        table["med"] = 1;
        table["high"] = 2;
        table["max"] = 2;
        break;
    case 3:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 2;
        table["high"] = 2;
        table["max"] = 3;
        break;
    case 4:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 2;
        table["high"] = 3;
        table["max"] = 4;
        break;
    case 5:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 3;
        table["high"] = 4;
        table["max"] = 5;
        break;
    case 6:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 3;
        table["high"] = 4;
        table["max"] = 6;
        break;
    case 7:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 3;
        table["high"] = 5;
        table["max"] = 7;
        break;
    case 8:
        table["min"] = 1;
        table["low"] = 2;
        table["med"] = 4;
        table["high"] = 6;
        table["max"] = 8;
        break;
    case 9:
        table["min"] = 1;
        table["low"] = 3;
        table["med"] = 5;
        table["high"] = 7;
        table["max"] = 8;
        break;
    case 10:
        table["min"] = 1;
        table["low"] = 3;
        table["med"] = 5;
        table["high"] = 8;
        table["max"] = 9;
        break;
    case 11:
        table["min"] = 1;
        table["low"] = 3;
        table["med"] = 6;
        table["high"] = 8;
        table["max"] = 10;
        break;
    case 12:
        table["min"] = 1;
        table["low"] = 3;
        table["med"] = 6;
        table["high"] = 9;
        table["max"] = 11;
        break;
    default:
        table["min"] = 1;
        table["low"] = (uint)std::ceil(nbLogicalCores / 4.);
        table["med"] = (uint)std::ceil(nbLogicalCores / 2.);
        table["high"] = (uint)std::ceil(3 * nbLogicalCores / 4.);
        table["max"] = nbLogicalCores - 1;
        break;
    }

    return table;
}

void Study::getNumberOfCores(const bool forceParallel, const uint nbYearsParallelForced)
{
    /*
            Getting the number of parallel years based on the number
            of cores level.
            This number is limited by the smallest refresh span (if at least
            one type of time series is generated)
    */

    std::map<std::string, uint> table = getRawNumberCoresPerLevel();

    // Getting the number of parallel years based on the number of cores level.
    switch (parameters.nbCores.ncMode)
    {
    case ncMin:
        nbYearsParallelRaw = table["min"];
        break;
    case ncLow:
        nbYearsParallelRaw = table["low"];
        break;
    case ncAvg:
        nbYearsParallelRaw = table["med"];
        break;
    case ncHigh:
        nbYearsParallelRaw = table["high"];
        break;
    case ncMax:
        nbYearsParallelRaw = table["max"];
        break;
    default:
        logs.fatal() << "Simulation cores level not correct : " << (int)parameters.nbCores.ncMode;
        break;
    }

    maxNbYearsInParallel = nbYearsParallelRaw;

    // In case solver option '--force-parallel n' is used, previous computation is overridden.
    if (forceParallel)
        maxNbYearsInParallel = nbYearsParallelForced;

    // Limiting the number of parallel years by the smallest refresh span
    auto& p = parameters;
    uint TSlimit = UINT_MAX;
    if ((p.timeSeriesToGenerate & timeSeriesLoad) && (p.timeSeriesToRefresh & timeSeriesLoad))
        TSlimit = p.refreshIntervalLoad;
    if ((p.timeSeriesToGenerate & timeSeriesSolar) && (p.timeSeriesToRefresh & timeSeriesSolar))
        TSlimit = (p.refreshIntervalSolar < TSlimit) ? p.refreshIntervalSolar : TSlimit;
    if ((p.timeSeriesToGenerate & timeSeriesHydro) && (p.timeSeriesToRefresh & timeSeriesHydro))
        TSlimit = (p.refreshIntervalHydro < TSlimit) ? p.refreshIntervalHydro : TSlimit;
    if ((p.timeSeriesToGenerate & timeSeriesWind) && (p.timeSeriesToRefresh & timeSeriesWind))
        TSlimit = (p.refreshIntervalWind < TSlimit) ? p.refreshIntervalWind : TSlimit;
    if ((p.timeSeriesToGenerate & timeSeriesThermal) && (p.timeSeriesToRefresh & timeSeriesThermal))
        TSlimit = (p.refreshIntervalThermal < TSlimit) ? p.refreshIntervalThermal : TSlimit;

    if (TSlimit < maxNbYearsInParallel)
        maxNbYearsInParallel = TSlimit;

    // Limiting the number of parallel years by the total number of years
    if (p.nbYears < maxNbYearsInParallel)
        maxNbYearsInParallel = p.nbYears;

    // Getting the minimum number of years in a set of parallel years.
    // To get this number, we have to divide all years into sets of parallel
    // years and pick the size of the smallest set.

    std::vector<uint>* set = nullptr;
    bool buildNewSet = true;
    std::vector<std::vector<uint>> setsOfParallelYears;

    for (uint y = 0; y < p.nbYears; ++y)
    {
        bool performCalculations = true;
        if (p.userPlaylist)
            performCalculations = p.yearsFilter[y];

        // Do we have to refresh ?
        bool refreshing = false;
        refreshing = (p.timeSeriesToGenerate & timeSeriesLoad)
                     && (p.timeSeriesToRefresh & timeSeriesLoad)
                     && (!y || ((y % p.refreshIntervalLoad) == 0));
        refreshing = refreshing
                     || ((p.timeSeriesToGenerate & timeSeriesSolar)
                         && (p.timeSeriesToRefresh & timeSeriesSolar)
                         && (!y || ((y % p.refreshIntervalSolar) == 0)));
        refreshing = refreshing
                     || ((p.timeSeriesToGenerate & timeSeriesWind)
                         && (p.timeSeriesToRefresh & timeSeriesWind)
                         && (!y || ((y % p.refreshIntervalWind) == 0)));
        refreshing = refreshing
                     || ((p.timeSeriesToGenerate & timeSeriesHydro)
                         && (p.timeSeriesToRefresh & timeSeriesHydro)
                         && (!y || ((y % p.refreshIntervalHydro) == 0)));
        refreshing = refreshing
                     || ((p.timeSeriesToGenerate & timeSeriesThermal)
                         && (p.timeSeriesToRefresh & timeSeriesThermal)
                         && (!y || ((y % p.refreshIntervalThermal) == 0)));

        buildNewSet = buildNewSet || refreshing;

        // We build a new set of parallel years if one of these conditions is fulfilled :
        //	- We have to refresh (or regenerate) some or all time series before running the
        // current year
        //	- This is the first year after the previous set is full with years to be actually
        // executed (not skipped). 	  That is : in the previous set filled, the max number of
        // years to be actually run is reached.
        if (buildNewSet)
        {
            std::vector<uint> setToCreate;
            setsOfParallelYears.push_back(setToCreate);
            set = &(setsOfParallelYears.back());
        }

        if (performCalculations)
            set->push_back(y);

        // Do we build a new set at next iteration (for years to be executed or not) ?
        if (set->size() == maxNbYearsInParallel)
            buildNewSet = true;
        else
            buildNewSet = false;
    } // End of loop over years

    // Now finding the smallest size among all sets.
    minNbYearsInParallel = maxNbYearsInParallel;
    for (uint s = 0; s < setsOfParallelYears.size(); s++)
    {
        uint setSize = (uint)setsOfParallelYears[s].size();
        // Empty sets are not taken into account because, on the solver side,
        // they will contain only skipped years
        if (setSize && (setSize < minNbYearsInParallel))
            minNbYearsInParallel = setSize;
    }

    // GUI : storing minimum number of parallel years (in a set of parallel years).
    //		 Useful in the run window's simulation cores field in case parallel mode is enabled
    // by user.
    minNbYearsInParallel_save = minNbYearsInParallel;

    // The max number of years to run in parallel is limited by the max number years in a set of
    // parallel years. This latter number can be limited by the smallest interval between 2 refresh
    // points and determined by the unrun MC years in case of play-list.
    uint maxNbYearsOverAllSets = 0;
    for (uint s = 0; s < setsOfParallelYears.size(); s++)
    {
        if (setsOfParallelYears[s].size() > maxNbYearsOverAllSets)
            maxNbYearsOverAllSets = (uint)setsOfParallelYears[s].size();
    }
    maxNbYearsInParallel = maxNbYearsOverAllSets;

    // GUI : storing max nb of parallel years (in a set of parallel years) in case parallel mode is
    // enabled.
    //		 Useful for RAM estimation.
    maxNbYearsInParallel_save = maxNbYearsInParallel;

    // Here we answer the question (useful only if hydro hot start is asked) : do all sets of
    // parallel years have the same size ?
    if (parameters.initialReservoirLevels.iniLevels == Antares::Data::irlHotStart
        && setsOfParallelYears.size() && maxNbYearsInParallel > 1)
    {
        uint currentSetSize = (uint)setsOfParallelYears[0].size();
        if (setsOfParallelYears.size() > 1)
        {
            for (uint s = 1; s < setsOfParallelYears.size(); s++)
            {
                if (setsOfParallelYears[s].size() != currentSetSize)
                {
                    parameters.allSetsHaveSameSize = false;
                    break;
                }
            }
        }
    } // End if hot start
}

bool Study::checkHydroHotStart()
{
    bool hydroHotStart = (parameters.initialReservoirLevels.iniLevels == irlHotStart);

    // No need to check further if hydro hot start is not required
    if (!hydroHotStart)
        return true;

    // Here we answer the question (useful only if hydro hot start is asked) : In case of parallel
    // run, do all sets of parallel years have the same size ?
    if (maxNbYearsInParallel != 1 && !parameters.allSetsHaveSameSize)
    {
        logs.error() << "Hot Start Hydro option : conflict with parallelization parameters.";
        logs.error()
          << "Please update relevant simulation parameters or use Cold Start option.    ";
        return false;
    }

    // Checking calendar conditions
    // ... The simulation lasts one year exactly
    uint nbDaysInSimulation = parameters.simulationDays.end - parameters.simulationDays.first + 1;
    if (nbDaysInSimulation < 364)
    {
        logs.error()
          << "Hot Start Hydro option : simulation calendar must cover one complete year.    ";
        logs.error() << "Please update data or use Cold Start option.";
        return false;
    }

    // ... For all areas for which reservoir management is enabled :
    //     - Their starting level is initialized on the same day
    //     - This day is the first day of the simulation calendar
    const Area::Map::iterator end = areas.end();
    for (Area::Map::iterator i = areas.begin(); i != end; ++i)
    {
        // Reference to the area
        Area* area = i->second;

        // No need to make a check on level initialization when reservoir management
        // is not activated for the current area
        if (!area->hydro.reservoirManagement)
            continue;

        // Month the reservoir level is initialized according to.
        // This month number is given in the civil calendar, from january to december (0 is
        // january).
        int initLevelOnMonth = area->hydro.initializeReservoirLevelDate;

        // Conversion of the previous month into simulation calendar
        uint initLevelOnSimMonth = calendar.mapping.months[initLevelOnMonth];

        // Previous month's first day in the year
        uint initLevelOnSimDay = calendar.months[initLevelOnSimMonth].daysYear.first;

        // Check the day of level initialization is the first day of simulation
        if (initLevelOnSimDay != parameters.simulationDays.first)
        {
            logs.error()
              << "Hot Start Hydro option : area '" << area->name
              << "' - hydro level must be initialized on the first simulation month.    ";
            logs.error() << "Please update data or use Cold Start option.";
            return false;
        }
    } // End loop over areas

    return true;
}

bool Study::initializeRuntimeInfos()
{
    delete runtime;
    runtime = new StudyRuntimeInfos();
    return runtime->loadFromStudy(*this);
}

void Study::performTransformationsBeforeLaunchingSimulation()
{
// Those computations are also made from the TS-Generator (ts-generator/xcast/xcast.cpp)
#ifndef NDEBUG
    logs.debug();
    logs.debug() << "applying transformations required by the simulation...";
    logs.debug() << "  > adding DSM values";
#endif

    // ForEach area
    areas.each([&](Data::Area& area) {
        if (not parameters.geographicTrimming)
        {
            // reset filtering
            area.filterSynthesis = (uint)filterAll;
            area.filterYearByYear = (uint)filterAll;
        }

        // Informations about time-series for the load
        auto& matrix = area.load.series.timeSeries;
        auto& dsmvalues = area.reserves[fhrDSM];

        // Adding DSM values
        for (uint timeSeries = 0; timeSeries < matrix.width; ++timeSeries)
        {
            auto& perHour = matrix[timeSeries];
            for (uint h = 0; h < matrix.height; ++h)
            {
                perHour[h] += dsmvalues[h];
                // MBO - 13/05/2014 - #20
                // Starting v4.5 load can be negative
                /*if (perHour[h] < 0.)
                {
                        logs.warning() << area.id << ", hour " << h << ": `load - dsm` can not be
                negative. Reset to 0"; perHour[h] = 0.;
                }*/
            }
        }
    });
}

// This function is a helper. It should be completed when adding new formats
static std::string getOutputSuffix(ResultFormat fmt)
{
    switch (fmt)
    {
    case zipArchive:
        return ".zip";
    default:
        return "";
    }
}

YString StudyCreateOutputPath(StudyMode mode,
                              ResultFormat fmt,
                              const YString& outputRoot,
                              const YString& label,
                              int64_t startTime)
{
    auto suffix = getOutputSuffix(fmt);

    YString folderOutput;

    // Determining the new output folder
    // This folder is composed by the name of the simulation + the current date/time
    folderOutput.clear() << outputRoot << SEP;
    DateTime::TimestampToString(folderOutput, "%Y%m%d-%H%M", startTime, false);

    switch (mode)
    {
    case stdmEconomy:
        folderOutput += "eco";
        break;
    case stdmAdequacy:
        folderOutput += "adq";
        break;
    case stdmUnknown:
    case stdmExpansion:
    case stdmMax:
        break;
    }

    // Temporary buffer
    String buffer;
    buffer.reserve(1024);

    // Folder output
    if (not label.empty())
    {
        buffer.clear();
        TransformNameIntoID(label, buffer);
        folderOutput << '-' << buffer;
    }

    buffer.clear() << folderOutput << suffix;
    // avoid creating the same output twice
    if (IO::Exists(buffer))
    {
        String newpath;
        uint index = 1; // will start from 2
        do
        {
            ++index;
            newpath.clear() << folderOutput << '-' << index << suffix;
        } while (IO::Exists(newpath) and index < 2000);

        folderOutput << '-' << index;
    }
    return folderOutput;
}

void Study::prepareOutput()
{
    pStartTime = DateTime::Now();

    if (parameters.noOutput || !usedByTheSolver)
        return;

    buffer.clear() << folder << SEP << "output";

    folderOutput = StudyCreateOutputPath(
      parameters.mode, parameters.resultFormat, buffer, simulationComments.name, pStartTime);

    logs.info() << "  Output folder : " << folderOutput;
}

void Study::saveAboutTheStudy(Solver::IResultWriter& resultWriter)
{
    String path;
    path.reserve(1024);

    path.clear() << "about-the-study";
    simulationComments.saveUsingWriter(resultWriter, path);

    // Write the header as a reminder
    {
        path.clear() << "about-the-study" << SEP << "study.ini";
        Antares::IniFile ini;
        header.CopySettingsToIni(ini, false);

        std::string writeBuffer;
        ini.saveToString(writeBuffer);

        resultWriter.addEntryFromBuffer(path.c_str(), writeBuffer);
    }

    // Write parameters.ini
    {
        String dest;
        dest << "about-the-study" << SEP << "parameters.ini";

        buffer.clear() << folderSettings << SEP << "generaldata.ini";
        resultWriter.addEntryFromFile(dest.c_str(), buffer.c_str());
    }

    // antares-output.info
    path.clear() << "info.antares-output";
    std::ostringstream f;
    String startTimeStr;
    DateTime::TimestampToString(startTimeStr, "%Y.%m.%d - %H:%M", pStartTime);
    f << "[general]";
    f << "\nversion = " << (uint)Data::versionLatest;
    f << "\nname = " << simulationComments.name;
    f << "\nmode = " << StudyModeToCString(parameters.mode);
    f << "\ndate = " << startTimeStr;
    f << "\ntitle = " << startTimeStr;
    f << "\ntimestamp = " << pStartTime;
    f << "\n\n";
    auto output = f.str();
    resultWriter.addEntryFromBuffer(path.c_str(), output);

    if (usedByTheSolver and !parameters.noOutput)
    {
        // Write all available areas as a reminder
        {
            Yuni::Clob buffer;
            path.clear() << "about-the-study" << SEP << "areas.txt";
            for (auto i = setsOfAreas.begin(); i != setsOfAreas.end(); ++i)
            {
                if (setsOfAreas.hasOutput(i->first))
                    buffer << "@ " << i->first << "\r\n";
            }
            areas.each([&](const Data::Area& area) { buffer << area.name << "\r\n"; });
            resultWriter.addEntryFromBuffer(path.c_str(), buffer);
        }

        // Write all available links as a reminder
        {
            path.clear() << "about-the-study" << SEP << "links.txt";
            Yuni::Clob buffer;
            areas.saveLinkListToBuffer(buffer);
            resultWriter.addEntryFromBuffer(path.c_str(), buffer);
        }
    }
}

Area* Study::areaAdd(const AreaName& name, bool updateMode)
{
    if (name.empty())
        return nullptr;
    if (CheckForbiddenCharacterInAreaName(name))
    {
        logs.error() << "character '*' is forbidden in area name: `" << name << "`";
        return nullptr;
    }

    // Result
    Area* area = nullptr;
    logs.info() << "adding new area " << name;

    // The new scope is mandatory to rebuild the correlation matrices
    // and the scenario builder data
    {
        // These are only useful for the GUI, remove afterwards
        // We need the constructors to be called here, and the destructors
        // to be called at the end of the scope. Using std::optional is merely
        // a means to that end.
        std::optional<CorrelationUpdater> updater;
        std::optional<ScenarioBuilderUpdater> updaterSB;
        if (updateMode)
        {
            updater.emplace(*this);
            updaterSB.emplace(*this);
        }
        // Adding an area
        AreaName newName;
        if (not modifyAreaNameIfAlreadyTaken(newName, name) or newName.empty())
        {
            logs.error() << "Impossible to find a name for a new area";
            return nullptr;
        }

        // Adding an area
        area = addAreaToListOfAreas(areas, newName);
        if (not area)
            return nullptr;

        // Rebuild indexes for all areas
        areas.rebuildIndexes();

        // Default values for the area
        area->createMissingData();
        area->resetToDefaultValues();
    }

    if (uiinfo)
        uiinfo->reload();
    return area;
}

bool Study::areaDelete(Area* area)
{
    if (not area)
        return true;
    if (not AreaListLFind(&areas, area->id.c_str()))
        return false;

    logs.info() << "destroying the area: " << area->name;

    // The new scope is mandatory to rebuild the correlation matrices
    // and the scenario builder data *before* reloading uiinfo.
    {
        // Updating all hydro allocation
        areas.each([&](Data::Area& areait) { areait.hydro.allocation.remove(area->id); });

        // We __must__ update the scenario builder data
        // We may delete an area and re-create a new one with the same
        // name (or rename) for example, but the data related to the old
        // area must be gone.
        scenarioRulesLoadIfNotAvailable();

        CorrelationUpdater updater(*this);
        ScenarioBuilderUpdater updaterSB(*this);

        // Remove a single area
        // Remove all binding constraints attached to the area
        bindingConstraints.remove(area);
        // Remove all Maintenance Group-s attached to the area
        maintenanceGroups.remove(area);
        // Delete the area from the list
        areas.remove(area->id);

        // Rebuild indexes for all areas
        areas.rebuildIndexes();

        // delete updates here
    }

    if (uiinfo)
        uiinfo->reloadAll();
    return true;
}

void Study::areaDelete(Area::Vector& arealist)
{
    if (arealist.empty())
        return;
    if (arealist.size() > 1)
        logs.info() << "destroying " << arealist.size() << " areas...";

    // The new scope is mandatory to rebuild the correlation matrices
    // and the scenario builder data
    {
        // We __must__ update the scenario builder data
        // We may delete an area and re-create a new one with the same
        // name (or rename) for example, but the data related to the old
        // area must be gone.
        scenarioRulesLoadIfNotAvailable();

        CorrelationUpdater updater(*this);
        ScenarioBuilderUpdater updaterSB(*this);

        // Remove all areas
        {
            uint count = 0;
            auto end = arealist.end();
            for (auto i = arealist.begin(); i != end; ++i)
            {
                // The current area
                Area& area = *(*i);
                ++count;

                logs.info() << "destroying the area " << count << '/' << arealist.size() << ": "
                            << area.name;

                // Updating all hydro allocation
                areas.each([&](Data::Area& areait) { areait.hydro.allocation.remove(area.id); });

                // Remove all binding constraints attached to the area
                bindingConstraints.remove(*i);
                // Remove all Maintenance Group-s attached to the area
                maintenanceGroups.remove(*i);
                // Delete the area from the list
                areas.remove(area.id);
            }

            // Rebuild indexes for all areas
            areas.rebuildIndexes();
        }
    }

    if (uiinfo)
        uiinfo->reloadAll();

    if (arealist.size() > 1)
        logs.info() << arealist.size() << " areas have been destroyed";
}

bool Study::linkDelete(AreaLink* lnk)
{
    // Impossible to find the attached area
    // The link might be already deleted
    if (not lnk or !AreaListFindPtr(&areas, lnk->from) or !AreaListFindPtr(&areas, lnk->with))
        return false;

    assert(lnk->with);
    assert(lnk->from);

    logs.info() << "destroying the link " << lnk->from->name << " -> " << lnk->with->name;
    // Remove all associated binding constraints
    bindingConstraints.remove(lnk);
    // The area in the study must be removed
    AreaLinkRemove(lnk);

    if (uiinfo)
        uiinfo->reloadAll();
    return true;
}

bool Study::areaRename(Area* area, AreaName newName)
{
    // A name must not be empty
    if (not area or newName.empty())
        return false;

    String beautifyname;
    BeautifyName(beautifyname, newName);
    if (beautifyname.empty())
        return false;
    newName = beautifyname;

    // Preparing the new area ID
    AreaName newid;
    TransformNameIntoID(newName, newid);
    if (newid.empty())
        return false;

    // Checking if the area exists
    {
        Area* found;
        if ((found = areas.find(newid)))
        {
            if (found->name != newName)
            {
                // The ID will remain identical
                found->name = newName;
                return true;
            }
            return false;
        }
    }

    // We must have the scenario rules to rename properly the area
    scenarioRulesLoadIfNotAvailable();

    // We will temporary override the id of the area in order to have to
    // the new name in the archive
    // Otherwise the values associated to the area will be lost.
    AreaName oldid;
    oldid = area->id;
    area->id = newid;
    logs.info() << "renaming area " << area->name << " into " << newName;

    // Updating all hydro allocation
    areas.each([&](Data::Area& areait) { areait.hydro.allocation.rename(oldid, newid); });

    ScenarioBuilderUpdater updaterSB(*this);
    bool ret = true;

    // Archiving data
    {
        CorrelationUpdater updater(*this);

        // Restoring the old ID
        area->id = oldid;

        // Rename the ares
        ret = areas.renameArea(oldid, newid, newName);
        // Rebuild indexes for all areas
        areas.rebuildIndexes();

        // reloading correlation and scenario builder
    }

    // ReAdjust all interconnections
    areas.fixOrientationForAllInterconnections(bindingConstraints);

    if (uiinfo)
        uiinfo->reloadAll();

    return ret;
}

bool Study::clusterRename(Cluster* cluster, ClusterName newName)
{
    // A name must not be empty
    if (!cluster or newName.empty())
        return false;

    String beautifyname;
    BeautifyName(beautifyname, newName);
    if (!beautifyname)
        return false;
    newName = beautifyname.c_str();

    // Preparing the new area ID
    ClusterName newID;
    TransformNameIntoID(newName, newID);
    if (newID.empty())
    {
        logs.error() << "invalid id transformation";
        return false;
    }

    Area& area = *cluster->parentArea;
    if (not cluster->parentArea)
    {
        logs.error() << "renaming cluster: no parent area";
        return false;
    }

    // Checking if the area exists
    Cluster* found = nullptr;

    enum
    {
        kThermal,
        kRenewable,
        kUnknown
    } type
      = kUnknown;

    if (dynamic_cast<ThermalCluster*>(cluster))
    {
        found = area.thermal.list.find(newID);
        type = kThermal;
    }
    else if (dynamic_cast<RenewableCluster*>(cluster))
    {
        found = area.renewable.list.find(newID);
        type = kRenewable;
    }

    if (found)
    {
        if (found->name() != newName)
        {
            area.invalidateJIT = true;
            found->setName(newName);
            return true;
        }
        return false;
    }

    // gp : to be updated with renewable clusters
    // We must have the scenario rules to rename properly the cluster
    scenarioRulesLoadIfNotAvailable();

    // We will temporary override the id of the area in order to have to
    // the new name in the archive
    // Otherwise the values associated to the area will be lost.
    logs.info() << "  renaming cluster '" << cluster->name() << "' into '" << newName << "'";

    area.invalidateJIT = true;

    bool ret = true;

    // Archiving data
    switch (type)
    {
    case kRenewable:
        ret = area.renewable.list.rename(cluster->id(), newName);
        area.renewable.prepareAreaWideIndexes();
        break;
    case kThermal:
        ret = area.thermal.list.rename(cluster->id(), newName);
        area.thermal.prepareAreaWideIndexes();
        break;
    case kUnknown:
        logs.error() << "Unknown cluster type";
        break;
    }

    ScenarioBuilderUpdater updaterSB(*this);

    if (uiinfo)
        uiinfo->reloadAll();

    return ret;
}

void Study::destroyAllLoadTSGeneratorData()
{
    areas.each([&](Data::Area& area) { FreeAndNil(area.load.prepro); });
}

void Study::destroyAllSolarTSGeneratorData()
{
    areas.each([&](Data::Area& area) { FreeAndNil(area.solar.prepro); });
}

void Study::destroyAllHydroTSGeneratorData()
{
    areas.each([&](Data::Area& area) { FreeAndNil(area.hydro.prepro); });
}

void Study::destroyAllWindTSGeneratorData()
{
    areas.each([&](Data::Area& area) { FreeAndNil(area.wind.prepro); });
}

void Study::destroyAllThermalTSGeneratorData()
{
    areas.each([&](Data::Area& area) {
        auto pend = area.thermal.list.end();
        for (auto j = area.thermal.list.begin(); j != pend; ++j)
        {
            ThermalCluster& cluster = *(j->second);
            FreeAndNil(cluster.prepro);
        }
    });
}

// TODO CR27: see if we need this one for mant groups - only used in UI - so II phase

void Study::ensureDataAreLoadedForAllBindingConstraints()
{
    for(const auto& constraint: bindingConstraints)
    {
        if (not JIT::IsReady(constraint->RHSTimeSeries().jit))
            constraint->forceReload(true);
    }
}

namespace // anonymous
{
template<enum TimeSeriesType T>
struct TS final
{
    static bool IsNeeded(const Study& s, const uint y)
    {
        if (not(T & s.parameters.timeSeriesToRefresh))
            return false;

        switch (T)
        {
        case timeSeriesLoad:
            return (!(y % s.parameters.refreshIntervalLoad));
        case timeSeriesSolar:
            return (!(y % s.parameters.refreshIntervalSolar));
        case timeSeriesWind:
            return (!(y % s.parameters.refreshIntervalWind));
        case timeSeriesHydro:
            return (!(y % s.parameters.refreshIntervalHydro));
        case timeSeriesThermal:
            return (!(y % s.parameters.refreshIntervalThermal));
        }
        return false;
    }
};

} // anonymous namespace

void Study::initializeProgressMeter(bool tsGeneratorOnly)
{
    uint years = tsGeneratorOnly ? 1 : (runtime->rangeLimits.year[rangeEnd] + 1);
    assert(runtime);

    int ticksPerYear = 0;
    int ticksPerOutput = 0;

    if (not tsGeneratorOnly)
    {
        // One tick at the begining and 2 at the end of the year
        // Output - Areas
        ticksPerOutput += (int)areas.size();
        // Output - Links
        ticksPerOutput += (int)runtime->interconnectionsCount();
        // Output - digest
        ticksPerOutput += 1;
        ticksPerYear = 1;
    }

    int n;

    for (uint y = 0; y != years; ++y)
    {
        if (TS<timeSeriesLoad>::IsNeeded(*this, y))
        {
            n = parameters.nbTimeSeriesLoad * areas.size() * 365;
            if (0 != (timeSeriesLoad & parameters.timeSeriesToArchive))
                n += (int)areas.size();
            progression.add(y, Solver::Progression::sectTSGLoad, n);
        }
        if (TS<timeSeriesSolar>::IsNeeded(*this, y))
        {
            n = parameters.nbTimeSeriesSolar * areas.size() * 365;
            if (0 != (timeSeriesSolar & parameters.timeSeriesToArchive))
                n += (int)areas.size();
            progression.add(y, Solver::Progression::sectTSGSolar, n);
        }
        if (TS<timeSeriesWind>::IsNeeded(*this, y))
        {
            n = parameters.nbTimeSeriesWind * areas.size() * 365;
            if (0 != (timeSeriesWind & parameters.timeSeriesToArchive))
                n += (int)areas.size();
            progression.add(y, Solver::Progression::sectTSGWind, n);
        }
        if (TS<timeSeriesHydro>::IsNeeded(*this, y))
        {
            // n += parameters.nbTimeSeriesHydro * areas.size() * 12;
            n = parameters.nbTimeSeriesHydro;
            if (0 != (timeSeriesHydro & parameters.timeSeriesToArchive))
                n += (int)areas.size();
            progression.add(y, Solver::Progression::sectTSGHydro, n);
        }
        if (TS<timeSeriesThermal>::IsNeeded(*this, y))
        {
            n = runtime->thermalPlantTotalCount;
            if (0 != (timeSeriesThermal & parameters.timeSeriesToArchive))
            {
                n += (int)runtime->thermalPlantTotalCount;
                n += (int)runtime->thermalPlantTotalCountMustRun;
            }
            progression.add(y, Solver::Progression::sectTSGThermal, n);
        }

        progression.add(y, Solver::Progression::sectYear, ticksPerYear);

        if (parameters.yearByYear)
            progression.add(y, Solver::Progression::sectOutput, ticksPerOutput);
    }

    // Output
    progression.add(Solver::Progression::sectOutput, ticksPerOutput);

    // Import
    n = 0;
    if (0 != (timeSeriesLoad & parameters.exportTimeSeriesInInput))
        n += (int)areas.size();
    if (0 != (timeSeriesSolar & parameters.exportTimeSeriesInInput))
        n += (int)areas.size();
    if (0 != (timeSeriesWind & parameters.exportTimeSeriesInInput))
        n += (int)areas.size();
    if (0 != (timeSeriesHydro & parameters.exportTimeSeriesInInput))
        n += (int)areas.size();
    if (0 != (timeSeriesThermal & parameters.exportTimeSeriesInInput))
        n += (int)areas.size();
    if (n)
        progression.add(Solver::Progression::sectImportTS, n);

    // Needed by the progression meter thread to retrieve properly
    // messages from all MC years
    progression.setNumberOfParallelYears(maxNbYearsInParallel);
}

bool Study::forceReload(bool reload) const
{
    bool ret = true;

    // Invalidate all areas
    ret = areas.forceReload(reload) and ret;
    // Binding constraints
    bindingConstraints.forceReload(reload);

    ret = preproLoadCorrelation.forceReload(reload) and ret;
    ret = preproSolarCorrelation.forceReload(reload) and ret;
    ret = preproWindCorrelation.forceReload(reload) and ret;
    ret = preproHydroCorrelation.forceReload(reload) and ret;

    ret = setsOfAreas.forceReload(reload) and ret;
    ret = setsOfLinks.forceReload(reload) and ret;
    return ret;
}

void Study::markAsModified() const
{
    areas.markAsModified();

    preproLoadCorrelation.markAsModified();
    preproSolarCorrelation.markAsModified();
    preproWindCorrelation.markAsModified();
    preproHydroCorrelation.markAsModified();

    bindingConstraints.markAsModified();

    setsOfAreas.markAsModified();
    setsOfLinks.markAsModified();
}

void Study::relocate(AnyString newFolder)
{
    folder = newFolder;
    folderInput.clear() << newFolder << SEP << "input";
    folderOutput.clear() << newFolder << SEP << "output";
    folderSettings.clear() << newFolder << SEP << "settings";
}

void Study::resizeAllTimeseriesNumbers(uint n)
{
    logs.debug() << "  resizing timeseries numbers";
    areas.resizeAllTimeseriesNumbers(n);
    bindingConstraintsGroups.resizeAllTimeseriesNumbers(n);
}

bool Study::checkForFilenameLimits(bool output, const String& chfolder) const
{
    enum
    {
#ifdef YUNI_OS_WINDOWS
        limit = 255, // zero-terminated
#else
        limit = 4090,
#endif
    };

    if (areas.empty())
        return true;

    String studyfolder;
    if (chfolder.empty())
        studyfolder = folder;
    else
        studyfolder = chfolder;

    if (output)
    {
        String linkname;
        String areaname;

        areas.each([&](const Area& area) {
            if (areaname.size() < area.id.size())
                areaname = area.id;

            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
            {
                auto& link = *(i->second);
                uint len = link.from->id.size() + link.with->id.size();
                len += output ? 3 : 1;
                if (len > linkname.size())
                {
                    linkname.clear();
                    linkname << i->second->from->id;
                    if (output)
                        linkname << " - "; // 3
                    else
                        linkname << SEP;
                    linkname << i->second->with->id;
                }
            }
        });

        String filename;
        filename << studyfolder << SEP << "output" << SEP;

        if (linkname.empty())
        {
            if (areaname.empty())
                filename.clear();
            else
            {
                // no links : obtained from areas
                // The maximum filename should be obtained with links :
                // Adequacy/mc-all/areas/languedocroussillon/without-network-hourly.txt
                filename << (parameters.economy() ? "economy" : "adequacy") << SEP;
                filename << "mc-all" << SEP << "areas";
                filename << SEP << areaname << SEP;
                filename << "values-hourly.txt";
            }
        }
        else
        {
            // The maximum filename should be obtained with links :
            // economy/mc-ind/00001/links/pyrennees\ -\ languedocroussillon/values-hourly.txt
            filename << (parameters.adequacy() ? "adequacy" : "economy") << SEP;
            filename << "mc-all" << SEP << "links";
            filename << SEP << linkname << SEP << "values-hourly.txt";
        }

        if (not filename.empty() and filename.size() >= limit)
        {
            logs.error() << "OS Maximum path length limitation obtained with the link '" << linkname
                         << "' (got " << filename.size() << " characters)";
            logs.error() << "You may experience problems while accessing to this file: "
                         << filename;
            return false;
        }
    }
    else
    {
        String areaname;
        String clustername;

        // For input, the maximum filename length can be obtained from multiple
        // sources :
        // /input/thermal/series/languedocroussillon/aggregate\ 1/series.txt
        // /input/hydro/common/capacity/maxcapacityexpectation_languedocroussillon.txt
        // or even constraints

        areas.each([&](const Area& area) {
            if (areaname.size() < area.id.size())
                areaname = area.id;
            auto& cname = clustername;
            cname.clear();

            area.thermal.list.each([&](const Cluster& cluster) {
                if (cluster.id().size() > cname.size())
                    cname = cluster.id();
            });
        });

        String filename;

        // Checking for thermal clusters
        if (not areaname.empty() and not clustername.empty())
        {
            filename.clear();
            filename << studyfolder << SEP << "input" << SEP;
            filename << "thermal" << SEP << "series" << SEP << areaname << SEP;
            filename << clustername << SEP << "series.txt";

            if (filename.size() >= limit)
            {
                logs.error()
                  << "OS Maximum path length limitation  obtained with the thermal plant '"
                  << areaname << "::" << clustername << "' (got " << filename.size()
                  << " characters)";
                logs.error() << "You may experience problems while accessing to this file: "
                             << filename;
                return false;
            }
        }

        // Checking for hydro files
        if (not areaname.empty())
        {
            filename.clear();
            filename << studyfolder << "input" << SEP;
            filename << "hydro" << SEP << "common" << SEP << "capacity" << SEP;
            areaname << "maxcapacityexpectation_" << areaname << ".txt";

            if (filename.size() >= limit)
            {
                logs.error() << "OS Maximum path length limitation obtained with the area '"
                             << areaname << "' (got " << filename.size() << " characters)";
                logs.error() << "You may experience problems while accessing to this file: "
                             << filename;
                return false;
            }
        }

        // Checking constraints
        filename.clear();
        // /input/bindingconstraints/bindingconstraints.ini
        filename << studyfolder << "input" << SEP;
        filename << "bindingconstraints" << SEP << "bindingconstraints.ini";
        if (filename.size() >= limit)
        {
            logs.error()
              << "OS Maximum path length limitation obtained with the binding constraint list"
              << " (got " << filename.size() << " characters)";
            logs.error() << "You may experience problems while accessing to this file: "
                         << filename;
            return false;
        }

        if (not bindingConstraints.empty())
        {
            // /input/bindingconstraints/maille1_down.txt
            auto end = bindingConstraints.end();
            for (auto i = bindingConstraints.begin(); i != end; ++i)
            {
                // The current constraint
                auto& constraint = *(*i);

                filename.clear();
                filename << studyfolder << "input" << SEP << "bindingconstraints" << SEP;
                filename << constraint.id() << ".ini";

                if (filename.size() >= limit)
                {
                    logs.error()
                      << "OS Maximum path length limitation obtained with the constraint '"
                      << constraint.name() << "' (got " << filename.size() << " characters)";
                    logs.error() << "You may experience problems while accessing to this file: "
                                 << filename;
                    return false;
                }
            }
        }

        if (not maintenanceGroups.empty())
        {
            auto end = maintenanceGroups.end();
            for (auto i = maintenanceGroups.begin(); i != end; ++i)
            {
                // The current constraint
                auto& mntGroup = *(*i);

                filename.clear();
                filename << studyfolder << "input" << SEP << "maintenanceplanning" << SEP;
                filename << mntGroup.id() << ".ini";

                if (filename.size() >= limit)
                {
                    logs.error()
                      << "OS Maximum path length limitation obtained with the maintenance group '"
                      << mntGroup.name() << "' (got " << filename.size() << " characters)";
                    logs.error() << "You may experience problems while accessing to this file: "
                                 << filename;
                    return false;
                }
            }
        }
    }
    return true;
}

void Study::removeTimeseriesIfTSGeneratorEnabled()
{
    if (0 != parameters.timeSeriesToGenerate)
    {
        if (0 != (parameters.timeSeriesToGenerate & timeSeriesLoad))
            areas.removeLoadTimeseries();
        if (0 != (parameters.timeSeriesToGenerate & timeSeriesHydro))
            areas.removeHydroTimeseries();
        if (0 != (parameters.timeSeriesToGenerate & timeSeriesSolar))
            areas.removeSolarTimeseries();
        if (0 != (parameters.timeSeriesToGenerate & timeSeriesWind))
            areas.removeWindTimeseries();
        if (0 != (parameters.timeSeriesToGenerate & timeSeriesThermal))
            areas.removeThermalTimeseries();
    }
}

void Study::computePThetaInfForThermalClusters() const
{
    for (uint i = 0; i != this->areas.size(); i++)
    {
        // Alias de la zone courant
        const auto& area = *(this->areas.byIndex[i]);

        for (uint j = 0; j < area.thermal.list.size(); j++)
        {
            // Alias du cluster courant
            auto& cluster = area.thermal.list.byIndex[j];
            for (uint k = 0; k < HOURS_PER_YEAR; k++)
                cluster->PthetaInf[k] = cluster->modulation[Data::thermalMinGenModulation][k]
                                        * cluster->unitCount * cluster->nominalCapacity;
        }
    }
}

} // namespace Antares::Data

