// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <antares/study/study.h>

namespace Antares::ViewBuilder
{

void exportSystemForView(const Data::Study& study, Solver::IResultWriter* resultWriter);

YAML::Node generateSystemForView(const Antares::Data::Study& study);
YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study);
YAML::Node mergeHybridSystemYaml(const Antares::Data::Study& study, const YAML::Node& legacyYaml);

} // namespace Antares::ViewBuilder
