// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <antares/study/study.h>

namespace Antares::ViewBuilder
{

YAML::Node studyToSystemYaml(const Antares::Data::Study& study);

YAML::Node areaToYaml(const Antares::Data::Area& area);
YAML::Node loadToYaml(const Antares::Data::Area& area);
YAML::Node windToYaml(const Antares::Data::Area& area);
YAML::Node solarToYaml(const Antares::Data::Area& area);
YAML::Node rorToYaml(const Antares::Data::Area& area);
YAML::Node linkToYaml(const Antares::Data::AreaLink& link);
YAML::Node thermalClusterToYaml(const Antares::Data::ThermalCluster& cluster);
YAML::Node renewableClusterToYaml(const Antares::Data::RenewableCluster& cluster);
YAML::Node miscGenToYaml(const Antares::Data::Area& area, int miscGenIndex);
YAML::Node shortTermStorageToYaml(const Antares::Data::Area& area, const Antares::Data::ShortTermStorage::STStorageCluster& storage);
YAML::Node longTermStorageToYaml(const Antares::Data::Area& area);

} // namespace Antares::ViewBuilder
