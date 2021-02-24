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

#define THERMALAGGREGATELIST_INITIAL_CAPACITY 10

#define SEP IO::Separator

namespace Yuni
{
namespace Extension
{
namespace CString
{
bool Into<Antares::Data::ThermalLaw>::Perform(AnyString string, TargetType& out)
{
    string.trim();
    if (string.empty())
        return false;

    if (string.equalsInsensitive("uniform"))
    {
        out = Antares::Data::thermalLawUniform;
        return true;
    }
    if (string.equalsInsensitive("geometric"))
    {
        out = Antares::Data::thermalLawGeometric;
        return true;
    }
    return false;
}

} // namespace CString
} // namespace Extension
} // namespace Yuni

namespace Antares
{
namespace Data
{
static bool ThermalClusterLoadFromProperty(ThermalCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;

    switch (p->key[0])
    {
    case 'a':
    {
        if (p->key == "annuityinvestment")
            return p->value.to<uint>(cluster.annuityInvestment);
        break;
    }
    case 'c':
    {
        if (p->key == "co2")
            return p->value.to<double>(cluster.co2);
        break;
    }
    case 'd':
    {
        if (p->key == "dailyminimumcapacity")
        {
            double d = p->value.to<double>();
            if (not Math::Zero(d) and cluster.minUpTime < 24)
                cluster.minUpTime = 24;
            if (not Math::Zero(d) and cluster.minDownTime < 24)
                cluster.minDownTime = 24;
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    case 'e':
    {
        if (p->key == "enabled")
            return p->value.to<bool>(cluster.enabled);
        break;
    }

    case 'f':
    {
        if (p->key == "fixed-cost")
            return p->value.to<double>(cluster.fixedCost);
        if (p->key == "flexibility")
        {
            // The flexibility is now ignored since v3.5
            return true;
        }
        break;
    }
    case 'g':
    {
        if (p->key == "groupmincount")
            return p->value.to<uint>(cluster.groupMinCount);
        if (p->key == "groupmaxcount")
            return p->value.to<uint>(cluster.groupMaxCount);
        if (p->key == "group")
        {
            cluster.group(p->value);
            return true;
        }
        break;
    }
    case 'h':
    {
        if (p->key == "hourlyminimumcapacity")
        {
            double d = p->value.to<double>();
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    case 'l':
    {
        if (p->key == "law.planned")
            return p->value.to(cluster.plannedLaw);
        if (p->key == "law.forced")
            return p->value.to(cluster.forcedLaw);
        break;
    }
    case 'm':
    {
        if (p->key == "market-bid-cost")
            return p->value.to<double>(cluster.marketBidCost);
        if (p->key == "marginal-cost")
            return p->value.to<double>(cluster.marginalCost);
        if (p->key == "must-run")
            // mustrunOrigin will be initialized later, after LoadFromSection
            return p->value.to<bool>(cluster.mustrun);
        if (p->key == "min-stable-power")
            return p->value.to<double>(cluster.minStablePower);

        if (p->key == "min-up-time")
        {
            if (p->value.to<uint>(cluster.minUpTime))
            {
                if (cluster.minUpTime < 1)
                    cluster.minUpTime = 1;
                if (cluster.minUpTime > 168)
                    cluster.minUpTime = 168;
                return true;
            }
            return false;
        }
        if (p->key == "min-down-time")
        {
            if (p->value.to<uint>(cluster.minDownTime))
            {
                if (cluster.minDownTime < 1)
                    cluster.minDownTime = 1;
                if (cluster.minDownTime > 168)
                    cluster.minDownTime = 168;
                return true;
            }
            return false;
        }
        // for compatibility < 5.0
        if (p->key == "min-updown-time")
        {
            uint val;
            p->value.to<uint>(val);
            if (val)
            {
                if (val < 1)
                    val = 1;
                if (val > 168)
                    val = 168;
                cluster.minUpTime = val;
                cluster.minDownTime = val;
                return true;
            }
            return false;
        }
        break;
    }
    case 'n':
    {
        if (p->key == "name")
            return true; // silently ignore it
        if (p->key == "nominalcapacity")
            return p->value.to<double>(cluster.nominalCapacity);
        break;
    }
    case 'o':
    {
        // for compatibility <3.5
        if (p->key == "operatingcost")
            return p->value.to<double>(cluster.marketBidCost);
        break;
    }
    case 's':
    {
        if (p->key == "spread-cost")
            return p->value.to<double>(cluster.spreadCost);
        if (p->key == "spinning")
            return p->value.to<double>(cluster.spinning);
        if (p->key == "startup-cost")
            return p->value.to<double>(cluster.startupCost);
        // for compatibility <3.5
        if (p->key == "stddeviationannualcost")
            return p->value.to<double>(cluster.spreadCost);
        break;
    }
    case 'u':
    {
        if (p->key == "unitcount")
            return p->value.to<uint>(cluster.unitCount);
        break;
    }
    case 'v':
    {
        if (p->key == "volatility.planned")
            return p->value.to(cluster.plannedVolatility);
        if (p->key == "volatility.forced")
            return p->value.to(cluster.forcedVolatility);
        break;
    }
    case 'w':
    {
        if (p->key == "weeklyminimumcapacity")
        {
            double d = p->value.to<double>();
            if (not Math::Zero(d) and cluster.minUpTime < 168)
                cluster.minUpTime = 168;
            if (not Math::Zero(d) and cluster.minDownTime < 168)
                cluster.minDownTime = 168;
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    }

    // The property is unknown
    return false;
}

static bool ThermalClusterLoadFromSection(const AnyString& filename,
                                          ThermalCluster& cluster,
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
            if (not ThermalClusterLoadFromProperty(cluster, property))
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

static bool ThermalClusterLoadCouplingSection(const AnyString& filename,
                                              ThermalClusterList& list,
                                              const IniFile::Section* s)
{
    if (s->firstProperty)
    {
        Data::ThermalClusterName from;
        Data::ThermalClusterName with;
        Data::ThermalCluster* clusterFrom;
        Data::ThermalCluster* clusterWith;

        // Browse all properties
        for (const IniFile::Property* p = s->firstProperty; p; p = p->next)
        {
            from = p->key;
            with = p->value;
            if (not from or !with)
            {
                logs.warning() << '`' << filename << "`: `" << s->name << "`: Invalid key/value";
                continue;
            }
            from.toLower();
            with.toLower();
            clusterFrom = list.find(from);
            if (not clusterFrom)
            {
                logs.error() << filename << ": impossible to find the cluster '" << from << "'";
                continue;
            }
            clusterWith = list.find(with);
            if (not clusterWith)
            {
                logs.error() << filename << ": impossible to find the cluster '" << with << "'";
                continue;
            }

            if (clusterFrom->coupling.end() != clusterFrom->coupling.find(clusterWith))
                // already referenced
                continue;

            // Adding the reference in both clusters
            clusterFrom->coupling.insert(clusterWith);
            clusterWith->coupling.insert(clusterFrom);
            logs.info() << "  cluster coupling : " << clusterFrom->name() << " <-> "
                        << clusterWith->name();
        }
    }
    return true;
}

Data::ThermalCluster::ThermalCluster(Area* parent, uint nbParallelYears) :
 groupID(thermalDispatchGrpOther),
 index(0),
 areaWideIndex((uint)-1),
 parentArea(parent),
 enabled(true),
 mustrun(false),
 mustrunOrigin(false),
 unitCount(0),
 nominalCapacity(0.),
 nominalCapacityWithSpinning(0.),
 minStablePower(0.),
 minUpTime(1),
 minDownTime(1),
 spinning(0.),
 co2(0.),
 forcedVolatility(0.),
 plannedVolatility(0.),
 forcedLaw(thermalLawUniform),
 plannedLaw(thermalLawUniform),
 marginalCost(0.),
 spreadCost(0.),
 fixedCost(0.),
 startupCost(0.),
 marketBidCost(0.),
 groupMinCount(0),
 groupMaxCount(0),
 annuityInvestment(0),
 PthetaInf(HOURS_PER_YEAR, 0),
 prepro(nullptr),
 series(nullptr),
 productionCost(nullptr),
 unitCountLastHour(nullptr),
 productionLastHour(nullptr),
 pminOfAGroup(nullptr)
{
    // assert
    assert(parent and "A parent for a thermal dispatchable cluster can not be null");

    unitCountLastHour = new uint[nbParallelYears];
    productionLastHour = new double[nbParallelYears];
    pminOfAGroup = new double[nbParallelYears];
    for (uint numSpace = 0; numSpace < nbParallelYears; ++numSpace)
    {
        unitCountLastHour[numSpace] = 0;
        productionLastHour[numSpace] = 0.;
        pminOfAGroup[numSpace] = 0.;
    }
}

Data::ThermalCluster::ThermalCluster(Area* parent) :
 groupID(thermalDispatchGrpOther),
 index(0),
 areaWideIndex((uint)-1),
 parentArea(parent),
 enabled(true),
 mustrun(false),
 mustrunOrigin(false),
 unitCount(0),
 nominalCapacity(0.),
 nominalCapacityWithSpinning(0.),
 minStablePower(0.),
 minUpTime(1),
 minDownTime(1),
 spinning(0.),
 co2(0.),
 forcedVolatility(0.),
 plannedVolatility(0.),
 forcedLaw(thermalLawUniform),
 plannedLaw(thermalLawUniform),
 marginalCost(0.),
 spreadCost(0.),
 fixedCost(0.),
 startupCost(0.),
 marketBidCost(0.),
 groupMinCount(0),
 groupMaxCount(0),
 annuityInvestment(0),
 PthetaInf(HOURS_PER_YEAR, 0),
 prepro(nullptr),
 series(nullptr),
 productionCost(nullptr),
 unitCountLastHour(nullptr),
 productionLastHour(nullptr),
 pminOfAGroup(nullptr)
{
    // assert
    assert(parent and "A parent for a thermal dispatchable cluster can not be null");
}

Data::ThermalCluster::~ThermalCluster()
{
    delete[] productionCost;
    delete prepro;
    delete series;

    if (unitCountLastHour)
        delete[] unitCountLastHour;
    if (productionLastHour)
        delete[] productionLastHour;
    if (pminOfAGroup)
        delete[] pminOfAGroup;
}

#ifdef ANTARES_SWAP_SUPPORT
void ThermalCluster::flush()
{
    modulation.flush();
    if (prepro)
        prepro->flush();
    if (series)
        series->flush();
}
#endif

#ifdef ANTARES_SWAP_SUPPORT
void ThermalClusterList::flush()
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
        i->second->flush();
}
#endif

void Data::ThermalCluster::invalidateArea()
{
    if (parentArea)
        parentArea->invalidate();
}

String Antares::Data::ThermalCluster::getFullName() const
{
    String s;
    s << parentArea->name << "." << pID;
    return s;
}

void Data::ThermalCluster::copyFrom(const ThermalCluster& cluster)
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

    minDivModulation = cluster.minDivModulation;

    minStablePower = cluster.minStablePower;
    minUpTime = cluster.minUpTime;
    minDownTime = cluster.minDownTime;

    // spinning
    spinning = cluster.spinning;
    // co2
    co2 = cluster.co2;

    // volatility
    forcedVolatility = cluster.forcedVolatility;
    plannedVolatility = cluster.plannedVolatility;
    // law
    forcedLaw = cluster.forcedLaw;
    plannedLaw = cluster.plannedLaw;

    // costs
    marginalCost = cluster.marginalCost;
    spreadCost = cluster.spreadCost;
    fixedCost = cluster.fixedCost;
    startupCost = cluster.startupCost;
    marketBidCost = cluster.marketBidCost;

    // group {min,max}
    groupMinCount = cluster.groupMinCount;
    groupMaxCount = cluster.groupMaxCount;

    // Annuity investment (kEuros/MW)
    annuityInvestment = cluster.annuityInvestment;

    // modulation
    modulation = cluster.modulation;
    cluster.modulation.unloadFromMemory();

    // Making sure that the data related to the prepro and timeseries are present
    // prepro
    if (not prepro)
        prepro = new PreproThermal();
    if (not series)
        series = new DataSeriesThermal();

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

Data::ThermalClusterList::ThermalClusterList() : byIndex(nullptr)
{
    (void)::memset(&groupCount, 0, sizeof(groupCount));
}

Data::ThermalClusterList::~ThermalClusterList()
{
    // deleting all thermal clusters
    clear();
}

void ThermalClusterList::clear()
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

const ThermalCluster* ThermalClusterList::find(const ThermalCluster* p) const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

Data::ThermalCluster* ThermalClusterList::find(const ThermalCluster* p)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

void ThermalClusterList::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number
    if (not cluster.empty())
    {
        if (0 == n)
        {
            each([&](ThermalCluster& cluster) { cluster.series->timeseriesNumbers.clear(); });
        }
        else
        {
            each([&](ThermalCluster& cluster) { cluster.series->timeseriesNumbers.resize(1, n); });
        }
    }
}

void ThermalClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const ThermalCluster& cluster) {
        u.requiredMemoryForInput += sizeof(ThermalCluster);
        u.requiredMemoryForInput += sizeof(void*);
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // productionCost
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // PthetaInf
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // dispatchedUnitsCount
        cluster.modulation.estimateMemoryUsage(u, true, thermalModulationMax, HOURS_PER_YEAR);

        if (cluster.series)
            cluster.series->estimateMemoryUsage(u);
        if (cluster.prepro)
            cluster.prepro->estimateMemoryUsage(u);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

void Data::ThermalCluster::group(Data::ThermalClusterName newgrp)
{
    if (not newgrp)
    {
        groupID = thermalDispatchGrpOther;
        pGroup.clear();
        return;
    }
    pGroup = newgrp;
    newgrp.toLower();

    switch (newgrp[0])
    {
    case 'g':
    {
        if (newgrp == "gas")
        {
            groupID = thermalDispatchGrpGas;
            return;
        }
        break;
    }
    case 'h':
    {
        if (newgrp == "hard coal")
        {
            groupID = thermalDispatchGrpHardCoal;
            return;
        }
        break;
    }
    case 'l':
    {
        if (newgrp == "lignite")
        {
            groupID = thermalDispatchGrpLignite;
            return;
        }
        break;
    }
    case 'm':
    {
        if (newgrp == "mixed fuel")
        {
            groupID = thermalDispatchGrpMixedFuel;
            return;
        }
        break;
    }
    case 'n':
    {
        if (newgrp == "nuclear")
        {
            groupID = thermalDispatchGrpNuclear;
            return;
        }
        break;
    }
    case 'o':
    {
        if (newgrp == "oil")
        {
            groupID = thermalDispatchGrpOil;
            return;
        }
        break;
    }
    }
    // assigning a default value
    groupID = thermalDispatchGrpOther;
}

void ThermalClusterList::rebuildIndex()
{
    delete[] byIndex;

    if (not empty())
    {
        uint indx = 0;
        typedef ThermalCluster* ThermalClusterWeakPtr;
        byIndex = new ThermalClusterWeakPtr[size()];

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

bool ThermalClusterListSaveToFolder(const ThermalClusterList* l, const AnyString& folder)
{
    // Assert
    assert(l);

    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;
        bool hasCoupling = false;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        l->each([&](const Data::ThermalCluster& cluster) {
            // Coupling
            if (not cluster.coupling.empty())
                hasCoupling = true;

            // Adding a section to the inifile
            IniFile::Section* s = ini.addSection(cluster.name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", cluster.name());

            if (not cluster.group().empty())
                s->add("group", cluster.group());
            if (not cluster.enabled)
                s->add("enabled", "false");
            if (not Math::Zero(cluster.unitCount))
                s->add("unitCount", cluster.unitCount);
            if (not Math::Zero(cluster.nominalCapacity))
                s->add("nominalCapacity", cluster.nominalCapacity);

            // Min. Stable Power
            if (not Math::Zero(cluster.minStablePower))
                s->add("min-stable-power", cluster.minStablePower);

            // Min up and min down time
            if (cluster.minUpTime != 1)
                s->add("min-up-time", cluster.minUpTime);
            if (cluster.minDownTime != 1)
                s->add("min-down-time", cluster.minDownTime);

            // must-run
            if (cluster.mustrun)
                s->add("must-run", "true");

            // spinning
            if (not Math::Zero(cluster.spinning))
                s->add("spinning", cluster.spinning);
            // co2
            if (not Math::Zero(cluster.co2))
                s->add("co2", cluster.co2);

            // volatility
            if (not Math::Zero(cluster.forcedVolatility))
                s->add("volatility.forced", Math::Round(cluster.forcedVolatility, 3));
            if (not Math::Zero(cluster.plannedVolatility))
                s->add("volatility.planned", Math::Round(cluster.plannedVolatility, 3));

            // laws
            if (cluster.forcedLaw != thermalLawUniform)
                s->add("law.forced", cluster.forcedLaw);
            if (cluster.plannedLaw != thermalLawUniform)
                s->add("law.planned", cluster.plannedLaw);

            // costs
            if (not Math::Zero(cluster.marginalCost))
                s->add("marginal-cost", Math::Round(cluster.marginalCost, 3));
            if (not Math::Zero(cluster.spreadCost))
                s->add("spread-cost", cluster.spreadCost);
            if (not Math::Zero(cluster.fixedCost))
                s->add("fixed-cost", Math::Round(cluster.fixedCost, 3));
            if (not Math::Zero(cluster.startupCost))
                s->add("startup-cost", Math::Round(cluster.startupCost, 3));
            if (not Math::Zero(cluster.marketBidCost))
                s->add("market-bid-cost", Math::Round(cluster.marketBidCost, 3));

            // groun{min,max}
            if (not Math::Zero(cluster.groupMinCount))
                s->add("groupMinCount", cluster.groupMinCount);
            if (not Math::Zero(cluster.groupMaxCount))
                s->add("groupMaxCount", cluster.groupMaxCount);
            if (not Math::Zero(cluster.annuityInvestment))
                s->add("annuityInvestment", cluster.annuityInvestment);

            buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                           << cluster.parentArea->id << SEP << cluster.id();
            if (IO::Directory::Create(buffer))
            {
                buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                               << cluster.parentArea->id << SEP << cluster.id() << SEP
                               << "modulation.txt";

                ret = cluster.modulation.saveToCSVFile(buffer) and ret;
            }
            else
                ret = 0;
        });

        if (hasCoupling)
        {
            IniFile::Section* s = ini.addSection("~_-_coupling_-_~");
            l->each([&](const Data::ThermalCluster& cluster) {
                if (cluster.coupling.empty())
                    return;
                auto send = cluster.coupling.end();
                for (auto j = cluster.coupling.begin(); j != send; ++j)
                    s->add(cluster.id(), (*j)->id());
            });
        }

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

bool ThermalClusterList::add(ThermalCluster* newcluster)
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

bool ThermalClusterList::loadFromFolder(Study& study, const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading thermal configuration for the area " << area->name;

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

                if (section->name == "~_-_coupling_-_~")
                {
                    Data::ThermalClusterLoadCouplingSection(study.buffer, *this, section);

                    // ignoring all other sections
                    section = section->next;
                    for (; section; section = section->next)
                        logs.warning() << "Ignoring the section " << section->name;
                    break;
                }

                auto* cluster = new ThermalCluster(area, study.maxNbYearsInParallel);

                // Load data of a thermal cluster from a ini file section
                if (not ThermalClusterLoadFromSection(study.buffer, *cluster, *section))
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
                        modulation.resizeWithoutDataLost(thermalModulationMax, modulation.height);
                        // Copy of the modulation cost into the market bid modulation
                        // modulation.fillColumn(2, 1.);
                        modulation.pasteToColumn(thermalModulationMarketBid,
                                                 modulation[thermalModulationCost]);
                        modulation.fillColumn(thermalMinGenModulation, 0.);
                    }
                    else
                    {
                        modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
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
                        modulation.resizeWithoutDataLost(thermalModulationMax, modulation.height);
                    }
                    else
                    {
                        modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
                        modulation.fill(1.);
                    }
                    // switch thermalModulationMarketBid and thermalModulationCapacity
                    // They have a wrong order
                    modulation.pasteToColumn(thermalMinGenModulation,
                                             modulation[thermalModulationMarketBid]);
                    modulation.pasteToColumn(thermalModulationMarketBid,
                                             modulation[thermalModulationCapacity]);
                    modulation.pasteToColumn(thermalModulationCapacity,
                                             modulation[thermalMinGenModulation]);
                    modulation.fillColumn(thermalMinGenModulation, 0.);
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
                      modulationFile, thermalModulationMax, HOURS_PER_YEAR, options);
                    if (not r and study.usedByTheSolver)
                    {
                        cluster->modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
                        cluster->modulation.fill(1.);
                        cluster->modulation.fillColumn(thermalMinGenModulation, 0.);
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
                    auto& modulation = cluster->modulation[thermalModulationCost];
                    for (uint h = 0; h != cluster->modulation.height; ++h)
                        prodCost[h] = marginalCost * modulation[h];

                    if (not study.parameters.include.thermal.minStablePower)
                        cluster->minStablePower = 0.;
                    if (not study.parameters.include.thermal.minUPTime)
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

                // adding the thermal cluster
                if (not add(cluster))
                {
                    // This error should never happen
                    logs.error() << "Impossible to add the thermal cluster '" << cluster->name()
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

bool ThermalClusterListSavePreproToFolder(const ThermalClusterList* list, const AnyString& folder)
{
    assert(list);
    if (list->empty())
        return true;

    Clob buffer;
    bool ret = true;

    list->each([&](const Data::ThermalCluster& cluster) {
        if (cluster.prepro)
        {
            assert(cluster.parentArea and "cluster: invalid parent area");
            buffer.clear() << folder << SEP << cluster.parentArea->id << SEP << cluster.id();
            ret = cluster.prepro->saveToFolder(buffer) and ret;
        }
    });
    return ret;
}

bool ThermalClusterListLoadPreproFromFolder(Study& study,
                                            const StudyLoadOptions& options,
                                            ThermalClusterList* list,
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

int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l, const AnyString& folder)
{
    if (l->empty())
        return 1;

    int ret = 1;

    auto end = l->cluster.end();
    for (auto it = l->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (cluster.series)
            ret = DataSeriesThermalSaveToFolder(cluster.series, &cluster, folder) and ret;
    }
    return ret;
}

int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l,
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
            ret = DataSeriesThermalSaveToFolder(cluster.series, &cluster, folder) and ret;
        }
        ++ticks;
    }
    return ret;
}

int ThermalClusterListLoadDataSeriesFromFolder(Study& s,
                                               const StudyLoadOptions& options,
                                               ThermalClusterList* l,
                                               const AnyString& folder,
                                               int fast)
{
    if (l->empty())
        return 1;

    int ret = 1;

    l->each([&](Data::ThermalCluster& cluster) {
        if (cluster.series and (!fast or !cluster.prepro))
            ret = DataSeriesThermalLoadFromFolder(s, cluster.series, &cluster, folder) and ret;

        ++options.progressTicks;
        options.pushProgressLogs();
    });
    return ret;
}

void ThermalClusterListEnsureDataPrepro(ThermalClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.prepro)
            cluster.prepro = new PreproThermal();
    }
}

void ThermalClusterListEnsureDataTimeSeries(ThermalClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.series)
            cluster.series = new DataSeriesThermal();
    }
}

Yuni::uint64 ThermalClusterList::memoryUsage() const
{
    uint64 ret = sizeof(ThermalClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const Data::ThermalCluster& cluster) { ret += cluster.memoryUsage(); });
    return ret;
}

bool ThermalClusterList::rename(Data::ThermalClusterName idToFind, Data::ThermalClusterName newName)
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
    Data::ThermalClusterName newID;
    TransformNameIntoID(newName, newID);

    // Looking for the thermal cluster in the list
    auto it = cluster.find(idToFind);
    if (it == cluster.end())
        return true;

    Data::ThermalCluster* p = it->second;

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
    // It is a bit excessive (all matrices not only those related to the thermal cluster)
    // will be rewritten but currently it is the less error-prone.
    if (p->parentArea)
        (p->parentArea)->invalidateJIT = true;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

bool Data::ThermalCluster::FlexibilityIsValid(uint f)
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

bool Data::ThermalClusterList::invalidate(bool reload) const
{
    bool ret = true;
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        ret = (i->second)->invalidate(reload) and ret;
    return ret;
}

bool Data::ThermalCluster::invalidate(bool reload) const
{
    bool ret = true;
    ret = modulation.invalidate(reload) and ret;
    if (series)
        ret = series->invalidate(reload) and ret;
    if (prepro)
        ret = prepro->invalidate(reload) and ret;
    return ret;
}

void Data::ThermalClusterList::markAsModified() const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        (i->second)->markAsModified();
}

void Data::ThermalCluster::markAsModified() const
{
    modulation.markAsModified();
    if (series)
        series->markAsModified();
    if (prepro)
        prepro->markAsModified();
}

void Data::ThermalCluster::calculationOfSpinning()
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

void ThermalClusterList::calculationOfSpinning()
{
    each([&](Data::ThermalCluster& cluster) { cluster.calculationOfSpinning(); });
}

void Data::ThermalCluster::reverseCalculationOfSpinning()
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

void ThermalClusterList::reverseCalculationOfSpinning()
{
    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        cluster.reverseCalculationOfSpinning();
    }
}

void Data::ThermalCluster::reset()
{
    // production cost
    // reminder: this variable should be considered as valid only when used from the
    // solver
    if (productionCost)
        (void)::memset(productionCost, 0, HOURS_PER_YEAR * sizeof(double));

    mustrun = false;
    mustrunOrigin = false;
    unitCount = 0;
    enabled = true;
    nominalCapacity = 0.;
    nominalCapacityWithSpinning = 0.;
    minDivModulation.isCalculated = false;
    minStablePower = 0.;
    minUpDownTime = 1;
    minUpTime = 1;
    minDownTime = 1;

    // spinning
    spinning = 0.;
    // co2
    co2 = 0.;

    // volatility
    forcedVolatility = 0.;
    plannedVolatility = 0.;
    // laws
    plannedLaw = thermalLawUniform;
    forcedLaw = thermalLawUniform;

    // costs
    marginalCost = 0.;
    spreadCost = 0.;
    fixedCost = 0.;
    startupCost = 0.;
    marketBidCost = 0.;

    // group{min,max}
    groupMinCount = 0;
    groupMaxCount = 0;
    annuityInvestment = 0;

    // modulation
    modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    modulation.fill(1.);
    modulation.fillColumn(thermalMinGenModulation, 0.);
    modulation.flush();

    // timeseries & prepro
    // warning: the variables `prepro` and `series` __must__ not be destroyed
    //   since the interface may still have a pointer to them.
    //   we must simply reset their content.
    if (not prepro)
        prepro = new PreproThermal();
    if (not series)
        series = new DataSeriesThermal();

    series->series.reset(1, HOURS_PER_YEAR);
    series->series.flush();
    prepro->reset();

    // Links
    coupling.clear();
}

bool Data::ThermalCluster::integrityCheck()
{
    if (not parentArea)
    {
        logs.error() << "Thermal cluster " << pName << ": The parent area is missing";
        return false;
    }

    if (Math::NaN(marketBidCost))
    {
        logs.error() << "Thermal cluster " << pName << ": NaN detected for market bid cost";
        return false;
    }
    if (Math::NaN(marginalCost))
    {
        logs.error() << "Thermal cluster " << parentArea->name << '/' << pName
                     << ": NaN detected for marginal cost";
        return false;
    }
    if (Math::NaN(spreadCost))
    {
        logs.error() << "Thermal cluster " << parentArea->name << '/' << pName
                     << ": NaN detected for marginal cost";
        return false;
    }

    bool ret = true;

    if (minUpTime > 168 or 0 == minUpTime)
    {
        logs.error() << "Thermal cluster " << parentArea->name << "/" << pName
                     << ": The min. up time must be between 1 and 168";
        minUpTime = 1;
        ret = false;
    }
    if (minDownTime > 168 or 0 == minDownTime)
    {
        logs.error() << "Thermal cluster " << parentArea->name << "/" << pName
                     << ": The min. down time must be between 1 and 168";
        minDownTime = 1;
        ret = false;
    }
    if (nominalCapacity < 0.)
    {
        logs.error() << "Thermal cluster " << parentArea->name << "/" << pName
                     << ": The Nominal capacity must be positive or null";
        nominalCapacity = 0.;
        nominalCapacityWithSpinning = 0.;
        ret = false;
    }
    if (unitCount > 100)
    {
        logs.error() << "Thermal cluster " << pID << ": The variable `unitCount` must be < 100";
        unitCount = 100;
        ret = false;
    }
    if (spinning < 0. or spinning > 100.)
    {
        if (spinning < 0.)
            spinning = 0;
        else
            spinning = 100.;
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The spinning must be within the range [0,+100] (rounded to " << spinning
                     << ')';
        ret = false;
        nominalCapacityWithSpinning = nominalCapacity;
    }
    if (co2 < 0.)
    {
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The co2 must be positive or null";
        co2 = 0;
        ret = false;
    }
    if (spreadCost < 0.)
    {
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The spread must be positive or null";
        spreadCost = 0.;
        ret = false;
    }

    // Modulation
    if (modulation.height > 0)
    {
        CString<ant_k_cluster_name_max_length + ant_k_area_name_max_length + 50, false> buffer;
        buffer << "Thermal cluster: " << parentArea->name << '/' << pName << ": Modulation";
        ret = MatrixTestForPositiveValues(buffer.c_str(), &modulation) and ret;
    }

    // la valeur minStablePower should not be modified
    /*
    if (minStablePower > nominalCapacity)
    {
            logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                    << ": failed min stable power < nominal capacity (with min power = "
                    << minStablePower << ", nominal power = " << nominalCapacity;
            minStablePower = nominalCapacity;
            ret = false;
    }*/

    return ret;
}

const char* Data::ThermalCluster::GroupName(enum ThermalDispatchableGroup grp)
{
    switch (grp)
    {
    case thermalDispatchGrpNuclear:
        return "Nuclear";
    case thermalDispatchGrpLignite:
        return "Lignite";
    case thermalDispatchGrpHardCoal:
        return "Hard Coal";
    case thermalDispatchGrpGas:
        return "Gas";
    case thermalDispatchGrpOil:
        return "Oil";
    case thermalDispatchGrpMixedFuel:
        return "Mixed Fuel";
    case thermalDispatchGrpOther:
        return "Other";
    case thermalDispatchGrpMax:
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

bool ThermalClusterList::storeTimeseriesNumbers(Study& study)
{
    if (cluster.empty())
        return true;

    bool ret = true;
    TSNumbersPredicate predicate;

    each([&](const Data::ThermalCluster& cluster) {
        study.buffer = study.folderOutput;
        study.buffer << SEP << "ts-numbers" << SEP << "thermal" << SEP << cluster.parentArea->id
                     << SEP << cluster.id() << ".txt";
        ret = cluster.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate)
              and ret;
    });
    return ret;
}

void ThermalClusterList::retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const
{
    total = 0.;
    unitCount = 0;

    if (not cluster.empty())
    {
        auto end = cluster.cend();
        for (auto i = cluster.cbegin(); i != end; ++i)
        {
            if (not i->second)
                return;

            // Reference to the thermal cluster
            auto& cluster = *(i->second);
            unitCount += cluster.unitCount;
            total += cluster.unitCount * cluster.nominalCapacity;
        }
    }
}

bool ThermalClusterList::remove(const Data::ThermalClusterName& id)
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

    // Remove all cluster coupling
    if (not c->coupling.empty())
    {
        auto end = c->coupling.end();
        for (auto j = c->coupling.begin(); j != end; ++j)
        {
            auto* link = *j;
            link->parentArea->invalidate();
            link->coupling.erase(c);
        }
    }

    // delete the cluster
    delete c;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

ThermalCluster* ThermalClusterList::detach(iterator i)
{
    auto* c = i->second;
    cluster.erase(i);
    return c;
}

void ThermalClusterList::remove(iterator i)
{
    cluster.erase(i);
}

bool ThermalClusterList::exists(const Data::ThermalClusterName& id) const
{
    if (not cluster.empty())
    {
        auto element = cluster.find(id);
        return (element != cluster.end());
    }
    return false;
}

uint64 ThermalCluster::memoryUsage() const
{
    uint64 amount = sizeof(ThermalCluster) + modulation.memoryUsage();
    if (prepro)
        amount += prepro->memoryUsage();
    if (series)
        amount += DataSeriesThermalMemoryUsage(series);
    return amount;
}

void ThermalCluster::calculatMinDivModulation()
{
    minDivModulation.value = (modulation[thermalModulationCapacity][0]
                              / Math::Ceil(modulation[thermalModulationCapacity][0]));
    minDivModulation.index = 0;

    for (uint t = 1; t < modulation.height; t++)
    {
        double div = modulation[thermalModulationCapacity][t]
                     / ceil(modulation[thermalModulationCapacity][t]);

        if (div < minDivModulation.value)
        {
            minDivModulation.value = div;
            minDivModulation.index = t;
        }
    }
    minDivModulation.isCalculated = true;
}

bool ThermalCluster::checkMinStablePower()
{
    if (not minDivModulation.isCalculated) // not has been initialized
        calculatMinDivModulation();

    if (minDivModulation.value < 0)
    {
        minDivModulation.isValidated = false;
        return false;
    }

    // calculate nominalCapacityWithSpinning
    double nomCapacityWithSpinning = nominalCapacity * (1 - spinning / 101);

    if (Math::Zero(1 - spinning / 101))
        minDivModulation.border = .0;
    else
        minDivModulation.border
          = Math::Min(nomCapacityWithSpinning, minStablePower) / nomCapacityWithSpinning;

    if (minDivModulation.value < minDivModulation.border)
    {
        minDivModulation.isValidated = false;
        return false;
    }

    minDivModulation.isValidated = true;
    return true;
}

bool ThermalCluster::checkMinStablePowerWithNewModulation(uint index, double value)
{
    if (not minDivModulation.isCalculated || index == minDivModulation.index)
        calculatMinDivModulation();
    else
    {
        double div = value / ceil(value);
        if (div < minDivModulation.value)
        {
            minDivModulation.value = div;
            minDivModulation.index = index;
        }
    }

    return checkMinStablePower();
}

void ThermalClusterList::enableMustrunForEveryone()
{
    // enabling the mustrun mode
    each([&](Data::ThermalCluster& cluster) { cluster.mustrun = true; });
}

void ThermalCluster::name(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    TransformNameIntoID(pName, pID);
}

bool ThermalCluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

} // namespace Data
} // namespace Antares
