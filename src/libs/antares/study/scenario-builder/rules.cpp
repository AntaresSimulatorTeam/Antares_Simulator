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

#include <algorithm>
#include "rules.h"
#include "../study.h"
#include "../../logs.h"
#include "scBuilderUtils.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
Rules::Rules(Study& study) : study_(study), pAreaCount(0)
{
}

void Rules::saveToINIFile(Yuni::IO::File::Stream& file) const
{
    file << "[" << pName << "]\n";
    if (pAreaCount)
    {
        // load
        load.saveToINIFile(study_, file);
        // solar
        solar.saveToINIFile(study_, file);
        // hydro
        hydro.saveToINIFile(study_, file);
        // wind
        wind.saveToINIFile(study_, file);
        // Thermal, each area
        for (uint i = 0; i != pAreaCount; ++i)
        {
            thermal[i].saveToINIFile(study_, file);
            renewable[i].saveToINIFile(study_, file);
            linksNTC[i].saveToINIFile(study_, file);
        }
        // hydro levels
        hydroLevels.saveToINIFile(study_, file);
    }
    file << '\n';
}

bool Rules::reset()
{
    // The new area count
    pAreaCount = study_.areas.size();

    load.reset(study_);
    solar.reset(study_);
    hydro.reset(study_);
    wind.reset(study_);

    // Thermal
    thermal.clear();
    thermal.resize(pAreaCount);

    for (uint i = 0; i != pAreaCount; ++i)
    {
        thermal[i].attachArea(study_.areas.byIndex[i]);
        thermal[i].reset(study_);
    }

    // Renewable
    renewable.clear();
    renewable.resize(pAreaCount);

    for (uint i = 0; i != pAreaCount; ++i)
    {
        renewable[i].attachArea(study_.areas.byIndex[i]);
        renewable[i].reset(study_);
    }

    hydroLevels.reset(study_);

    // links NTC
    linksNTC.clear();
    linksNTC.resize(pAreaCount);

    for (uint i = 0; i != pAreaCount; ++i)
    {
        linksNTC[i].attachArea(study_.areas.byIndex[i]);
        linksNTC[i].reset(study_);
    }


    return true;
}

Data::Area* Rules::getArea(const AreaName& areaname, bool updaterMode)
{
    Data::Area* area = study_.areas.find(areaname);
    if (!area && not updaterMode)
    {
        // silently ignore the error
        logs.warning() << "[scenario-builder] The area '" << areaname << "' has not been found";

    }
    return area;
}

void Rules::readThermalCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (clustername.empty())
        return;

    Data::Area* area = getArea(areaname, updaterMode);
    if (! area)
    return area;
}

    const ThermalCluster* cluster = area->thermal.list.find(clustername);
    if (not cluster)
        cluster = area->thermal.mustrunList.find(clustername);

    if (cluster)
    {
        uint val = fromStringToTSnumber(value);
        thermal[area->index].set(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (not updaterMode and isTheActiveRule)
        {
            string clusterId = (area->id).to<string>() + "." + clustername.to<string>();
            disabledClustersOnRuleActive[clusterId].push_back(year);
        }
    }
}

void Rules::readRenewableCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (not study_.parameters.renewableGeneration.isClusters())
        return;

    if (clustername.empty())
        return;

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    const RenewableCluster* cluster = area->renewable.list.find(clustername);

    if (cluster)
    {
        uint val = fromStringToTSnumber(value);
        renewable[area->index].set(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (not updaterMode and isTheActiveRule)
        {
            string clusterId = (area->id).to<string>() + "." + clustername.to<string>();
            disabledClustersOnRuleActive[clusterId].push_back(year);
        }
    }
}

void Rules::readLoad(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    uint val = fromStringToTSnumber(value);
    load.set(area->index, year, val);
}

void Rules::readWind(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    uint val = fromStringToTSnumber(value);
    wind.set(area->index, year, val);
}

void Rules::readHydro(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    uint val = fromStringToTSnumber(value);
    hydro.set(area->index, year, val);
}

void Rules::readSolar(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    uint val = fromStringToTSnumber(value);
    solar.set(area->index, year, val);
}

void Rules::readHydroLevels(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return;

    double val = fromStringToHydroLevel(value, 1.);
    hydroLevels.set(area->index, year, val);
}

Data::AreaLink* Rules::getLink(const AreaName& fromAreaName, const AreaName& toAreaName, bool updaterMode)
{
    Data::AreaLink* link = study_.areas.findLink(fromAreaName, toAreaName);
    if (!link && not updaterMode)
    {
        // silently ignore the error
        logs.warning() << "[scenario-builder] The link '" << fromAreaName << " / " << toAreaName << "' has not been found";

    }
    return link;
}

void Rules::readLink(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& fromAreaName = splitKey[1];
    const AreaName& toAreaName = splitKey[2];
    const uint year = splitKey[3].to<uint>();

    Data::Area* fromArea = getArea(fromAreaName, updaterMode);
    if (!fromArea)
        return;

    const Data::Area* toArea = getArea(toAreaName, updaterMode);
    if (!toArea)
        return;

    AreaLink* link = getLink(fromAreaName, toAreaName, updaterMode);
    if (!link)
        return;

    uint val = fromStringToTSnumber(value);
    linksNTC[fromArea->index].set(link, year, val);
}

void Rules::readLine(const AreaName::Vector& splitKey,
                           String value,
                           bool updaterMode = false)
{
    assert(splitKey.size() > 2);

    const AreaName& kind_of_scenario = splitKey[0]; // load, thermal, hydro, ..., hydro levels, ...
    if (kind_of_scenario.size() > 2)
        return;

    if (kind_of_scenario == "t")
        readThermalCluster(splitKey, value, updaterMode);
    if (kind_of_scenario == "r")
        readRenewableCluster(splitKey, value, updaterMode);
    if (kind_of_scenario == "l")
        readLoad(splitKey, value, updaterMode);
    if (kind_of_scenario == "w")
        readWind(splitKey, value, updaterMode);
    if (kind_of_scenario == "h")
        readHydro(splitKey, value, updaterMode);
    if (kind_of_scenario == "s")
        readSolar(splitKey, value, updaterMode);
    if (kind_of_scenario == "hl")
        readHydroLevels(splitKey, value, updaterMode);
    if (kind_of_scenario == "tc")
        readLink(splitKey, value, updaterMode);
}

void Rules::apply()
{
    if (pAreaCount)
    {
        load.apply(study_);
        solar.apply(study_);
        hydro.apply(study_);
        wind.apply(study_);
        for (uint i = 0; i != pAreaCount; ++i)
        {
            thermal[i].apply(study_);
            renewable[i].apply(study_);
            linksNTC[i].apply(study_);
        }
        hydroLevels.apply(study_);
    }
}

void Rules::sendWarningsForDisabledClusters()
{
    for (map<string, vector<uint>>::iterator it = disabledClustersOnRuleActive.begin();
         it != disabledClustersOnRuleActive.end();
         it++)
    {
        vector<uint>& scenariiForCurrentCluster = it->second;
        int nbScenariiForCluster = (int)scenariiForCurrentCluster.size();
        vector<uint>::iterator itv = scenariiForCurrentCluster.begin();

        // Listing the 10 first scenarii numbers (years numbers) where current cluster is refered
        // to. Notice that these scenarii could be less then 10, but are at least 1.
        string listScenarii = to_string(*itv + 1);
        itv++;
        for (; itv != scenariiForCurrentCluster.end(); itv++)
            listScenarii += ", " + to_string(*itv + 1);

        // Adding last scenario to the list
        if (nbScenariiForCluster > 10)
            listScenarii += "..." + to_string(scenariiForCurrentCluster.back());

        logs.warning()
          << "Cluster " << it->first
          << " not found: it may be disabled, though given TS numbers in sc builder for year(s) :";
        logs.warning() << listScenarii;
    }
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares
