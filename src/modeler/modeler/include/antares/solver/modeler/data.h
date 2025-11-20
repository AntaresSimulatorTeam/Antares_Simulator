// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
#pragma once

#include <vector>

#include <antares/modeler-optimisation-container/scenarioGroupRepo.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

namespace Antares::Modeler
{

enum class ResolutionMode : unsigned
{
    BENDERS_DECOMPOSITION = 0,
    SEQUENTIAL_SUBPROBLEMS = 1
};

struct Data
{
    std::vector<ModelerStudy::SystemModel::Library> libraries;
    std::unique_ptr<ModelerStudy::SystemModel::System> system;
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> dataSeries;
    Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    ResolutionMode resolutionMode = ResolutionMode::SEQUENTIAL_SUBPROBLEMS;
};

} // namespace Antares::Modeler
