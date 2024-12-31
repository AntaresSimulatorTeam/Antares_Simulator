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

#include <algorithm>
#include <numeric>
#include <regex>
#include <string>

#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>
#include "antares/study/parts/short-term-storage/container.h"

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::ranges::all_of(storagesByIndex, [](auto& cluster) { return cluster.validate(); });
}

bool STStorageInput::createSTStorageClustersFromIniFile(const fs::path& path)
{
    const fs::path pathIni = path / "list.ini";
    IniFile ini;
    if (!ini.open(pathIni))
    {
        return false;
    }

    if (!ini.firstSection)
    {
        return true;
    }

    logs.debug() << "  :: loading `" << pathIni << "`";

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STStorageCluster cluster;
        if (!cluster.loadFromSection(*section))
        {
            return false;
        }

        storagesByIndex.push_back(cluster);
    }

    std::ranges::sort(storagesByIndex,
                      [](const auto& a, const auto& b)
                      { return a.properties.name < b.properties.name; });

    return true;
}

static void loadHours(const std::string& hoursStr, AdditionalConstraints& additional_constraints)
{
    //
    // std::stringstream ss(value.c_str());
    // std::string hour;
    // while (std::getline(ss, hour, ','))
    // {
    //     int hourVal = std::stoi(hour);
    //     constraint.hours.insert(hourVal);
    // }

    // Split the `hours` field into multiple groups
    std::regex groupRegex(R"(\[(.*?)\])");
    // Match each group enclosed in square brackets
    auto groupsBegin = std::sregex_iterator(hoursStr.begin(), hoursStr.end(), groupRegex);
    auto groupsEnd = std::sregex_iterator();
    unsigned int localIndex = 0;
    for (auto it = groupsBegin; it != groupsEnd; ++it)
    {
        // Extract the contents of the square brackets
        std::string group = (*it)[1].str();
        std::stringstream ss(group);
        std::string hour;
        std::set<int> hourSet;

        while (std::getline(ss, hour, ','))
        {
            int hourVal = std::stoi(hour);
            hourSet.insert(hourVal);
        }
        if (!hourSet.empty())
        {
            // Add this group to the `hours` vec
            additional_constraints.constraints.push_back(
              {.hours = hourSet, .localIndex = localIndex});
            ++localIndex;
        }
    }
}

static bool readRHS(AdditionalConstraints& additional_constraints,
                    const fs::path& parentPath,
                    const std::string& sectionName)
{
    loadFile(parentPath / ("rhs_" + additional_constraints.name + ".txt"),
             additional_constraints.rhs);
    fillIfEmpty(additional_constraints.rhs, 0.0);

    if (auto [ok, error_msg] = additional_constraints.validate(); !ok)
    {
        logs.error() << "Invalid constraint in section: " << sectionName;
        logs.error() << error_msg;
        return false;
    }
    return true;
}

bool STStorageInput::loadAdditionalConstraints(const fs::path& parentPath)
{
    IniFile ini;
    const auto pathIni = parentPath / "additional-constraints.ini";
    if (!ini.open(pathIni, false))
    {
        logs.info() << "There is no: " << pathIni;
        return true;
    }

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        AdditionalConstraints additional_constraints;
        additional_constraints.name = section->name.c_str();
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            const std::string key = property->key;
            const auto value = property->value;

            if (key == "cluster")
            {
                // TODO do i have to transform the name to id? TransformNameIntoID
                std::string clusterName;
                value.to<std::string>(clusterName);
                additional_constraints.cluster_id = transformNameIntoID(clusterName);
            }
            else if (key == "variable")
            {
                value.to<std::string>(additional_constraints.variable);
            }
            else if (key == "operator")
            {
                value.to<std::string>(additional_constraints.operatorType);
            }
            else if (key == "hours")
            {
                loadHours(value.c_str(), additional_constraints);
            }
        }

        if (!readRHS(additional_constraints, parentPath, section->name))
        {
            return false;
        }

        auto it = std::ranges::find_if(storagesByIndex,
                                       [&additional_constraints](const STStorageCluster& cluster)
                                       { return cluster.id == additional_constraints.cluster_id; });
        if (it == storagesByIndex.end())
        {
            logs.warning() << " from file " << pathIni;
            logs.warning() << "Constraint " << section->name
                           << " does not reference an existing cluster";
            return false;
        }
        else
        {
            it->additional_constraints.push_back(additional_constraints);
        }
    }

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const fs::path& folder) const
{
    if (folder.empty())
    {
        return false;
    }

    bool ret = true;

    for (auto& cluster: storagesByIndex)
    {
        fs::path seriesFolder = folder / cluster.id;
        ret = cluster.loadSeries(seriesFolder) && ret;
    }

    return ret;
}

bool STStorageInput::saveToFolder(const std::string& folder) const
{
    // create empty list.ini if there's no sts in this area
    Yuni::IO::Directory::Create(folder);
    const std::string pathIni(folder + SEP + "list.ini");
    IniFile ini;

    logs.debug() << "saving file " << pathIni;
    std::ranges::for_each(storagesByIndex,
                          [&ini](auto& storage) { return storage.saveProperties(ini); });

    return ini.save(pathIni);
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::ranges::all_of(storagesByIndex,
                               [&folder](auto& storage)
                               { return storage.saveSeries(folder + SEP + storage.id); });
}

std::size_t STStorageInput::cumulativeConstraintCount() const
{
    return std::accumulate(
      storagesByIndex.begin(),
      storagesByIndex.end(),
      0,
      [](size_t outer_constraint_count, const auto& cluster)
      {
          return outer_constraint_count
                 + std::accumulate(
                   cluster.additional_constraints.begin(),
                   cluster.additional_constraints.end(),
                   0,
                   [](size_t inner_constraint_count, const auto& additional_constraints)
                   { return inner_constraint_count + additional_constraints.constraints.size(); });
      });
}

std::size_t STStorageInput::count() const
{
    return std::ranges::count_if(storagesByIndex,
                                 [](const STStorageCluster& st) { return st.properties.enabled; });
}

uint STStorageInput::removeDisabledClusters()
{
    return std::erase_if(storagesByIndex, [](const auto& c) { return !c.enabled(); });
}
} // namespace Antares::Data::ShortTermStorage
