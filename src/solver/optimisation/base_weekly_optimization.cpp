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

#include <memory>

#include "antares/solver/optimisation/adequacy_patch_local_matching/adequacy_patch_weekly_optimization.h"
#include "antares/solver/optimisation/weekly_optimization.h"

using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

namespace Antares::Solver::Optimization
{
WeeklyOptimization::WeeklyOptimization(const OptimizationOptions& options,
                                       PROBLEME_HEBDO* problemesHebdo,
                                       AdqPatchParams& adqPatchParams,
                                       uint thread_number,
                                       IResultWriter& writer,
                                       Simulation::ISimulationObserver& simulationObserver):
    options_(options),
    problemeHebdo_(problemesHebdo),
    adqPatchParams_(adqPatchParams),
    thread_number_(thread_number),
    writer_(writer),
    simulationObserver_(simulationObserver)
{
}

} // namespace Antares::Solver::Optimization
