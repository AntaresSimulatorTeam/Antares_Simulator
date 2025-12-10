#pragma once

#include <filesystem>

#include "antares/io/inputs/yml-optim-config/OptimConfig.h"

using namespace Antares::IO::Inputs::YmlOptimConfig;

namespace Antares::Solver::LoadFiles
{
OptimConfig loadOptimConfigFromYaml(const std::filesystem::path& studyPath);
}
