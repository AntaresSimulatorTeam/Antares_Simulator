// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-help.h"

#include <algorithm> // For std::ranges::min_element(...)
#include <ranges>

namespace vws = std::views;

constexpr double eps = 1e-3;

namespace Antares::Solver::Simulation
{

bool Exchange::isPossible() const
{
    return amount > eps && hourOfMinGen.has_value() && hourOfMaxGen.has_value();
}

std::set<unsigned> ValidHours(const std::vector<double>& Spillage,
                              const std::vector<double>& DTG_MRG)
{
    auto isValidHour = [&](int h) { return std::abs(Spillage[h] + DTG_MRG[h]) < eps; };

    auto allHours = vws::iota(0, (int)Spillage.size());
    auto validHours = allHours | vws::filter(isValidHour);

    return {validHours.begin(), validHours.end()};
}

void updateValidHours(std::set<unsigned>& validHours,
                      const IStorageForRemix& storage,
                      const std::vector<double>& UnsupEinit)
{
    std::erase_if(validHours,
                  [&](int h) { return storage.initWithdrawal()[h] + UnsupEinit[h] <= eps; });
}

double computeExchange(const unsigned hourOfMinGen,
                       const unsigned hourOfMaxGen,
                       const double maxVariationGen,
                       const IStorageForRemix& storage)
{
    double maxExchangeFromStorage = storage.maxExchange(hourOfMaxGen, hourOfMinGen);
    return std::max(std::min(maxExchangeFromStorage, maxVariationGen / 2.), 0.);
}

Exchange searchForExhange(const std::set<unsigned>& validHours,
                          const std::vector<double>& TotalGen,
                          const std::vector<double>& UnsupE,
                          const IStorageForRemix& storage)
{
    auto totalGenProjection = [&](int h) { return TotalGen[h]; };

    std::set<unsigned> validHoursForMin(validHours);
    std::erase_if(validHoursForMin, [&](int h) { return UnsupE[h] <= eps; });

    while (!validHoursForMin.empty())
    {
        auto hourOfMinGen = std::ranges::min_element(validHoursForMin, {}, totalGenProjection);

        std::set<unsigned> validHoursForMax(validHours);
        double totalGenMin = TotalGen[*hourOfMinGen];
        std::erase_if(validHoursForMax, [&](int h) { return TotalGen[h] < totalGenMin + eps; });

        while (!validHoursForMax.empty())
        {
            auto hourOfMaxGen = std::ranges::max_element(validHoursForMax, {}, totalGenProjection);

            double maxVariationGen = TotalGen[*hourOfMaxGen] - totalGenMin;
            auto exchange = computeExchange(*hourOfMinGen, *hourOfMaxGen, maxVariationGen, storage);

            if (exchange > eps)
            {
                return {*hourOfMinGen, *hourOfMaxGen, exchange};
            }
            validHoursForMax.erase(hourOfMaxGen);
        }
        validHoursForMin.erase(hourOfMinGen);
    }
    return {};
}

void update(const Exchange& exchange,
            IStorageForRemix& storage,
            std::vector<double>& UnsupE,
            std::vector<double>& TotalGen)
{
    double amount = exchange.amount;
    unsigned hourOfMinGen = exchange.hourOfMinGen.value();
    unsigned hourOfMaxGen = exchange.hourOfMaxGen.value();

    storage.withdrawal()[hourOfMaxGen] -= amount;
    storage.withdrawal()[hourOfMinGen] += amount;
    storage.update();

    UnsupE[hourOfMaxGen] += amount;
    UnsupE[hourOfMinGen] -= amount;

    TotalGen[hourOfMaxGen] -= amount;
    TotalGen[hourOfMinGen] += amount;
}

} // namespace Antares::Solver::Simulation
