// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <unordered_set>
#include <yaml-cpp/yaml.h>

#include <antares/study/study.h>

namespace Antares::ViewBuilder
{

enum class ViewType
{
    area,
    link,
    thermal,
    storage,
    renewable,
};

void exportSystemForView(const Data::Study& study, Solver::IResultWriter* resultWriter);
bool exportViewConfig(const Data::Study& study, Solver::IResultWriter* resultWriter, ViewType type);

YAML::Node generateSystemForView(const Antares::Data::Study& study);
YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study);
YAML::Node generateAreaViewConfig(const Data::Study& study);
YAML::Node generateViewConfig(const Data::Study& study, ViewType type);

} // namespace Antares::ViewBuilder
