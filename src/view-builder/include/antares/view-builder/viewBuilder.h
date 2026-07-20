// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <unordered_set>

#include <yaml-cpp/yaml.h>

#include <antares/study/study.h>

namespace Antares::ViewBuilder
{

void exportSystemForView(const Data::Study& study, Solver::IResultWriter* resultWriter);

YAML::Node generateSystemForView(const Antares::Data::Study& study);
YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study,
                                           std::unordered_set<std::string>& legacyComponentIds);

} // namespace Antares::ViewBuilder
