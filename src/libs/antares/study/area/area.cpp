// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/area/area.h"

#include <cassert>

#include <yuni/yuni.h>

#include "antares/study/area/scratchpad.h"
#include "antares/study/parts/load/prepro.h"
#include "antares/study/study.h"
#include "antares/utils/utils.h"

using namespace Yuni;

namespace Antares::Data
{
Area::Area():
    reserves(fhrMax, HOURS_PER_YEAR),
    miscGen(fhhMax, HOURS_PER_YEAR)
{
}

Area::Area(const AnyString& name):
    Area()
{
    this->name = name;
    this->id = Antares::transformNameIntoID(this->name);
}

Area::Area(const AnyString& name, const AnyString& id):
    Area()
{
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

const AreaLink* Area::findExistingLinkWith(const Area& with) const
{
    if (&with == this)
    {
        return nullptr;
    }

    for (const auto& [key, link]: links)
    {
        if (link->from == &with || link->with == &with)
        {
            return link;
        }
    }

    for (const auto& [key, link]: with.links)
    {
        if (link->from == this || link->with == this)
        {
            return link;
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
    //  invalidate the whole area
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
    shortTermStorage.resizeTimeseriesNumbers(nbYears);
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
