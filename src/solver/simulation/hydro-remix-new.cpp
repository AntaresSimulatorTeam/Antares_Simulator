#include "include/antares/solver/simulation/hydro-remix-new.h"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace Antares::Solver::Simulation
{

int find_min_index(const std::vector<double>& G_plus_H,
                   const std::vector<double>& new_D,
                   const std::vector<double>& new_H,
                   const std::vector<int>& tried_creux,
                   const std::vector<double>& HydroPmax,
                   const std::vector<bool>& filter_hours_remix,
                   double top)
{
    double min_val = top;
    int min_idx = -1;
    for (int i = 0; i < G_plus_H.size(); ++i)
    {
        if (new_D[i] > 0 && new_H[i] < HydroPmax[i] && tried_creux[i] == 0 && filter_hours_remix[i])
        {
            if (G_plus_H[i] < min_val)
            {
                min_val = G_plus_H[i];
                min_idx = i;
            }
        }
    }
    return min_idx;
}

int find_max_index(const std::vector<double>& G_plus_H,
                   const std::vector<double>& new_H,
                   const std::vector<int>& tried_pic,
                   const std::vector<double>& HydroPmin,
                   const std::vector<bool>& filter_hours_remix,
                   double ref_value,
                   double eps)
{
    double max_val = 0;
    int max_idx = -1;
    for (int i = 0; i < G_plus_H.size(); ++i)
    {
        if (new_H[i] > HydroPmin[i] && G_plus_H[i] >= ref_value + eps && tried_pic[i] == 0
            && filter_hours_remix[i])
        {
            if (G_plus_H[i] > max_val)
            {
                max_val = G_plus_H[i];
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

static void checkInputCorrectness(const std::vector<double>& ThermalGen,
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
    std::vector<size_t> sizes = {ThermalGen.size(),
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
    if (!ThermalGen.size())
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

RemixHydroOutput new_remix_hydro(const std::vector<double>& ThermalGen,
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
    std::vector<double> levels(ThermalGen.size());
    if (levels.size())
    {
        levels[0] = initial_level + inflows[0] - overflow[0] + pump[0] - HydroGen[0];
        for (size_t i = 1; i < levels.size(); ++i)
        {
            levels[i] = levels[i - 1] + inflows[i] - overflow[i] + pump[i] - HydroGen[i];
        }
    }

    checkInputCorrectness(ThermalGen,
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

    std::vector<double> new_H = HydroGen;
    std::vector<double> new_D = UnsupE;

    int loop = 1000;
    double eps = 1e-3;
    double top = *std::max_element(ThermalGen.begin(), ThermalGen.end())
                 + *std::max_element(HydroGen.begin(), HydroGen.end())
                 + *std::max_element(UnsupE.begin(), UnsupE.end()) + 1;

    std::vector<bool> filter_hours_remix(ThermalGen.size(), false);
    for (unsigned int h = 0; h < filter_hours_remix.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && HydroGen[h] + UnsupE[h] > 0.)
        {
            filter_hours_remix[h] = true;
        }
    }

    std::vector<double> G_plus_H(ThermalGen.size());
    std::transform(ThermalGen.begin(),
                   ThermalGen.end(),
                   new_H.begin(),
                   G_plus_H.begin(),
                   std::plus<>());

    while (loop-- > 0)
    {
        std::vector<int> tried_creux(ThermalGen.size(), 0);
        double delta = 0;

        while (true)
        {
            int idx_creux = find_min_index(G_plus_H,
                                           new_D,
                                           new_H,
                                           tried_creux,
                                           HydroPmax,
                                           filter_hours_remix,
                                           top);
            if (idx_creux == -1)
            {
                break;
            }

            std::vector<int> tried_pic(ThermalGen.size(), 0);
            while (true)
            {
                int idx_pic = find_max_index(G_plus_H,
                                             new_H,
                                             tried_pic,
                                             HydroPmin,
                                             filter_hours_remix,
                                             G_plus_H[idx_creux],
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

                max_pic = std::min(new_H[idx_pic] - HydroPmin[idx_pic], max_pic);
                max_creux = std::min(
                  {HydroPmax[idx_creux] - new_H[idx_creux], new_D[idx_creux], max_creux});

                double dif_pic_creux = std::max(G_plus_H[idx_pic] - G_plus_H[idx_creux], 0.);

                delta = std::max(std::min({max_pic, max_creux, dif_pic_creux / 2.}), 0.);

                if (delta > 0)
                {
                    new_H[idx_pic] -= delta;
                    new_H[idx_creux] += delta;
                    new_D[idx_pic] = HydroGen[idx_pic] + UnsupE[idx_pic] - new_H[idx_pic];
                    new_D[idx_creux] = HydroGen[idx_creux] + UnsupE[idx_creux] - new_H[idx_creux];
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

        std::transform(ThermalGen.begin(),
                       ThermalGen.end(),
                       new_H.begin(),
                       G_plus_H.begin(),
                       std::plus<>());
        levels[0] = initial_level + inflows[0] - overflow[0] + pump[0] - new_H[0];
        for (size_t i = 1; i < levels.size(); ++i)
        {
            levels[i] = levels[i - 1] + inflows[i] - overflow[i] + pump[i] - new_H[i];
        }
    }
    return {new_H, new_D, levels};
}

} // End namespace Antares::Solver::Simulation
