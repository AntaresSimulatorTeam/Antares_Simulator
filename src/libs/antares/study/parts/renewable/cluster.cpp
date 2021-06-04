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
Data::RenewableCluster::RenewableCluster(Area* parent, uint /*nbParallelYears*/) :
 Cluster(parent),
 groupID(renewableOther1),
 areaWideIndex((uint)-1)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::RenewableCluster(Area* parent) :
 Cluster(parent),
 groupID(renewableOther1),
 areaWideIndex((uint)-1)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::~RenewableCluster()
{
    delete series;
}

void RenewableCluster::flush()
{
#ifdef ANTARES_SWAP_SUPPORT
    if (series)
        series->flush();
#endif
}

void Data::RenewableCluster::invalidateArea()
{
    if (parentArea)
        parentArea->invalidate();
}

String Antares::Data::RenewableCluster::getFullName() const
{
    String s;
    s << parentArea->name << "." << pID;
    return s;
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

    // nominal capacity
    nominalCapacity = cluster.nominalCapacity;

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
        parentArea->invalidate();
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

    if (newgrp == "concentration solar")
    {
        groupID = concentrationSolar;
        return;
    }
    if (newgrp == "pv solar")
    {
        groupID = PVSolar;
        return;
    }
    if (newgrp == "rooftop solar")
    {
        groupID = rooftopSolar;
        return;
    }
    if (newgrp == "wind on-shore")
    {
        groupID = windOnShore;
        return;
    }
    if (newgrp == "wind off-shore")
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

bool RenewableClusterListSaveToFolder(const RenewableClusterList* l, const AnyString& folder)
{
    // Assert
    assert(l);

    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        l->each([&](const Data::Cluster& cluster) {
            // Adding a section to the inifile
            IniFile::Section* s = ini.addSection(cluster.name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", cluster.name());

            if (not cluster.group().empty())
                s->add("group", cluster.group());
            if (not cluster.enabled)
                s->add("enabled", "false");
        });

        // Write the ini file
        buffer.clear() << folder << SEP << "list.ini";
        ret = ini.save(buffer) and ret;
    }
    else
    {
        logs.error() << "I/O Error: impossible to create '" << folder << "'";
        return false;
    }

    return true;
}

int RenewableClusterListSaveDataSeriesToFolder(const RenewableClusterList* l,
                                               const AnyString& folder)
{
    if (l->empty())
        return 1;

    int ret = 1;

    auto end = l->cluster.end();
    for (auto it = l->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (cluster.series)
            ret = DataSeriesSaveToFolder(cluster.series, &cluster, folder) and ret;
    }
    return ret;
}

int RenewableClusterListSaveDataSeriesToFolder(const RenewableClusterList* l,
                                               const AnyString& folder,
                                               const String& msg)
{
    if (l->empty())
        return 1;

    int ret = 1;
    uint ticks = 0;

    auto end = l->cluster.end();
    for (auto it = l->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (cluster.series)
        {
            logs.info() << msg << "  " << (ticks * 100 / (1 + l->cluster.size())) << "% complete";
            ret = DataSeriesSaveToFolder(cluster.series, &cluster, folder) and ret;
        }
        ++ticks;
    }
    return ret;
}

int RenewableClusterListLoadDataSeriesFromFolder(Study& s,
                                                 const StudyLoadOptions& options,
                                                 RenewableClusterList* l,
                                                 const AnyString& folder)
{
    if (l->empty())
        return 1;

    int ret = 1;

    l->each([&](Data::RenewableCluster& cluster) {
        if (cluster.series)
            ret = DataSeriesLoadFromFolder(s, cluster.series, &cluster, folder) and ret;

        ++options.progressTicks;
        options.pushProgressLogs();
    });
    return ret;
}

void RenewableClusterListEnsureDataTimeSeries(RenewableClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.series)
            cluster.series = new DataSeriesCommon();
    }
}

bool Data::RenewableCluster::invalidate(bool reload) const
{
    bool ret = true;
    if (series)
        ret = series->invalidate(reload) and ret;
    return ret;
}

void Data::RenewableCluster::markAsModified() const
{
    if (series)
        series->markAsModified();
}

void Data::RenewableCluster::reset()
{
    enabled = true;
    nominalCapacity = 0.;

    // timeseries
    // warning: the variables `series` __must__ not be destroyed
    //   since the interface may still have a pointer to them.
    //   we must simply reset their content.
    if (not series)
        series = new DataSeriesCommon();

    series->series.reset(1, HOURS_PER_YEAR);
    series->series.flush();
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
        return "Wind off-shore";
    case windOnShore:
        return "Wind on-shore";
    case concentrationSolar:
        return "Concentration solar";
    case PVSolar:
        return "PV solar";
    case rooftopSolar:
        return "Rooftop solar";
    case renewableOther1:
        return "Renewable 1";
    case renewableOther2:
        return "Renewable 2";
    case renewableOther3:
        return "Renewable 3";
    case renewableOther4:
        return "Renewable 4";
    case renewableGroupMax:
        return "";
    }
    return "";
}

namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32 operator()(uint32 value) const
    {
        return value + 1;
    }
};

} // anonymous namespace

uint64 RenewableCluster::memoryUsage() const
{
    uint64 amount = sizeof(RenewableCluster);
    if (series)
        amount += DataSeriesMemoryUsage(series);
    return amount;
}

bool RenewableCluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

} // namespace Data
} // namespace Antares
