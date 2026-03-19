// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/rules.h"

#include <algorithm>

#include <antares/logs/logs.h>
#include "antares/study/scenario-builder/scBuilderUtils.h"
#include "antares/study/study.h"

using namespace Yuni;

namespace
{
template<class T>
static void reset(T& v, Antares::Data::Study& study)
{
    const uint areaCount = study.areas.size();

    v.clear();
    v.resize(areaCount);
    for (uint i = 0; i != areaCount; ++i)
    {
        v[i].attachArea(study.areas.byIndex[i]);
        v[i].reset(study);
    }
}
} // namespace

namespace Antares::Data::ScenarioBuilder
{
Rules::Rules(Study& study):
    study_(study),
    pAreaCount(0)
{
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
    ::reset(thermal, study_);

    // Renewable
    ::reset(renewable, study_);

    hydroInitialLevels.reset(study_);
    hydroFinalLevels.reset(study_);

    // links NTC
    ::reset(linksNTC, study_);

    binding_constraints.reset(study_);

    ::reset(shortTermStorageInflows, study_);
    ::reset(shortTermStorageAdditionalConstraints, study_);

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

bool Rules::readThermalCluster(const AreaName::Vector& splitKey,
                               const String& value,
                               bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const std::string& clustername = splitKey[3];

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

bool Rules::readRenewableCluster(const AreaName::Vector& splitKey,
                                 const String& value,
                                 bool updaterMode)
{
    const AreaName& areaname = splitKey[1];
    const uint year = splitKey[2].to<uint>();
    const std::string& clustername = splitKey[3];

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

bool Rules::readLoad(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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

bool Rules::readWind(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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

bool Rules::readHydro(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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

bool Rules::readSolar(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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

bool Rules::readInitialHydroLevels(const AreaName::Vector& splitKey,
                                   const String& value,
                                   bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    double val = fromStringToHydroLevel(value, 1.);
    hydroInitialLevels.setTSnumber(area->index, year, val);
    return true;
}

bool Rules::readFinalHydroLevels(const AreaName::Vector& splitKey,
                                 const String& value,
                                 bool updaterMode)
{
    const uint year = splitKey[2].to<uint>();
    const AreaName& areaname = splitKey[1];

    const Data::Area* area = getArea(areaname, updaterMode);
    if (!area)
    {
        return false;
    }

    double finalLevel = fromStringToHydroLevel(value, 1.);
    hydroFinalLevels.setTSnumber(area->index, year, finalLevel);
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

bool Rules::readLink(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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
    linksNTC[fromArea->index].setTSnumber(link, year, val);
    return true;
}

bool Rules::checkGroupExists(const std::string& groupName) const
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

bool Rules::readBindingConstraints(const AreaName::Vector& splitKey, const String& value)
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

ShortTermStorage::STStorageCluster* getShortTermStorage(Area* area,
                                                        const std::string& stStorageClusterName)
{
    auto stStorageCluster = std::ranges::find_if(area->shortTermStorage.storagesByIndex,
                                                 [&stStorageClusterName](
                                                   const ShortTermStorage::STStorageCluster& s)
                                                 { return s.id == stStorageClusterName; });
    if (stStorageCluster == area->shortTermStorage.storagesByIndex.end())
    {
        logs.warning() << "[scenario-builder] In area '" << area->name
                       << "' the short-term storage cluster '" << stStorageClusterName
                       << "' does not exist";
        return nullptr;
    }
    // iterator -> raw pointer
    return &(*stStorageCluster);
}

ShortTermStorage::AdditionalConstraints* getShortTermStorageAdditionalConstraint(
  ShortTermStorage::STStorageCluster* sts,
  const std::string& constraintName)
{
    if (!sts)
    {
        logs.warning() << "[scenario-builder] Short-term storage does not exist";
        return nullptr;
    }
    auto constraint = std::ranges::find_if(
      sts->additionalConstraints,
      [&constraintName](std::shared_ptr<ShortTermStorage::AdditionalConstraints> c)
      { return c->id == constraintName; });
    if (constraint == sts->additionalConstraints.end())
    {
        logs.warning() << "[scenario-builder] In short-term storage '" << sts->id
                       << "' the additional constraint '" << constraintName << "' does not exist";
        return nullptr;
    }
    // iterator -> raw pointer
    return constraint->get();
}

bool Rules::readShortTermStorageInflows(const AreaName::Vector& splitKey,
                                        const String& value,
                                        bool updaterMode)
{
    const AreaName& areaName = splitKey[1];

    Data::Area* area = getArea(areaName, updaterMode);
    if (!area)
    {
        return false;
    }
    const uint year = splitKey[2].to<uint>();

    const std::string stStorageClusterName = splitKey[3];
    if (auto* sts = getShortTermStorage(area, stStorageClusterName))
    {
        shortTermStorageInflows[area->index].setTSnumber(sts, year, fromStringToTSnumber(value));
        return true;
    }
    return false;
}

bool Rules::readShortTermStorageAdditionalConstraints(const AreaName::Vector& splitKey,
                                                      const String& value,
                                                      bool updaterMode)
{
    const AreaName& areaName = splitKey[1];

    Data::Area* area = getArea(areaName, updaterMode);
    if (!area)
    {
        return false;
    }
    const uint year = splitKey[2].to<uint>();

    const std::string stStorageClusterName = splitKey[3];
    if (auto* sts = getShortTermStorage(area, stStorageClusterName))
    {
        const std::string constraintName = splitKey[4];
        if (auto* ct = getShortTermStorageAdditionalConstraint(sts, constraintName))
        {
            shortTermStorageAdditionalConstraints[area->index]
              .setTSnumber(ct, year, fromStringToTSnumber(value));
            return true;
        }
    }
    return false;
}

bool Rules::readLine(const AreaName::Vector& splitKey, const String& value, bool updaterMode)
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
    else if (kind_of_scenario == "s")
    {
        return readSolar(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "hl")
    {
        return readInitialHydroLevels(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "hfl")
    {
        return readFinalHydroLevels(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "ntc")
    {
        return readLink(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "bc")
    {
        return readBindingConstraints(splitKey, value);
    }
    else if (kind_of_scenario == "sts")
    {
        return readShortTermStorageInflows(splitKey, value, updaterMode);
    }
    else if (kind_of_scenario == "sta")
    {
        return readShortTermStorageAdditionalConstraints(splitKey, value, updaterMode);
    }

    return false;
}

bool Rules::apply()
{
    bool returned_status = true;
    if (pAreaCount)
    {
        returned_status = load.apply(study_) && returned_status;
        returned_status = solar.apply(study_) && returned_status;
        returned_status = hydro.apply(study_) && returned_status;
        returned_status = wind.apply(study_) && returned_status;
        for (uint i = 0; i != pAreaCount; ++i)
        {
            returned_status = thermal[i].apply(study_) && returned_status;
            returned_status = renewable[i].apply(study_) && returned_status;
            returned_status = linksNTC[i].apply(study_) && returned_status;
            returned_status = shortTermStorageInflows[i].apply(study_) && returned_status;
            returned_status = shortTermStorageAdditionalConstraints[i].apply(study_)
                              && returned_status;
        }
        returned_status = hydroInitialLevels.apply(study_) && returned_status;
        returned_status = hydroFinalLevels.apply(study_) && returned_status;
        returned_status = binding_constraints.apply(study_) && returned_status;
    }
    else
    {
        returned_status = false;
    }
    return returned_status;
}

void Rules::sendWarningsForDisabledClusters()
{
    for (auto it = disabledClustersOnRuleActive.begin(); it != disabledClustersOnRuleActive.end();
         it++)
    {
        std::vector<uint>& scenariiForCurrentCluster = it->second;
        int nbScenariiForCluster = (int)scenariiForCurrentCluster.size();
        std::vector<uint>::iterator itv = scenariiForCurrentCluster.begin();

        // Listing the 10 first years for which the current cluster was given a specific TS
        // number in the scenario builder. Note that this list of years size could be less then
        // 10, but are at least 1.
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
                       << " not found: it may be disabled, though given TS numbers in sc "
                          "builder for year(s) :";
        logs.warning() << listYears;
    }
}

} // namespace Antares::Data::ScenarioBuilder
