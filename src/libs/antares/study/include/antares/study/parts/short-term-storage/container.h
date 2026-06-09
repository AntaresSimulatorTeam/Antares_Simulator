// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <string>

#include <antares/inifile/inifile.h>
#include <antares/study/fwd.h>
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

    bool loadReserveParticipations(Area& area, const std::filesystem::path& file);

    /// Number of enabled ST storages, ignoring disabled ST storages
    std::size_t count() const;

    bool loadAdditionalConstraints(const std::filesystem::path& filePath);

    /// erase disabled cluster from the vector
    uint removeDisabledClusters();

    /// Get the names of the cluster and reserve of the participation
    std::pair<std::string, ReserveID> reserveParticipationClusterAt(const Area* area,
                                                                    unsigned int index) const;

    /// Get the names of the group and reserve of the participation
    std::pair<std::string, ReserveID> reserveParticipationGroupAt(const Area* area,
                                                                  unsigned int index) const;

    bool saveToFolder(const std::string& folder) const;

    bool saveDataSeriesToFolder(const std::string& folder) const;

    STStorageCluster* findInAll(const std::string& name);

    size_t getClusterIdx(STStorageCluster& cluster) const;

    // returns the number of reserve participations of all clusters
    uint reserveParticipationsCount() const;

    std::vector<STStorageCluster> storagesByIndex;
    void resizeTimeseriesNumbers(unsigned int nbYears);

    /// Number cumulative - constraint
    std::size_t cumulativeConstraintCount() const;

private:
    bool loadAdditionalConstraintsFromIni(const std::filesystem::path& filePath);
    bool loadAdditionalConstraintsRHS(const std::filesystem::path& filePath);
};
} // namespace Antares::Data::ShortTermStorage
