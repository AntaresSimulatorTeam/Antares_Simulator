
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "SimulationObserver.h"

#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"

namespace Antares::API
{
namespace
{
auto translate(const PROBLEME_HEBDO& problemeHebdo,
               std::string_view name,
               const Solver::HebdoProblemToLpsTranslator& translator,
               std::once_flag& flag)
{
    auto&& weekly_data = translator.translate(problemeHebdo.ProblemeAResoudre.get(), name);
    std::optional<Solver::ConstantDataFromAntares> common_data;
    std::call_once(flag,
                   [&problemeHebdo, &translator, &common_data]() {
                       common_data = translator.commonProblemData(
                         problemeHebdo.ProblemeAResoudre.get());
                   });
    return std::make_pair(std::move(common_data), std::move(weekly_data));
}
} // namespace

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
    auto&& [common_data, weekly_data] = translate(problemeHebdo, name, translator, flag_);
    std::lock_guard lock(lps_mutex_);
    if (common_data)
    {
        lps_.setConstantData(common_data.value());
    }
    lps_.addWeeklyData({year, week}, std::move(weekly_data));
}

Solver::LpsFromAntares&& SimulationObserver::acquireLps() noexcept
{
    std::lock_guard lock(lps_mutex_);
    return std::move(lps_);
}
} // namespace Antares::API
