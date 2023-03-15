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

#include <antares/logs.h>
#include <filesystem>

#include "container.h"

#define SEP std::filesystem::path::preferred_separator

namespace Antares::Data::ShortTermStorage
{

bool STstorageInput::validate()
{
    bool ret = true;
    for (auto& cluster : storagesByIndex)
        ret &= cluster->validate();
    return ret;
}


bool STstorageInput::createSTstorageClustersFromIniFile(const std::string& path,
        const std::string& parentId)
{
    const std::string pathIni(path + SEP + "list.ini");
    IniFile ini;
    if (!ini.open(pathIni))
        return false;

    if (!ini.firstSection)
        return true;

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STstorageCluster cluster;
        if (!cluster.loadFromSection(*section))
            return false;
        cluster.parentId = parentId;

        storagesById.insert(std::pair<std::string, STstorageCluster>(section->name.c_str(), cluster));
    }

    for (auto& storage : storagesById)
        storagesByIndex.push_back(&storage.second);

    return true;
}

bool STstorageInput::loadSeriesFromFolder(const std::string& folder)
{
    if (folder.empty())
        return false;

    bool ret = true;

    for (auto& cluster : storagesByIndex)
    {
        const std::string buffer = folder + SEP + cluster->getName()
            + SEP + "series.txt";
        /* ret = series->series.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret; */
        cluster->loadSeries(folder);
    }

    /* if (s.usedByTheSolver && s.parameters.derated) */
    /*     series->series.averageTimeseries(); */

    /* series->timeseriesNumbers.clear(); */

    return ret;
}

} // namespace Antares::Data::ShortTermStorage
