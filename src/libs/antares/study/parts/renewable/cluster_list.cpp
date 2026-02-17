// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/renewable/cluster_list.h"

#include <antares/inifile/inifile.h>
#include <antares/study/area/area.h>
#include <antares/utils/utils.h>
#include "antares/study/study.h"

using namespace Yuni;

namespace fs = std::filesystem;

namespace Antares::Data
{

#define SEP IO::Separator

std::string RenewableClusterList::typeID() const
{
    return "renewables";
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

static bool clusterLoadFromSection(const fs::path& filename,
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

bool RenewableClusterList::loadFromFolder(const fs::path& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading renewable configuration for the area " << area->name;

    // Open the ini file
    fs::path filename = folder / "list.ini";

    IniFile ini;
    if (ini.open(filename, false))
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
                if (!clusterLoadFromSection(filename, *cluster, *section))
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
