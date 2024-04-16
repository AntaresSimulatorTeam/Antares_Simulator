/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/study/scenario-builder/rules.h"

#include <algorithm>

#include <antares/logs/logs.h>
#include "antares/study/scenario-builder/TSnumberData.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"
#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data::ScenarioBuilder
{
Rules::Rules(Study& study):
    study_(study),
    pAreaCount(0)
{
}

void
Rules::saveToINIFile(Yuni::IO::File::Stream& file) const
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
        // hydroMaxPower
        hydroMaxPower.saveToINIFile(study_, file);
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
    binding_constraints.saveToINIFile(study_, file);
    file << '\n';
}

bool
Rules::reset()
{
    // The new area count
    pAreaCount = study_.areas.size();

    load.reset(study_);
    solar.reset(study_);
    hydro.reset(study_);
    hydroMaxPower.reset(study_);
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

    binding_constraints.reset(study_);
    return true;
}

Data::Area*
Rules::getArea(const AreaName& areaname, bool updaterMode)
{
    Data::Area* area = study_.areas.find(areaname);
    if (!area && !updaterMode)
    {
        // silently ignore the error
        logs.warning() << "[scenario-builder] The area '" << areaname << "' has not been found";
    }
    return area;
}

bool
Rules::readThermalCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (clustername.empty())
    {
        return false;
    }

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    if (const ThermalCluster* cluster = area->thermal.list.findInAll(clustername); cluster)
    {
        uint val = fromStringToTSnumber(value);
        thermal[area->index].setTSnumber(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (!updaterMode and isTheActiveRule)
        {
            std::string clusterId = (area->id).to<std::string>() + "." + clustername;
            disabledClustersOnRuleActive[clusterId].push_back(year + 1);
            return false;
        }
    }
    return true;
}

bool
Rules::readRenewableCluster(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const ClusterName& clustername = splitKey[3];

    if (!study_.parameters.renewableGeneration.isClusters())
    {
        return false;
    }

    if (clustername.empty())
    {
        return false;
    }

    Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    const RenewableCluster* cluster = area->renewable.list.findInAll(clustername);

    if (cluster)
    {
        uint val = fromStringToTSnumber(value);
        renewable[area->index].setTSnumber(cluster, year, val);
    }
    else
    {
        bool isTheActiveRule = (pName.toLower() == study_.parameters.activeRulesScenario.toLower());
        if (!updaterMode and isTheActiveRule)
        {
            std::string clusterId = (area->id).to<std::string>() + "." + clustername;
            disabledClustersOnRuleActive[clusterId].push_back(year + 1);
            return false;
        }
    }
    return true;
}

bool
Rules::readLoad(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    uint val = fromStringToTSnumber(value);
    load.setTSnumber(area->index, year, val);
    return true;
}

bool
Rules::readWind(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    uint val = fromStringToTSnumber(value);
    wind.setTSnumber(area->index, year, val);
    return true;
}

bool
Rules::readHydro(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    uint val = fromStringToTSnumber(value);
    hydro.setTSnumber(area->index, year, val);
    return true;
}

bool
Rules::readHydroMaxPower(const AreaName::Vector& splitKey,
                         String tsNumberAsString,
                         bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    uint tsNumber = fromStringToTSnumber(tsNumberAsString);
    hydroMaxPower.setTSnumber(area->index, year, tsNumber);
    return true;
}

bool
Rules::readSolar(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    uint val = fromStringToTSnumber(value);
    solar.setTSnumber(area->index, year, val);
    return true;
}

bool
Rules::readHydroLevels(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    double val = fromStringToHydroLevel(value, 1.);
    hydroLevels.setTSnumber(area->index, year, val);
    return true;
}

Data::AreaLink*
Rules::getLink(const AreaName& fromAreaName, const AreaName& toAreaName, bool updaterMode)
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

bool
Rules::readLink(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    const AreaName& fromAreaName = splitKey[1];
    const AreaName& toAreaName = splitKey[2];
    const uint year = splitKey[3].to<uint>();

    Data::Area* fromArea = getArea(fromAreaName, updaterMode);
    if (!fromArea)
    {
        return false;
    }

    const Data::Area* toArea = getArea(toAreaName, updaterMode);
    if (!toArea)
    {
        return false;
    }

    AreaLink* link = getLink(fromAreaName, toAreaName, updaterMode);
    if (!link)
    {
        return false;
    }

    uint val = fromStringToTSnumber(value);
    fromArea = link->from;
    linksNTC[fromArea->index].setDataForLink(link, year, val);
    return true;
}

bool
Rules::checkGroupExists(const std::string& groupName) const
{
    const auto& groups = study_.bindingConstraintsGroups;
    if (!groups[groupName])
    {
        logs.warning() << "[scenario-builder] The binding constraint group '" << groupName
                       << "' does not exist";
        return false;
    }
    return true;
}

bool
Rules::readBindingConstraints(const AreaName::Vector& splitKey, String value)
{
    std::string group_name = splitKey[1].c_str();
    auto year = std::stoi(splitKey[2].c_str());

    if (!checkGroupExists(group_name))
    {
        return false;
    }

    auto tsNumber = fromStringToTSnumber(value);
    binding_constraints.setTSnumber(group_name, year, tsNumber);
    return true;
}

bool
Rules::readLine(const AreaName::Vector& splitKey, String value, bool updaterMode)
{
    if (splitKey.size() <= 2)
    {
        return false;
    }

    const AreaName& kind_of_scenario = splitKey[0]; // load, thermal, hydro, ..., hydro levels, ...
    if (kind_of_scenario.size() > 3)
    {
        return false;
    }

    if (kind_of_scenario == "t")
    {
        return readThermalCluster(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "r")
    {
        return readRenewableCluster(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "l")
    {
        return readLoad(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "w")
    {
        return readWind(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "h")
    {
        return readHydro(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "hgp")
    {
        return readHydroMaxPower(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "s")
    {
        return readSolar(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "hl")
    {
        return readHydroLevels(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "ntc")
    {
        return readLink(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "bc")
    {
        return readBindingConstraints(splitKey, value);
    }
    return false;
}

bool
Rules::apply()
{
    bool returned_status = true;
    if (pAreaCount)
    {
        returned_status = load.apply(study_) && returned_status;
        returned_status = solar.apply(study_) && returned_status;
        returned_status = hydro.apply(study_) && returned_status;
        returned_status = hydroMaxPower.apply(study_) && returned_status;
        returned_status = wind.apply(study_) && returned_status;
        for (uint i = 0; i != pAreaCount; ++i)
        {
            returned_status = thermal[i].apply(study_) && returned_status;
            returned_status = renewable[i].apply(study_) && returned_status;
            returned_status = linksNTC[i].apply(study_) && returned_status;
        }
        returned_status = hydroLevels.apply(study_) && returned_status;
        returned_status = binding_constraints.apply(study_) && returned_status;
    }
    else
    {
        returned_status = false;
    }
    return returned_status;
}

void
Rules::sendWarningsForDisabledClusters()
{
    for (auto it = disabledClustersOnRuleActive.begin(); it != disabledClustersOnRuleActive.end();
         it++)
    {
        std::vector<uint>& scenariiForCurrentCluster = it->second;
        int nbScenariiForCluster = (int)scenariiForCurrentCluster.size();
        std::vector<uint>::iterator itv = scenariiForCurrentCluster.begin();

        // Listing the 10 first years for which the current cluster was given a specific TS number
        // in the scenario builder.
        // Note that this list of years size could be less then 10, but are at least 1.
        std::string listYears = std::to_string(*itv);
        itv++;
        for (int year_count = 1; itv != scenariiForCurrentCluster.end() && year_count < 10;
             itv++, year_count++)
        {
            listYears += ", " + std::to_string(*itv);
        }

        // Adding last scenario to the list
        if (nbScenariiForCluster > 10)
        {
            listYears += ", ..., " + std::to_string(scenariiForCurrentCluster.back());
        }

        logs.warning() << "Cluster " << it->first
                       << " not found: it may be disabled, though given TS numbers in sc builder "
                          "for year(s) :";
        logs.warning() << listYears;
    }
}

} // namespace Antares::Data::ScenarioBuilder
