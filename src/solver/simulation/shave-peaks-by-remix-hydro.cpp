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

HydroStorage::HydroStorage(std::vector<double>& generation,
                           std::vector<double>& unsupE,
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
    levels_.assign(generation.size(), 0.);
}

double HydroStorage::computeBound(unsigned hourMax, unsigned hourMin)
{
    // max slice we can take from hydro generation, at an hour when the total
    // production reaches a max.
    double boundAtMax = std::numeric_limits<double>::max();
    // max slice we can add to hydro generation, at an hour when the total
    // production reaches a min.
    double boundAtMin = std::numeric_limits<double>::max();

    if (reservoirManagement_)
    {
        unsigned minHour = std::min(hourMin, hourMax);
        unsigned maxHour = std::max(hourMin, hourMax);
        std::span<double> intermediate_level(levels_.begin() + minHour, levels_.begin() + maxHour);

        if (hourMin < hourMax)
        {
            boundAtMax = capacity_;
            boundAtMin = *std::ranges::min_element(intermediate_level);
        }
        else
        {
            boundAtMax = capacity_ - *std::ranges::max_element(intermediate_level);
            boundAtMin = capacity_;
        }
    }

    boundAtMax = std::min(generation_[hourMax] - pmin_[hourMax], boundAtMax);
    boundAtMin = std::min({pmax_[hourMin] - generation_[hourMin], unsupE_[hourMin], boundAtMin});
    return std::min(boundAtMax, boundAtMin);
}

void HydroStorage::checkInput(size_t size)
{
    std::vector<size_t> sizes = {generation_.size(), pmin_.size(), pmax_.size()};

    if (reservoirManagement_)
    {
        if (initLevel_ >= capacity_ + LEVEL_TOLERANCE)
        {
            throw std::invalid_argument(error_msg_start + "initial level > reservoir capacity");
        }

        sizes.push_back(inflows_.size());
        sizes.push_back(overflow_.size());
        sizes.push_back(pump_.size());
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

void HydroStorage::update()
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

std::vector<double>& HydroStorage::generation()
{
    return generation_;
}

std::vector<double> HydroStorage::levels()
{
    return levels_;
}

static int hourForTotalGenMin(const std::vector<double>& TotalGen,
                              const std::vector<double>& OutUnsupE,
                              const std::vector<bool>& triedMins,
                              const std::vector<bool>& validHours,
                              double top)
{
    double minTotalGen = top;
    int min_hour = -1;
    for (unsigned h = 0; h < TotalGen.size(); ++h)
    {
        if (OutUnsupE[h] > 0 && !triedMins[h] && validHours[h])
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

static int hourForTotalGenMax(const std::vector<double>& TotalGen,
                              const std::vector<bool>& triedMaxs,
                              const std::vector<bool>& validHours,
                              double minTotalGen)
{
    double maxTotalGen = 0;
    int max_hour = -1;
    for (unsigned h = 0; h < TotalGen.size(); ++h)
    {
        if (TotalGen[h] >= minTotalGen + eps && !triedMaxs[h] && validHours[h])
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

static void checkInput(const std::vector<double>& DispatchGen,
                       const std::vector<double>& UnsupE,
                       const std::vector<double>& Spillage,
                       const std::vector<double>& DTG_MRG)
{
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {DispatchGen.size(),
                                 UnsupE.size(),
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
}

static std::vector<double> updateTotalGen(const std::vector<double>& DispatchGen,
                                          const std::vector<double>& StorageGen)
{
    std::vector<double> totalGen(DispatchGen.size());
    std::transform(DispatchGen.begin(),
                   DispatchGen.end(),
                   StorageGen.begin(),
                   totalGen.begin(),
                   std::plus<>());
    return totalGen;
}

std::vector<bool> ValidHours(const std::vector<double>& Spillage,
                             const std::vector<double>& DTG_MRG,
                             const std::vector<double>& StorageGen,
                             const std::vector<double>& UnsupE)
{
    std::vector<bool> validHours(Spillage.size(), false);
    for (unsigned h = 0; h < validHours.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && StorageGen[h] + UnsupE[h] > 0.)
        {
            validHours[h] = true;
        }
    }
    return validHours;
}

void shavePeaksByRemixingHydro(std::vector<double>& UnsupE,
                               const std::vector<double>& DispatchGen,
                               const std::vector<double>& Spillage,
                               const std::vector<double>& DTG_MRG,
                               std::shared_ptr<Storage> storage)
{
    const std::vector<double> storageGenInit = storage->generation();
    const std::vector<double> UnsupEinit = UnsupE;

    checkInput(DispatchGen, UnsupEinit, Spillage, DTG_MRG);

    storage->checkInput(DispatchGen.size());
    storage->update();

    int loop = 1000;
    double top = *std::max_element(DispatchGen.begin(), DispatchGen.end())
                 + *std::max_element(storageGenInit.begin(), storageGenInit.end())
                 + *std::max_element(UnsupEinit.begin(), UnsupEinit.end()) + 1;

    const auto validHours = ValidHours(Spillage, DTG_MRG, storageGenInit, UnsupEinit);

    std::vector<double> TotalGen = updateTotalGen(DispatchGen, storageGenInit);

    while (loop-- > 0)
    {
        std::vector<bool> triedMins(DispatchGen.size(), false);
        double delta = 0;

        while (true)
        {
            int hourMin = hourForTotalGenMin(TotalGen, UnsupE, triedMins, validHours, top);
            if (hourMin == -1)
            {
                break;
            }

            std::vector<bool> triedMaxs(DispatchGen.size(), false);
            while (true)
            {
                int hourMax = hourForTotalGenMax(TotalGen,
                                                 triedMaxs,
                                                 validHours,
                                                 TotalGen[hourMin]);
                if (hourMax == -1)
                {
                    break;
                }

                double maxVariation = std::max(TotalGen[hourMax] - TotalGen[hourMin], 0.);
                double storageBound = storage->computeBound(hourMax, hourMin);
                delta = std::max(std::min(storageBound, maxVariation / 2.), 0.);

                if (delta > eps)
                {
                    storage->generation()[hourMax] -= delta;
                    storage->generation()[hourMin] += delta;
                    UnsupE[hourMax] = storageGenInit[hourMax] + UnsupEinit[hourMax]
                                      - storage->generation()[hourMax];

                    storage->update();

                    TotalGen = updateTotalGen(DispatchGen, storage->generation());
                    break;
                }
                triedMaxs[hourMax] = true;
            }

            UnsupE[hourMin] = storageGenInit[hourMin] + UnsupEinit[hourMin]
                              - storage->generation()[hourMin];
            if (delta > eps)
            {
                break;
            }
            triedMins[hourMin] = true;
        }

        if (delta <= eps)
        {
            break;
        }
    }
}

} // End namespace Antares::Solver::Simulation
