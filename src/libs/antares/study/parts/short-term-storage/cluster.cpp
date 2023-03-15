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

#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{

bool STstorageCluster::loadFromSection(const IniFile::Section& section)
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
            logs.warning() << "Loading clusters: `" << section.name << "`/`"
                << property->key << "`: The property is unknown and ignored";
        }
    }

    return true;
}

bool STstorageCluster::validate()
{
    return properties.validate() && series.validate();
}

bool STstorageCluster::loadSeries(const std::string& path)
{
    return series.loadFromFolder(path);
}

const std::string STstorageCluster::getName()
{
    return properties.name;
}

void STstorageCluster::printProperties()
{
    logs.notice() << "name : " << properties.name;
    logs.notice() << "injectionnominalcapacity : " << properties.injectionCapacity;
    logs.notice() << "withdrawalnominalcapacity : " << properties.withdrawalCapacity;
    logs.notice() << "reservoircapacity : " << properties.capacity;
    if (properties.initialLevel.has_value())
        logs.notice() << "initiallevel : " << *properties.initialLevel;
    else
        logs.notice() << "initiallevel : Not initialized";
    logs.notice() << "efficiency : " << properties.efficiencyFactor;
    logs.notice() << "storagecycle : " << properties.cycleDuration;
    for (auto& i : Properties::stStoragePropertyGroupEnum)
        if (i.second == properties.group)
            logs.notice() << "group : " << i.first;
}

} // namespace Antares::Data::ShortTermStorage
