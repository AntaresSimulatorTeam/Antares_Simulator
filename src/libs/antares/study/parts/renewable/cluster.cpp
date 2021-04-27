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
        cluster.minUpDownTime = Math::Max(cluster.minUpTime, cluster.minDownTime);
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
 prepro(nullptr),
 series(nullptr),
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
 prepro(nullptr),
 series(nullptr),
{
    // assert
    assert(parent and "A parent for a renewable dispatchable cluster can not be null");
}

Data::RenewableCluster::~RenewableCluster()
{
    delete prepro;
    delete series;
}

#ifdef ANTARES_SWAP_SUPPORT
void RenewableCluster::flush()
{
    if (prepro)
        prepro->flush();
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

    // production cost, even if this variable should not be used
    if (productionCost)
    {
        if (cluster.productionCost)
            memcpy(productionCost, cluster.productionCost, HOURS_PER_YEAR * sizeof(double));
        else
            memset(productionCost, 0, HOURS_PER_YEAR * sizeof(double));
    }

    // mustrun
    mustrun = cluster.mustrun;
    mustrunOrigin = cluster.mustrunOrigin;

    // group
    groupID = cluster.groupID;
    pGroup = cluster.pGroup;

    // Enabled
    enabled = cluster.enabled;

    // unit count
    unitCount = cluster.unitCount;
    // nominal capacity
    nominalCapacity = cluster.nominalCapacity;
    nominalCapacityWithSpinning = cluster.nominalCapacityWithSpinning;

    // Making sure that the data related to the prepro and timeseries are present
    // prepro
    if (not prepro)
        prepro = new PreproRenewable();
    if (not series)
        series = new DataSeriesRenewable();

    prepro->copyFrom(*cluster.prepro);
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
            each([&](RenewableCluster& cluster) { cluster.series->timeseriesNumbers.resize(1, n); });
        }
    }
}

void RenewableClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const RenewableCluster& cluster) {
        u.requiredMemoryForInput += sizeof(RenewableCluster);
        u.requiredMemoryForInput += sizeof(void*);
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // productionCost
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // PthetaInf
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // dispatchedUnitsCount
        cluster.modulation.estimateMemoryUsage(u, true, renewableModulationMax, HOURS_PER_YEAR);

        if (cluster.series)
            cluster.series->estimateMemoryUsage(u);
        if (cluster.prepro)
            cluster.prepro->estimateMemoryUsage(u);

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

            buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                           << cluster.parentArea->id << SEP << cluster.id();
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
            String modulationFile;

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

                // Keeping the current value of 'mustrun' somewhere else
                cluster->mustrunOrigin = cluster->mustrun;

                // MBO 15/04/2014
                // new rounding scheme starting version 450
                // if abs(value) < 1.e-3 => 0 ; if abs(value) > 5.e-4 => 5.e-4
                // applies to
                //	- Market Bid cost
                //	- Marginal cost
                //	- Spread cost
                //	- Fixed cost
                //	- Startup cost
                // MBO 23/12/2015
                // v5.0 format
                // allow startup cost between [-5 000 000 ;-5 000 000] (was [-50 000;50 000])

                if (study.header.version <= 500)
                {
                    // cluster->marketBidCost = Math::Round(cluster->marketBidCost, 3);

                    // Market bid cost
                    if (Math::Abs(cluster->marketBidCost) < 5.e-3)
                        cluster->marketBidCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->marketBidCost) > 5.e4)
                            (cluster->marketBidCost > 0) ? cluster->marketBidCost = 5.e4
                                                         : cluster->marketBidCost = -5.e4;
                        else
                            cluster->marketBidCost = Math::Round(cluster->marketBidCost, 3);
                    }

                    // Marginal cost
                    if (Math::Abs(cluster->marginalCost) < 5.e-3)
                        cluster->marginalCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->marginalCost) > 5.e4)
                            (cluster->marginalCost > 0) ? cluster->marginalCost = 5.e4
                                                        : cluster->marginalCost = -5.e4;
                        else
                            cluster->marginalCost = Math::Round(cluster->marginalCost, 3);
                    }

                    // Spread cost - no negative values, must be 0 or >= 0.005
                    if (cluster->spreadCost < 5.e-3)
                        cluster->spreadCost = 0.;
                    else
                    {
                        if (cluster->spreadCost > 5.e4)
                            cluster->spreadCost = 5.e4;
                        else
                            cluster->spreadCost = Math::Round(cluster->spreadCost, 3);
                    }

                    // Fixed cost
                    if (Math::Abs(cluster->fixedCost) < 5.e-3)
                        cluster->fixedCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->fixedCost) > 5.e4)
                            (cluster->fixedCost > 0) ? cluster->fixedCost = 5.e4
                                                     : cluster->fixedCost = -5.e4;
                        else
                            cluster->fixedCost = Math::Round(cluster->fixedCost, 3);
                    }

                    // Startup cost
                    if (Math::Abs(cluster->startupCost) < 5.e-3)
                        cluster->startupCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->startupCost) > 5.e6)
                            (cluster->startupCost > 0) ? cluster->startupCost = 5.e6
                                                       : cluster->startupCost = -5.e6;
                        else
                            cluster->startupCost = Math::Round(cluster->startupCost, 3);
                    }

                    // Before v3.5, the marginal cost and the market bid cost were the same
                    // (and was named 'operatingCost')
                    // Rounding to 3 decimal places
                    if (study.header.version < 350)
                        cluster->marginalCost = cluster->marketBidCost;

                    //	if (not Math::Zero(it->weeklyMinimumCapacity))
                    //		it->minUpDownTime = 168;
                    //	else
                    //	{
                    //		if (not Math::Zero(it->dailyMinimumCapacity))
                    //			it->minUpDownTime = 24;
                    //		else
                    //			it->minUpDownTime = 1;
                    //	}
                    //	it->minStablePower =
                    //		Math::Max(it->hourlyMinimumCapacity,
                    //			Math::Max(it->dailyMinimumCapacity,
                    // it->weeklyMinimumCapacity));
                }
                // Backward compatibility
                // switch (it->minUpDownTime)
                // {
                //	case 1:
                //		it->hourlyMinimumCapacity = it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;
                //		it->weeklyMinimumCapacity = 0;
                //		break;
                //	case 24:
                //		it->hourlyMinimumCapacity = 0;//it->minStablePower;
                //		it->dailyMinimumCapacity  = it->minStablePower;
                //		it->weeklyMinimumCapacity = 0;
                //		break;
                //	case 168:
                //		it->hourlyMinimumCapacity = 0;//it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;//it->minStablePower;
                //		it->weeklyMinimumCapacity = it->minStablePower;
                //		break;
                //	default:
                //		logs.error() << "Invalid 'Min. Up/Down time' for the cluster '" <<
                // it->name() << "'"; 		it->hourlyMinimumCapacity = it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;
                //		it->weeklyMinimumCapacity = 0;
                // }

                // Modulation
                modulationFile.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro"
                                       << SEP << cluster->parentArea->id << SEP << cluster->id()
                                       << SEP << "modulation." << study.inputExtension;

                if (study.header.version < 350)
                {
                    auto& modulation = cluster->modulation;
                    // Before v3.5, the market bid modulation is missing
                    bool r = modulation.loadFromCSVFile(
                      modulationFile,
                      2,
                      HOURS_PER_YEAR,
                      Matrix<>::optImmediate | Matrix<>::optMarkAsModified);
                    if (r and modulation.width == 2)
                    {
                        modulation.resizeWithoutDataLost(renewableModulationMax, modulation.height);
                        // Copy of the modulation cost into the market bid modulation
                        // modulation.fillColumn(2, 1.);
                        modulation.pasteToColumn(renewableModulationMarketBid,
                                                 modulation[renewableModulationCost]);
                        modulation.fillColumn(renewableMinGenModulation, 0.);
                    }
                    else
                    {
                        modulation.reset(renewableModulationMax, HOURS_PER_YEAR);
                        modulation.fill(1.);
                    }
                    modulation.markAsModified();
                    ret = ret and r;
                }
                else if (study.header.version <= 450)
                {
                    auto& modulation = cluster->modulation;
                    // Before v4.5, the modulation generation relative is missing
                    bool r = cluster->modulation.loadFromCSVFile(
                      modulationFile,
                      3,
                      HOURS_PER_YEAR,
                      Matrix<>::optImmediate | Matrix<>::optMarkAsModified);

                    if (r and modulation.width == 3)
                    {
                        modulation.resizeWithoutDataLost(renewableModulationMax, modulation.height);
                    }
                    else
                    {
                        modulation.reset(renewableModulationMax, HOURS_PER_YEAR);
                        modulation.fill(1.);
                    }
                    // switch renewableModulationMarketBid and renewableModulationCapacity
                    // They have a wrong order
                    modulation.pasteToColumn(renewableMinGenModulation,
                                             modulation[renewableModulationMarketBid]);
                    modulation.pasteToColumn(renewableModulationMarketBid,
                                             modulation[renewableModulationCapacity]);
                    modulation.pasteToColumn(renewableModulationCapacity,
                                             modulation[renewableMinGenModulation]);
                    modulation.fillColumn(renewableMinGenModulation, 0.);
                    modulation.markAsModified();
                    ret = ret and r;
                }
                else
                {
                    enum
                    {
                        options = Matrix<>::optFixedSize,
                    };
                    bool r = cluster->modulation.loadFromCSVFile(
                      modulationFile, renewableModulationMax, HOURS_PER_YEAR, options);
                    if (not r and study.usedByTheSolver)
                    {
                        cluster->modulation.reset(renewableModulationMax, HOURS_PER_YEAR);
                        cluster->modulation.fill(1.);
                        cluster->modulation.fillColumn(renewableMinGenModulation, 0.);
                    }
                    ret = ret and r;
                }

                // Special operations when not ran from the interface (aka solver)
                if (study.usedByTheSolver)
                {
                    if (not cluster->productionCost)
                        cluster->productionCost = new double[HOURS_PER_YEAR];

                    // alias to the production cost
                    double* prodCost = cluster->productionCost;
                    // alias to the marginal cost
                    double marginalCost = cluster->marginalCost;
                    // Production cost
                    auto& modulation = cluster->modulation[renewableModulationCost];
                    for (uint h = 0; h != cluster->modulation.height; ++h)
                        prodCost[h] = marginalCost * modulation[h];

                    if (not study.parameters.include.renewable.minStablePower)
                        cluster->minStablePower = 0.;
                    if (not study.parameters.include.renewable.minUPTime)
                    {
                        cluster->minUpDownTime = 1;
                        cluster->minUpTime = 1;
                        cluster->minDownTime = 1;
                    }
                    else
                        cluster->minUpDownTime
                          = Math::Max(cluster->minUpTime, cluster->minDownTime);

                    if (not study.parameters.include.reserve.spinning)
                        cluster->spinning = 0;

                    cluster->nominalCapacityWithSpinning = cluster->nominalCapacity;
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

bool RenewableClusterListSavePreproToFolder(const RenewableClusterList* list, const AnyString& folder)
{
    assert(list);
    if (list->empty())
        return true;

    Clob buffer;
    bool ret = true;

    list->each([&](const Data::RenewableCluster& cluster) {
        if (cluster.prepro)
        {
            assert(cluster.parentArea and "cluster: invalid parent area");
            buffer.clear() << folder << SEP << cluster.parentArea->id << SEP << cluster.id();
            ret = cluster.prepro->saveToFolder(buffer) and ret;
        }
    });
    return ret;
}

bool RenewableClusterListLoadPreproFromFolder(Study& study,
                                            const StudyLoadOptions& options,
                                            RenewableClusterList* list,
                                            const AnyString& folder)
{
    if (list->empty())
        return true;

    Clob buffer;
    bool ret = true;

    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (cluster.prepro)
        {
            assert(cluster.parentArea and "cluster: invalid parent area");
            buffer.clear() << folder << SEP << cluster.parentArea->id << SEP << cluster.id();

            bool result
              = cluster.prepro->loadFromFolder(study, buffer, cluster.parentArea->id, cluster.id());

            if (result and study.usedByTheSolver)
            {
                // checking NPO max
                result = cluster.prepro->normalizeAndCheckNPO(cluster.name(), cluster.unitCount);
            }

            ret = result and ret;
        }
        ++options.progressTicks;
        options.pushProgressLogs();
    }
    return ret;
}

int RenewableClusterListSaveDataSeriesToFolder(const RenewableClusterList* l, const AnyString& folder)
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
        if (cluster.series and (!fast or !cluster.prepro))
            ret = DataSeriesRenewableLoadFromFolder(s, cluster.series, &cluster, folder) and ret;

        ++options.progressTicks;
        options.pushProgressLogs();
    });
    return ret;
}

void RenewableClusterListEnsureDataPrepro(RenewableClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.prepro)
            cluster.prepro = new PreproRenewable();
    }
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

bool RenewableClusterList::rename(Data::RenewableClusterName idToFind, Data::RenewableClusterName newName)
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

bool Data::RenewableCluster::FlexibilityIsValid(uint f)
{
    switch (f)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8:
    case 12:
    case 24:
        return true;
    }
    return false;
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
    ret = modulation.invalidate(reload) and ret;
    if (series)
        ret = series->invalidate(reload) and ret;
    if (prepro)
        ret = prepro->invalidate(reload) and ret;
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
    modulation.markAsModified();
    if (series)
        series->markAsModified();
    if (prepro)
        prepro->markAsModified();
}

void Data::RenewableCluster::calculationOfSpinning()
{
    assert(this->series);

    // nominal capacity (for solver)
    nominalCapacityWithSpinning = nominalCapacity;

    // Nothing to do if the spinning is equal to zero
    // because it will the same multiply all entries of the matrix by 1.
    if (not Math::Zero(spinning))
    {
        logs.debug() << "  Calculation of spinning... " << parentArea->name << "::" << pName;

        auto& ts = series->series;
        // The formula
        // const double s = 1. - cluster.spinning / 100.; */

        // All values in the matrix will be multiply by this coeff
        // It is no really useful to test if the result of the formula
        // is equal to zero, since the method `Matrix::multiplyAllValuesBy()`
        // already does this test.
        nominalCapacityWithSpinning *= 1 - (spinning / 100.);
        ts.multiplyAllEntriesBy(1. - (spinning / 100.));
        ts.flush();
    }
}

void RenewableClusterList::calculationOfSpinning()
{
    each([&](Data::RenewableCluster& cluster) { cluster.calculationOfSpinning(); });
}

void Data::RenewableCluster::reverseCalculationOfSpinning()
{
    assert(this->series);

    // Nothing to do if the spinning is equal to zero
    // because it will the same multiply all entries of the matrix by 1.
    if (not Math::Zero(spinning))
    {
        logs.debug() << "  Calculation of spinning (reverse)... " << parentArea->name
                     << "::" << pName;

        auto& ts = series->series;
        // The formula
        // const double s = 1. - cluster.spinning / 100.;

        // All values in the matrix will be multiply by this coeff
        // It is no really useful to test if the result of the formula
        // is equal to zero, since the method `Matrix::multiplyAllValuesBy()`
        // already does this test.
        ts.multiplyAllEntriesBy(1. / (1. - (spinning / 100.)));
        ts.roundAllEntries();
        ts.flush();
    }
}

void RenewableClusterList::reverseCalculationOfSpinning()
{
    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        cluster.reverseCalculationOfSpinning();
    }
}

void Data::RenewableCluster::reset()
{
    // production cost
    // reminder: this variable should be considered as valid only when used from the
    // solver
    if (productionCost)
        (void)::memset(productionCost, 0, HOURS_PER_YEAR * sizeof(double));

    enabled = true;
    nominalCapacity = 0.;

    // timeseries & prepro
    // warning: the variables `prepro` and `series` __must__ not be destroyed
    //   since the interface may still have a pointer to them.
    //   we must simply reset their content.
    if (not prepro)
        prepro = new PreproRenewable();
    if (not series)
        series = new DataSeriesRenewable();

    series->series.reset(1, HOURS_PER_YEAR);
    series->series.flush();
    prepro->reset();
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
        nominalCapacityWithSpinning = 0.;
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
            total += cluster.unitCount * cluster.nominalCapacity;
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
    uint64 amount = sizeof(RenewableCluster) + modulation.memoryUsage();
    if (prepro)
        amount += prepro->memoryUsage();
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
