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
