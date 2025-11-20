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

#include "antares/solver/simulation/remix-storage/storage-for-remix-with-levels.h"

#include <stdexcept>

#include "antares/solver/simulation/remix-storage/remix-utils.h"

namespace Antares::Solver::Simulation
{

StorageForRemixWithLevels::StorageForRemixWithLevels(std::vector<double>& withdrawal,
                                                     std::vector<double>& unsupE,
                                                     std::vector<double>& levels,
                                                     const std::vector<double> Pmax,
                                                     const std::vector<double> Pmin,
                                                     const std::vector<double> inflows,
                                                     const std::vector<double> overflow,
                                                     const std::vector<double>& injection,
                                                     const std::vector<double> lowRuleCurve,
                                                     const std::vector<double> upRuleCurve,
                                                     const double initLevel,
                                                     const double withdrawalEff,
                                                     const double injectionEff):
    StorageForRemixNoLevels(withdrawal, unsupE, Pmax, Pmin),
    levels_(levels),
    inflows_(inflows),
    overflow_(overflow),
    injection_(injection),
    ruleCurveLow_(lowRuleCurve),
    ruleCurveUp_(upRuleCurve),
    initLevel_(initLevel),
    withdrawalEff_(withdrawalEff),
    injectionEff_(injectionEff)
{
    checkInput(unsupE_.size());
    update();
    checkLevels();
}

double StorageForRemixWithLevels::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const
{
    double boundNoLevels = StorageForRemixNoLevels::maxExchange(hourOfMaxGen, hourOfMinGen);

    unsigned hour = std::min(hourOfMinGen, hourOfMaxGen);
    unsigned HOUR = std::max(hourOfMinGen, hourOfMaxGen);

    if (hourOfMinGen < hourOfMaxGen)
    {
        return std::min(boundNoLevels, min_on_subrange(levels_ - ruleCurveLow_, hour, HOUR));
    }
    else
    {
        return std::min(boundNoLevels, min_on_subrange(ruleCurveUp_ - levels_, hour, HOUR));
    }
}

void StorageForRemixWithLevels::update()
{
    levels_[0] = initLevel_ + inflows_[0] - overflow_[0] + injectionEff_ * injection_[0]
                 - withdrawalEff_ * withdrawal_[0];
    for (size_t h = 1; h < levels_.size(); ++h)
    {
        levels_[h] = levels_[h - 1] + inflows_[h] - overflow_[h] + injectionEff_ * injection_[h]
                     - withdrawalEff_ * withdrawal_[h];
    }
}

void StorageForRemixWithLevels::checkLevels()
{
    if (!(levels_ <= ruleCurveUp_ + TOLERANCE) || !(levels_ >= -TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

void StorageForRemixWithLevels::checkInput(size_t size)
{
    StorageForRemixNoLevels::checkInput(size);

    std::vector<size_t> sizes = {size,
                                 inflows_.size(),
                                 overflow_.size(),
                                 injection_.size(),
                                 levels_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "arrays of different sizes");
    }

    if (!size)
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "all arrays of sizes 0");
    }

    if (ruleCurveUp_ + TOLERANCE <= initLevel_)
    {
        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "initial level > reservoir capacity");
    }
}

} // namespace Antares::Solver::Simulation
