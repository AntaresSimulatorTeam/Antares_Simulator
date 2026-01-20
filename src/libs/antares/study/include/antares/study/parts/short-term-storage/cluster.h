// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <list>
#include <memory>
#include <string>

#include <antares/inifile/inifile.h>
#include <antares/study/version.h>

#include "additionalConstraints.h"
#include "properties.h"
#include "series.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster final
{
public:
    bool enabled() const;

    bool validate(StudyVersion studyVersion) const;

    bool loadFromSection(const IniFile::Section& section);

    bool loadSeries(const std::filesystem::path& folder, StudyVersion studyVersion) const;

    void saveProperties(IniFile& ini) const;

    bool saveSeries(const std::string& path) const;

    std::string id;

    std::shared_ptr<Series> series = std::make_shared<Series>();
    mutable Properties properties;
    std::vector<std::shared_ptr<AdditionalConstraints>> additionalConstraints;
};
} // namespace Antares::Data::ShortTermStorage
