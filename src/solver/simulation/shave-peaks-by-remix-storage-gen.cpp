#include "include/antares/solver/simulation/shave-peaks-by-remix-storage-gen.h"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "include/antares/solver/simulation/remix-utils.h"

namespace rng = std::ranges;
namespace vws = std::ranges::views;

constexpr double eps = 1e-3;
constexpr unsigned maxNbLoops = 1000;
const std::string error_msg_start = "Remix storage input : ";

namespace Antares::Solver::Simulation
{

static std::vector<unsigned> filterHoursForMin(const std::vector<double>& UnsupE,
                                               const std::vector<bool>& triedMins,
                                               const std::vector<unsigned>& validHours)
{
    auto filter = [&](int h) { return UnsupE[h] > 0 && !triedMins[h]; };
    auto filterHoursView = validHours | vws::filter(filter);
    return {filterHoursView.begin(), filterHoursView.end()};
}

static unsigned hourForTotalGenMin(const std::vector<double>& TotalGen,
                                   const std::vector<unsigned>& filteredHours)
{
    return *rng::min_element(filteredHours, {}, [&](int h) { return TotalGen[h]; });
}

static std::vector<unsigned> filterHoursForMax(const std::vector<double>& TotalGen,
                                               const std::vector<bool>& triedMaxs,
                                               const std::vector<unsigned>& validHours,
                                               double minTotalGen)
{
    auto filter = [&](int h) { return TotalGen[h] >= minTotalGen + eps && !triedMaxs[h]; };
    auto filterHoursView = validHours | vws::filter(filter);
    return {filterHoursView.begin(), filterHoursView.end()};
}

static unsigned hourForTotalGenMax(const std::vector<double>& TotalGen,
                                   const std::vector<unsigned>& filteredHours)
{
    return *rng::max_element(filteredHours, {}, [&](int h) { return TotalGen[h]; });
}

void checkInput(const std::vector<double>& Load,
                const std::vector<double>& UnsupE,
                const std::vector<double>& Spillage,
                const std::vector<double>& DTG_MRG,
                const std::vector<double>& storageGen)
{
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {Load.size(),
                                 UnsupE.size(),
                                 Spillage.size(),
                                 DTG_MRG.size(),
                                 storageGen.size()};

    if (!rng::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(error_msg_start + "arrays of different sizes");
    }

    if (!Load.size())
    {
        throw std::invalid_argument(error_msg_start + "all arrays of sizes 0");
    }
}

static std::vector<unsigned> ValidHours(const std::vector<double>& Spillage,
                                        const std::vector<double>& DTG_MRG,
                                        const std::vector<double>& StorageGen,
                                        const std::vector<double>& UnsupE)
{
    auto filter = [&](int h)
    { return std::abs(Spillage[h] + DTG_MRG[h]) < eps && StorageGen[h] + UnsupE[h] > 0.; };

    auto filterHoursView = vws::iota(0, static_cast<int>(Spillage.size())) | vws::filter(filter);
    return {filterHoursView.begin(), filterHoursView.end()};
}

static double makeExchange(std::vector<double>& TotalGen,
                           std::vector<double>& UnsupE,
                           const std::vector<unsigned>& validHours,
                           std::shared_ptr<StorageForRemix>& storage)
{
    double exchange = 0.; // To be returned
    size_t nbHours = TotalGen.size();

    std::vector<bool> triedMins(nbHours, false);
    while (true)
    {
        auto filteredHours = filterHoursForMin(UnsupE, triedMins, validHours);
        if (!filteredHours.size())
        {
            return 0.;
        }

        unsigned hourOfMinGen = hourForTotalGenMin(TotalGen, filteredHours);

        std::vector<bool> triedMaxs(nbHours, false);
        while (true)
        {
            double totaGenMin = TotalGen[hourOfMinGen];
            filteredHours = filterHoursForMax(TotalGen, triedMaxs, validHours, totaGenMin);
            if (!filteredHours.size())
            {
                break;
            }

            unsigned hourOfMaxGen = hourForTotalGenMax(TotalGen, filteredHours);

            double maxVariation = std::max(TotalGen[hourOfMaxGen] - TotalGen[hourOfMinGen], 0.);
            double maxExchangeFromStorage = storage->maxExchange(hourOfMaxGen, hourOfMinGen);
            exchange = std::max(std::min(maxExchangeFromStorage, maxVariation / 2.), 0.);

            if (exchange > eps)
            {
                storage->generation()[hourOfMaxGen] -= exchange;
                storage->generation()[hourOfMinGen] += exchange;
                storage->update();

                UnsupE[hourOfMaxGen] += exchange;
                UnsupE[hourOfMinGen] -= exchange;

                TotalGen[hourOfMaxGen] -= exchange;
                TotalGen[hourOfMinGen] += exchange;

                return exchange;
            }
            triedMaxs[hourOfMaxGen] = true;
        }

        triedMins[hourOfMinGen] = true;
    }
}

void shavePeaksByRemixingStorageGen(const std::vector<double>& Load,
                                    std::vector<double>& UnsupE,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    std::shared_ptr<StorageForRemix> storage)
{
    const std::vector<double> UnsupEinit = UnsupE;

    std::vector<double> TotalGen = Load - UnsupEinit;

    unsigned nbLoops = maxNbLoops;
    while (nbLoops-- > 0)
    {
        // Valid hours could be computed once for all, not at each iterations.
        const auto validHours = ValidHours(Spillage, DTG_MRG, storage->initialGen(), UnsupEinit);
        double exchange = makeExchange(TotalGen, UnsupE, validHours, storage);

        if (exchange <= eps)
        {
            break;
        }
    }
}

} // End namespace Antares::Solver::Simulation
