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
#include <filesystem>
#include <string>

#include "AdditionalConstraint.h"
#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageInput
{
public:
    bool validate() const;

    /// 1. Read list.ini
    bool createSTStorageClustersFromIniFile(const std::filesystem::path& path);

    /// 2. Read ALL series
    bool loadSeriesFromFolder(const std::filesystem::path& folder) const;

    /// Number of enabled ST storages, ignoring disabled ST storages
    std::size_t count() const;

    bool LoadConstraintsFromIniFile(const std::filesystem::path& filePath);

    /// erase disabled cluster from the vector
    uint removeDisabledClusters();

    bool saveToFolder(const std::string& folder) const;

    bool saveDataSeriesToFolder(const std::string& folder) const;

    std::vector<STStorageCluster> storagesByIndex;

    /// Number cumulative - constraint
    std::size_t cumulativeConstraintCount() const;
};
} // namespace Antares::Data::ShortTermStorage
