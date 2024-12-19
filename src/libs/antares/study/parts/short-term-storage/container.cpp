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

bool STStorageInput::LoadConstraintsFromIniFile(const fs::path& parent_path)
{
    IniFile ini;
    const auto pathIni = parent_path / "additional-constraints.ini";
    if (!ini.open(pathIni, false))
    {
        logs.info() << "There is no: " << pathIni;
        return true;
    }

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        AdditionalConstraint constraint;
        constraint.name = section->name.c_str();
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            const std::string key = property->key;
            const auto value = property->value;

            if (key == "cluster")
            {
                // TODO do i have to transform the name to id? TransformNameIntoID
                std::string clusterName;
                value.to<std::string>(clusterName);
                constraint.cluster_id = transformNameIntoID(clusterName);
            }
            else if (key == "variable")
            {
                value.to<std::string>(constraint.variable);
            }
            else if (key == "operator")
            {
                value.to<std::string>(constraint.operatorType);
            }
            else if (key == "hours")
            {
                std::stringstream ss(value.c_str());
                std::string hour;
                while (std::getline(ss, hour, ','))
                {
                    int hourVal = std::stoi(hour);
                    constraint.hours.insert(hourVal);
                }
            }
            else if (key == "rhs")
            {
                property->value.to<double>(constraint.rhs);
            }
        }

        if (auto ret = constraint.validate(); !ret.ok)
        {
            logs.error() << "Invalid constraint in section: " << section->name;
            logs.error() << ret.error_msg;
            return false;
        }

        auto it = std::find_if(storagesByIndex.begin(),
                               storagesByIndex.end(),
                               [&constraint](const STStorageCluster& cluster)
                               { return cluster.id == constraint.cluster_id; });
        if (it == storagesByIndex.end())
        {
            logs.warning() << " from file " << pathIni;
            logs.warning() << "Constraint " << section->name
                           << " does not reference an existing cluster";
            return false;
        }
        else
        {
            it->additional_constraints.push_back(constraint);
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
    return std::accumulate(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           0,
                           [](int acc, const auto& cluster)
                           { return acc + cluster.additional_constraints.size(); });
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
