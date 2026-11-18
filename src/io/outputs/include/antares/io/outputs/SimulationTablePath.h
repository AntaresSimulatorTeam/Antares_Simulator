// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

namespace Antares::IO::Outputs
{
inline constexpr char simulationTableDirectoryName[] = "simulation-tables";

[[nodiscard]] inline std::filesystem::path simulationTableOutputDirectory(
  const std::filesystem::path& outputDirectory)
{
    return outputDirectory / simulationTableDirectoryName;
}
} // namespace Antares::IO::Outputs
