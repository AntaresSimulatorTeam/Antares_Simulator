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
#include <antares/utils/utils.h>
#include <antares/logs/logs.h>
#include <yuni/core/string.h>
#include <yuni/io/file.h>

#include "antares/study/parts/common/cluster.h"

namespace Antares::Data::ShortTermStorage
{

bool STStorageCluster::loadFromSection(const IniFile::Section& section)
{
    if (!section.firstProperty)
        return false;

    for (auto* property = section.firstProperty; property; property = property->next)
    {
        if (property->key.empty())
        {
            logs.warning() << "Loading clusters: `" << section.name << "`: Invalid key/value";
            continue;
        }
        if (!properties.loadKey(property))
        {
            logs.warning() << "Loading clusters: `" << section.name << "`/`" << property->key
                           << "`: The property is unknown and ignored";
        }
    }

    if (properties.name.empty())
        return false;

    Yuni::CString<50, false> tmp;
    TransformNameIntoID(properties.name, tmp);
    id = tmp.to<std::string>();

    return true;
}

bool STStorageCluster::validate()
{
    logs.debug() << "Validating properties and series for st storage: " << id;
    return properties.validate() && series->validate();
}

bool STStorageCluster::loadSeries(const std::string& folder) const
{
    bool ret = series->loadFromFolder(folder);
    series->fillDefaultSeriesIfEmpty(); // fill series if no file series
    return ret;
}

bool STStorageCluster::saveProperties(const std::string& path) const
{
    return properties.saveToFolder(path);
}

bool STStorageCluster::saveSeries(const std::string& path) const
{
    return series->saveToFolder(path);
}

} // namespace Antares::Data::ShortTermStorage
