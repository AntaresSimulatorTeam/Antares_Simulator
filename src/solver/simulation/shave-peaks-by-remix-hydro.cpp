#include "include/antares/solver/simulation/shave-peaks-by-remix-hydro.h"

#include <algorithm>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

constexpr double RESERVOIR_LEVEL_TOLERANCE = 1.e-6;
constexpr double eps = 1e-3;

namespace Antares::Solver::Simulation
{

int hour_for_totalGen_min(const std::vector<double>& TotalGen,
                          const std::vector<double>& OutUnsupE,
                          const std::vector<double>& OutHydroGen,
                          const std::vector<bool>& triedBottom,
                          const std::vector<double>& HydroPmax,
                          const std::vector<bool>& enabledHours,
                          double top)
{
    double minTotalGen = top;
    int min_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutUnsupE[h] > 0 && OutHydroGen[h] < HydroPmax[h] && !triedBottom[h] && enabledHours[h])
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

int hour_for_totalGen_max(const std::vector<double>& TotalGen,
                          const std::vector<double>& OutHydroGen,
                          const std::vector<bool>& triedPeak,
                          const std::vector<double>& HydroPmin,
                          const std::vector<bool>& enabledHours,
                          double minTotalGen)
{
    double maxTotalGen = 0;
    int max_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutHydroGen[h] > HydroPmin[h] && TotalGen[h] >= minTotalGen + eps && !triedPeak[h]
            && enabledHours[h])
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

namespace
{
bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && std::ranges::all_of(std::views::iota(size_t{0}, a.size()),
                                  [&](size_t i) { return a[i] <= b[i]; });
}

bool operator<=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e <= c; });
}

bool operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e >= c; });
}

void checkInputCorrectness(const std::vector<double>& DispatchGen,
                           const std::vector<double>& HydroGen,
                           const std::vector<double>& UnsupE,
                           const std::vector<double>& levels,
                           const std::vector<double>& HydroPmax,
                           const std::vector<double>& HydroPmin,
                           double initialLevel,
                           double reservoirCapacity,
                           bool reservoirManagement,
                           const std::vector<double>& inflows,
                           const std::vector<double>& overflow,
                           const std::vector<double>& pump,
                           const std::vector<double>& Spillage,
                           const std::vector<double>& DTG_MRG)
{
    std::string msg_prefix = "Remix hydro input : ";

    // Initial level smaller than capacity
    if (initialLevel >= reservoirCapacity + RESERVOIR_LEVEL_TOLERANCE)
    {
        throw std::invalid_argument(msg_prefix + "initial level > reservoir capacity");
    }
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {DispatchGen.size(),
                                 HydroGen.size(),
                                 UnsupE.size(),
                                 levels.size(),
                                 HydroPmax.size(),
                                 HydroPmin.size(),
                                 inflows.size(),
                                 overflow.size(),
                                 pump.size(),
                                 Spillage.size(),
                                 DTG_MRG.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(msg_prefix + "arrays of different sizes");
    }

    // Arrays are of size 0
    if (!DispatchGen.size())
    {
        throw std::invalid_argument(msg_prefix + "all arrays of sizes 0");
    }

    // Hydro production < Pmax
    if (!(HydroGen <= HydroPmax))
    {
        throw std::invalid_argument(msg_prefix
                                    + "Hydro generation not smaller than Pmax everywhere");
    }

    // Hydro production > Pmin
    if (!(HydroPmin <= HydroGen))
    {
        throw std::invalid_argument(msg_prefix
                                    + "Hydro generation not greater than Pmin everywhere");
    }

    if (reservoirManagement)
    {
        if (!(levels <= reservoirCapacity + RESERVOIR_LEVEL_TOLERANCE)
            || !(levels >= -RESERVOIR_LEVEL_TOLERANCE))
        {
            throw std::invalid_argument(
              msg_prefix + "levels computed from input don't respect reservoir bounds");
        }
    }
}
} // namespace

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<double>& HydroGen,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           const double initialLevel,
                                           const double reservoirCapacity,
                                           const double pumpingEfficiency,
                                           const bool reservoirManagement,
                                           const std::vector<double>& inflows,
                                           const std::vector<double>& overflow,
                                           const std::vector<double>& pump,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG)
{
    std::vector<double> levels(DispatchGen.size());
    if (!levels.empty() && reservoirManagement)
    {
        levels[0] = initialLevel + inflows[0] - overflow[0] + pumpingEfficiency * pump[0]
                    - HydroGen[0];
        for (size_t h = 1; h < levels.size(); ++h)
        {
            levels[h] = levels[h - 1] + inflows[h] - overflow[h] + pumpingEfficiency * pump[h]
                        - HydroGen[h];
        }
    }

    checkInputCorrectness(DispatchGen,
                          HydroGen,
                          UnsupE,
                          levels,
                          HydroPmax,
                          HydroPmin,
                          initialLevel,
                          reservoirCapacity,
                          reservoirManagement,
                          inflows,
                          overflow,
                          pump,
                          Spillage,
                          DTG_MRG);

    std::vector<double> OutHydroGen = HydroGen;
    std::vector<double> OutUnsupE = UnsupE;

    int loop = 1000;
    double top = *std::max_element(DispatchGen.begin(), DispatchGen.end())
                 + *std::max_element(HydroGen.begin(), HydroGen.end())
                 + *std::max_element(UnsupE.begin(), UnsupE.end()) + 1;

    std::vector<bool> enabledHours(DispatchGen.size(), false);
    for (unsigned int h = 0; h < enabledHours.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && HydroGen[h] + UnsupE[h] > 0.)
        {
            enabledHours[h] = true;
        }
    }

    std::vector<double> TotalGen(DispatchGen.size());
    std::transform(DispatchGen.begin(),
                   DispatchGen.end(),
                   HydroGen.begin(),
                   TotalGen.begin(),
                   std::plus<>());

    while (loop-- > 0)
    {
        std::vector<bool> triedBottom(DispatchGen.size(), false);
        double delta = 0;

        while (true)
        {
            int hourBottom = hour_for_totalGen_min(TotalGen,
                                                   OutUnsupE,
                                                   OutHydroGen,
                                                   triedBottom,
                                                   HydroPmax,
                                                   enabledHours,
                                                   top);
            if (hourBottom == -1)
            {
                break;
            }

            std::vector<bool> triedPeak(DispatchGen.size(), false);
            while (true)
            {
                int hourPeak = hour_for_totalGen_max(TotalGen,
                                                     OutHydroGen,
                                                     triedPeak,
                                                     HydroPmin,
                                                     enabledHours,
                                                     TotalGen[hourBottom]);
                if (hourPeak == -1)
                {
                    break;
                }

                double max_pic = std::numeric_limits<double>::max();
                double max_creux = std::numeric_limits<double>::max();

                if (reservoirManagement)
                {
                    unsigned minHour = std::min(hourBottom, hourPeak);
                    unsigned maxHour = std::max(hourBottom, hourPeak);
                    std::span<double> intermediate_level(levels.begin() + minHour,
                                                         levels.begin() + maxHour);

                    if (hourBottom < hourPeak)
                    {
                        max_pic = reservoirCapacity;
                        max_creux = *std::ranges::min_element(intermediate_level);
                    }
                    else
                    {
                        max_pic = reservoirCapacity - *std::ranges::max_element(intermediate_level);
                        max_creux = reservoirCapacity;
                    }
                }

                max_pic = std::min(OutHydroGen[hourPeak] - HydroPmin[hourPeak], max_pic);
                max_creux = std::min({HydroPmax[hourBottom] - OutHydroGen[hourBottom],
                                      OutUnsupE[hourBottom],
                                      max_creux});

                double dif_pic_creux = std::max(TotalGen[hourPeak] - TotalGen[hourBottom], 0.);

                delta = std::max(std::min({max_pic, max_creux, dif_pic_creux / 2.}), 0.);

                if (delta > eps)
                {
                    OutHydroGen[hourPeak] -= delta;
                    OutHydroGen[hourBottom] += delta;
                    OutUnsupE[hourPeak] = HydroGen[hourPeak] + UnsupE[hourPeak]
                                          - OutHydroGen[hourPeak];
                    OutUnsupE[hourBottom] = HydroGen[hourBottom] + UnsupE[hourBottom]
                                            - OutHydroGen[hourBottom];
                    break;
                }
                triedPeak[hourPeak] = true;
            }

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

        std::transform(DispatchGen.begin(),
                       DispatchGen.end(),
                       OutHydroGen.begin(),
                       TotalGen.begin(),
                       std::plus<>());
        if (reservoirManagement)
        {
            levels[0] = initialLevel + inflows[0] - overflow[0] + pumpingEfficiency * pump[0]
                        - OutHydroGen[0];
            for (size_t h = 1; h < levels.size(); ++h)
            {
                levels[h] = levels[h - 1] + inflows[h] - overflow[h] + pumpingEfficiency * pump[h]
                            - OutHydroGen[h];
            }
        }
    }
    return {OutHydroGen, OutUnsupE, levels};
}

} // End namespace Antares::Solver::Simulation
