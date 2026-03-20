// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <list>
#include <memory>
#include <string>

#include <antares/inifile/inifile.h>
#include <antares/study/area/ReserveOpt.h>
#include <antares/study/area/reserveParticipationContainer.h>
#include <antares/study/version.h>

#include "additionalConstraints.h"
#include "properties.h"
#include "series.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster final
{
public:
    //! \brief Get the group name string
    std::string getGroup();

    bool enabled() const;

    bool validate(StudyVersion studyVersion) const;

    bool loadFromSection(const IniFile::Section& section);

    bool loadSeries(const std::filesystem::path& folder, StudyVersion studyVersion) const;

    std::string id;

    std::shared_ptr<Series> series = std::make_shared<Series>();
    mutable Properties properties;
    std::vector<std::shared_ptr<AdditionalConstraints>> additionalConstraints;

    //! Reserve participation container to store the participation of the cluster in the reserves
    //! and the symmetries
    ReserveOpt<ReserveParticipationContainer<StorageClusterReserveParticipation>>
      reserveParticipationContainer;
};
} // namespace Antares::Data::ShortTermStorage
