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
#include <yuni/io/file.h>
#include <yuni/core/math.h>
#include <cassert>
#include "../../study.h"
#include "../../memory-usage.h"
#include "cluster.h"
#include "../../../inifile.h"
#include "../../../logs.h"
#include "../../../utils.h"

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
    series->series = cluster.series->series;
    cluster.series->series.unloadFromMemory();
    series->timeseriesNumbers.clear();

    // The parent must be invalidated to make sure that the clusters are really
    // re-written at the next 'Save' from the user interface.
    if (parentArea)
        parentArea->forceReload();
}

void Data::RenewableCluster::setGroup(Data::ClusterName newgrp)
{
    if (not newgrp)
    {
        groupID = renewableOther1;
        pGroup.clear();
        return;
    }
    pGroup = newgrp;
    newgrp.toLower();

    if (newgrp == "solar thermal")
    {
        groupID = thermalSolar;
        return;
    }
    if (newgrp == "solar pv")
    {
        groupID = PVSolar;
        return;
    }
    if (newgrp == "solar rooftop")
    {
        groupID = rooftopSolar;
        return;
    }
    if (newgrp == "wind onshore")
    {
        groupID = windOnShore;
        return;
    }
    if (newgrp == "wind offshore")
    {
        groupID = windOffShore;
        return;
    }
    if (newgrp == "other renewable 1")
    {
        groupID = renewableOther1;
        return;
    }
    if (newgrp == "other renewable 2")
    {
        groupID = renewableOther2;
        return;
    }
    if (newgrp == "other renewable 3")
    {
        groupID = renewableOther3;
        return;
    }
    if (newgrp == "other renewable 4")
    {
        groupID = renewableOther4;
        return;
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

const char* Data::RenewableCluster::GroupName(enum RenewableGroup grp)
{
    switch (grp)
    {
    case windOffShore:
        return "Wind offshore";
    case windOnShore:
        return "Wind onshore";
    case thermalSolar:
        return "Solar thermal";
    case PVSolar:
        return "Solar PV";
    case rooftopSolar:
        return "Solar rooftop";
    case renewableOther1:
        return "Other RES 1";
    case renewableOther2:
        return "Other RES 2";
    case renewableOther3:
        return "Other RES 3";
    case renewableOther4:
        return "Other RES 4";
    case groupMax:
        return "";
    }
    return "";
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

double RenewableCluster::valueAtTimeStep(uint timeSeriesIndex, uint timeStepIndex) const
{
    if (!enabled)
        return 0.;

    assert(timeStepIndex < series->series.height);
    assert(timeSeriesIndex < series->series.width);
    const double tsValue = series->series[timeSeriesIndex][timeStepIndex];
    switch (tsMode)
    {
    case powerGeneration:
        return tsValue;
    case productionFactor:
        return unitCount * nominalCapacity * tsValue;
    }
    return 0.;
}

uint64 RenewableCluster::memoryUsage() const
{
    uint64 amount = sizeof(RenewableCluster);
    if (series)
        amount += DataSeriesMemoryUsage(series);
    return amount;
}

unsigned int RenewableCluster::precision() const
{
    return 4;
}
} // namespace Data
} // namespace Antares
