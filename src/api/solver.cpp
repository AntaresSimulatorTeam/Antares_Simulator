/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <antares/api/solver.h>
#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/study-loader/IStudyLoader.h"

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
