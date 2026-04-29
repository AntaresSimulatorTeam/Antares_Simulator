// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/short-term-storage/cluster.h"

#include <yuni/core/string.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

namespace Antares::Data::ShortTermStorage
{
bool STStorageCluster::loadFromSection(const IniFile::Section& section)
{
    if (!section.firstProperty)
    {
        return false;
    }

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
    {
        return false;
    }

    id = transformNameIntoID(properties.name);

    return true;
}

bool STStorageCluster::enabled() const
{
    return properties.enabled;
}

bool STStorageCluster::validate(StudyVersion studyVersion) const
{
    if (!enabled())
    {
        return true;
    }

    logs.debug() << "Validating properties and series for st storage: " << id;
    return properties.validate() && series->validate(id, studyVersion);
}

bool STStorageCluster::loadSeries(const std::filesystem::path& folder,
                                  StudyVersion studyVersion) const
{
    bool ret = series->loadFromFolder(folder, studyVersion);
    series->fillDefaultSeriesIfEmpty(); // fill series if no file series
    return ret;
}

} // namespace Antares::Data::ShortTermStorage
