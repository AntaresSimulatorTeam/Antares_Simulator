/*
** Copyright 2007-2022 RTE
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

#include <utility>
#include <vector>
#include <climits>
#include <cassert>

#include <yuni/core/system/cpu.h> // For use of Yuni::System::CPU::Count()
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/fwd.h>

#include <antares/logs.h>
#include <antares/utils.hxx>
#include "parameters.h"
#include "parallel-years.h"

#define SEP Yuni::IO::Separator

namespace Antares
{

/*!
** \brief Checks if clusters have the "gen-ts" param
**
** \warning TODO: REMOVE THIS METHOD ONCE THE TSGEN HAS BEEN DECOUPLED FROM SOLVER
*/

bool TempAreaListHolder::checkThermalTSGeneration(YString folder_)
{
    inputFolder = std::move(folder_);
    loadAreaList(); //Build areaNames

    return std::any_of(areaNames.begin(), areaNames.end(),[this](const Yuni::String& areaName){
            auto folder = inputFolder;
            Yuni::Clob thermalPlant = folder << SEP << "thermal" << SEP << "clusters" << SEP << areaName << SEP << "list.ini";
            IniFile ini;
            bool ret = false;
            if (ini.open(thermalPlant))
            {
                if(ini.firstSection)
                {
                    for (IniFile::Section* section = ini.firstSection; section; section = section->next)
                    {
                        for (IniFile::Property* property = section->firstProperty; property; property = property->next)
                        {
                            ret = ret || (property->key == "gen-ts");
                        }
                    }
                }
                return ret;
            }
            logs.error() << "Thermal Cluster Ini file cannot be opened: " << thermalPlant.c_str();
            return false;
        }
    );
}

/*!
** \brief Loads Area List to later check Thermal Cluster's refresh
**
** \warning TODO: REMOVE THIS METHOD ONCE THE TSGEN HAS BEEN DECOUPLED FROM SOLVER
*/

void TempAreaListHolder::loadAreaList()
{
    auto folder = inputFolder;
    Yuni::Clob filename = folder << SEP << "areas" << SEP << "list.txt";
    
    Yuni::IO::File::Stream file;
    Yuni::String buffer;
    buffer.reserve(1024 /* to force the allocation */);

    if (!file.open(filename))
    {
        logs.error() << "I/O error: " << filename << ": Impossible to open the file";
        return;
    }
    while(file.readline(buffer))
    {
        // The area name
        Yuni::String name;
        Yuni::String lname;
        name = buffer;
        name.trim(" \t\n\r");
        TransformNameIntoID(name, lname);
        areaNames.push_back(lname);
    }

}


void SetsOfParallelYearCalculator::computeRawNbParallelYear()
{
    // In case solver option '--force-parallel n' is used, this computation is not needed
    // and n will remain the forcedNbOfParallelYears
    if (forceParallel)
        return;

    std::map<NumberOfCoresMode, int> numberOfMCYearThreads;
    const uint nbLogicalCores = number_of_cores_;

    numberOfMCYearThreads[ncMin] = 1;
    switch (nbLogicalCores)
    {
    case 0:
        logs.fatal() << "Number of logical cores available is 0.";
        break;
    case 1:
        numberOfMCYearThreads[ncLow] = 1;
        numberOfMCYearThreads[ncAvg] = 1;
        numberOfMCYearThreads[ncHigh] = 1;
        numberOfMCYearThreads[ncMax] = 1;
        break;
    case 2:
        numberOfMCYearThreads[ncLow] = 1;
        numberOfMCYearThreads[ncAvg] = 1;
        numberOfMCYearThreads[ncHigh] = 2;
        numberOfMCYearThreads[ncMax] = 2;
        break;
    case 3:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 2;
        numberOfMCYearThreads[ncHigh] = 2;
        numberOfMCYearThreads[ncMax] = 3;
        break;
    case 4:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 2;
        numberOfMCYearThreads[ncHigh] = 3;
        numberOfMCYearThreads[ncMax] = 4;
        break;
    case 5:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 3;
        numberOfMCYearThreads[ncHigh] = 4;
        numberOfMCYearThreads[ncMax] = 5;
        break;
    case 6:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 3;
        numberOfMCYearThreads[ncHigh] = 4;
        numberOfMCYearThreads[ncMax] = 6;
        break;
    case 7:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 3;
        numberOfMCYearThreads[ncHigh] = 5;
        numberOfMCYearThreads[ncMax] = 7;
        break;
    case 8:
        numberOfMCYearThreads[ncLow] = 2;
        numberOfMCYearThreads[ncAvg] = 4;
        numberOfMCYearThreads[ncHigh] = 6;
        numberOfMCYearThreads[ncMax] = 8;
        break;
    case 9:
        numberOfMCYearThreads[ncLow] = 3;
        numberOfMCYearThreads[ncAvg] = 5;
        numberOfMCYearThreads[ncHigh] = 7;
        numberOfMCYearThreads[ncMax] = 8;
        break;
    case 10:
        numberOfMCYearThreads[ncLow] = 3;
        numberOfMCYearThreads[ncAvg] = 5;
        numberOfMCYearThreads[ncHigh] = 8;
        numberOfMCYearThreads[ncMax] = 9;
        break;
    case 11:
        numberOfMCYearThreads[ncLow] = 3;
        numberOfMCYearThreads[ncAvg] = 6;
        numberOfMCYearThreads[ncHigh] = 8;
        numberOfMCYearThreads[ncMax] = 10;
        break;
    case 12:
        numberOfMCYearThreads[ncLow] = 3;
        numberOfMCYearThreads[ncAvg] = 6;
        numberOfMCYearThreads[ncHigh] = 9;
        numberOfMCYearThreads[ncMax] = 11;
        break;
    default:
        numberOfMCYearThreads[ncLow] = (uint)std::ceil(nbLogicalCores / 4.);
        numberOfMCYearThreads[ncAvg] = (uint)std::ceil(nbLogicalCores / 2.);
        numberOfMCYearThreads[ncHigh] = (uint)std::ceil(3 * nbLogicalCores / 4.);
        numberOfMCYearThreads[ncMax] = nbLogicalCores - 1;
        break;
    }

    /*
            Getting the number of parallel years based on the number
            of cores level.
            This number is limited by the smallest refresh span (if at least
            one type of time series is generated)
    */

    try
    {
        forcedNbOfParallelYears = numberOfMCYearThreads.at(p.nbCores.ncMode);
    }
    catch(const std::out_of_range& e)
    {
        logs.fatal() << "Simulation cores level not correct : " << (int)p.nbCores.ncMode;
    }
    
}


void SetsOfParallelYearCalculator::limitNbOfParallelYearsbyMinRefreshSpan()
{
    uint TSlimit = UINT_MAX;
    if ((p.timeSeriesToGenerate & timeSeriesLoad) && (p.timeSeriesToRefresh & timeSeriesLoad))
        TSlimit = p.refreshIntervalLoad;
    if ((p.timeSeriesToGenerate & timeSeriesSolar) && (p.timeSeriesToRefresh & timeSeriesSolar))
        TSlimit = std::min(p.refreshIntervalSolar,  TSlimit);
    if ((p.timeSeriesToGenerate & timeSeriesHydro) && (p.timeSeriesToRefresh & timeSeriesHydro))
        TSlimit = std::min(p.refreshIntervalHydro, TSlimit);
    if ((p.timeSeriesToGenerate & timeSeriesWind) && (p.timeSeriesToRefresh & timeSeriesWind))
        TSlimit = std::min(p.refreshIntervalWind, TSlimit);
    if ((p.timeSeriesToGenerate & timeSeriesThermal) && (p.timeSeriesToRefresh & timeSeriesThermal))
        TSlimit = std::min(p.refreshIntervalThermal, TSlimit);

    forcedNbOfParallelYears = std::min({p.nbYears, TSlimit, forcedNbOfParallelYears});
}

bool SetsOfParallelYearCalculator::isRefreshNeededForCurrentYear(uint y)
{
    bool refreshing = false;
    refreshing = (p.timeSeriesToGenerate & timeSeriesLoad)
                    && (p.timeSeriesToRefresh & timeSeriesLoad)
                    && ((y % p.refreshIntervalLoad) == 0);
    refreshing = refreshing
                    || ((p.timeSeriesToGenerate & timeSeriesSolar)
                        && (p.timeSeriesToRefresh & timeSeriesSolar)
                        && (y % p.refreshIntervalSolar) == 0);
    refreshing = refreshing
                    || ((p.timeSeriesToGenerate & timeSeriesWind)
                        && (p.timeSeriesToRefresh & timeSeriesWind)
                        && (y % p.refreshIntervalWind) == 0);
    refreshing = refreshing
                    || ((p.timeSeriesToGenerate & timeSeriesHydro)
                        && (p.timeSeriesToRefresh & timeSeriesHydro)
                        && (y % p.refreshIntervalHydro) == 0);

    bool haveToRefreshTSThermal 
        = ((p.timeSeriesToGenerate & timeSeriesThermal)
                        && (p.timeSeriesToRefresh & timeSeriesThermal)) || thermalTSRefresh;
    refreshing
        = refreshing || (haveToRefreshTSThermal && (y % p.refreshIntervalThermal == 0));

    return refreshing;

}

void SetsOfParallelYearCalculator::buildSetsOfParallelYears()
{
    setOfParallelYears* set = nullptr;
    bool buildNewSet = true;
    bool foundFirstPerformedYearOfCurrentSet = false;
    // Gets information on each parallel years set
    for (uint y = 0; y < p.nbYears; ++y)
    {
        unsigned int indexSpace = 999999;
        bool performCalculations = true;
        
        if(p.userPlaylist)
            performCalculations = p.yearsFilter[y];
        
        bool refreshing = isRefreshNeededForCurrentYear(y);
        buildNewSet = buildNewSet || refreshing;

        if (buildNewSet)
        {
            setOfParallelYears setToCreate;
            setsOfParallelYears.push_back(setToCreate);
            set = &(setsOfParallelYears.back());

            // Initializations
            set->nbPerformedYears = 0;
            set->nbYears = 0;
            set->regenerateTS = false;
            set->yearForTSgeneration = 999999;

            // In case we have to regenerate times series before run the current set of parallel
            // years
            if (refreshing)
            {
                set->regenerateTS = true;
                set->yearForTSgeneration = y;
            }
        }

        set->yearsIndices.push_back(y); 
        set->nbYears++;
        set->yearFailed[y] = true;
        set->isFirstPerformedYearOfASet[y] = false;

        if (performCalculations)
        {
            set->setsSizes++;
            // Another year performed
            ++nbYearsReallyPerformed;

            // Number of actually performed years in the current set (up to now).
            set->nbPerformedYears++;
            // Index of the MC year's space (useful if this year is actually run)
            indexSpace = set->nbPerformedYears - 1;

            set->isYearPerformed[y] = true;
            set->performedYearToSpace[y] = indexSpace;
            set->spaceToPerformedYear[indexSpace] = y;

            if (!foundFirstPerformedYearOfCurrentSet)
            {
                set->isFirstPerformedYearOfASet[y] = true;
                foundFirstPerformedYearOfCurrentSet = true;
            }
        }
        else
        {
            set->isYearPerformed[y] = false;
        }

        // Do we build a new set at next iteration (for years to be executed or not) ?

        // In case the study is run in the draft mode, only 1 core is allowed
        if (p.mode == Antares::Data::stdmAdequacyDraft){
            forcedNbOfParallelYears = 1;
        }

        // In case parallel mode was not chosen, only 1 core is allowed
        if (!enableParallel && !forceParallel){
            forcedNbOfParallelYears = 1;
        }

        if (indexSpace == forcedNbOfParallelYears - 1 || y == p.nbYears - 1)
        {
            buildNewSet = true;
            foundFirstPerformedYearOfCurrentSet = false;
            if (set->nbPerformedYears > forcedNbOfParallelYears)
                forcedNbOfParallelYears = set->nbPerformedYears;
        }
        else
            buildNewSet = false;

     // End of loop over years

    }

}


bool SetsOfParallelYearCalculator::allSetsParallelYearsHaveSameSize()
{
    if (p.initialReservoirLevels.iniLevels == Antares::Data::irlHotStart
        && !setsOfParallelYears.empty() && forcedNbOfParallelYears > 1)
    {
        uint currentSetSize = (uint)setsOfParallelYears[0].setsSizes;
        return all_of(setsOfParallelYears.begin(), setsOfParallelYears.end(),
                    [currentSetSize](const setOfParallelYears& v){ return v.setsSizes == currentSetSize; } );
    } // End if hot start
    return true;
    // parameters.allSetsHaveSameSize takes this result;
}

uint SetsOfParallelYearCalculator::computeMinNbParallelYears() const
{
    // Now finding the smallest size among all sets.
    uint minNbYearsInParallel = forcedNbOfParallelYears;
    for (uint s = 0; s < setsOfParallelYears.size(); s++)
    {
        uint setSize = (uint)setsOfParallelYears[s].setsSizes;
        // Empty sets are not taken into account because, on the solver side,
        // they will contain only skipped years
        if (setSize && (setSize < minNbYearsInParallel))
            minNbYearsInParallel = setSize;
    }
    return minNbYearsInParallel;
}

void SetsOfParallelYearCalculator::computeForcedNbYearsInParallelYearSet()
{
    
    uint maxNbYearsOverAllSets = 0;
    for (uint s = 0; s < setsOfParallelYears.size(); s++)
    {
        if (setsOfParallelYears[s].setsSizes > maxNbYearsOverAllSets)
            maxNbYearsOverAllSets = (uint)setsOfParallelYears[s].setsSizes;
    }

    forcedNbOfParallelYears = maxNbYearsOverAllSets;

}

void SetsOfParallelYearCalculator::build()
{

    computeRawNbParallelYear();
    limitNbOfParallelYearsbyMinRefreshSpan();

    buildSetsOfParallelYears();

    computeForcedNbYearsInParallelYearSet();    

}

} // namespace Antares
