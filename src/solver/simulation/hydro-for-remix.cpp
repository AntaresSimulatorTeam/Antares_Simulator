#include "include/antares/solver/simulation/hydro-for-remix.h"

#include <limits>
#include <span>
#include <stdexcept>

#include "include/antares/solver/simulation/remix-utils.h"

constexpr double LEVEL_TOLERANCE = 1.e-6;
const std::string error_msg_start = "Remix hydro input : ";

namespace Antares::Solver::Simulation
{
HydroForRemix::HydroForRemix(std::vector<double>& generation,
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
                             const bool& reservoirManagement):
    generation_(generation),
    unsupE_(unsupE),
    levels_(levels),
    pmax_(Pmax),
    pmin_(Pmin),
    inflows_(inflows),
    overflow_(overflow),
    pump_(pump),
    initLevel_(initLevel),
    capacity_(capacity),
    pumpEff_(pumpEfficiency),
    reservoirManagement_(reservoirManagement)
{
}

double HydroForRemix::maxExchange(unsigned hourMax, unsigned hourMin)
{
    // Max slice we can take from hydro generation, at an hour when the total
    // production reaches a max.
    double boundAtMax = std::numeric_limits<double>::max();
    // Max slice we can add to hydro generation, at an hour when the total
    // production reaches a min.
    double boundAtMin = std::numeric_limits<double>::max();

    if (reservoirManagement_)
    {
        unsigned smallestHour = std::min(hourMin, hourMax);
        unsigned greatestHour = std::max(hourMin, hourMax);
        std::span<double> level_subset(levels_.begin() + smallestHour,
                                       levels_.begin() + greatestHour);

        if (hourMin < hourMax)
        {
            boundAtMax = capacity_;
            boundAtMin = *std::ranges::min_element(level_subset);
        }
        else
        {
            boundAtMax = capacity_ - *std::ranges::max_element(level_subset);
            boundAtMin = capacity_;
        }
    }

    boundAtMax = std::min(generation_[hourMax] - pmin_[hourMax], boundAtMax);
    boundAtMin = std::min({pmax_[hourMin] - generation_[hourMin], unsupE_[hourMin], boundAtMin});
    return std::min(boundAtMax, boundAtMin);
}

void HydroForRemix::checkInput(size_t size)
{
    std::vector<size_t> sizes = {generation_.size(), pmin_.size(), pmax_.size()};

    if (reservoirManagement_)
    {
        if (initLevel_ >= capacity_ + LEVEL_TOLERANCE)
        {
            throw std::invalid_argument(error_msg_start + "initial level > reservoir capacity");
        }
        std::vector<size_t> toAppend = {inflows_.size(),
                                        overflow_.size(),
                                        pump_.size(),
                                        levels_.size()};
        std::ranges::copy(sizes, std::back_inserter(toAppend));
    }

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!(generation_ <= pmax_))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not smaller than Pmax everywhere");
    }

    if (!(pmin_ <= generation_))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not greater than Pmin everywhere");
    }
}

void HydroForRemix::update()
{
    if (!reservoirManagement_)
    {
        return;
    }

    levels_[0] = initLevel_ + inflows_[0] - overflow_[0] + pumpEff_ * pump_[0] - generation_[0];
    for (size_t h = 1; h < levels_.size(); ++h)
    {
        levels_[h] = levels_[h - 1] + inflows_[h] - overflow_[h] + pumpEff_ * pump_[h]
                     - generation_[h];
    }

    if (!(levels_ <= capacity_ + LEVEL_TOLERANCE) || !(levels_ >= -LEVEL_TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

std::vector<double>& HydroForRemix::generation()
{
    return generation_;
}
} // namespace Antares::Solver::Simulation
