
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
namespace
{
auto translate(const PROBLEME_HEBDO& problemeHebdo,
               std::string_view name,
               const Solver::HebdoProblemToLpsTranslator& translator,
               bool translateCommonData)
{
    auto weekly_data = translator.translate(problemeHebdo.ProblemeAResoudre.get(), name);
    Solver::ConstantDataFromAntares common_data;
    if (translateCommonData)
    {
        common_data = translator.commonProblemData(problemeHebdo.ProblemeAResoudre.get());
    }
    return std::make_pair(common_data, weekly_data);
}
} // namespace

/**
 * @brief Compute whether or not to translate common data.
 * @details This method is thread-safe.
 * Common data need to be translated only once.
 * @return
 */
bool SimulationObserver::shouldTranslateCommonData() const
{
    /**
     * Static variable used to share state between threads.
     */
    bool translateCommonData = false;
    static bool mustTranslateCommonData = true;
    std::lock_guard lock(lps_mutex_);
    translateCommonData = mustTranslateCommonData;
    if (mustTranslateCommonData && lps_.empty())
    {
        mustTranslateCommonData = false;
    }
    return translateCommonData;
}

void SimulationObserver::notifyHebdoProblem(const PROBLEME_HEBDO& problemeHebdo,
                                            int optimizationNumber,
                                            std::string_view name)
{
    if (optimizationNumber != 1)
    {
        return; // We only care about first optimization
    }
    Solver::HebdoProblemToLpsTranslator translator;
    const unsigned int year = problemeHebdo.year + 1;
    const unsigned int week = problemeHebdo.weekInTheYear + 1;
    // common_data and weekly_data computed before the mutex lock to prevent blocking the thread
    bool translateCommonData = shouldTranslateCommonData();
    auto [common_data, weekly_data] = translate(problemeHebdo,
                                                name,
                                                translator,
                                                translateCommonData);
    std::lock_guard lock(lps_mutex_);
    if (translateCommonData)
    {
        lps_.setConstantData(common_data);
    }
    lps_.addWeeklyData({year, week}, weekly_data);
}

Solver::LpsFromAntares&& SimulationObserver::acquireLps() noexcept
{
    std::lock_guard lock(lps_mutex_);
    return std::move(lps_);
}
} // namespace Antares::API
