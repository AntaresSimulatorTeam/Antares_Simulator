#include "include/antares/solver/simulation/shave-peaks-by-remix-hydro.h"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace Antares::Solver::Simulation
{

int find_min_index(const std::vector<double>& TotalGen,
                   const std::vector<double>& OutUnsupE,
                   const std::vector<double>& OutHydroGen,
                   const std::vector<bool>& triedBottom,
                   const std::vector<double>& HydroPmax,
                   const std::vector<bool>& enabledHours,
                   double top)
{
    double min_val = top;
    int min_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutUnsupE[h] > 0 && OutHydroGen[h] < HydroPmax[h] && !triedBottom[h] && enabledHours[h])
        {
            if (TotalGen[h] < min_val)
            {
                min_val = TotalGen[h];
                min_hour = h;
            }
        }
    }
    return min_hour;
}

int find_max_index(const std::vector<double>& TotalGen,
                   const std::vector<double>& OutHydroGen,
                   const std::vector<bool>& triedPeak,
                   const std::vector<double>& HydroPmin,
                   const std::vector<bool>& enabledHours,
                   double ref_value,
                   double eps)
{
    double max_val = 0;
    int max_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutHydroGen[h] > HydroPmin[h] && TotalGen[h] >= ref_value + eps && !triedPeak[h]
            && enabledHours[h])
        {
            if (TotalGen[h] > max_val)
            {
                max_val = TotalGen[h];
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
    return std::ranges::all_of(v, [&c](const double& e) { return e <= c; });
}

static bool operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](const double& e) { return e >= c; });
}

static void checkInputCorrectness(const std::vector<double>& DispatchGen,
                                  const std::vector<double>& HydroGen,
                                  const std::vector<double>& UnsupE,
                                  const std::vector<double>& levels,
                                  const std::vector<double>& HydroPmax,
                                  const std::vector<double>& HydroPmin,
                                  double initial_level,
                                  double capacity,
                                  const std::vector<double>& inflows,
                                  const std::vector<double>& overflow,
                                  const std::vector<double>& pump,
                                  const std::vector<double>& Spillage,
                                  const std::vector<double>& DTG_MRG)
{
    std::string msg_prefix = "Remix hydro input : ";

    // Initial level smaller than capacity
    if (initial_level > capacity)
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

    if (!(levels <= capacity) || !(levels >= 0.))
    {
        throw std::invalid_argument(msg_prefix
                                    + "levels computed from input don't respect reservoir bounds");
    }
}

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<double>& HydroGen,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           double initial_level,
                                           double capa,
                                           const std::vector<double>& inflows,
                                           const std::vector<double>& overflow,
                                           const std::vector<double>& pump,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG)
{
    std::vector<double> levels(DispatchGen.size());
    if (!levels.empty())
    {
        levels[0] = initial_level + inflows[0] - overflow[0] + pump[0] - HydroGen[0];
        for (size_t h = 1; h < levels.size(); ++h)
        {
            levels[h] = levels[h - 1] + inflows[h] - overflow[h] + pump[h] - HydroGen[h];
        }
    }

    checkInputCorrectness(DispatchGen,
                          HydroGen,
                          UnsupE,
                          levels,
                          HydroPmax,
                          HydroPmin,
                          initial_level,
                          capa,
                          inflows,
                          overflow,
                          pump,
                          Spillage,
                          DTG_MRG);

    std::vector<double> OutHydroGen = HydroGen;
    std::vector<double> OutUnsupE = UnsupE;

    int loop = 1000;
    double eps = 1e-3;
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
            int hourBottom = find_min_index(TotalGen,
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
                int hourPeak = find_max_index(TotalGen,
                                              OutHydroGen,
                                              triedPeak,
                                              HydroPmin,
                                              enabledHours,
                                              TotalGen[hourBottom],
                                              eps);
                if (hourPeak == -1)
                {
                    break;
                }

                std::vector<double> intermediate_level(levels.begin()
                                                         + std::min(hourBottom, hourPeak),
                                                       levels.begin()
                                                         + std::max(hourBottom, hourPeak));
                double max_pic, max_creux;
                if (hourBottom < hourPeak)
                {
                    max_pic = capa;
                    max_creux = *std::min_element(intermediate_level.begin(),
                                                  intermediate_level.end());
                }
                else
                {
                    max_pic = capa
                              - *std::max_element(intermediate_level.begin(),
                                                  intermediate_level.end());
                    max_creux = capa;
                }

                max_pic = std::min(OutHydroGen[hourPeak] - HydroPmin[hourPeak], max_pic);
                max_creux = std::min({HydroPmax[hourBottom] - OutHydroGen[hourBottom],
                                      OutUnsupE[hourBottom],
                                      max_creux});

                double dif_pic_creux = std::max(TotalGen[hourPeak] - TotalGen[hourBottom], 0.);

                delta = std::max(std::min({max_pic, max_creux, dif_pic_creux / 2.}), 0.);

                if (delta > 0)
                {
                    OutHydroGen[hourPeak] -= delta;
                    OutHydroGen[hourBottom] += delta;
                    OutUnsupE[hourPeak] = HydroGen[hourPeak] + UnsupE[hourPeak]
                                          - OutHydroGen[hourPeak];
                    OutUnsupE[hourBottom] = HydroGen[hourBottom] + UnsupE[hourBottom]
                                            - OutHydroGen[hourBottom];
                    break;
                }
                else
                {
                    triedPeak[hourPeak] = true;
                }
            }

            if (delta > 0)
            {
                break;
            }
            triedBottom[hourBottom] = true;
        }

        if (delta == 0)
        {
            break;
        }

        std::transform(DispatchGen.begin(),
                       DispatchGen.end(),
                       OutHydroGen.begin(),
                       TotalGen.begin(),
                       std::plus<>());
        levels[0] = initial_level + inflows[0] - overflow[0] + pump[0] - OutHydroGen[0];
        for (size_t h = 1; h < levels.size(); ++h)
        {
            levels[h] = levels[h - 1] + inflows[h] - overflow[h] + pump[h] - OutHydroGen[h];
        }
    }
    return {OutHydroGen, OutUnsupE, levels};
}

} // End namespace Antares::Solver::Simulation
