// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

    std::vector<STStorageCluster> storagesByIndex;
    void resizeTimeseriesNumbers(unsigned int nbYears);

    /// Number cumulative - constraint
    std::size_t cumulativeConstraintCount() const;

private:
    bool loadAdditionalConstraintsFromIni(const std::filesystem::path& filePath);
    bool loadAdditionalConstraintsRHS(const std::filesystem::path& filePath);
};
} // namespace Antares::Data::ShortTermStorage
