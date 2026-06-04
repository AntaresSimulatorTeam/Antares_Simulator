// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include "antares/io/inputs/yml-optim-config/OptimConfig.h"

using namespace Antares::IO::Inputs::YmlOptimConfig;

namespace Antares::Solver::LoadFiles
{
OptimConfig loadOptimConfigFromYaml(const std::filesystem::path& studyPath);
}
