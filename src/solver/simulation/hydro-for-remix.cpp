#include "include/antares/solver/simulation/hydro-for-remix.h"

#include <span>
#include <stdexcept>

#include "include/antares/solver/simulation/remix-utils.h"

constexpr double TOLERANCE = 1.e-6;
const std::string error_msg_start = "Remix hydro input : ";

namespace Antares::Solver::Simulation
{

std::shared_ptr<StorageForRemix> makeHydroForRemix(std::vector<double>& generation,
                                                   std::vector<double>& unsupE,
                                                   std::vector<double>& levels,
                                                   const std::vector<double>& Pmax,
                                                   const std::vector<double>& Pmin,
                                                   const std::vector<double>& inflows,
                                                   const std::vector<double>& overflow,
                                                   const std::vector<double>& pump,
                                                   const double& initLevel,
                                                   const double& capacity,
                                                   const double& pumpEfficiency,
                                                   bool reservoirManagement)
{
    if (!reservoirManagement)
    {
        return std::make_shared<HydroForRemix>(generation, unsupE, Pmax, Pmin);
    }
    return std::make_shared<HydroForRemixWithLevels>(generation,
                                                     unsupE,
                                                     levels,
                                                     Pmax,
                                                     Pmin,
                                                     inflows,
                                                     overflow,
                                                     pump,
                                                     initLevel,
                                                     capacity,
                                                     pumpEfficiency);
}

HydroForRemix::HydroForRemix(std::vector<double>& generation,
                             std::vector<double>& unsupE,
                             const std::vector<double>& Pmax,
                             const std::vector<double>& Pmin):
    generation_(generation),
    unsupE_(unsupE),
    pmax_(Pmax),
    pmin_(Pmin)
{
    checkInput(generation_.size());
}

double HydroForRemix::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen)
{
    // Max slice we can take from hydro generation, at an hour when the total
    // production reaches a max.
    double boundAtMax = generation_[hourOfMaxGen] - pmin_[hourOfMaxGen];
    // Max slice we can add to hydro generation, at an hour when the total
    // production reaches a min.
    double boundAtMin = std::min(
      {pmax_[hourOfMinGen] - generation_[hourOfMinGen], unsupE_[hourOfMinGen]});

    return std::min(boundAtMax, boundAtMin);
}

void HydroForRemix::checkInput(size_t size)
{
    std::vector<size_t> sizes = {generation_.size(), pmin_.size(), pmax_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!(generation_ <= pmax_ + TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not smaller than Pmax everywhere");
    }

    if (!(pmin_ - TOLERANCE <= generation_))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not greater than Pmin everywhere");
    }
}

void HydroForRemix::update()
{
}

std::vector<double>& HydroForRemix::generation()
{
    return generation_;
}

HydroForRemixWithLevels::HydroForRemixWithLevels(std::vector<double>& generation,
                                                 std::vector<double>& unsupE,
                                                 std::vector<double>& levels,
                                                 const std::vector<double>& Pmax,
                                                 const std::vector<double>& Pmin,
                                                 const std::vector<double>& inflows,
                                                 const std::vector<double>& overflow,
                                                 const std::vector<double>& pump,
                                                 const double& initLevel,
                                                 const double& capacity,
                                                 const double& pumpEfficiency):
    HydroForRemix(generation, unsupE, Pmax, Pmin),
    levels_(levels),
    inflows_(inflows),
    overflow_(overflow),
    pump_(pump),
    initLevel_(initLevel),
    capacity_(capacity),
    pumpEff_(pumpEfficiency)
{
    checkInput(generation_.size());
    update();
    checkLevels();
}

double HydroForRemixWithLevels::maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen)
{
    double bound = HydroForRemix::maxExchange(hourOfMaxGen, hourOfMinGen);

    unsigned smallestHour = std::min(hourOfMinGen, hourOfMaxGen);
    unsigned greatestHour = std::max(hourOfMinGen, hourOfMaxGen);
    std::span<double> level_subset(levels_.begin() + smallestHour, levels_.begin() + greatestHour);

    double boundAtMax = std::numeric_limits<double>::max();
    double boundAtMin = std::numeric_limits<double>::max();
    if (hourOfMinGen < hourOfMaxGen)
    {
        boundAtMax = capacity_;
        boundAtMin = *std::ranges::min_element(level_subset);
    }
    else
    {
        boundAtMax = capacity_ - *std::ranges::max_element(level_subset);
        boundAtMin = capacity_;
    }

    return std::min({bound, boundAtMax, boundAtMin});
}

void HydroForRemixWithLevels::checkInput(size_t size)
{
    HydroForRemix::checkInput(size);

    std::vector<size_t> sizes = {size,
                                 inflows_.size(),
                                 overflow_.size(),
                                 pump_.size(),
                                 levels_.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!size)
    {
        throw std::invalid_argument(error_msg_start + "all arrays of sizes 0");
    }

    if (initLevel_ >= capacity_ + TOLERANCE)
    {
        throw std::invalid_argument(error_msg_start + "initial level > reservoir capacity");
    }
}

void HydroForRemixWithLevels::checkLevels()
{
    if (!(levels_ <= capacity_ + TOLERANCE) || !(levels_ >= -TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

void HydroForRemixWithLevels::update()
{
    levels_[0] = initLevel_ + inflows_[0] - overflow_[0] + pumpEff_ * pump_[0] - generation_[0];
    for (size_t h = 1; h < levels_.size(); ++h)
    {
        levels_[h] = levels_[h - 1] + inflows_[h] - overflow_[h] + pumpEff_ * pump_[h]
                     - generation_[h];
    }
}

} // namespace Antares::Solver::Simulation
