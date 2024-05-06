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
#include <filesystem>
#include <string>

#include <yuni/io/file.h>

#include <antares/logs/logs.h>

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::all_of(storagesByIndex.cbegin(),
                       storagesByIndex.cend(),
                       [](auto& cluster) { return cluster.validate(); });
}

bool STStorageInput::createSTStorageClustersFromIniFile(const fs::path& path)
{
    const fs::path pathIni = path / "list.ini";
    IniFile ini;
    if (!ini.open(pathIni.string()))
        return false;

    if (!ini.firstSection)
    {
        return true;
    }

    logs.debug() << "  :: loading `" << pathIni.string() << "`";

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STStorageCluster cluster;
        if (!cluster.loadFromSection(*section))
        {
            return false;
        }

        storagesByIndex.push_back(cluster);
    }

    std::sort(storagesByIndex.begin(),
              storagesByIndex.end(),
              [&](const auto& a, const auto& b) { return a.properties.name < b.properties.name; });

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const std::string& folder) const
{
    if (folder.empty())
    {
        return false;
    }

    bool ret = true;

    for (auto& cluster: storagesByIndex)
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
    std::for_each(storagesByIndex.cbegin(),
                  storagesByIndex.cend(),
                  [&ini](auto& storage) { return storage.saveProperties(ini); });

    return ini.save(pathIni);
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::all_of(storagesByIndex.cbegin(),
                       storagesByIndex.cend(),
                       [&folder](auto& storage)
                       { return storage.saveSeries(folder + SEP + storage.id); });
}

std::size_t STStorageInput::count() const
{
    return std::count_if(storagesByIndex.begin(),
                         storagesByIndex.end(),
                         [](const STStorageCluster& st) { return st.properties.enabled; });
}

uint STStorageInput::removeDisabledClusters()
{
    const auto& it = std::remove_if(storagesByIndex.begin(),
                                    storagesByIndex.end(),
                                    [](const auto& c) { return !c.enabled(); });

    uint disabledCount = std::distance(it, storagesByIndex.end());
    storagesByIndex.erase(it, storagesByIndex.end());

    return disabledCount;
}

} // namespace Antares::Data::ShortTermStorage
