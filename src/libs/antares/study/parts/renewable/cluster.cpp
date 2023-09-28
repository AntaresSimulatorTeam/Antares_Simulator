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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/core/math.h>
#include <cassert>
#include <cmath>
#include <boost/algorithm/string/case_conv.hpp>
#include "../../study.h"
#include "cluster.h"
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
Data::RenewableCluster::RenewableCluster(Area* parent) :
 Cluster(parent), groupID(renewableOther1), tsMode(powerGeneration)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::~RenewableCluster()
{
    delete series;
}

uint RenewableCluster::groupId() const
{
    return groupID;
}

void Data::RenewableCluster::copyFrom(const RenewableCluster& cluster)
{
    // Note: In this method, only the data can be copied (and not the name or
    //   the ID for example)

    // group
    groupID = cluster.groupID;
    pGroup = cluster.pGroup;

    // Enabled
    enabled = cluster.enabled;
    // unit count
    unitCount = cluster.unitCount;
    // nominal capacity
    nominalCapacity = cluster.nominalCapacity;

    // ts-mode
    tsMode = cluster.tsMode;

    // Making sure that the data related to the timeseries are present
    if (not series)
        series = new DataSeriesCommon();

    // timseries
    series->timeSeries = cluster.series->timeSeries;
    cluster.series->timeSeries.unloadFromMemory();
    series->timeseriesNumbers.clear();

    // The parent must be invalidated to make sure that the clusters are really
    // re-written at the next 'Save' from the user interface.
    if (parentArea)
        parentArea->forceReload();
}

const std::map < RenewableCluster::RenewableGroup, const char* > groupToName =
{
    {RenewableCluster::thermalSolar, "solar thermal"},
    {RenewableCluster::PVSolar, "solar pv"},
    {RenewableCluster::rooftopSolar, "solar rooftop"},
    {RenewableCluster::windOnShore, "wind onshore"},
    {RenewableCluster::windOffShore,"wind offshore"},
    {RenewableCluster::renewableOther1, "other res 1"},
    {RenewableCluster::renewableOther2, "other res 2"},
    {RenewableCluster::renewableOther3, "other res 3"},
    {RenewableCluster::renewableOther4, "other res 4"}
};

void Data::RenewableCluster::setGroup(Data::ClusterName newgrp)
{
    if (newgrp.empty())
    {
        groupID = renewableOther1;
        pGroup.clear();
        return;
    }
    pGroup = newgrp;
    boost::to_lower(newgrp);

    for (const auto& [group, name] : groupToName)
    {
        if (newgrp == name)
        {
            groupID = group;
            return;
        }        
    }

    // assigning a default value
    groupID = renewableOther1;
}

bool Data::RenewableCluster::forceReload(bool reload) const
{
    bool ret = true;
    if (series)
        ret = series->forceReload(reload) and ret;
    return ret;
}

void Data::RenewableCluster::markAsModified() const
{
    if (series)
        series->markAsModified();
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

double RenewableCluster::valueAtTimeStep(uint year, uint timeStepIndex) const
{
    if (!enabled)
        return 0.;
    uint timeSeriesIndex = (series->timeSeries.width == 1) ? 0 : series->timeseriesNumbers[0][year];

    assert(timeStepIndex < series->timeSeries.height);
    assert(timeSeriesIndex < series->timeSeries.width);
    const double tsValue = series->timeSeries[timeSeriesIndex][timeStepIndex];
    switch (tsMode)
    {
    case powerGeneration:
        return std::round(tsValue);
    case productionFactor:
        return std::round(unitCount * nominalCapacity * tsValue);
    }
    return 0.;
}

uint64_t RenewableCluster::memoryUsage() const
{
    uint64_t amount = sizeof(RenewableCluster);
    if (series)
        amount += series->memoryUsage();
    return amount;
}

unsigned int RenewableCluster::precision() const
{
    return 4;
}
} // namespace Data
} // namespace Antares
