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

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <assert.h>
#include "../study.h"
#include "area.h"
#include "../../logs.h"
#include "../memory-usage.h"
#include "../filter.h"
#include "constants.h"
#include "ui.h"
#include "scratchpad.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
void Area::internalInitialize()
{
    // Make sure we have
    if (JIT::usedFromGUI)
        ui = new AreaUI();
}

Area::Area() :
 index((uint)(-1)),
 enabled(true),
 reserves(fhrMax, HOURS_PER_YEAR),
 miscGen(fhhMax, HOURS_PER_YEAR),
 nodalOptimization(anoAll),
 spreadUnsuppliedEnergyCost(0.),
 spreadSpilledEnergyCost(0.),
 filterSynthesis(filterAll),
 filterYearByYear(filterAll),
 ui(nullptr),
 nbYearsInParallel(0),
 scratchpad(nullptr),
 invalidateJIT(false)
{
    internalInitialize();
}

Area::Area(const AnyString& name, uint nbParallelYears) :
 index((uint)(-1)),
 reserves(fhrMax, HOURS_PER_YEAR),
 miscGen(fhhMax, HOURS_PER_YEAR),
 nodalOptimization(anoAll),
 spreadUnsuppliedEnergyCost(0.),
 spreadSpilledEnergyCost(0.),
 filterSynthesis(filterAll),
 filterYearByYear(filterAll),
 ui(NULL),
 nbYearsInParallel(nbParallelYears),
 scratchpad(nullptr),
 invalidateJIT(false)
{
    internalInitialize();
    this->name = name;
    Antares::TransformNameIntoID(this->name, this->id);
}

Area::Area(const AnyString& name, const AnyString& id, uint nbParallelYears, uint indx) :
 index(indx),
 reserves(fhrMax, HOURS_PER_YEAR),
 miscGen(fhhMax, HOURS_PER_YEAR),
 nodalOptimization(anoAll),
 spreadUnsuppliedEnergyCost(0.),
 spreadSpilledEnergyCost(0.),
 filterSynthesis(filterAll),
 filterYearByYear(filterAll),
 ui(nullptr),
 nbYearsInParallel(nbParallelYears),
 scratchpad(nullptr),
 invalidateJIT(false)
{
    internalInitialize();
    this->name = name;
    AreaName givenID = id;
    Antares::TransformNameIntoID(givenID, this->id);
}

Area::~Area()
{
    logs.debug() << "  :: destroying area " << name;

    if (scratchpad)
    {
        for (uint numSpace = 0; numSpace < nbYearsInParallel; numSpace++)
        {
            if (scratchpad[numSpace])
                delete scratchpad[numSpace];
            scratchpad[numSpace] = nullptr;
        }
        delete[] scratchpad;
        scratchpad = nullptr;
    }

    // Delete all links
    clearAllLinks();

    reserves.clear();
    miscGen.clear();

    delete ui;
    ui = nullptr;
}

void Area::clearAllLinks()
{
    if (not links.empty())
    {
        // Delete all links
        {
            auto end = links.end();
            for (auto i = links.begin(); i != end; ++i)
                delete i->second;
        }
        // Empty the container
        links.clear();
    }
}

void Area::detachAllLinks()
{
    while (not links.empty())
        AreaLinkRemove((links.begin())->second);
}

AreaLink* Area::findExistingLinkWith(Area& with)
{
    if (&with == this)
        return nullptr;

    if (not links.empty())
    {
        const AreaLink::Map::iterator end = links.end();
        for (AreaLink::Map::iterator i = links.begin(); i != end; ++i)
        {
            if (i->second->from == &with or i->second->with == &with)
                return i->second;
        }
    }
    if (!with.links.empty())
    {
        const AreaLink::Map::iterator end = with.links.end();
        for (AreaLink::Map::iterator i = with.links.begin(); i != end; ++i)
        {
            if (i->second->from == this or i->second->with == this)
                return i->second;
        }
    }
    return nullptr;
}

const AreaLink* Area::findExistingLinkWith(const Area& with) const
{
    if (&with != this)
    {
        if (not links.empty())
        {
            const AreaLink::Map::const_iterator end = links.end();
            for (AreaLink::Map::const_iterator i = links.begin(); i != end; ++i)
            {
                if (i->second->from == &with or i->second->with == &with)
                    return i->second;
            }
        }
        if (!with.links.empty())
        {
            const AreaLink::Map::const_iterator end = with.links.end();
            for (AreaLink::Map::const_iterator i = with.links.begin(); i != end; ++i)
            {
                if (i->second->from == this or i->second->with == this)
                    return i->second;
            }
        }
    }
    return nullptr;
}

Yuni::uint64 Area::memoryUsage() const
{
    Yuni::uint64 ret = 0;

    // Misc gen. (previously called Fatal hors hydro)
    ret += miscGen.valuesMemoryUsage();
    // Reserves
    ret += reserves.valuesMemoryUsage();

    ret += sizeof(Area);
    // Load
    ret += load.memoryUsage();
    // Solar
    ret += solar.memoryUsage();
    // Wind
    ret += wind.memoryUsage();

    // Hydro
    ret += PreproHydroMemoryUsage(hydro.prepro);
    if (hydro.series)
        ret += hydro.series->memoryUsage();

    // Thermal
    ret += thermal.list.memoryUsage();

    // Renewable
    ret += renewable.list.memoryUsage();

    // UI
    if (ui)
        ret += ui->memoryUsage();

    // scratchpad
    if (scratchpad)
        ret += sizeof(AreaScratchpad) * nbYearsInParallel;

    // links
    auto end = links.end();
    for (auto i = links.begin(); i != end; ++i)
        ret += (i->second)->memoryUsage();

    return ret;
}

void Area::ensureAllDataAreCreated()
{
    // Timeseries
    if (!load.series)
        load.series = new DataSeriesLoad();
    if (!solar.series)
        solar.series = new DataSeriesSolar();
    if (!wind.series)
        wind.series = new DataSeriesWind();
    if (!hydro.series)
        hydro.series = new DataSeriesHydro();
    ThermalClusterListEnsureDataTimeSeries(&thermal.list);
    renewable.list.ensureDataTimeSeries();

    // Prepro
    if (!load.prepro)
        load.prepro = new Data::Load::Prepro();
    if (!solar.prepro)
        solar.prepro = new Data::Solar::Prepro();
    if (!wind.prepro)
        wind.prepro = new Data::Wind::Prepro();
    if (!hydro.prepro)
        hydro.prepro = new PreproHydro();
    thermal.list.ensureDataPrepro();
}

void Area::resetToDefaultValues()
{
    // Nodal optimization
    nodalOptimization = anoAll;

    // Spread
    spreadUnsuppliedEnergyCost = 0.;
    spreadSpilledEnergyCost = 0.;

    // Filtering
    filterSynthesis = (uint)filterAll;
    filterYearByYear = (uint)filterAll;

    // Load
    load.resetToDefault();
    // Solar
    solar.resetToDefault();
    // Wind
    wind.resetToDefault();
    // Hydro
    hydro.reset();
    hydro.allocation.fromArea(id, 1.);
    // Thermal
    thermal.reset();
    // Renewable
    renewable.reset();
    // Fatal hors hydro
    miscGen.reset(fhhMax, HOURS_PER_YEAR, true);
    // reserves
    reserves.reset(fhrMax, HOURS_PER_YEAR, true);

    // invalidate the whole area
    invalidateJIT = true;

    // -- No thermal cluster by default, since 3.6.3348

    // -- Code for creating a new thermal cluster
    // if (JIT::usedFromGUI)
    // {
    // 	if (thermal.list.empty())
    // 	{
    // 		ThermalCluster* ag = new ThermalCluster(this);
    // 		if (!ag)
    // 		{
    // 			logs.error() << "Impossible to allocate in memory a new thermal cluster.";
    // 			return;
    // 		}
    // 		ag->reset();
    // 		ag->name("default");

    // 		thermal.list.add(ag);
    // 		thermal.list.rebuildIndex();
    // 		thermal.list.ensureDataPrepro();
    // 		ThermalClusterListEnsureDataTimeSeries(&thermal.list);
    // 	}
    // }
}

void Area::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number

    // asserts
    assert(load.series and "load.series must not be nullptr !");
    assert(solar.series and "solar.series must not be nullptr !");
    assert(wind.series and "wind.series must not be nullptr !");
    assert(hydro.series and "series must not be nullptr !");

    if (!n)
    {
        load.series->timeseriesNumbers.clear();
        solar.series->timeseriesNumbers.clear();
        wind.series->timeseriesNumbers.clear();
        hydro.series->timeseriesNumbers.clear();
    }
    else
    {
        load.series->timeseriesNumbers.resize(1, n);
        solar.series->timeseriesNumbers.resize(1, n);
        wind.series->timeseriesNumbers.resize(1, n);
        hydro.series->timeseriesNumbers.resize(1, n);
    }
    thermal.resizeAllTimeseriesNumbers(n);
    renewable.resizeAllTimeseriesNumbers(n);
}

void Area::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(Area);

    // reserves
    Matrix<>::EstimateMemoryUsage(u, fhrMax, HOURS_PER_YEAR);
    // Misc Gen.
    Matrix<>::EstimateMemoryUsage(u, fhhMax, HOURS_PER_YEAR);

    // Load
    if (load.series)
        load.series->estimateMemoryUsage(u);
    if (load.prepro)
        load.prepro->estimateMemoryUsage(u);
    // Solar
    if (solar.series)
        solar.series->estimateMemoryUsage(u);
    if (solar.prepro)
        solar.prepro->estimateMemoryUsage(u);
    // Wind
    if (wind.series)
        wind.series->estimateMemoryUsage(u);
    if (wind.prepro)
        wind.prepro->estimateMemoryUsage(u);

    // Hydro
    if (hydro.series)
        hydro.series->estimateMemoryUsage(u);
    if (hydro.prepro)
        hydro.prepro->estimateMemoryUsage(u);

    // Thermal
    thermal.estimateMemoryUsage(u);

    // Renewable
    renewable.estimateMemoryUsage(u);

    // Scratchpad
    u.requiredMemoryForInput += sizeof(AreaScratchpad) * u.nbYearsParallel;

    // Links
    if (not links.empty())
    {
        u.requiredMemoryForInput += (sizeof(AreaLink*) * 2) * links.size();
        auto end = links.end();
        for (auto i = links.begin(); i != end; ++i)
            (i->second)->estimateMemoryUsage(u);
    }

    if (u.swappingSupport)
    {
        // + something
        u.requiredMemoryForInput += ((/*1.5*/ 15 * 1024 * 1024) / 10);
    }
}

bool Area::thermalClustersMinStablePowerValidity(std::vector<YString>& output) const
{
    bool noErrorMinStabPow = true;
    for (uint l = 0; l != thermal.clusterCount; ++l)
    {
        auto& cluster = thermal.clusters[l];
        logs.debug() << "cluster : " << cluster->name();
        if ((not cluster->checkMinStablePower())
            || (cluster->minStablePower
                > cluster->nominalCapacity * (1 - cluster->spinning / 100.)))
        {
            output.push_back(cluster->name());
            noErrorMinStabPow = false;
        }
    }
    return noErrorMinStabPow;
}

bool Area::invalidate(bool reload) const
{
    // To not break the entire constness design of the library
    // this method should remain const event if the operations
    // performed are obviously not const
    auto& self = *(const_cast<Area*>(this));

    bool ret = true;
    invalidateJIT = false;

    // Misc Gen
    ret = self.miscGen.invalidate(reload) and ret;
    // Reserves
    ret = self.reserves.invalidate(reload) and ret;

    // Load
    ret = self.load.invalidate(reload) and ret;
    // Solar
    ret = self.solar.invalidate(reload) and ret;
    // Hydro
    ret = self.hydro.invalidate(reload) and ret;
    // Wind
    ret = self.wind.invalidate(reload) and ret;
    // Thermal
    ret = self.thermal.invalidate(reload) and ret;
    // Renewable
    ret = self.renewable.invalidate(reload) and ret;
    if (not links.empty())
    {
        auto end = self.links.end();
        for (auto i = self.links.begin(); i != end; ++i)
            ret = (i->second)->invalidate(reload) and ret;
    }

    if (ui)
        self.ui->markAsModified();

    return ret;
}

void Area::markAsModified() const
{
    // Misc Gen
    miscGen.markAsModified();
    // Reserves
    reserves.markAsModified();

    // Load
    load.markAsModified();
    // Solar
    solar.markAsModified();
    // Hydro
    hydro.markAsModified();
    // Wind
    wind.markAsModified();
    // Thermal
    thermal.markAsModified();
    // Renewable
    renewable.markAsModified();

    if (not links.empty())
    {
        auto end = links.end();
        for (auto i = links.begin(); i != end; ++i)
            (i->second)->markAsModified();
    }
    if (ui)
        ui->markAsModified();
}

void Area::detachLinkFromID(const AreaName& id)
{
    auto i = links.find(id);
    if (i != links.end())
        links.erase(i);
}

void Area::detachLink(const AreaLink* lnk)
{
    assert(lnk);
    assert(lnk->from);
    assert(lnk->with);

    lnk->from->detachLinkFromID(lnk->with->id);
}

AreaLink* Area::findLinkByID(const AreaName& id)
{
    auto i = links.find(id);
    return (i != links.end()) ? i->second : nullptr;
}

const AreaLink* Area::findLinkByID(const AreaName& id) const
{
    auto i = links.find(id);
    return (i != links.end()) ? i->second : nullptr;
}

} // namespace Data
} // namespace Antares
