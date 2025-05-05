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

#include "antares/study/area/area.h"

#include <cassert>

#include <yuni/yuni.h>

#include "antares/study//study.h"
#include "antares/study/area/scratchpad.h"
#include "antares/study/area/ui.h"
#include "antares/study/parts/load/prepro.h"
#include "antares/utils/utils.h"

using namespace Yuni;

namespace Antares::Data
{
void Area::internalInitialize()
{
    // Make sure we have
    if (JIT::usedFromGUI)
    {
        ui = std::make_unique<AreaUI>();
    }
}

Area::Area():
    reserves(fhrMax, HOURS_PER_YEAR),
    miscGen(fhhMax, HOURS_PER_YEAR)
{
    internalInitialize();
}

Area::Area(const AnyString& name):
    Area()
{
    internalInitialize();
    this->name = name;
    this->id = Antares::transformNameIntoID(this->name);
}

Area::Area(const AnyString& name, const AnyString& id):
    Area()
{
    internalInitialize();
    this->name = name;
    this->id = Antares::transformNameIntoID(id);
}

Area::~Area()
{
    logs.debug() << "  :: destroying area " << name;

    // Delete all links
    clearAllLinks();

    reserves.clear();
    miscGen.clear();
}

void Area::clearAllLinks()
{
    if (not links.empty())
    {
        // Delete all links
        {
            auto end = links.end();
            for (auto i = links.begin(); i != end; ++i)
            {
                delete i->second;
            }
        }
        // Empty the container
        links.clear();
    }
}

void Area::detachAllLinks()
{
    while (not links.empty())
    {
        AreaLinkRemove((links.begin())->second);
    }
}

AreaLink* Area::findExistingLinkWith(Area& with)
{
    if (&with == this)
    {
        return nullptr;
    }

    if (not links.empty())
    {
        const AreaLink::Map::iterator end = links.end();
        for (AreaLink::Map::iterator i = links.begin(); i != end; ++i)
        {
            if (i->second->from == &with or i->second->with == &with)
            {
                return i->second;
            }
        }
    }
    if (!with.links.empty())
    {
        for (auto i = with.links.begin(); i != with.links.end(); ++i)
        {
            if (i->second->from == this or i->second->with == this)
            {
                return i->second;
            }
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
            const auto end = links.end();
            for (auto i = links.begin(); i != end; ++i)
            {
                if (i->second->from == &with or i->second->with == &with)
                {
                    return i->second;
                }
            }
        }
        if (!with.links.empty())
        {
            const auto end = with.links.end();
            for (auto i = with.links.begin(); i != end; ++i)
            {
                if (i->second->from == this or i->second->with == this)
                {
                    return i->second;
                }
            }
        }
    }
    return nullptr;
}

void Area::createMissingData()
{
    createMissingTimeSeries();
    createMissingPrepros();
}

void Area::createMissingTimeSeries()
{
    if (!hydro.series)
    {
        hydro.series = std::make_unique<DataSeriesHydro>();
    }
}

void Area::createMissingPrepros()
{
    if (!load.prepro)
    {
        load.prepro = std::make_unique<Data::Load::Prepro>();
    }
    if (!solar.prepro)
    {
        solar.prepro = std::make_unique<Data::Solar::Prepro>();
    }
    if (!wind.prepro)
    {
        wind.prepro = std::make_unique<Data::Wind::Prepro>();
    }
    if (!hydro.prepro)
    {
        hydro.prepro = std::make_unique<PreproHydro>();
    }
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
}

void Area::resizeAllTimeseriesNumbers(uint nbYears)
{
    assert(hydro.series and "series must not be nullptr !");

    load.series.timeseriesNumbers.reset(nbYears);
    solar.series.timeseriesNumbers.reset(nbYears);
    wind.series.timeseriesNumbers.reset(nbYears);
    hydro.series->timeseriesNumbers.reset(nbYears);
    for (auto& namedLink: links)
    {
        AreaLink* link = namedLink.second;
        link->timeseriesNumbers.reset(nbYears);
    }
    thermal.resizeAllTimeseriesNumbers(nbYears);
    renewable.resizeAllTimeseriesNumbers(nbYears);
}

bool Area::thermalClustersMinStablePowerValidity(std::vector<YString>& output) const
{
    bool noErrorMinStabPow = true;
    for (auto& cluster: thermal.list.each_enabled())
    {
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

bool Area::forceReload(bool reload) const
{
    // To not break the entire constness design of the library
    // this method should remain const event if the operations
    // performed are obviously not const
    auto& self = *(const_cast<Area*>(this));

    bool ret = true;
    invalidateJIT = false;

    // Misc Gen
    ret = self.miscGen.forceReload(reload) and ret;
    // Reserves
    ret = self.reserves.forceReload(reload) and ret;

    // Load
    ret = self.load.forceReload(reload) and ret;
    // Solar
    ret = self.solar.forceReload(reload) and ret;
    // Hydro
    ret = self.hydro.forceReload(reload) and ret;
    // Wind
    ret = self.wind.forceReload(reload) and ret;
    // Thermal
    ret = self.thermal.forceReload(reload) and ret;
    // Renewable
    ret = self.renewable.forceReload(reload) and ret;
    if (not links.empty())
    {
        auto end = self.links.end();
        for (auto i = self.links.begin(); i != end; ++i)
        {
            ret = (i->second)->forceReload(reload) and ret;
        }
    }

    if (ui)
    {
        self.ui->markAsModified();
    }

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
        {
            (i->second)->markAsModified();
        }
    }
    if (ui)
    {
        ui->markAsModified();
    }
}

void Area::detachLinkFromID(const AreaName& id)
{
    auto i = links.find(id);
    if (i != links.end())
    {
        links.erase(i);
    }
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

void Area::buildLinksIndexes()
{
    uint areaIndx = 0;

    auto end = links.end();
    for (auto i = links.begin(); i != end; ++i)
    {
        auto* link = i->second;
        link->indexForArea = areaIndx;
        ++areaIndx;
    }
}

} // namespace Antares::Data
