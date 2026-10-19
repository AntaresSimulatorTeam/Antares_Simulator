// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <antares/study/study.h>

namespace Antares::ViewBuilder
{

std::string areaLocation(const std::string& areaId);

YAML::Node areaToYaml(const Data::Area& area);
YAML::Node loadToYaml(const Data::Area& area);
YAML::Node windToYaml(const Data::Area& area);
YAML::Node solarToYaml(const Data::Area& area);
YAML::Node rorToYaml(const Data::Area& area);
YAML::Node linkToYaml(const Data::AreaLink& link);
YAML::Node thermalClusterToYaml(const Data::ThermalCluster& cluster);
YAML::Node renewableClusterToYaml(const Data::RenewableCluster& cluster);
YAML::Node miscGenToYaml(const Data::Area& area, int miscGenIndex);
YAML::Node shortTermStorageToYaml(const Data::Area& area,
                                  const Data::ShortTermStorage::STStorageCluster& storage);
YAML::Node longTermStorageToYaml(const Data::Area& area);

} // namespace Antares::ViewBuilder
