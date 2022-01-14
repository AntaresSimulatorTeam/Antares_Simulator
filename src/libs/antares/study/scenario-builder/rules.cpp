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
        // Thermal clusters, renewable clusters, links NTS : each area
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
    if (!area && !updaterMode)
    {
        // silently ignore the error
        logs.warning() << "[scenario-builder] The area '" << areaname << "' has not been found";
    }
    return area;
}

bool Rules::readThermalCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (clustername.empty())
        return false;

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    const ThermalCluster* cluster = area->thermal.list.find(clustername);
    if (!cluster)
        cluster = area->thermal.mustrunList.find(clustername);

    if (cluster)
    {
        uint val = fromStringToTSnumber(value);
        thermal[area->index].set(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (!updaterMode and isTheActiveRule)
        {
            string clusterId = (area->id).to<string>() + "." + clustername.to<string>();
            disabledClustersOnRuleActive[clusterId].push_back(year);
            return false;
        }
    }
    return true;
}

bool Rules::readRenewableCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (!study_.parameters.renewableGeneration.isClusters())
        return false;

    if (clustername.empty())
        return false;

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    const RenewableCluster* cluster = area->renewable.list.find(clustername);

    if (cluster)
    {
        uint val = fromStringToTSnumber(value);
        renewable[area->index].set(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (!updaterMode and isTheActiveRule)
        {
            string clusterId = (area->id).to<string>() + "." + clustername.to<string>();
            disabledClustersOnRuleActive[clusterId].push_back(year);
            return false;
        }
    }
    return true;
}

bool Rules::readLoad(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    uint val = fromStringToTSnumber(value);
    load.set(area->index, year, val);
    return true;
}

bool Rules::readWind(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    uint val = fromStringToTSnumber(value);
    wind.set(area->index, year, val);
    return true;
}

bool Rules::readHydro(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    uint val = fromStringToTSnumber(value);
    hydro.set(area->index, year, val);
    return true;
}

bool Rules::readSolar(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    uint val = fromStringToTSnumber(value);
    solar.set(area->index, year, val);
    return true;
}

bool Rules::readHydroLevels(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
        return false;

    double val = fromStringToHydroLevel(value, 1.);
    hydroLevels.set(area->index, year, val);
    return true;
}

Data::AreaLink* Rules::getLink(const AreaName& fromAreaName,
                               const AreaName& toAreaName,
                               bool updaterMode)
{
    Data::AreaLink* link = study_.areas.findLink(fromAreaName, toAreaName);
    if (!link && !updaterMode)
    {
        // silently ignore the error
        logs.warning() << "[scenario-builder] The link '" << fromAreaName << " / " << toAreaName
                       << "' has not been found";
    }
    return link;
}

bool Rules::readLink(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& fromAreaName = splitKey[1];
    const AreaName& toAreaName = splitKey[2];
    const uint year = splitKey[3].to<uint>();

    Data::Area* fromArea = getArea(fromAreaName, updaterMode);
    if (!fromArea)
        return false;

    const Data::Area* toArea = getArea(toAreaName, updaterMode);
    if (!toArea)
        return false;

    AreaLink* link = getLink(fromAreaName, toAreaName, updaterMode);
    if (!link)
        return false;

    uint val = fromStringToTSnumber(value);
    linksNTC[fromArea->index].setDataForLink(link, year, val);
    return true;
}

bool Rules::readLine(const AreaName::Vector& splitKey, String value, bool updaterMode = false)
{
    if (splitKey.size() <= 2)
        return false;

    const AreaName& kind_of_scenario = splitKey[0]; // load, thermal, hydro, ..., hydro levels, ...
    if (kind_of_scenario.size() > 3)
        return false;

    if (kind_of_scenario == "t")
        return readThermalCluster(splitKey, value, updaterMode);
    else if (kind_of_scenario == "r")
        return readRenewableCluster(splitKey, value, updaterMode);
    else if (kind_of_scenario == "l")
        return readLoad(splitKey, value, updaterMode);
    else if (kind_of_scenario == "w")
        return readWind(splitKey, value, updaterMode);
    else if (kind_of_scenario == "h")
        return readHydro(splitKey, value, updaterMode);
    else if (kind_of_scenario == "s")
        return readSolar(splitKey, value, updaterMode);
    else if (kind_of_scenario == "hl")
        return readHydroLevels(splitKey, value, updaterMode);
    else if (kind_of_scenario == "ntc")
        return readLink(splitKey, value, updaterMode);
    return false;
}

bool Rules::apply()
{
    bool returned_status = true;
    if (pAreaCount)
    {
        returned_status = returned_status && load.apply(study_);
        returned_status = returned_status && solar.apply(study_);
        returned_status = returned_status && hydro.apply(study_);
        returned_status = returned_status && wind.apply(study_);
        for (uint i = 0; i != pAreaCount; ++i)
        {
            returned_status = returned_status && thermal[i].apply(study_);
            returned_status = returned_status && renewable[i].apply(study_);
            returned_status = returned_status && linksNTC[i].apply(study_);
        }
        returned_status = returned_status && hydroLevels.apply(study_);
    }
    else
        returned_status = false;
    return returned_status;
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
