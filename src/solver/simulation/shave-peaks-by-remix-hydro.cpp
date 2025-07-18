#include "include/antares/solver/simulation/shave-peaks-by-remix-hydro.h"

#include <algorithm>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

constexpr double LEVEL_TOLERANCE = 1.e-6;
constexpr double eps = 1e-3;
const std::string error_msg_start = "Remix hydro input : ";

namespace Antares::Solver::Simulation
{

static int hour_for_totalGen_min(const std::vector<double>& TotalGen,
                                 const std::vector<double>& OutUnsupE,
                                 const std::vector<bool>& triedBottom,
                                 const std::vector<bool>& validHours,
                                 double top)
{
    double minTotalGen = top;
    int min_hour = -1;
    for (unsigned h = 0; h < TotalGen.size(); ++h)
    {
        if (OutUnsupE[h] > 0 && !triedBottom[h] && validHours[h])
        {
            if (TotalGen[h] < minTotalGen)
            {
                minTotalGen = TotalGen[h];
                min_hour = h;
            }
        }
    }
    return min_hour;
}

static int hour_for_totalGen_max(const std::vector<double>& TotalGen,
                                 const std::vector<bool>& triedPeak,
                                 const std::vector<bool>& validHours,
                                 double minTotalGen)
{
    double maxTotalGen = 0;
    int max_hour = -1;
    for (unsigned h = 0; h < TotalGen.size(); ++h)
    {
        if (TotalGen[h] >= minTotalGen + eps && !triedPeak[h] && validHours[h])
        {
            if (TotalGen[h] > maxTotalGen)
            {
                maxTotalGen = TotalGen[h];
                max_hour = h;
            }
        }
    }
    return max_hour;
}

static bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && std::ranges::all_of(std::views::iota(size_t{0}, a.size()),
                                  [&](size_t i) { return a[i] <= b[i]; });
}

static bool operator<=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e <= c; });
}

static bool operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e >= c; });
}

static void checkInput(const std::vector<double>& DispatchGen,
                       const std::vector<double>& HydroGen,
                       const std::vector<double>& UnsupE,
                       const std::vector<double>& HydroPmax,
                       const std::vector<double>& HydroPmin,
                       const std::vector<double>& Spillage,
                       const std::vector<double>& DTG_MRG)
{
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {DispatchGen.size(),
                                 HydroGen.size(),
                                 UnsupE.size(),
                                 HydroPmax.size(),
                                 HydroPmin.size(),
                                 Spillage.size(),
                                 DTG_MRG.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!DispatchGen.size())
    {
        throw std::invalid_argument(error_msg_start + "all arrays of sizes 0");
    }

    if (!(HydroGen <= HydroPmax))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not smaller than Pmax everywhere");
    }

    if (!(HydroPmin <= HydroGen))
    {
        throw std::invalid_argument(error_msg_start
                                    + "Hydro generation not greater than Pmin everywhere");
    }
}

static void checkReservoirManagementInput(const double initLevel,
                                          const double capacity,
                                          const std::vector<double>& inflows,
                                          const std::vector<double>& overflow,
                                          const std::vector<double>& pump)
{
    if (initLevel >= capacity + LEVEL_TOLERANCE)
    {
        throw std::invalid_argument(error_msg_start + "initial level > reservoir capacity");
    }

    std::vector<size_t> sizes = {inflows.size(), overflow.size(), pump.size()};
    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }
}

static void checkLevels(const std::vector<double>& levels, const double capacity)
{
    if (!(levels <= capacity + LEVEL_TOLERANCE) || !(levels >= -LEVEL_TOLERANCE))
    {
        throw std::invalid_argument(error_msg_start
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

static std::vector<double> updateTotalGen(const std::vector<double>& DispatchGen,
                                          const std::vector<double>& HydroGen)
{
    std::vector<double> totalGen(DispatchGen.size());
    std::transform(DispatchGen.begin(),
                   DispatchGen.end(),
                   HydroGen.begin(),
                   totalGen.begin(),
                   std::plus<>());
    return totalGen;
}

static std::vector<double> updateLevels(const double initLevel,
                                        const double pumpEfficiency,
                                        const std::vector<double>& HydroGen,
                                        const std::vector<double>& inflows,
                                        const std::vector<double>& overflow,
                                        const std::vector<double>& pump)
{
    std::vector<double> levels(HydroGen.size());
    levels[0] = initLevel + inflows[0] - overflow[0] + pumpEfficiency * pump[0] - HydroGen[0];
    for (size_t h = 1; h < levels.size(); ++h)
    {
        levels[h] = levels[h - 1] + inflows[h] - overflow[h] + pumpEfficiency * pump[h]
                    - HydroGen[h];
    }
    return levels;
}

std::vector<bool> ValidHours(const std::vector<double>& Spillage,
                             const std::vector<double>& DTG_MRG,
                             const std::vector<double>& HydroGen,
                             const std::vector<double>& UnsupE)
{
    std::vector<bool> validHours(Spillage.size(), false);
    for (unsigned h = 0; h < validHours.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && HydroGen[h] + UnsupE[h] > 0.)
        {
            validHours[h] = true;
        }
    }
    return validHours;
}

std::vector<double> shavePeaksByRemixingHydro(std::vector<double>& HydroGen,
                                              std::vector<double>& UnsupE,
                                              const std::vector<double>& DispatchGen,
                                              const std::vector<double>& HydroPmax,
                                              const std::vector<double>& HydroPmin,
                                              const double initLevel,
                                              const double capacity,
                                              const double pumpEfficiency,
                                              const bool reservoirManagement,
                                              const std::vector<double>& inflows,
                                              const std::vector<double>& overflow,
                                              const std::vector<double>& pump,
                                              const std::vector<double>& Spillage,
                                              const std::vector<double>& DTG_MRG)
{
    const std::vector<double> HydroGenInit = HydroGen;
    const std::vector<double> UnsupEinit = UnsupE;

    checkInput(DispatchGen, HydroGenInit, UnsupEinit, HydroPmax, HydroPmin, Spillage, DTG_MRG);

    std::vector<double> levels;
    if (reservoirManagement)
    {
        size_t size = DispatchGen.size();
        checkReservoirManagementInput(initLevel, capacity, inflows, overflow, pump);
        levels = updateLevels(initLevel, pumpEfficiency, HydroGenInit, inflows, overflow, pump);
        checkLevels(levels, capacity);
    }

    int loop = 1000;
    double top = *std::max_element(DispatchGen.begin(), DispatchGen.end())
                 + *std::max_element(HydroGenInit.begin(), HydroGenInit.end())
                 + *std::max_element(UnsupEinit.begin(), UnsupEinit.end()) + 1;

    const auto validHours = ValidHours(Spillage, DTG_MRG, HydroGenInit, UnsupEinit);

    std::vector<double> TotalGen = updateTotalGen(DispatchGen, HydroGenInit);

    while (loop-- > 0)
    {
        std::vector<bool> triedBottom(DispatchGen.size(), false);
        double delta = 0;

        while (true)
        {
            int hourBottom = hour_for_totalGen_min(TotalGen, UnsupE, triedBottom, validHours, top);
            if (hourBottom == -1)
            {
                break;
            }

            std::vector<bool> triedPeak(DispatchGen.size(), false);
            while (true)
            {
                int hourPeak = hour_for_totalGen_max(TotalGen,
                                                     triedPeak,
                                                     validHours,
                                                     TotalGen[hourBottom]);
                if (hourPeak == -1)
                {
                    break;
                }
                // max slice we can take from hydro generation, at an hour when the total
                // production reaches a peak.
                double maxSliceOfHydroAtPeak = std::numeric_limits<double>::max();
                // max slice we can add to hydro generation, at an hour when the total
                // production reaches a bottom.
                double maxSliceOfHydroAtBottom = std::numeric_limits<double>::max();

                if (reservoirManagement)
                {
                    unsigned minHour = std::min(hourBottom, hourPeak);
                    unsigned maxHour = std::max(hourBottom, hourPeak);
                    std::span<double> intermediate_level(levels.begin() + minHour,
                                                         levels.begin() + maxHour);

                    if (hourBottom < hourPeak)
                    {
                        maxSliceOfHydroAtPeak = capacity;
                        maxSliceOfHydroAtBottom = *std::ranges::min_element(intermediate_level);
                    }
                    else
                    {
                        maxSliceOfHydroAtPeak = capacity
                                                - *std::ranges::max_element(intermediate_level);
                        maxSliceOfHydroAtBottom = capacity;
                    }
                }

                maxSliceOfHydroAtPeak = std::min(HydroGen[hourPeak] - HydroPmin[hourPeak],
                                                 maxSliceOfHydroAtPeak);
                maxSliceOfHydroAtBottom = std::min({HydroPmax[hourBottom] - HydroGen[hourBottom],
                                                    UnsupE[hourBottom],
                                                    maxSliceOfHydroAtBottom});

                double maxVariation = std::max(TotalGen[hourPeak] - TotalGen[hourBottom], 0.);

                delta = std::max(
                  std::min({maxSliceOfHydroAtPeak, maxSliceOfHydroAtBottom, maxVariation / 2.}),
                  0.);

                if (delta > eps)
                {
                    HydroGen[hourPeak] -= delta;
                    HydroGen[hourBottom] += delta;
                    UnsupE[hourPeak] = HydroGenInit[hourPeak] + UnsupEinit[hourPeak]
                                       - HydroGen[hourPeak];
                    if (reservoirManagement)
                    {
                        levels = updateLevels(initLevel,
                                              pumpEfficiency,
                                              HydroGen,
                                              inflows,
                                              overflow,
                                              pump);
                    }
                    TotalGen = updateTotalGen(DispatchGen, HydroGen);
                    break;
                }
                triedPeak[hourPeak] = true;
            }

            UnsupE[hourBottom] = HydroGenInit[hourBottom] + UnsupEinit[hourBottom]
                                 - HydroGen[hourBottom];
            if (delta > eps)
            {
                break;
            }
            triedBottom[hourBottom] = true;
        }

        if (delta <= eps)
        {
            break;
        }
    }
    return levels;
}

} // End namespace Antares::Solver::Simulation
