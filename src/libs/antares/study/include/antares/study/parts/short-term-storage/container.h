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

#pragma once
#include <string>
#include <map>
#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageInput
{
public:
    bool validate() const;
    /// 1. Read list.ini
    bool createSTStorageClustersFromIniFile(const std::string& path);
    /// 2. Read ALL series
    bool loadSeriesFromFolder(const std::string& folder) const;

    bool loadReserveParticipations(Area& area, const AnyString& file);

    /// Number of enabled ST storages, ignoring disabled ST storages
    std::size_t count() const;
    /// erase disabled cluster from the vector
    uint removeDisabledClusters();

    /// Get the names of the cluster and reserve of the participation
    std::pair<Data::ClusterName, Data::ReserveName> reserveParticipationClusterAt(
      const Area* area,
      unsigned int index) const;

    /// Get the names of the group and reserve of the participation
    std::pair<Data::ShortTermStorage::Group, Data::ReserveName> reserveParticipationGroupAt(
      const Area* area,
      unsigned int index) const;

    bool saveToFolder(const std::string& folder) const;
    bool saveDataSeriesToFolder(const std::string& folder) const;

    std::optional<std::reference_wrapper<STStorageCluster>> getClusterByName(
      const std::string& name);

    size_t getClusterIdx(STStorageCluster& cluster);

    // returns the number of reserve participations of all clusters
    uint reserveParticipationsCount();

    std::vector<STStorageCluster> storagesByIndex;
};
} // namespace Antares::Data::ShortTermStorage
