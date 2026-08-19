// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/cluster_list.h"

#include <numeric>
#include <ranges>

#include <antares/utils/utils.h>
#include "antares/study/parts/common/cluster.h"
#include "antares/study/study.h"

namespace Antares::Data
{

namespace fs = std::filesystem;

ThermalClusterList::ThermalClusterList()
{
}

ThermalClusterList::~ThermalClusterList()
{
    clearAll();
}

std::string ThermalClusterList::typeID() const
{
    return "thermal";
}

static bool ThermalClusterLoadFromSection(const std::string& areaName,
                                          ThermalCluster& cluster,
                                          const IniFile::Section& section,
                                          bool rampingEnabledGlobal);

void ThermalClusterList::rebuildIndex() const
{
    unsigned int indx = 0;
    for (auto& c: each_enabled_and_not_mustrun())
    {
        c->index = indx++;
    }
}

std::size_t ThermalClusterList::enabledAndNotMustRunCount() const
{
    return std::ranges::count_if(allClusters_,
                                 [](auto c) { return c->isEnabled() && !c->isMustRun(); });
}

std::size_t ThermalClusterList::enabledAndMustRunCount() const
{
    return std::ranges::count_if(allClusters_,
                                 [](auto c) { return c->isEnabled() && c->isMustRun(); });
}

std::size_t ThermalClusterList::reserveParticipationsCount() const
{
    return std::accumulate(
      allClusters_.begin(),
      allClusters_.end(),
      0,
      [](std::size_t total, const std::shared_ptr<ThermalCluster> cluster)
      {
          if (cluster->reserveParticipationContainer.has_value() && cluster->isEnabled())
          {
              return total
                     + cluster->reserveParticipationContainer.value().reserveParticipationsCount();
          }
          else
          {
              return total;
          }
      });
}

std::size_t ThermalClusterList::capacityReservationsCount() const
{
    std::set<const CapacityReservation*> uniqueReservations;
    for (auto& cluster: allClusters_)
    {
        for (const auto& [_, reserveParticipation]:
             cluster->reserveParticipationContainer.value().getReservesParticipations())
        {
            const CapacityReservation* reservationPtr = reserveParticipation.capacityReservation;
            uniqueReservations.insert(reservationPtr);
        }
    }

    return uniqueReservations.size();
}

bool ThermalClusterList::loadFromFolder(const fs::path& folder,
                                        Area* area,
                                        bool rampingEnabledGlobal)
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
        if (!ThermalClusterLoadFromSection(area->name, *cluster, *section, rampingEnabledGlobal))
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

    buildIndexes();
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
        return stringToCostGeneration(std::string(p->value), cluster.costgeneration);
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
        return stringToLocalTSGenerationBehavior(std::string(p->value), cluster.tsGenBehavior);
    }
    if (p->key == "law.planned")
    {
        return stringToStatisticalLaw(std::string(p->value), cluster.plannedLaw);
    }
    if (p->key == "law.forced")
    {
        return stringToStatisticalLaw(std::string(p->value), cluster.forcedLaw);
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
        return p->value.to<unsigned int>(cluster.minUpTime);
    }
    if (p->key == "min-down-time")
    {
        return p->value.to<unsigned int>(cluster.minDownTime);
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

    // initialize the ramping attributes only if ramping is enabled, else ignore these properties
    if (p->key == "power-increase-cost")
    {
        return (cluster.ramping) ? p->value.to<double>(cluster.ramping.value().powerIncreaseCost)
                                 : true;
    }
    if (p->key == "power-decrease-cost")
    {
        return (cluster.ramping) ? p->value.to<double>(cluster.ramping.value().powerDecreaseCost)
                                 : true;
    }
    if (p->key == "max-upward-power-ramping-rate")
    {
        return (cluster.ramping)
                 ? p->value.to<double>(cluster.ramping.value().maxUpwardPowerRampingRate)
                 : true;
    }
    if (p->key == "max-downward-power-ramping-rate")
    {
        return (cluster.ramping)
                 ? p->value.to<double>(cluster.ramping.value().maxDownwardPowerRampingRate)
                 : true;
    }
    // we ignore this property as it was already handled in ThermalClusterLoadFromSection
    if (p->key == "ramping-enabled")
    {
        return true;
    }

    if (p->key == "unitcount")
    {
        return p->value.to<unsigned int>(cluster.unitCount);
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

bool ThermalClusterLoadFromSection(const std::string& areaName,
                                   ThermalCluster& cluster,
                                   const IniFile::Section& section,
                                   bool rampingEnabledGlobal)
{
    if (section.name.empty())
    {
        return false;
    }

    cluster.setName(section.name);
    if (rampingEnabledGlobal)
    {
        // initialize the ramping attributes only if ramping-enabled=true
        auto* rampingEnabledProperty = section.find("ramping-enabled");
        if (rampingEnabledProperty)
        {
            bool rampingEnabled = false;
            bool attributeOK = rampingEnabledProperty->value.to<bool>(rampingEnabled);
            if (rampingEnabled && attributeOK)
            {
                cluster.ramping = ThermalCluster::Ramping();
            }
        }
    }

    if (section.firstProperty)
    {
        // Browse all properties
        for (auto* property = section.firstProperty; property; property = property->next)
        {
            if (property->key.empty())
            {
                logs.warning() << '`' << areaName << "`: thermal cluster: `" << section.name
                               << "`: Invalid key/value";
                continue;
            }
            if (!ThermalClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << areaName << "`: thermal cluster: `" << section.name
                               << "`/`" << property->key
                               << "`: The property is unknown and ignored";
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

} // namespace Antares::Data
