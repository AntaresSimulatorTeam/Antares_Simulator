// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/remix-storage/storage-for-remix-no-levels.h"

#include <stdexcept>

#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/utils/vector-utils.h"

using namespace Antares::Utils;

namespace Antares::Solver::Simulation
{

StorageForRemixNoLevels::StorageForRemixNoLevels(std::vector<double>& withdrawal,
                                                 std::vector<double>& unsupE,
                                                 const std::vector<double> Pmax,
                                                 const std::vector<double> Pmin,
                                                 const std::string& name):
    IStorageForRemix(name),
    withdrawal_(withdrawal),
    initWithdrawal_(withdrawal),
    unsupE_(unsupE),
    pmax_(Pmax),
    pmin_(Pmin)
{
    checkInput(unsupE_.size());
}

double StorageForRemixNoLevels::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const
{
    // Max amont we can take from hydro withdrawal, at hour when the total
    // production reaches a max.
    double boundAtMax = withdrawal_[hourOfMaxGen] - pmin_[hourOfMaxGen];
    // Max amont we can add to hydro withdrawal, at hour when the total
    // production reaches a min.
    double boundAtMin = std::min(
      {pmax_[hourOfMinGen] - withdrawal_[hourOfMinGen], unsupE_[hourOfMinGen]});

    return std::min(boundAtMax, boundAtMin);
}

void StorageForRemixNoLevels::checkInput(size_t size)
{
    std::vector<size_t> sizes = {size, withdrawal_.size(), pmin_.size(), pmax_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "arrays of different sizes");
    }

    if (!(withdrawal_ <= pmax_ + TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "Storage withdrawal not smaller than Pmax everywhere");
    }

    if (!(pmin_ - TOLERANCE <= withdrawal_))
    {
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

std::vector<double>& StorageForRemixNoLevels::withdrawal()
{
    return withdrawal_;
}

} // namespace Antares::Solver::Simulation
