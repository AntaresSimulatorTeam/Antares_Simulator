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

#include "antares/study/parts/renewable/cluster_list.h"

#include <antares/inifile/inifile.h>
#include <antares/study/area/area.h>
#include <antares/utils/utils.h>
#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data
{

#define SEP IO::Separator

std::string RenewableClusterList::typeID() const
{
    return "renewables";
}

bool RenewableClusterList::saveToFolder(const AnyString& folder) const
{
    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        for (auto& c: all())
        {
            // Adding a section to the inifile
            IniFile::Section* s = ini.addSection(c->name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", c->name());

            if (!c->group().empty())
            {
                s->add("group", c->group());
            }
            if (!c->enabled)
            {
                s->add("enabled", "false");
            }

            if (!Utils::isZero(c->nominalCapacity))
            {
                s->add("nominalCapacity", c->nominalCapacity);
            }

            if (!Utils::isZero(c->unitCount))
            {
                s->add("unitCount", c->unitCount);
            }

            s->add("ts-interpretation", c->getTimeSeriesModeAsString());
        }

        // Write the ini file
        buffer.clear() << folder << SEP << "list.ini";
        ret = ini.save(buffer) and ret;
    }
    else
    {
        logs.error() << "I/O Error: impossible to create '" << folder << "'";
        return false;
    }

    return true;
}

static bool ClusterLoadFromProperty(RenewableCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
    {
        return false;
    }

    if (p->key == "group")
    {
        cluster.setGroup(p->value);
        return true;
    }

    if (p->key == "name")
    {
        return true;
    }

    if (p->key == "enabled")
    {
        return p->value.to<bool>(cluster.enabled);
    }

    if (p->key == "unitcount")
    {
        return p->value.to<uint>(cluster.unitCount);
    }

    if (p->key == "nominalcapacity")
    {
        return p->value.to<double>(cluster.nominalCapacity);
    }

    if (p->key == "ts-interpretation")
    {
        return cluster.setTimeSeriesModeFromString(p->value);
    }

    // The property is unknown
    return false;
}

static bool ClusterLoadFromSection(const AnyString& filename,
                                   RenewableCluster& cluster,
                                   const IniFile::Section& section)
{
    if (section.name.empty())
    {
        return false;
    }

    cluster.setName(section.name);

    if (section.firstProperty)
    {
        // Browse all properties
        for (auto* property = section.firstProperty; property; property = property->next)
        {
            if (property->key.empty())
            {
                logs.warning() << '`' << filename << "`: `" << section.name
                               << "`: Invalid key/value";
                continue;
            }
            if (!ClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->key << "`: The property is unknown and ignored";
            }
        }
        // update the minUpDownTime
    }
    return true;
}

bool RenewableClusterList::loadFromFolder(const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading renewable configuration for the area " << area->name;

    // Open the ini file
    YString buffer;
    buffer << folder << SEP << "list.ini";

    IniFile ini;
    if (ini.open(buffer, false))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                {
                    continue;
                }

                auto cluster = std::make_shared<RenewableCluster>(area);

                // Load data of a renewable cluster from a ini file section
                if (!ClusterLoadFromSection(buffer, *cluster, *section))
                {
                    continue;
                }

                addToCompleteList(cluster);
            }
        }

        rebuildIndexes();

        return ret;
    }
    return false;
}

bool RenewableClusterList::validateClusters() const
{
    bool ret = true;
    for (const auto& cluster: allClusters_)
    {
        ret = cluster->integrityCheck() && ret;
    }

    return ret;
}

#undef SEP

} // namespace Antares::Data
