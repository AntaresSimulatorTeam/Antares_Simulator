#include "include/antares/solver/simulation/shave-peaks-by-remix-storage-gen.h"

#include <ranges>
#include <set>
#include <stdexcept>
#include <vector>

#include "include/antares/solver/simulation/remix-utils.h"

namespace rng = std::ranges;
namespace vws = std::views;

constexpr double eps = 1e-3;
constexpr unsigned maxNbLoops = 1000;
const std::string error_msg_start = "Remix storage input : ";

using set_iterator = std::set<unsigned>::iterator;

namespace Antares::Solver::Simulation
{

static std::set<unsigned> ValidHours(const std::vector<double>& Spillage,
                                     const std::vector<double>& DTG_MRG)
{
    auto filter = [&](int h) { return std::abs(Spillage[h] + DTG_MRG[h]) < eps; };
    auto validHoursView = vws::iota(0, static_cast<int>(Spillage.size())) | vws::filter(filter);
    return {validHoursView.begin(), validHoursView.end()};
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

static double makeExchange(const std::set<unsigned>& validHours,
                           std::vector<double>& TotalGen,
                           std::vector<double>& UnsupE,
                           std::shared_ptr<StorageForRemix>& storage)
{
    double exchange = 0.; // To be returned
    auto totalGenProjection = [&](int h) { return TotalGen[h]; };

    std::set<unsigned> validHoursForMin(validHours);
    while (true)
    {
        std::erase_if(validHoursForMin, [&](int h) { return UnsupE[h] <= eps; });
        if (!validHoursForMin.size())
        {
            return 0.;
        }

        auto hourOfMinGen = rng::min_element(validHoursForMin, {}, totalGenProjection);

        std::set<unsigned> validHoursForMax(validHours);
        while (true)
        {
            double totaGenMin = TotalGen[*hourOfMinGen];
            std::erase_if(validHoursForMax, [&](int h) { return TotalGen[h] < totaGenMin + eps; });
            if (!validHoursForMax.size())
            {
                break;
            }

            auto hourOfMaxGen = rng::max_element(validHoursForMax, {}, totalGenProjection);

            double maxVariation = std::max(TotalGen[*hourOfMaxGen] - TotalGen[*hourOfMinGen], 0.);
            double maxExchangeFromStorage = storage->maxExchange(*hourOfMaxGen, *hourOfMinGen);
            exchange = std::max(std::min(maxExchangeFromStorage, maxVariation / 2.), 0.);

            if (exchange > eps)
            {
                storage->generation()[*hourOfMaxGen] -= exchange;
                storage->generation()[*hourOfMinGen] += exchange;
                storage->update();

                UnsupE[*hourOfMaxGen] += exchange;
                UnsupE[*hourOfMinGen] -= exchange;

                TotalGen[*hourOfMaxGen] -= exchange;
                TotalGen[*hourOfMinGen] += exchange;

                return exchange;
            }
            validHoursForMax.erase(hourOfMaxGen);
        }
        validHoursForMin.erase(hourOfMinGen);
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

    const auto validHours = ValidHours(Spillage, DTG_MRG);

    unsigned nbLoops = maxNbLoops;
    while (nbLoops-- > 0)
    {
        std::set<unsigned> hoursForStorage;
        auto predicate = [&](int h) { return storage->initialGen()[h] + UnsupEinit[h] > eps; };
        rng::copy_if(validHours, std::inserter(hoursForStorage, hoursForStorage.end()), predicate);

        double exchange = makeExchange(hoursForStorage, TotalGen, UnsupE, storage);

        if (exchange <= eps)
        {
            break;
        }
    }
}

} // End namespace Antares::Solver::Simulation
