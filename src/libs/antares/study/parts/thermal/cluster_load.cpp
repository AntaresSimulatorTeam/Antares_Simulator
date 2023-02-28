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

template<class Type>
void ThermalClusterReader::addCallback(std::string&& key, Type ThermalCluster::*value_ptr)
{
    auto cb = [value_ptr](ThermalCluster& c, const IniFile::Property& p)
              { return c.*value_ptr = p.value.to<Type>();};
    callbackMap.emplace(key, cb);
}

ThermalClusterReader::ThermalClusterReader()
{
    for (auto const& [key, val] : Pollutant::namesToEnum)
    {
        callbackMap.emplace(key, [val](ThermalCluster& c, const IniFile::Property& p) {
            return c.emissions.emissionFactors[val] = p.value.to<double>();
        });
    }

    addCallback("annuityinvestment", &ThermalCluster::annuityInvestment);
    addCallback("must-run", &ThermalCluster::mustrun);
    // must cast: enabled is a member of class Cluster (base class of ThermalCluster)
    addCallback("enabled", static_cast<bool ThermalCluster::*>(&Cluster::enabled));
    addCallback("fixed-cost", &ThermalCluster::fixedCost);
    // must cast: tsGenBehavior is an enum class (uint)
    addCallback("gen-ts", static_cast<uint ThermalCluster::*>(&ThermalCluster::tsGenBehavior));
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


bool ThermalClusterReader::legacyLoadFromProperty(ThermalCluster& cluster, const IniFile::Property* p)
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
