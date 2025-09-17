/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"

namespace Antares::Solver::Simulation
{

std::shared_ptr<IStorageForRemix> makeHydroForRemix(std::vector<double>& generation,
                                                    std::vector<double>& unsupE,
                                                    std::vector<double>& levels,
                                                    const std::vector<double>& Pmax,
                                                    const std::vector<double>& Pmin,
                                                    const std::vector<double>& inflows,
                                                    const std::vector<double>& overflow,
                                                    const std::vector<double>& pump,
                                                    const double initLevel,
                                                    const double reservoirCapacity,
                                                    bool reservoirManagement)
{
    if (!reservoirManagement)
    {
        return std::make_shared<StorageForRemixNoLevels>(generation, unsupE, Pmax, Pmin);
    }

    size_t size = generation.size();
    const std::vector<double> lowRuleCurve(size, 0.);
    const std::vector<double> upRuleCurve(size, reservoirCapacity);
    const double withdrawalEff = 1.;
    return std::make_shared<StorageForRemixWithLevels>(generation,
                                                       unsupE,
                                                       levels,
                                                       Pmax,
                                                       Pmin,
                                                       inflows,
                                                       overflow,
                                                       pump,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       withdrawalEff);
}

std::shared_ptr<IStorageForRemix> makeSTSforRemix(std::vector<double>& withdrawal,
                                                  std::vector<double>& unsupE,
                                                  std::vector<double>& levels,
                                                  const std::vector<double>& pmax,
                                                  const std::vector<double>& inflows,
                                                  const std::vector<double>& injection,
                                                  const std::vector<double>& lowRuleCurve,
                                                  const std::vector<double>& upRuleCurve,
                                                  const double initLevel,
                                                  const double withdrawalEff,
                                                  const double injectionEff)
{
    size_t size = withdrawal.size();

    std::vector<double> overflows(size, 0.);
    std::vector<double> pmin(size, 0.);

    return std::make_shared<StorageForRemixWithLevels>(withdrawal,
                                                       unsupE,
                                                       levels,
                                                       pmax,
                                                       pmin,
                                                       inflows,
                                                       overflows,
                                                       injection,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       withdrawalEff,
                                                       injectionEff);
}

} // namespace Antares::Solver::Simulation
