/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/study/parts/thermal/cluster_list.h"

#include <ranges>

#include <antares/utils/utils.h>
#include "antares/study/parts/common/cluster.h"
#include "antares/study/study.h"

namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32_t operator()(uint32_t value) const
    {
        return value + 1;
    }
};

} // anonymous namespace

namespace Antares
{
namespace Data
{
using namespace Yuni;

namespace fs = std::filesystem;

ThermalClusterList::ThermalClusterList()
{
}

ThermalClusterList::~ThermalClusterList()
{
    clearAll();
}

#define SEP IO::Separator

std::string ThermalClusterList::typeID() const
{
    return "thermal";
}

static bool ThermalClusterLoadFromSection(const AnyString& filename,
                                          ThermalCluster& cluster,
                                          const IniFile::Section& section);

void ThermalClusterList::rebuildIndex() const
{
    uint indx = 0;
    for (auto& c: each_enabled_and_not_mustrun())
    {
        c->index = indx++;
    }
}

unsigned int ThermalClusterList::enabledAndNotMustRunCount() const
{
    return std::ranges::count_if(allClusters_,
                                 [](auto c) { return c->isEnabled() && !c->isMustRun(); });
}

unsigned int ThermalClusterList::enabledAndMustRunCount() const
{
    return std::ranges::count_if(allClusters_,
                                 [](auto c) { return c->isEnabled() && c->isMustRun(); });
}

bool ThermalClusterList::loadFromFolder(Study& study, const fs::path& folder, Area* area)
{
    assert(area && "A parent area is required");

    // logs
    logs.info() << "Loading thermal configuration for the area " << area->name;

    // Open the ini file
    IniFile ini;
    if (!ini.open(folder / "list.ini"))
    {
        return false;
    }

    bool ret = true;

    if (!ini.firstSection)
    {
        return ret;
    }

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        if (section->name.empty())
        {
            continue;
        }

        auto cluster = std::make_shared<ThermalCluster>(area);

        // Load data of a thermal cluster from a ini file section
        if (!ThermalClusterLoadFromSection(study.buffer, *cluster, *section))
        {
            continue;
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

        // Modulation
        auto modulationFile = folder.parent_path().parent_path() / "prepro"
                              / cluster->parentArea->id.c_str() / cluster->id() / "modulation.txt";

        enum
        {
            options = Matrix<>::optFixedSize,
        };

        ret = cluster->modulation.loadFromCSVFile(modulationFile.string(),
                                                  thermalModulationMax,
                                                  HOURS_PER_YEAR,
                                                  options)
              && ret;

        // Check the data integrity of the cluster
        addToCompleteList(cluster);
    }

    rebuildIndexes();
    rebuildIndex();

    return ret;
}

bool ThermalClusterList::validateClusters(const Parameters& parameters) const
{
    bool ret = true;

    for (const auto& cluster: allClusters_)
    {
        cluster->minUpTime = std::clamp(cluster->minUpTime, 1u, 168u);
        cluster->minDownTime = std::clamp(cluster->minDownTime, 1u, 168u);

        // update the minUpDownTime
        cluster->minUpDownTime = std::max(cluster->minUpTime, cluster->minDownTime);

        if (!parameters.include.thermal.minStablePower)
        {
            cluster->minStablePower = 0.;
        }
        if (!parameters.include.thermal.minUPTime)
        {
            cluster->minUpDownTime = 1;
            cluster->minUpTime = 1;
            cluster->minDownTime = 1;
        }

        if (!parameters.include.reserve.spinning)
        {
            cluster->spinning = 0;
        }

        cluster->nominalCapacityWithSpinning = cluster->nominalCapacity;

        ret = cluster->integrityCheck() && ret;
    }

    return ret;
}

static bool ThermalClusterLoadFromProperty(ThermalCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
    {
        return false;
    }
    if (p->key == "costgeneration")
    {
        return p->value.to(cluster.costgeneration);
    }
    if (p->key == "enabled")
    {
        return p->value.to<bool>(cluster.enabled);
    }
    if (p->key == "efficiency")
    {
        return p->value.to<double>(cluster.fuelEfficiency);
    }
    if (p->key == "fixed-cost")
    {
        return p->value.to<double>(cluster.fixedCost);
    }

    if (p->key == "group")
    {
        cluster.setGroup(p->value);
        return true;
    }
    if (p->key == "gen-ts")
    {
        return p->value.to(cluster.tsGenBehavior);
    }
    if (p->key == "law.planned")
    {
        return p->value.to(cluster.plannedLaw);
    }
    if (p->key == "law.forced")
    {
        return p->value.to(cluster.forcedLaw);
    }
    if (p->key == "market-bid-cost")
    {
        return p->value.to<double>(cluster.marketBidCost);
    }
    if (p->key == "marginal-cost")
    {
        return p->value.to<double>(cluster.marginalCost);
    }
    if (p->key == "must-run")
    {
        // mustrunOrigin will be initialized later, after LoadFromSection
        return p->value.to<bool>(cluster.mustrun);
    }
    if (p->key == "min-stable-power")
    {
        return p->value.to<double>(cluster.minStablePower);
    }

    if (p->key == "min-up-time")
    {
        return p->value.to<uint>(cluster.minUpTime);
    }
    if (p->key == "min-down-time")
    {
        return p->value.to<uint>(cluster.minDownTime);
    }
    if (p->key == "name")
    {
        return true; // silently ignore it
    }
    if (p->key == "nominalcapacity")
    {
        return p->value.to<double>(cluster.nominalCapacity);
    }

    if (p->key == "spread-cost")
    {
        return p->value.to<double>(cluster.spreadCost);
    }
    if (p->key == "spinning")
    {
        return p->value.to<double>(cluster.spinning);
    }
    if (p->key == "startup-cost")
    {
        return p->value.to<double>(cluster.startupCost);
    }

    if (p->key == "unitcount")
    {
        return p->value.to<uint>(cluster.unitCount);
    }
    if (p->key == "volatility.planned")
    {
        return p->value.to(cluster.plannedVolatility);
    }
    if (p->key == "volatility.forced")
    {
        return p->value.to(cluster.forcedVolatility);
    }
    if (p->key == "variableomcost")
    {
        return p->value.to<double>(cluster.variableomcost);
    }

    // pollutant
    if (auto it = Pollutant::namesToEnum.find(p->key.c_str()); it != Pollutant::namesToEnum.end())
    {
        return p->value.to<double>(cluster.emissions.factors[it->second]);
    }

    // The property is unknown
    return false;
}

bool ThermalClusterLoadFromSection(const AnyString& filename,
                                   ThermalCluster& cluster,
                                   const IniFile::Section& section)
{
    if (section.name.empty())
    {
        return false;
    }

    cluster.setName(section.name);

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
            if (!ThermalClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->key << "`: The property is unknown and ignored";
            }
        }
    }
    return true;
}

void ThermalClusterList::calculationOfSpinning()
{
    for (auto& cluster: each_enabled())
    {
        cluster->calculationOfSpinning();
    }
}

void ThermalClusterList::reverseCalculationOfSpinning()
{
    for (auto& cluster: each_enabled())
    {
        cluster->reverseCalculationOfSpinning();
    }
}

void ThermalClusterList::enableMustrunForEveryone()
{
    for (const auto& c: allClusters_)
    {
        c->mustrun = true;
    }
}

void ThermalClusterList::ensureDataPrepro()
{
    for (const auto& c: all())
    {
        if (!c->prepro)
        {
            c->prepro = std::make_unique<PreproAvailability>(c->id(), c->unitCount);
        }
    }
}

bool ThermalClusterList::saveToFolder(const AnyString& folder) const
{
    // Make sure the folder is created
    if (!IO::Directory::Create(folder))
    {
        logs.error() << "I/O Error: impossible to create '" << folder << "'";
        return false;
    }

    bool ret = true;
    Clob buffer;
    // Allocate the inifile structure
    IniFile ini;

    for (auto& c: allClusters_)
    {
        // Adding a section to the inifile
        IniFile::Section* s = ini.addSection(c->name());

        // The section must not be empty
        // This key will be silently ignored the next time
        s->add("name", c->name());

        if (!c->group().empty())
        {
            s->add("group", c->group());
        }
        if (!c->enabled)
        {
            s->add("enabled", "false");
        }
        if (!Utils::isZero(c->unitCount))
        {
            s->add("unitCount", c->unitCount);
        }
        if (!Utils::isZero(c->nominalCapacity))
        {
            s->add("nominalCapacity", c->nominalCapacity);
        }
        // TS generation
        if (c->tsGenBehavior != LocalTSGenerationBehavior::useGlobalParameter)
        {
            s->add("gen-ts", c->tsGenBehavior);
        }
        // Min. Stable Power
        if (!Utils::isZero(c->minStablePower))
        {
            s->add("min-stable-power", c->minStablePower);
        }

        // Min up and min down time
        if (c->minUpTime != 1)
        {
            s->add("min-up-time", c->minUpTime);
        }
        if (c->minDownTime != 1)
        {
            s->add("min-down-time", c->minDownTime);
        }

        // must-run
        if (c->mustrun)
        {
            s->add("must-run", "true");
        }

        // spinning
        if (!Utils::isZero(c->spinning))
        {
            s->add("spinning", c->spinning);
        }

        // efficiency
        if (c->fuelEfficiency != 100.0)
        {
            s->add("efficiency", c->fuelEfficiency);
        }

        // volatility
        if (!Utils::isZero(c->forcedVolatility))
        {
            s->add("volatility.forced", Utils::round(c->forcedVolatility, 3));
        }
        if (!Utils::isZero(c->plannedVolatility))
        {
            s->add("volatility.planned", Utils::round(c->plannedVolatility, 3));
        }

        // laws
        if (c->forcedLaw != LawUniform)
        {
            s->add("law.forced", c->forcedLaw);
        }
        if (c->plannedLaw != LawUniform)
        {
            s->add("law.planned", c->plannedLaw);
        }

        // costs
        if (c->costgeneration != setManually)
        {
            s->add("costgeneration", c->costgeneration);
        }
        if (!Utils::isZero(c->marginalCost))
        {
            s->add("marginal-cost", Utils::round(c->marginalCost, 3));
        }
        if (!Utils::isZero(c->spreadCost))
        {
            s->add("spread-cost", c->spreadCost);
        }
        if (!Utils::isZero(c->fixedCost))
        {
            s->add("fixed-cost", Utils::round(c->fixedCost, 3));
        }
        if (!Utils::isZero(c->startupCost))
        {
            s->add("startup-cost", Utils::round(c->startupCost, 3));
        }
        if (!Utils::isZero(c->marketBidCost))
        {
            s->add("market-bid-cost", Utils::round(c->marketBidCost, 3));
        }
        if (!Utils::isZero(c->variableomcost))
        {
            s->add("variableomcost", Utils::round(c->variableomcost, 3));
        }

        // pollutant factor
        for (const auto& [key, val]: Pollutant::namesToEnum)
        {
            s->add(key, c->emissions.factors[val]);
        }

        buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                       << c->parentArea->id << SEP << c->id();
        if (IO::Directory::Create(buffer))
        {
            buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                           << c->parentArea->id << SEP << c->id() << SEP << "modulation.txt";

            ret = c->modulation.saveToCSVFile(buffer) && ret;
        }
        else
        {
            ret = false;
        }
    }

    // Write the ini file
    buffer.clear() << folder << SEP << "list.ini";
    ret = ini.save(buffer) && ret;

    return ret;
}

bool ThermalClusterList::savePreproToFolder(const AnyString& folder) const
{
    Clob buffer;
    bool ret = true;

    for (auto& c: allClusters_)
    {
        if (c->prepro)
        {
            assert(c->parentArea && "cluster: invalid parent area");
            buffer.clear() << folder << SEP << c->parentArea->id << SEP << c->id();
            ret = c->prepro->saveToFolder(buffer) && ret;
        }
    }
    return ret;
}

bool ThermalClusterList::saveEconomicCosts(const AnyString& folder) const
{
    Clob buffer;
    bool ret = true;

    for (auto& c: allClusters_)
    {
        assert(c->parentArea && "cluster: invalid parent area");
        buffer.clear() << folder << SEP << c->parentArea->id << SEP << c->id();
        ret = c->ecoInput.saveToFolder(buffer) && ret;
    }
    return ret;
}

bool ThermalClusterList::loadPreproFromFolder(Study& study, const fs::path& folder)
{
    auto hasPrepro = [](auto c) { return (bool)c->prepro; };

    auto loadPrepro = [&folder, &study](auto& c)
    {
        assert(c->parentArea && "cluster: invalid parent area");

        auto preproPath = folder / c->parentArea->id.c_str() / c->id();
        return c->prepro->loadFromFolder(study, preproPath);
    };

    return std::ranges::all_of(allClusters_ | std::views::filter(hasPrepro), loadPrepro);
}

bool ThermalClusterList::validatePrepro(const Study& study)
{
    auto hasPrepro = [](auto c) { return (bool)c->prepro; };

    const bool globalThermalTSgeneration = study.parameters.timeSeriesToGenerate
                                           & timeSeriesThermal;

    if (!study.usedByTheSolver)
    {
        return true;
    }

    return std::ranges::all_of(allClusters_ | std::views::filter(hasPrepro),
                               [&globalThermalTSgeneration](auto& c)
                               {
                                   if (globalThermalTSgeneration && !c->prepro->validate())
                                   {
                                       return false;
                                   }

                                   if (c->doWeGenerateTS(globalThermalTSgeneration))
                                   {
                                       return c->prepro->normalizeAndCheckNPO();
                                   }
                                   return true;
                               });
}

bool ThermalClusterList::loadEconomicCosts(Study& study, const fs::path& folder)
{
    return std::ranges::all_of(allClusters_,
                               [&study, folder](const auto& c)
                               {
                                   assert(c->parentArea && "cluster: invalid parent area");
                                   auto filePath = folder / c->parentArea->id.c_str() / c->id();

                                   bool result = c->ecoInput.loadFromFolder(study, filePath);
                                   return result;
                               });
}

} // namespace Data
} // namespace Antares
