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
