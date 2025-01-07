#include "include/antares/solver/simulation/hydro-remix-new.h"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace Antares::Solver::Simulation
{

int find_min_index(const std::vector<double>& TotalGen,
                   const std::vector<double>& OutUnsupE,
                   const std::vector<double>& OutHydroGen,
                   const std::vector<int>& tried_creux,
                   const std::vector<double>& HydroPmax,
                   const std::vector<bool>& filter_hours_remix,
                   double top)
{
    double min_val = top;
    int min_idx = -1;
    for (int i = 0; i < TotalGen.size(); ++i)
    {
        if (OutUnsupE[i] > 0 && OutHydroGen[i] < HydroPmax[i] && tried_creux[i] == 0
            && filter_hours_remix[i])
        {
            if (TotalGen[i] < min_val)
            {
                min_val = TotalGen[i];
                min_idx = i;
            }
        }
    }
    return min_idx;
}

int find_max_index(const std::vector<double>& TotalGen,
                   const std::vector<double>& OutHydroGen,
                   const std::vector<int>& tried_pic,
                   const std::vector<double>& HydroPmin,
                   const std::vector<bool>& filter_hours_remix,
                   double ref_value,
                   double eps)
{
    double max_val = 0;
    int max_idx = -1;
    for (int i = 0; i < TotalGen.size(); ++i)
    {
        if (OutHydroGen[i] > HydroPmin[i] && TotalGen[i] >= ref_value + eps && tried_pic[i] == 0
            && filter_hours_remix[i])
        {
            if (TotalGen[i] > max_val)
            {
                max_val = TotalGen[i];
                max_idx = i;
            }
        }
    }
    return max_idx;
}

static bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    std::vector<double> a_minus_b;
    std::ranges::transform(a, b, std::back_inserter(a_minus_b), std::minus<double>());
    return std::ranges::all_of(a_minus_b, [](const double& e) { return e <= 0.; });
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

RemixHydroOutput new_remix_hydro(const std::vector<double>& DispatchGen,
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
    if (levels.size())
    {
        levels[0] = initial_level + inflows[0] - overflow[0] + pump[0] - HydroGen[0];
        for (size_t i = 1; i < levels.size(); ++i)
        {
            levels[i] = levels[i - 1] + inflows[i] - overflow[i] + pump[i] - HydroGen[i];
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

    std::vector<bool> filter_hours_remix(DispatchGen.size(), false);
    for (unsigned int h = 0; h < filter_hours_remix.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && HydroGen[h] + UnsupE[h] > 0.)
        {
            filter_hours_remix[h] = true;
        }
    }

    std::vector<double> TotalGen(DispatchGen.size());
    std::transform(DispatchGen.begin(),
                   DispatchGen.end(),
                   OutHydroGen.begin(),
                   TotalGen.begin(),
                   std::plus<>());

    while (loop-- > 0)
    {
        std::vector<int> tried_creux(DispatchGen.size(), 0);
        double delta = 0;

        while (true)
        {
            int idx_creux = find_min_index(TotalGen,
                                           OutUnsupE,
                                           OutHydroGen,
                                           tried_creux,
                                           HydroPmax,
                                           filter_hours_remix,
                                           top);
            if (idx_creux == -1)
            {
                break;
            }

            std::vector<int> tried_pic(DispatchGen.size(), 0);
            while (true)
            {
                int idx_pic = find_max_index(TotalGen,
                                             OutHydroGen,
                                             tried_pic,
                                             HydroPmin,
                                             filter_hours_remix,
                                             TotalGen[idx_creux],
                                             eps);
                if (idx_pic == -1)
                {
                    break;
                }

                std::vector<double> intermediate_level(levels.begin()
                                                         + std::min(idx_creux, idx_pic),
                                                       levels.begin()
                                                         + std::max(idx_creux, idx_pic));
                double max_pic, max_creux;
                if (idx_creux < idx_pic)
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

                max_pic = std::min(OutHydroGen[idx_pic] - HydroPmin[idx_pic], max_pic);
                max_creux = std::min(
                  {HydroPmax[idx_creux] - OutHydroGen[idx_creux], OutUnsupE[idx_creux], max_creux});

                double dif_pic_creux = std::max(TotalGen[idx_pic] - TotalGen[idx_creux], 0.);

                delta = std::max(std::min({max_pic, max_creux, dif_pic_creux / 2.}), 0.);

                if (delta > 0)
                {
                    OutHydroGen[idx_pic] -= delta;
                    OutHydroGen[idx_creux] += delta;
                    OutUnsupE[idx_pic] = HydroGen[idx_pic] + UnsupE[idx_pic] - OutHydroGen[idx_pic];
                    OutUnsupE[idx_creux] = HydroGen[idx_creux] + UnsupE[idx_creux]
                                           - OutHydroGen[idx_creux];
                    break;
                }
                else
                {
                    tried_pic[idx_pic] = 1;
                }
            }

            if (delta > 0)
            {
                break;
            }
            tried_creux[idx_creux] = 1;
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
        for (size_t i = 1; i < levels.size(); ++i)
        {
            levels[i] = levels[i - 1] + inflows[i] - overflow[i] + pump[i] - OutHydroGen[i];
        }
    }
    return {OutHydroGen, OutUnsupE, levels};
}

} // End namespace Antares::Solver::Simulation
