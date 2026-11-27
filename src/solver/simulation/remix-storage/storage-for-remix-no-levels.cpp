// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/remix-storage/storage-for-remix-no-levels.h"

#include <stdexcept>

#include <antares/logs/logs.h>
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/utils/vector-utils.h"

using namespace Antares::Utils;

namespace Antares::Solver::Simulation
{

StorageForRemixNoLevels::StorageForRemixNoLevels(std::vector<double>& withdrawal,
                                                 std::vector<double>& unsupE,
                                                 const std::vector<double> withdrawalMax,
                                                 const std::vector<double> Pmin,
                                                 const std::string& name):
    IStorageForRemix(name),
    withdrawal_(withdrawal),
    initWithdrawal_(withdrawal),
    noInjection_(withdrawal.size(), 0.),
    unsupE_(unsupE),
    withdrawalMax_(withdrawalMax),
    pmin_(Pmin),
    error_msg_start_hydro_remix("Remix hydro input for '" + name + "' : ")
{
    checkInput(unsupE_.size());
}

double StorageForRemixNoLevels::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const
{
    return maxWithdrawalExchange(hourOfMaxGen, hourOfMinGen);
}

void StorageForRemixNoLevels::applyExchange(unsigned hourOfMaxGen,
                                            unsigned hourOfMinGen,
                                            double amount)
{
    exchangeWithdrawal(hourOfMaxGen, hourOfMinGen, amount);
    update();
}

void StorageForRemixNoLevels::exchangeWithdrawal(unsigned hourOfMaxGen,
                                                 unsigned hourOfMinGen,
                                                 double amount)
{
    withdrawal_[hourOfMaxGen] -= amount;
    withdrawal_[hourOfMinGen] += amount;
}

double StorageForRemixNoLevels::maxWithdrawalExchange(unsigned hourOfMaxGen,
                                                      unsigned hourOfMinGen) const
{
    // Max amont we can take from hydro withdrawal, at hour when the total
    // production reaches a max.
    double boundAtMax = withdrawal_[hourOfMaxGen] - pmin_[hourOfMaxGen];
    // Max amont we can add to hydro withdrawal, at hour when the total
    // production reaches a min.
    double boundAtMin = std::min(
      {withdrawalMax_[hourOfMinGen] - withdrawal_[hourOfMinGen], unsupE_[hourOfMinGen]});

    return std::min(boundAtMax, boundAtMin);
}

void StorageForRemixNoLevels::checkInput(size_t size)
{
    std::vector<size_t> sizes = {size, withdrawal_.size(), pmin_.size(), withdrawalMax_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "arrays of different sizes");
    }

    if (!(withdrawal_ <= withdrawalMax_ + TOLERANCE))
    {
        for (size_t h = 0; h < withdrawal_.size(); ++h)
        {
            if (withdrawal_[h] > withdrawalMax_[h] + TOLERANCE)
            {
                logs.error().appendFormat(
                  "%s - hour %zu : withdrawal = %.17g, pmax = %.17g, tolerance = %.17g",
                  name().c_str(),
                  h,
                  withdrawal_[h],
                  withdrawalMax_[h],
                  TOLERANCE);
            }
        }
        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "Storage withdrawal not smaller than Pmax everywhere");
    }

    if (!(pmin_ - TOLERANCE <= withdrawal_))
    {
        for (size_t h = 0; h < withdrawal_.size(); ++h)
        {
            if (withdrawal_[h] < pmin_[h] - TOLERANCE)
            {
                logs.error().appendFormat(
                  "%s - hour %zu : withdrawal = %.17g, pmin = %.17g, tolerance = %.17g",
                  name().c_str(),
                  h,
                  withdrawal_[h],
                  pmin_[h],
                  TOLERANCE);
            }
        }
        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "Storage withdrawal not greater than Pmin everywhere");
    }
}

void StorageForRemixNoLevels::update()
{
}

const std::vector<double>& StorageForRemixNoLevels::initWithdrawal() const
{
    return initWithdrawal_;
}

const std::vector<double>& StorageForRemixNoLevels::initInjection() const
{
    return noInjection_;
}

std::vector<double>& StorageForRemixNoLevels::withdrawal()
{
    return withdrawal_;
}

} // namespace Antares::Solver::Simulation
