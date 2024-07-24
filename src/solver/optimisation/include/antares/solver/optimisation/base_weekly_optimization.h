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

#pragma once

#include <yuni/yuni.h> // for "uint" definition

#include <antares/study/parameters/adq-patch-params.h>
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Optimization
{
class WeeklyOptimization
{
public:
    virtual void solve() = 0;
    virtual ~WeeklyOptimization() = default;
    static std::unique_ptr<WeeklyOptimization> create(
      const Antares::Data::Study& study,
      const OptimizationOptions& options,
      Antares::Data::AdequacyPatch::AdqPatchParams& adqPatchParams,
      PROBLEME_HEBDO* problemesHebdo,
      uint numSpace,
      IResultWriter& writer,
      Simulation::ISimulationObserver& simulationObserver);

protected:
    explicit WeeklyOptimization(const OptimizationOptions& options,
                                PROBLEME_HEBDO* problemesHebdo,
                                Antares::Data::AdequacyPatch::AdqPatchParams&,
                                uint numSpace,
                                IResultWriter& writer,
                                Simulation::ISimulationObserver& simulationObserver);
    Antares::Solver::Optimization::OptimizationOptions options_;
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    Antares::Data::AdequacyPatch::AdqPatchParams& adqPatchParams_;
    const uint thread_number_ = 0;
    IResultWriter& writer_;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;
};
} // namespace Antares::Solver::Optimization
