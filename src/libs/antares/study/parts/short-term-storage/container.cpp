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

#include "antares/study/parts/short-term-storage/container.h"

#include <algorithm>
#include <numeric>
#include <regex>
#include <string>

#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

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

static bool loadHours(std::string hoursStr, AdditionalConstraints& additionalConstraints)
{
    std::erase_if(hoursStr, ::isspace);
    // Validate the entire string format
    if (std::regex fullFormatRegex(R"(^(\[\d+(,\d+)*\])(,(\[\d+(,\d+)*\]))*$)");
        !std::regex_match(hoursStr, fullFormatRegex))
    {
        logs.error() << "In constraint " << additionalConstraints.name
                     << ": Input string does not match the required format: " << hoursStr << '\n';
        return false;
    }
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
        int hourVal;
        while (std::getline(ss, hour, ','))
        {
            try
            {
                hourVal = std::stoi(hour);
                hourSet.insert(hourVal);
            }

            catch (const std::invalid_argument& ex)
            {
                logs.error() << "In constraint " << additionalConstraints.name
                             << " Hours sets contains invalid values: " << hour
                             << "\n exception thrown: " << ex.what() << '\n';

                return false;
            }
            catch (const std::out_of_range& ex)
            {
                logs.error() << "In constraint " << additionalConstraints.name
                             << " Hours sets contains out of range values: " << hour
                             << "\n exception thrown: " << ex.what() << '\n';
                return false;
            }
        }
        if (!hourSet.empty())
        {
            // Add this group to the `hours` vec
            additionalConstraints.constraints.push_back(
              {.hours = hourSet, .localIndex = localIndex});
            ++localIndex;
        }
    }
    return true;
}

static bool readRHS(AdditionalConstraints& additionalConstraints, const fs::path& rhsPath)
{
    const auto ret = loadFile(rhsPath, additionalConstraints.rhs);
    if (ret)
    {
        fillIfEmpty(additionalConstraints.rhs, 0.0);
    }
    return ret;
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
        AdditionalConstraints additionalConstraints;
        additionalConstraints.name = section->name.c_str();
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            const std::string key = property->key;
            const auto value = property->value;

            if (key == "cluster")
            {
                std::string clusterName;
                value.to<std::string>(clusterName);
                additionalConstraints.cluster_id = transformNameIntoID(clusterName);
            }
            else if (key == "enabled")
            {
                value.to<bool>(additionalConstraints.enabled);
            }
            else if (key == "variable")
            {
                value.to<std::string>(additionalConstraints.variable);
            }
            else if (key == "operator")
            {
                value.to<std::string>(additionalConstraints.operatorType);
            }
            else if (key == "hours" && !loadHours(value.c_str(), additionalConstraints))
            {
                return false;
            }
        }

        // We don't want load RHS and link the STS time if the constraint is disabled
        if (!additionalConstraints.enabled)
        {
            return true;
        }

        if (const auto rhsPath = parentPath / ("rhs_" + additionalConstraints.name + ".txt");
            !readRHS(additionalConstraints, rhsPath))
        {
            logs.error() << "Error while reading rhs file: " << rhsPath;
            return false;
        }

        if (auto [ok, error_msg] = additionalConstraints.validate(); !ok)
        {
            logs.error() << "Invalid constraint in section: " << section->name;
            logs.error() << error_msg;
            return false;
        }

        auto it = std::ranges::find_if(storagesByIndex,
                                       [&additionalConstraints](const STStorageCluster& cluster)
                                       { return cluster.id == additionalConstraints.cluster_id; });
        if (it == storagesByIndex.end())
        {
            logs.warning() << " from file " << pathIni;
            logs.warning() << "Constraint " << section->name
                           << " does not reference an existing cluster";
            return false;
        }
        else
        {
            it->additionalConstraints.push_back(additionalConstraints);
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
                   cluster.additionalConstraints.begin(),
                   cluster.additionalConstraints.end(),
                   0,
                   [](size_t inner_constraint_count, const auto& additionalConstraints)
                   { return inner_constraint_count + additionalConstraints.enabledConstraints(); });
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
