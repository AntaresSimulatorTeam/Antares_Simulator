#include "include/antares/solver/simulation/shave-peaks-by-remix-storage-gen.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "include/antares/solver/simulation/remix-utils.h"

namespace rng = std::ranges;
namespace vws = std::ranges::views;

constexpr double eps = 1e-3;
const std::string error_msg_start = "Remix storage input : ";

namespace Antares::Solver::Simulation
{

static std::vector<int> filterHoursForMin(const std::vector<double>& UnsupE,
                                          const std::vector<bool>& triedMins,
                                          const std::vector<bool>& validHours)
{
    auto selectHours = [&](int h) { return UnsupE[h] > 0 && !triedMins[h] && validHours[h]; };
    auto filterHoursView = vws::iota(0, static_cast<int>(UnsupE.size())) | vws::filter(selectHours);
    return {filterHoursView.begin(), filterHoursView.end()};
}

static int hourForTotalGenMin(const std::vector<double>& TotalGen,
                              std::vector<int>& filteredHours,
                              double top)
{
    auto min_it = rng::min_element(filteredHours, {}, [&](int h) { return TotalGen[h]; });
    return min_it == filteredHours.end() || TotalGen[*min_it] > top ? -1 : *min_it;
}

static std::vector<int> filterHoursForMax(const std::vector<double>& TotalGen,
                                          const std::vector<bool>& triedMaxs,
                                          const std::vector<bool>& validHours,
                                          double minTotalGen)
{
    auto selectHours = [&](int h)
    { return TotalGen[h] >= minTotalGen + eps && !triedMaxs[h] && validHours[h]; };
    auto filterHoursView = vws::iota(0, static_cast<int>(TotalGen.size()))
                           | vws::filter(selectHours);
    return {filterHoursView.begin(), filterHoursView.end()};
}

static int hourForTotalGenMax(const std::vector<double>& TotalGen, std::vector<int>& filteredHours)
{
    auto max_it = rng::max_element(filteredHours, {}, [&](int h) { return TotalGen[h]; });
    return max_it == filteredHours.end() ? -1 : *max_it;
}

static void checkInput(const std::vector<double>& DispatchGen,
                       const std::vector<double>& UnsupE,
                       const std::vector<double>& Spillage,
                       const std::vector<double>& DTG_MRG,
                       const std::vector<double>& storageGen)
{
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {DispatchGen.size(),
                                 UnsupE.size(),
                                 Spillage.size(),
                                 DTG_MRG.size(),
                                 storageGen.size()};

    if (!rng::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
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

static std::vector<bool> ValidHours(const std::vector<double>& Spillage,
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

void shavePeaksByRemixingStorageGen(std::vector<double>& UnsupE,
                                    const std::vector<double>& DispatchGen,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    std::shared_ptr<StorageForRemix> storage)
{
    const std::vector<double> storageGenInit = storage->generation();
    const std::vector<double> UnsupEinit = UnsupE;

    checkInput(DispatchGen, UnsupEinit, Spillage, DTG_MRG, storageGenInit);

    int loop = 1000;
    double top = *rng::max_element(DispatchGen) + *rng::max_element(storageGenInit)
                 + *rng::max_element(UnsupEinit) + 1;

    const auto validHours = ValidHours(Spillage, DTG_MRG, storageGenInit, UnsupEinit);

    std::vector<double> TotalGen = updateTotalGen(DispatchGen, storageGenInit);

    while (loop-- > 0)
    {
        std::vector<bool> triedMins(DispatchGen.size(), false);
        double maxExchange = 0;

        while (true)
        {
            auto filteredHours = filterHoursForMin(UnsupE, triedMins, validHours);
            int hourMin = hourForTotalGenMin(TotalGen, filteredHours, top);
            if (hourMin == -1)
            {
                break;
            }

            std::vector<bool> triedMaxs(DispatchGen.size(), false);
            while (true)
            {
                double totaGenMin = TotalGen[hourMin];
                filteredHours = filterHoursForMax(TotalGen, triedMaxs, validHours, totaGenMin);
                int hourMax = hourForTotalGenMax(TotalGen, filteredHours);
                if (hourMax == -1)
                {
                    break;
                }

                double maxVariation = std::max(TotalGen[hourMax] - TotalGen[hourMin], 0.);
                double maxExchangeFromStorage = storage->maxExchange(hourMax, hourMin);
                maxExchange = std::max(std::min(maxExchangeFromStorage, maxVariation / 2.), 0.);

                if (maxExchange > eps)
                {
                    storage->generation()[hourMax] -= maxExchange;
                    storage->generation()[hourMin] += maxExchange;
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
            if (maxExchange > eps)
            {
                break;
            }
            triedMins[hourMin] = true;
        }

        if (maxExchange <= eps)
        {
            break;
        }
    }
}

} // End namespace Antares::Solver::Simulation
