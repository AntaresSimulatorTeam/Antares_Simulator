// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/renewable/cluster.h"

#include <cassert>
#include <cmath>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares::Data
{
Data::RenewableCluster::RenewableCluster(Area* parent):
    Cluster(parent)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

void Data::RenewableCluster::copyFrom(const RenewableCluster& cluster)
{
    // Note: In this method, only the data can be copied (and not the name or
    //   the ID for example)

    // group
    setGroup(cluster.getGroup());

    // Enabled
    enabled = cluster.enabled;
    // unit count
    unitCount = cluster.unitCount;
    // nominal capacity
    nominalCapacity = cluster.nominalCapacity;

    // ts-mode
    tsMode = cluster.tsMode;

    // timseries
    series.timeSeries = cluster.series.timeSeries;
    cluster.series.timeSeries.unloadFromMemory();
    series.timeseriesNumbers.clear();

    // The parent must be invalidated to make sure that the clusters are really
    // re-written at the next 'Save' from the user interface.
    if (parentArea)
    {
        parentArea->forceReload();
    }
}

bool Data::RenewableCluster::forceReload(bool reload) const
{
    return series.forceReload(reload);
}

void Data::RenewableCluster::markAsModified() const
{
    series.markAsModified();
}

void Data::RenewableCluster::reset()
{
    Cluster::reset();
}

bool Data::RenewableCluster::integrityCheck()
{
    if (not parentArea)
    {
        logs.error() << "Renewable cluster " << pName << ": The parent area is missing";
        return false;
    }

    bool ret = true;

    if (nominalCapacity < 0.)
    {
        logs.error() << "Renewable cluster " << parentArea->name << "/" << pName
                     << ": The Nominal capacity must be positive or null";
        nominalCapacity = 0.;
        ret = false;
    }
    return ret;
}

bool Data::RenewableCluster::setTimeSeriesModeFromString(const YString& value)
{
    if (value == "power-generation")
    {
        tsMode = powerGeneration;
        return true;
    }
    if (value == "production-factor")
    {
        tsMode = productionFactor;
        return true;
    }
    return false;
}

YString Data::RenewableCluster::getTimeSeriesModeAsString() const
{
    switch (tsMode)
    {
    case powerGeneration:
        return "power-generation";
    case productionFactor:
        return "production-factor";
    }
    return "unknown";
}

double RenewableCluster::valueAtTimeStep(uint year, uint hourInYear) const
{
    if (!enabled)
    {
        return 0.;
    }

    const double tsValue = series.getCoefficient(year, hourInYear);
    switch (tsMode)
    {
    case powerGeneration:
        return std::round(tsValue);
    case productionFactor:
        return std::round(unitCount * nominalCapacity * tsValue);
    }
    return 0.;
}

unsigned int RenewableCluster::precision() const
{
    return 4;
}
} // namespace Antares::Data
