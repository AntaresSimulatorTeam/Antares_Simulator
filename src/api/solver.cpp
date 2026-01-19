// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/api/solver.h>
#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"

#include "private/API.h"

namespace Antares::API
{

SimulationResults PerformSimulation(
  const std::filesystem::path& study_path,
  const std::filesystem::path& output,
  const Antares::Solver::Optimization::OptimizationOptions& optOptions) noexcept
{
    try
    {
        APIInternal api;
        FileTreeStudyLoader study_loader(study_path);
        return api.run(study_loader, output, optOptions);
    }
    catch (const std::exception& e)
    {
        Antares::API::Error err{.reason = e.what()};
        return SimulationResults{.antares_problems{}, .error = err};
    }
}

} // namespace Antares::API
