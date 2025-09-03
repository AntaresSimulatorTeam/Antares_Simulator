#include "antares/solver/simulation/remix-storage/storage-for-remix.h"

#include <limits>
#include <span>
#include <stdexcept>

#include "antares/solver/simulation/remix-storage/remix-utils.h"

constexpr double TOLERANCE = 1.e-6;
const std::string error_msg_start = "Remix hydro input : ";

namespace Antares::Solver::Simulation
{

std::shared_ptr<IStorageForRemix> makeHydroForRemix(std::vector<double>& generation,
                                                    std::vector<double>& unsupE,
                                                    std::vector<double>& levels,
                                                    const std::vector<double>& Pmax,
                                                    const std::vector<double>& Pmin,
                                                    const std::vector<double>& inflows,
                                                    const std::vector<double>& overflow,
                                                    const std::vector<double>& pump,
                                                    const double initLevel,
                                                    const double reservoirCapacity,
                                                    const double pumpEfficiency,
                                                    bool reservoirManagement)
{
    if (!reservoirManagement)
    {
        return std::make_shared<StorageForRemixNoLevels>(generation, unsupE, Pmax, Pmin);
    }

    size_t size = generation.size();
    const std::vector<double> lowRuleCurve(size, 0.);
    const std::vector<double> upRuleCurve(size, reservoirCapacity);
    const double withdrawalEff = 1.;
    return std::make_shared<StorageForRemixWithLevels>(generation,
                                                       unsupE,
                                                       levels,
                                                       Pmax,
                                                       Pmin,
                                                       inflows,
                                                       overflow,
                                                       pump,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       withdrawalEff);
}

std::shared_ptr<StorageForRemixWithLevels> makeSTSforRemix(std::vector<double>& withdrawal,
                                                           std::vector<double>& unsupE,
                                                           std::vector<double>& levels,
                                                           const std::vector<double>& pmax,
                                                           const std::vector<double>& inflows,
                                                           const std::vector<double>& injection,
                                                           const std::vector<double>& lowRuleCurve,
                                                           const std::vector<double>& upRuleCurve,
                                                           const double initLevel,
                                                           const double withdrawalEff,
                                                           const double injectionEff)
{
    size_t size = withdrawal.size();

    std::vector<double> overflows(size, 0.);
    std::vector<double> pmin(size, 0.);

    return std::make_shared<StorageForRemixWithLevels>(withdrawal,
                                                       unsupE,
                                                       levels,
                                                       pmax,
                                                       pmin,
                                                       inflows,
                                                       overflows,
                                                       injection,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       withdrawalEff,
                                                       injectionEff);
}

StorageForRemixNoLevels::StorageForRemixNoLevels(std::vector<double>& withdrawal,
                                                 std::vector<double>& unsupE,
                                                 const std::vector<double> Pmax,
                                                 const std::vector<double> Pmin):
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
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!(withdrawal_ <= pmax_ + TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Storage withdrawal not smaller than Pmax everywhere");
    }

    if (!(pmin_ - TOLERANCE <= withdrawal_))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Storage withdrawal not greater than Pmin everywhere");
    }
}

void StorageForRemixNoLevels::update()
{
}

const std::vector<double>& StorageForRemixNoLevels::initWithdrawal()
{
    return initWithdrawal_;
}

std::vector<double>& StorageForRemixNoLevels::withdrawal()
{
    return withdrawal_;
}

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
    initLevel_(initLevel),
    withdrawalEff_(withdrawalEff),
    injectionEff_(injectionEff),
    ruleCurveLow_(lowRuleCurve),
    ruleCurveUp_(upRuleCurve)
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
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!size)
    {
        throw std::invalid_argument(error_msg_start + "all arrays of sizes 0");
    }

    if (ruleCurveUp_ + TOLERANCE <= initLevel_)
    {
        throw std::invalid_argument(error_msg_start + "initial level > reservoir capacity");
    }
}

void StorageForRemixWithLevels::checkLevels()
{
    if (!(levels_ <= ruleCurveUp_ + TOLERANCE) || !(levels_ >= -TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "levels computed from input don't respect reservoir bounds");
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

} // namespace Antares::Solver::Simulation
