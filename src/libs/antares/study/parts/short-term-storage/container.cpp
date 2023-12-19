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

#include <antares/logs/logs.h>
#include <yuni/io/file.h>
#include <filesystem>
#include <algorithm>
#include <string>

#include "container.h"

#define SEP Yuni::IO::Separator

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::all_of(storagesByIndex.cbegin(), storagesByIndex.cend(), [](auto& cluster) {
        return cluster.validate();
    });
}

bool STStorageInput::createSTStorageClustersFromIniFile(const std::string& path)
{
    const std::string pathIni(path + SEP + "list.ini");
    IniFile ini;
    if (!ini.open(pathIni))
        return false;

    if (!ini.firstSection)
        return true;

    logs.debug() << "  :: loading `" << pathIni << "`";

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STStorageCluster cluster;
        if (!cluster.loadFromSection(*section))
            return false;

        storagesByIndex.push_back(cluster);
    }

    std::sort(storagesByIndex.begin(), storagesByIndex.end(), [&](const auto& a, const auto& b){
        return a.properties.name < b.properties.name;
    });

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const std::string& folder) const
{
    if (folder.empty())
        return false;

    bool ret = true;

    for (auto& cluster : storagesByIndex)
    {
        const std::string buffer(folder + SEP + cluster.id);
        ret = cluster.loadSeries(buffer) && ret;
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

    std::for_each(storagesByIndex.cbegin(), storagesByIndex.cend(), [&ini](auto& storage) {
        return storage.saveProperties(ini);
    });

    return ini.save(pathIni);
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::all_of(storagesByIndex.cbegin(), storagesByIndex.cend(), [&folder](auto& storage) {
        return storage.saveSeries(folder + SEP + storage.id);
    });
}

std::size_t STStorageInput::count() const
{
  return std::count_if(storagesByIndex.begin(),
                       storagesByIndex.end(),
                       [](const STStorageCluster& st) {
                           return st.properties.enabled;
                       });
}

uint STStorageInput::removeDisabledClusters()
{
    const auto& it = std::remove_if(storagesByIndex.begin(), storagesByIndex.end(),
        std::mem_fn(&STStorageCluster::enabled));

    uint disabledCount = std::distance(it, storagesByIndex.end());
    storagesByIndex.erase(it, storagesByIndex.end());

    return disabledCount;
}

} // namespace Antares::Data::ShortTermStorage
