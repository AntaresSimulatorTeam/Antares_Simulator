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

#include <logs/logs.h>
#include <yuni/io/file.h>
#include <filesystem>
#include <string>

#include "container.h"

#define SEP Yuni::IO::Separator

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::all_of(storagesByIndex.cbegin(), storagesByIndex.cend(), [](auto& cluster) {
        return cluster->validate();
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

        storagesById.try_emplace(cluster.properties.name, cluster);
    }

    storagesByIndex.reserve(storagesById.size());
    for (auto& [id, storage] : storagesById)
        storagesByIndex.push_back(&storage);

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const std::string& folder) const
{
    if (folder.empty())
        return false;

    bool ret = true;

    for (auto& cluster : storagesByIndex)
    {
        const std::string buffer(folder + SEP + cluster->id);
        ret = cluster->loadSeries(buffer) && ret;
    }

    return ret;
}

bool STStorageInput::saveToFolder(const std::string& folder) const
{
    // create empty list.ini if there's no sts in this area
    Yuni::IO::Directory::Create(folder);
    Yuni::IO::File::CreateEmptyFile(folder + SEP + "list.ini");
    logs.notice() << "created empty ini: " << folder + SEP + "list.ini";

    return std::all_of(storagesByIndex.cbegin(), storagesByIndex.cend(), [&folder](auto& storage) {
        return storage->saveProperties(folder);
    });
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::all_of(storagesByIndex.cbegin(), storagesByIndex.cend(), [&folder](auto& storage) {
        return storage->saveSeries(folder + SEP + storage->id);
    });
}

std::size_t STStorageInput::count() const
{
    return storagesByIndex.size();
}
} // namespace Antares::Data::ShortTermStorage
