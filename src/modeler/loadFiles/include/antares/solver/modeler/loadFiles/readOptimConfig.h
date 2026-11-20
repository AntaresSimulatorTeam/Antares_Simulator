// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <vector>

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/library.h>
#include "antares/io/inputs/yml-optim-config/OptimConfig.h"

using namespace Antares::IO::Inputs::YmlOptimConfig;

namespace Antares::Solver::LoadFiles
{
OptimConfig loadOptimConfigFromYaml(const std::filesystem::path& studyPath);

/// Converts the heuristics of optim-config.yml and checks them against the models they refer to
HeuristicsByModel convertHeuristics(
  const OptimConfig& ymlOptimConfig,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);
} // namespace Antares::Solver::LoadFiles
