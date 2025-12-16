/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#pragma once
#include <filesystem>
#include <string>

#include <antares/study/version.h>

#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageInput final
{
public:
    bool validate(StudyVersion studyVersion) const;

    /// 1. Read list.ini
    bool createSTStorageClustersFromIniFile(const std::filesystem::path& path);

    /// 2. Read ALL series
    bool loadSeriesFromFolder(const std::filesystem::path& folder, StudyVersion studyVersion) const;

    /// Number of enabled ST storages, ignoring disabled ST storages
    std::size_t count() const;

    bool loadAdditionalConstraints(const std::filesystem::path& filePath);

    /// erase disabled cluster from the vector
    uint removeDisabledClusters();

    bool saveToFolder(const std::string& folder) const;

    bool saveDataSeriesToFolder(const std::string& folder) const;

    std::vector<STStorageCluster> storagesByIndex;
    void resizeTimeseriesNumbers(unsigned int nbYears);

    /// Number cumulative - constraint
    std::size_t cumulativeConstraintCount() const;

private:
    bool loadAdditionalConstraintsFromIni(const std::filesystem::path& filePath);
    bool loadAdditionalConstraintsRHS(const std::filesystem::path& filePath);
};
} // namespace Antares::Data::ShortTermStorage
