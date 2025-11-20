#pragma once

#include <memory>
#include <optional>
#include <set>
#include <vector>

#include "storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{

struct Exchange
{
    bool isPossible() const;

    std::optional<unsigned> hourOfMinGen;
    std::optional<unsigned> hourOfMaxGen;
    double amount = 0;
};

std::set<unsigned> ValidHours(const std::vector<double>& Spillage,
                              const std::vector<double>& DTG_MRG);

void updateValidHours(std::set<unsigned>& validHours,
                      const IStorageForRemix& storage,
                      const std::vector<double>& UnsupEinit);

double computeExchange(const unsigned hourOfMinGen,
                       const unsigned hourOfMaxGen,
                       const double maxVariationGen,
                       const IStorageForRemix& storage);

Exchange searchForExhange(const std::set<unsigned>& validHours,
                          const std::vector<double>& TotalGen,
                          const std::vector<double>& UnsupE,
                          const IStorageForRemix& storage);

void update(const Exchange& exchange,
            IStorageForRemix& storage,
            std::vector<double>& UnsupE,
            std::vector<double>& TotalGen);

} // namespace Antares::Solver::Simulation
