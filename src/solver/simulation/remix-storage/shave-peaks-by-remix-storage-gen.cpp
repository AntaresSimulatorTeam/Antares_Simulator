#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

#include <optional>
#include <ranges>
#include <set>
#include <stdexcept>
#include <vector>

#include "antares/solver/simulation/remix-storage/remix-utils.h"

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
    auto validHoursView = vws::iota(0, (int)Spillage.size()) | vws::filter(filter);
    return {validHoursView.begin(), validHoursView.end()};
}

void updateValidHours(std::set<unsigned>& validHours,
                      std::shared_ptr<IStorageForRemix>& storage,
                      const std::vector<double>& UnsupEinit)
{
    std::erase_if(validHours,
                  [&](int h) { return storage->initWithdrawal()[h] + UnsupEinit[h] <= eps; });
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

double computeExchange(unsigned hourOfMinGen,
                       unsigned hourOfMaxGen,
                       const std::vector<double>& TotalGen,
                       std::shared_ptr<IStorageForRemix> storage)
{
    double maxVariation = std::max(TotalGen[hourOfMaxGen] - TotalGen[hourOfMinGen], 0.);
    double maxExchangeFromStorage = storage->maxExchange(hourOfMaxGen, hourOfMinGen);
    return std::max(std::min(maxExchangeFromStorage, maxVariation / 2.), 0.);
}

struct Exchange
{
    bool valid()
    {
        return hourOfMinGen.has_value() && hourOfMaxGen.has_value() && amount > eps;
    }

    std::optional<unsigned> hourOfMinGen;
    std::optional<unsigned> hourOfMaxGen;
    double amount = 0;
};

static Exchange searchForExhange(const std::set<unsigned>& validHours,
                                 std::vector<double>& TotalGen,
                                 std::vector<double>& UnsupE,
                                 std::shared_ptr<IStorageForRemix>& storage)
{
    auto totalGenProjection = [&](int h) { return TotalGen[h]; };

    std::set<unsigned> validHoursForMin(validHours);
    std::erase_if(validHoursForMin, [&](int h) { return UnsupE[h] <= eps; });

    while (!validHoursForMin.empty())
    {
        auto hourOfMinGen = rng::min_element(validHoursForMin, {}, totalGenProjection);

        std::set<unsigned> validHoursForMax(validHours);
        double totalGenMin = TotalGen[*hourOfMinGen];
        std::erase_if(validHoursForMax, [&](int h) { return TotalGen[h] < totalGenMin + eps; });

        while (!validHoursForMax.empty())
        {
            auto hourOfMaxGen = rng::max_element(validHoursForMax, {}, totalGenProjection);
            auto exchange = computeExchange(*hourOfMinGen, *hourOfMaxGen, TotalGen, storage);

            if (exchange > eps)
            {
                return {*hourOfMinGen, *hourOfMaxGen, exchange};
            }
            validHoursForMax.erase(hourOfMaxGen);
        }
        validHoursForMin.erase(hourOfMinGen);
    }
    return {};
}

void update(Exchange& exchange,
            std::shared_ptr<IStorageForRemix> storage,
            std::vector<double>& UnsupE,
            std::vector<double>& TotalGen)
{
    double amount = exchange.amount;
    unsigned hourOfMinGen = exchange.hourOfMinGen.value();
    unsigned hourOfMaxGen = exchange.hourOfMaxGen.value();

    storage->withdrawal()[hourOfMaxGen] -= amount;
    storage->withdrawal()[hourOfMinGen] += amount;
    storage->update();

    UnsupE[hourOfMaxGen] += amount;
    UnsupE[hourOfMinGen] -= amount;

    TotalGen[hourOfMaxGen] -= amount;
    TotalGen[hourOfMinGen] += amount;
}

auto removeStorageFromList(std::vector<std::shared_ptr<IStorageForRemix>>::iterator storage,
                           ListStorageForRemix& listStorage)
{
    auto d = std::distance(listStorage.begin(), storage);
    listStorage.erase(storage, storage + 1);
    return listStorage.begin() + d;
}

void shavePeaksByRemixingStorageGen(const std::vector<double>& Load,
                                    std::vector<double>& UnsupE,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    ListStorageForRemix& listStorage)
{
    const std::vector<double> UnsupEinit = UnsupE;
    std::vector<double> TotalGen = Load - UnsupEinit;

    std::set<unsigned> validHours = ValidHours(Spillage, DTG_MRG);

    unsigned nbLoops = 0;
    auto storage = listStorage.begin();
    while (!listStorage.empty() || nbLoops == maxNbLoops)
    {
        if (nbLoops++ == maxNbLoops)
        {
            throw std::runtime_error("storage remix > max nb of iterations was reached");
        }

        if (storage == listStorage.end())
        {
            storage = listStorage.begin();
        }

        updateValidHours(validHours, *storage, UnsupEinit);
        auto exchange = searchForExhange(validHours, TotalGen, UnsupE, *storage);

        if (!exchange.valid())
        {
            storage = removeStorageFromList(storage, listStorage);
            continue;
        }
        update(exchange, *storage, UnsupE, TotalGen);
        storage++;
    }
}

} // End namespace Antares::Solver::Simulation
