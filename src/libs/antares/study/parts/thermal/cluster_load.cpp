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
#include "../../../inifile.h"
#include "../../../logs.h"

#include "cluster.h"
#include "cluster_load.h"

namespace Antares::Data
{

ThermalClusterReader::ThermalClusterReader()
{
    for (auto const& [key, val] : Pollutant::namesToEnum)
    {
        callbackMap.try_emplace(key, [val](ThermalCluster& c, const IniFile::Property& p) {
            return c.emissions.emissionFactors[val] = p.value.to<double>();
        });
    }

    callbackMap.try_emplace("annuityinvestment", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.annuityInvestment); });

    callbackMap.try_emplace("dailyminimumcapacity", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.minStablePower); });

    callbackMap.try_emplace("enabled", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<bool>(c.enabled); });

    callbackMap.try_emplace("fixed-cost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.fixedCost); });

    callbackMap.try_emplace("flexibility", [](ThermalCluster& c, const IniFile::Property& p)
            { (void)c; (void)p; return true; }); // ignored since 3.5

    callbackMap.try_emplace("groupmincount", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.groupMinCount); });

    callbackMap.try_emplace("groupmaxcount", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.groupMaxCount); });

    callbackMap.try_emplace("group", [](ThermalCluster& c, const IniFile::Property& p)
            { c.setGroup(p.value); return true; });

    callbackMap.try_emplace("gen-ts", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to(c.tsGenBehavior); });

    callbackMap.try_emplace("hourlyminimumcapacity", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.minStablePower); });

    callbackMap.try_emplace("law.planned", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to(c.plannedLaw); });
    callbackMap.try_emplace("law.forced", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to(c.forcedLaw); });

    callbackMap.try_emplace("market-bid-cost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.marketBidCost); });

    callbackMap.try_emplace("marginal-cost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.marginalCost); });

    callbackMap.try_emplace("must-run", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<bool>(c.mustrun); });

    callbackMap.try_emplace("min-stable-power", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.minStablePower); });

    callbackMap.try_emplace("min-up-time", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.minUpTime); });
    callbackMap.try_emplace("min-down-time", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.minDownTime); });
    // for compatibility < 5.0
    callbackMap.try_emplace("min-updown-time", [](ThermalCluster& c, const IniFile::Property& p)
            { p.value.to<uint>(c.minUpTime);  return p.value.to<uint>(c.minDownTime); });

    callbackMap.try_emplace("name", [](ThermalCluster& c, const IniFile::Property& p)
            { (void)c; (void)p; return true; }); //silently ignore it

    callbackMap.try_emplace("nominalcapacity", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.nominalCapacity); });

    // for compatibility <3.5
    callbackMap.try_emplace("operatingcost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.marketBidCost); });

    callbackMap.try_emplace("spread-cost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.spreadCost); });

    callbackMap.try_emplace("spinning", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.spinning); });

    callbackMap.try_emplace("startup-cost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.startupCost); });

    callbackMap.try_emplace("stddeviationannualcost", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.spreadCost); });

    callbackMap.try_emplace("unitcount", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<uint>(c.unitCount); });

    callbackMap.try_emplace("volatility.planned", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to(c.plannedVolatility); });

    callbackMap.try_emplace("volatility.forced", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to(c.forcedVolatility); });

    callbackMap.try_emplace("weeklyminimumcapacity", [](ThermalCluster& c, const IniFile::Property& p)
            { return p.value.to<double>(c.minStablePower); });

}

void ThermalClusterReader::checkAndFixIntegrity(ThermalCluster& cluster) const
{
    if (cluster.minUpTime < 1)
        cluster.minUpTime = 1;
    if (cluster.minUpTime > 168)
        cluster.minUpTime = 168;

    if (cluster.minDownTime < 1)
        cluster.minDownTime = 1;
    if (cluster.minDownTime > 168)
        cluster.minDownTime = 168;
}

bool ThermalClusterReader::loadFromProperty(ThermalCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;
    std::function<bool(ThermalCluster&, const IniFile::Property&)> cb;
    try {
        const std::string k = p->key.c_str();
        cb = callbackMap.at(k);
    } catch(const std::out_of_range& oor) {
        logs.error() << "Property not found: " << p->key;
        return false;
    }
    return cb(cluster, *p);
}

bool ThermalClusterReader::legacyLoadFromProperty(ThermalCluster& cluster, const IniFile::Property* p) const
{
    if (p->key.empty())
        return false;

    if (p->key == "annuityinvestment")
        return p->value.to<uint>(cluster.annuityInvestment);
    if (p->key == "dailyminimumcapacity")
    {
        double d = p->value.to<double>();
        if (d && cluster.minUpTime < 24)
            cluster.minUpTime = 24;
        if (d && cluster.minDownTime < 24)
            cluster.minDownTime = 24;
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
    }
    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);
    if (p->key == "fixed-cost")
        return p->value.to<double>(cluster.fixedCost);
    if (p->key == "flexibility")
    {
        // The flexibility is now ignored since v3.5
        return true;
    }
    if (p->key == "groupmincount")
        return p->value.to<uint>(cluster.groupMinCount);
    if (p->key == "groupmaxcount")
        return p->value.to<uint>(cluster.groupMaxCount);
    if (p->key == "group")
    {
        cluster.setGroup(p->value);
        return true;
    }
    if (p->key == "gen-ts")
    {
        return p->value.to(cluster.tsGenBehavior);
    }
    if (p->key == "hourlyminimumcapacity")
    {
        double d = p->value.to<double>();
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
    }
    if (p->key == "law.planned")
        return p->value.to(cluster.plannedLaw);
    if (p->key == "law.forced")
        return p->value.to(cluster.forcedLaw);
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
    if (p->key == "name")
        return true; // silently ignore it
    if (p->key == "nominalcapacity")
        return p->value.to<double>(cluster.nominalCapacity);

    // for compatibility <3.5
    if (p->key == "operatingcost")
        return p->value.to<double>(cluster.marketBidCost);

    if (p->key == "spread-cost")
        return p->value.to<double>(cluster.spreadCost);
    if (p->key == "spinning")
        return p->value.to<double>(cluster.spinning);
    if (p->key == "startup-cost")
        return p->value.to<double>(cluster.startupCost);
    // for compatibility <3.5
    if (p->key == "stddeviationannualcost")
        return p->value.to<double>(cluster.spreadCost);

    if (p->key == "unitcount")
        return p->value.to<uint>(cluster.unitCount);
    if (p->key == "volatility.planned")
        return p->value.to(cluster.plannedVolatility);
    if (p->key == "volatility.forced")
        return p->value.to(cluster.forcedVolatility);
    if (p->key == "weeklyminimumcapacity")
    {
        double d = p->value.to<double>();
        if (d && cluster.minUpTime < 168)
            cluster.minUpTime = 168;
        if (d && cluster.minDownTime < 168)
            cluster.minDownTime = 168;
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
    }

    //pollutant
    if (p->key == "co2")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::CO2]);
    if (p->key == "nh3")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::NH3]);
    if (p->key == "so2")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::SO2]);
    if (p->key == "nox")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::NOX]);
    if (p->key == "pm2_5")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::PM2_5]);
    if (p->key == "pm5")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::PM5]);
    if (p->key == "pm10")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::PM10]);
    if (p->key == "nmvoc")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::NMVOC]);
    if (p->key == "op1")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::OP1]);
    if (p->key == "op2")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::OP2]);
    if (p->key == "op3")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::OP3]);
    if (p->key == "op4")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::OP4]);
    if (p->key == "op5")
        return p->value.to<double>
            (cluster.emissions.emissionFactors[Pollutant::OP5]);


    // The property is unknown
    return false;
}

} //namespace Antares::Data
