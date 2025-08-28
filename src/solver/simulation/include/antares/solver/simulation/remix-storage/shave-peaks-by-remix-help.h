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
    bool valid();

    std::optional<unsigned> hourOfMinGen;
    std::optional<unsigned> hourOfMaxGen;
    double amount = 0;
};

std::set<unsigned> ValidHours(const std::vector<double>& Spillage,
                              const std::vector<double>& DTG_MRG);

void updateValidHours(std::set<unsigned>& validHours,
                      std::shared_ptr<IStorageForRemix> storage,
                      const std::vector<double>& UnsupEinit);

double computeExchange(unsigned hourOfMinGen,
                       unsigned hourOfMaxGen,
                       const double maxVariationGen,
                       std::shared_ptr<IStorageForRemix> storage);

Exchange searchForExhange(const std::set<unsigned>& validHours,
                          std::vector<double>& TotalGen,
                          std::vector<double>& UnsupE,
                          std::shared_ptr<IStorageForRemix> storage);

void update(Exchange& exchange,
            std::shared_ptr<IStorageForRemix> storage,
            std::vector<double>& UnsupE,
            std::vector<double>& TotalGen);

} // namespace Antares::Solver::Simulation
