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
static bool RenewableClusterLoadFromProperty(RenewableCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;

    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);

    // The property is unknown
    return false;
}

static bool RenewableClusterLoadFromSection(const AnyString& filename,
                                            RenewableCluster& cluster,
                                            const IniFile::Section& section)
{
    if (section.name.empty())
        return false;

    cluster.name(section.name);

    if (section.firstProperty)
    {
        // Browse all properties
        for (auto* property = section.firstProperty; property; property = property->next)
        {
            if (property->key.empty())
            {
                logs.warning() << '`' << filename << "`: `" << section.name
                               << "`: Invalid key/value";
                continue;
            }
            if (not RenewableClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->value << "`: The property is unknown and ignored";
            }
        }
        // update the minUpDownTime
    }
    return true;
}

Data::RenewableCluster::RenewableCluster(Area* parent, uint nbParallelYears) :
 groupID(renewableOther),
 index(0),
 areaWideIndex((uint)-1),
 parentArea(parent),
 enabled(true),
 nominalCapacity(0.),
 series(nullptr)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::RenewableCluster(Area* parent) :
 groupID(renewableOther),
 index(0),
 areaWideIndex((uint)-1),
 parentArea(parent),
 enabled(true),
 nominalCapacity(0.),
 series(nullptr)
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::~RenewableCluster()
{
    delete series;
}

#ifdef ANTARES_SWAP_SUPPORT
void RenewableCluster::flush()
{
    if (series)
        series->flush();
}
#endif

#ifdef ANTARES_SWAP_SUPPORT
void RenewableClusterList::flush()
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
        i->second->flush();
}
#endif

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
        series = new DataSeriesRenewable();

    // timseries
    series->series = cluster.series->series;
    cluster.series->series.unloadFromMemory();
    series->timeseriesNumbers.clear();

    // The parent must be invalidated to make sure that the clusters are really
    // re-written at the next 'Save' from the user interface.
    if (parentArea)
        parentArea->invalidate();
}

Data::RenewableClusterList::RenewableClusterList() : byIndex(nullptr)
{
    (void)::memset(&groupCount, 0, sizeof(groupCount));
}

Data::RenewableClusterList::~RenewableClusterList()
{
    // deleting all renewable clusters
    clear();
}

void RenewableClusterList::clear()
{
    if (byIndex)
    {
        delete[] byIndex;
        byIndex = nullptr;
    }

    auto end = mapping.end();
    for (auto it = mapping.begin(); it != end; ++it)
        delete it->second;
    mapping.clear();

    if (not cluster.empty())
        cluster.clear();
}

const RenewableCluster* RenewableClusterList::find(const RenewableCluster* p) const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

Data::RenewableCluster* RenewableClusterList::find(const RenewableCluster* p)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

void RenewableClusterList::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number
    if (not cluster.empty())
    {
        if (0 == n)
        {
            each([&](RenewableCluster& cluster) { cluster.series->timeseriesNumbers.clear(); });
        }
        else
        {
            each(
              [&](RenewableCluster& cluster) { cluster.series->timeseriesNumbers.resize(1, n); });
        }
    }
}

void RenewableClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const RenewableCluster& cluster) {
        u.requiredMemoryForInput += sizeof(RenewableCluster);
        u.requiredMemoryForInput += sizeof(void*);
        if (cluster.series)
            cluster.series->estimateMemoryUsage(u);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

void Data::RenewableCluster::group(Data::RenewableClusterName newgrp)
{
    if (not newgrp)
    {
        groupID = renewableOther;
        pGroup.clear();
        return;
    }
    pGroup = newgrp;
    newgrp.toLower();

    switch (newgrp[0])
    {
    case 'c':
    {
        if (newgrp == "concentration solar")
        {
            groupID = concentrationSolar;
            return;
        }
        break;
    }
    case 'p':
    {
        if (newgrp == "pv solar")
        {
            groupID = PVSolar;
            return;
        }
        break;
    }
    case 'w':
    {
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
        break;
    }
    }
    // assigning a default value
    groupID = renewableOther;
}

void RenewableClusterList::rebuildIndex()
{
    delete[] byIndex;

    if (not empty())
    {
        uint indx = 0;
        typedef RenewableCluster* RenewableClusterWeakPtr;
        byIndex = new RenewableClusterWeakPtr[size()];

        auto end = cluster.end();
        for (auto i = cluster.begin(); i != end; ++i)
        {
            auto* cluster = i->second;
            byIndex[indx] = cluster;
            cluster->index = indx;
            ++indx;
        }
    }
    else
        byIndex = nullptr;
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
        l->each([&](const Data::RenewableCluster& cluster) {
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

bool RenewableClusterList::add(RenewableCluster* newcluster)
{
    if (newcluster)
    {
        if (exists(newcluster->id()))
            return true;

        newcluster->index = (uint)size();
        cluster[newcluster->id()] = newcluster;
        ++(groupCount[newcluster->groupID]);
        rebuildIndex();
        return true;
    }
    return false;
}

bool RenewableClusterList::loadFromFolder(Study& study, const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading renewable configuration for the area " << area->name;

    // Open the ini file
    study.buffer.clear() << folder << SEP << "list.ini";
    IniFile ini;
    if (ini.open(study.buffer))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                    continue;

                auto* cluster = new RenewableCluster(area, study.maxNbYearsInParallel);

                // Load data of a renewable cluster from a ini file section
                if (not RenewableClusterLoadFromSection(study.buffer, *cluster, *section))
                {
                    delete cluster;
                    continue;
                }

                if (study.header.version < 390)
                {
                    // We may have some strange name/id in older studies
                    // temporary reverting to the old naming convention
                    cluster->pID = cluster->name();
                    cluster->pID.toLower();
                }

                // Check the data integrity of the cluster
                cluster->integrityCheck();

                // adding the renewable cluster
                if (not add(cluster))
                {
                    // This error should never happen
                    logs.error() << "Impossible to add the renewable cluster '" << cluster->name()
                                 << "'";
                    delete cluster;
                    continue;
                }
                // keeping track of the cluster
                mapping[cluster->id()] = cluster;

                cluster->flush();
            }
        }

        return ret;
    }
    return false;
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
            ret = DataSeriesRenewableSaveToFolder(cluster.series, &cluster, folder) and ret;
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

    auto end = l->mapping.end();
    for (auto it = l->mapping.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (cluster.series)
        {
            logs.info() << msg << "  " << (ticks * 100 / (1 + l->mapping.size())) << "% complete";
            ret = DataSeriesRenewableSaveToFolder(cluster.series, &cluster, folder) and ret;
        }
        ++ticks;
    }
    return ret;
}

int RenewableClusterListLoadDataSeriesFromFolder(Study& s,
                                                 const StudyLoadOptions& options,
                                                 RenewableClusterList* l,
                                                 const AnyString& folder,
                                                 int fast)
{
    if (l->empty())
        return 1;

    int ret = 1;

    l->each([&](Data::RenewableCluster& cluster) {
        if (cluster.series and !fast)
            ret = DataSeriesRenewableLoadFromFolder(s, cluster.series, &cluster, folder) and ret;

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
            cluster.series = new DataSeriesRenewable();
    }
}

Yuni::uint64 RenewableClusterList::memoryUsage() const
{
    uint64 ret = sizeof(RenewableClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const Data::RenewableCluster& cluster) { ret += cluster.memoryUsage(); });
    return ret;
}

bool RenewableClusterList::rename(Data::RenewableClusterName idToFind,
                                  Data::RenewableClusterName newName)
{
    if (not idToFind or newName.empty())
        return false;

    // Internal:
    // It is vital to make copy of these strings. We can not make assumption that these
    // CString are not from the same buffer (name, id) than ours.
    // It may have an undefined behavior.
    // Consequently, the parameters `idToFind` and `newName` shall not be `const &`.

    // Making sure that the id is lowercase
    idToFind.toLower();

    // The new ID
    Data::RenewableClusterName newID;
    TransformNameIntoID(newName, newID);

    // Looking for the renewable cluster in the list
    auto it = cluster.find(idToFind);
    if (it == cluster.end())
        return true;

    Data::RenewableCluster* p = it->second;

    if (idToFind == newID)
    {
        p->name(newName);
        return true;
    }

    // The name is the same. Aborting nicely.
    if (p->name() == newName)
        return true;

    // Already exist
    if (this->exists(newID))
        return false;

    cluster.erase(it);

    p->name(newName);
    cluster[newID] = p;

    // Invalidate matrices attached to the area
    // It is a bit excessive (all matrices not only those related to the renewable cluster)
    // will be rewritten but currently it is the less error-prone.
    if (p->parentArea)
        (p->parentArea)->invalidateJIT = true;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

bool Data::RenewableClusterList::invalidate(bool reload) const
{
    bool ret = true;
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        ret = (i->second)->invalidate(reload) and ret;
    return ret;
}

bool Data::RenewableCluster::invalidate(bool reload) const
{
    bool ret = true;
    if (series)
        ret = series->invalidate(reload) and ret;
    return ret;
}

void Data::RenewableClusterList::markAsModified() const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        (i->second)->markAsModified();
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
        series = new DataSeriesRenewable();

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

bool RenewableClusterList::storeTimeseriesNumbers(Study& study)
{
    if (cluster.empty())
        return true;

    bool ret = true;
    TSNumbersPredicate predicate;

    each([&](const Data::RenewableCluster& cluster) {
        study.buffer = study.folderOutput;
        study.buffer << SEP << "ts-numbers" << SEP << "renewable" << SEP << cluster.parentArea->id
                     << SEP << cluster.id() << ".txt";
        ret = cluster.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate)
              and ret;
    });
    return ret;
}

void RenewableClusterList::retrieveTotalCapacity(double& total) const
{
    total = 0.;

    if (not cluster.empty())
    {
        auto end = cluster.cend();
        for (auto i = cluster.cbegin(); i != end; ++i)
        {
            if (not i->second)
                return;

            // Reference to the renewable cluster
            auto& cluster = *(i->second);
            total += cluster.nominalCapacity;
        }
    }
}

bool RenewableClusterList::remove(const Data::RenewableClusterName& id)
{
    auto i = cluster.find(id);
    if (i == cluster.end())
        return false;

    // Getting the pointer on the cluster
    auto* c = i->second;

    // Removing it from the list
    cluster.erase(i);
    // Invalidating the parent area
    c->parentArea->invalidate();

    // delete the cluster
    delete c;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

RenewableCluster* RenewableClusterList::detach(iterator i)
{
    auto* c = i->second;
    cluster.erase(i);
    return c;
}

void RenewableClusterList::remove(iterator i)
{
    cluster.erase(i);
}

bool RenewableClusterList::exists(const Data::RenewableClusterName& id) const
{
    if (not cluster.empty())
    {
        auto element = cluster.find(id);
        return (element != cluster.end());
    }
    return false;
}

uint64 RenewableCluster::memoryUsage() const
{
    uint64 amount = sizeof(RenewableCluster);
    if (series)
        amount += DataSeriesRenewableMemoryUsage(series);
    return amount;
}

void RenewableCluster::name(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    TransformNameIntoID(pName, pID);
}

bool RenewableCluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

} // namespace Data
} // namespace Antares
