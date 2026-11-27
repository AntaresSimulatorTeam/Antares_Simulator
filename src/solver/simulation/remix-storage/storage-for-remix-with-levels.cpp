// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/remix-storage/storage-for-remix-with-levels.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

#include <antares/logs/logs.h>
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/utils/vector-utils.h"

using namespace Antares::Utils;

namespace Antares::Solver::Simulation
{

StorageForRemixWithLevels::StorageForRemixWithLevels(std::vector<double>& withdrawal,
                                                     std::vector<double>& unsupE,
                                                     std::vector<double>& levels,
                                                     const std::vector<double> withdrawalMax,
                                                     const std::vector<double> Pmin,
                                                     const std::vector<double> inflows,
                                                     const std::vector<double> overflow,
                                                     std::vector<double>& injection,
                                                     const std::vector<double> injectionMax,
                                                     const std::vector<double> lowRuleCurve,
                                                     const std::vector<double> upRuleCurve,
                                                     const double initLevel,
                                                     const double withdrawalEff,
                                                     const double injectionEff,
                                                     const std::string& name,
                                                     std::vector<CumulationConstraintForRemix>
                                                       cumulationConstraints):
    StorageForRemixNoLevels(withdrawal, unsupE, withdrawalMax, Pmin, name),
    levels_(levels),
    inflows_(inflows),
    overflow_(overflow),
    injection_(injection),
    initInjection_(injection),
    injectionMax_(injectionMax),
    ruleCurveLow_(lowRuleCurve),
    ruleCurveUp_(upRuleCurve),
    initLevel_(initLevel),
    withdrawalEff_(withdrawalEff),
    injectionEff_(injectionEff),
    cumulationConstraints_(std::move(cumulationConstraints))
{
    checkInput(unsupE_.size());
    update();
    checkLevels();
    checkCumulationConstraints();
}

// Both levers (withdrawal, injection) lower the level between hourOfMinGen and hourOfMaxGen
// if hourOfMinGen comes first, raise it otherwise. Levels after both hours are unchanged.
double StorageForRemixWithLevels::levelMargin(unsigned hourOfMaxGen, unsigned hourOfMinGen) const
{
    unsigned hour = std::min(hourOfMinGen, hourOfMaxGen);
    unsigned HOUR = std::max(hourOfMinGen, hourOfMaxGen);

    if (hourOfMinGen < hourOfMaxGen)
    {
        return min_on_subrange(levels_ - ruleCurveLow_, hour, HOUR);
    }
    return min_on_subrange(ruleCurveUp_ - levels_, hour, HOUR);
}

static double levelBoundToPower(double levelMargin, double efficiency)
{
    return efficiency > 0. ? levelMargin / efficiency : std::numeric_limits<double>::max();
}

double StorageForRemixWithLevels::maxWithdrawalExchangeWithLevels(unsigned hourOfMaxGen,
                                                                  unsigned hourOfMinGen) const
{
    return std::min({maxWithdrawalExchange(hourOfMaxGen, hourOfMinGen),
                     levelBoundToPower(levelMargin(hourOfMaxGen, hourOfMinGen), withdrawalEff_),
                     cumulationBound(RemixLever::Withdrawal, hourOfMaxGen, hourOfMinGen)});
}

double StorageForRemixWithLevels::cumulationBound(RemixLever lever,
                                                  unsigned hourOfMaxGen,
                                                  unsigned hourOfMinGen) const
{
    double bound = std::numeric_limits<double>::max();
    for (const auto& constraint: cumulationConstraints_)
    {
        bound = std::min(bound,
                         maxExchangeUnderCumulation(constraint,
                                                    lever,
                                                    hourOfMaxGen,
                                                    hourOfMinGen,
                                                    withdrawal_,
                                                    injection_,
                                                    withdrawalEff_,
                                                    injectionEff_));
    }
    return bound;
}

double StorageForRemixWithLevels::maxInjectionExchange(unsigned hourOfMaxGen,
                                                       unsigned hourOfMinGen) const
{
    // Less injection at hourOfMinGen, more injection at hourOfMaxGen
    double boundPower = std::min({injection_[hourOfMinGen],
                                  injectionMax_[hourOfMaxGen] - injection_[hourOfMaxGen],
                                  unsupE_[hourOfMinGen]});
    return std::min({boundPower,
                     levelBoundToPower(levelMargin(hourOfMaxGen, hourOfMinGen), injectionEff_),
                     cumulationBound(RemixLever::Injection, hourOfMaxGen, hourOfMinGen)});
}

double StorageForRemixWithLevels::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const
{
    return std::max(maxWithdrawalExchangeWithLevels(hourOfMaxGen, hourOfMinGen),
                    maxInjectionExchange(hourOfMaxGen, hourOfMinGen));
}

void StorageForRemixWithLevels::applyExchange(unsigned hourOfMaxGen,
                                              unsigned hourOfMinGen,
                                              double amount)
{
    // Only one lever per exchange so that total withdrawal and total injection are kept
    if (maxWithdrawalExchangeWithLevels(hourOfMaxGen, hourOfMinGen) >= amount - TOLERANCE)
    {
        exchangeWithdrawal(hourOfMaxGen, hourOfMinGen, amount);
    }
    else
    {
        injection_[hourOfMinGen] -= amount;
        injection_[hourOfMaxGen] += amount;
    }
    update();
}

const std::vector<double>& StorageForRemixWithLevels::initInjection() const
{
    return initInjection_;
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
    if (!(levels_ <= ruleCurveUp_ + TOLERANCE) || !(ruleCurveLow_ - TOLERANCE <= levels_))
    {
        for (size_t h = 0; h < levels_.size(); ++h)
        {
            if (levels_[h] > ruleCurveUp_[h] + TOLERANCE
                || levels_[h] < ruleCurveLow_[h] - TOLERANCE)
            {
                logs.error().appendFormat(
                  "%s - hour %zu : level = %.17g, low rule curve = %.17g, up rule curve = %.17g, "
                  "tolerance = %.17g",
                  name().c_str(),
                  h,
                  levels_[h],
                  ruleCurveLow_[h],
                  ruleCurveUp_[h],
                  TOLERANCE);
            }
        }

        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

void StorageForRemixWithLevels::checkCumulationConstraints() const
{
    for (const auto& constraint: cumulationConstraints_)
    {
        if (std::ranges::any_of(constraint.hours, [this](unsigned h) { return h >= levels_.size(); }))
        {
            throw std::invalid_argument(error_msg_start_hydro_remix
                                        + "additional constraint hour out of bounds");
        }

        // Not an error : the remix bound is then clamped to 0 in the violating direction
        const double sum = cumulationSum(constraint,
                                         withdrawal_,
                                         injection_,
                                         withdrawalEff_,
                                         injectionEff_);
        const bool violated = (constraint.sense != CumulationSense::Greater
                               && sum > constraint.rhs + TOLERANCE)
                              || (constraint.sense != CumulationSense::Less
                                  && sum < constraint.rhs - TOLERANCE);
        if (violated)
        {
            logs.warning().appendFormat(
              "%s - additional constraint not satisfied before remix : sum = %.17g, rhs = %.17g",
              name().c_str(),
              sum,
              constraint.rhs);
        }
    }
}

void StorageForRemixWithLevels::checkInput(size_t size)
{
    StorageForRemixNoLevels::checkInput(size);

    std::vector<size_t> sizes = {size,
                                 inflows_.size(),
                                 overflow_.size(),
                                 injection_.size(),
                                 injectionMax_.size(),
                                 levels_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "arrays of different sizes");
    }

    if (!size)
    {
        throw std::invalid_argument(error_msg_start_hydro_remix + "all arrays of sizes 0");
    }

    for (size_t h = 0; h < injection_.size(); ++h)
    {
        if (injection_[h] > injectionMax_[h] + TOLERANCE || injection_[h] < -TOLERANCE)
        {
            logs.error().appendFormat(
              "%s - hour %zu : injection = %.17g, injection max = %.17g, tolerance = %.17g",
              name().c_str(),
              h,
              injection_[h],
              injectionMax_[h],
              TOLERANCE);
            throw std::invalid_argument(error_msg_start_hydro_remix
                                        + "Storage injection not within [0, injection max]");
        }
    }

    if (ruleCurveUp_[0] + TOLERANCE <= initLevel_)
    {
        logs.error().appendFormat(
          "%s - initial level = %.17g, up rule curve at hour 0 = %.17g, tolerance = %.17g",
          name().c_str(),
          initLevel_,
          ruleCurveUp_[0],
          TOLERANCE);

        throw std::invalid_argument(error_msg_start_hydro_remix
                                    + "initial level > reservoir capacity");
    }
}

} // namespace Antares::Solver::Simulation
