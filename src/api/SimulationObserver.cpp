
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

#include "SimulationObserver.h"
#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"

namespace Antares::API
{
void SimulationObserver::notifyHebdoProblem(const PROBLEME_HEBDO* problemeHebdo,
                                            int optimizationNumber,
                                            std::string_view name)
{
    if (optimizationNumber != 1) return; //We only care about first optimization
    Solver::HebdoProblemToLpsTranslator translator;
    unsigned int const year = problemeHebdo->year + 1;
    unsigned int const week = problemeHebdo->weekInTheYear + 1;
    std::lock_guard lock(mutex_);
    if (year == 1 && week == 1) {
        lps_.setConstantData(translator.commonProblemData(problemeHebdo->ProblemeAResoudre.get()));
    }
    lps_.addWeeklyData({year, week},
                       translator.translate(problemeHebdo->ProblemeAResoudre.get(), name));
}
Solver::LpsFromAntares&& SimulationObserver::acquireLps() noexcept
{
    std::lock_guard lock(mutex_);
    return std::move(lps_);
}
} // namespace Api