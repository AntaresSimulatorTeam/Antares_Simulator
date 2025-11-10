#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

#include <ranges>
#include <set>
#include <stdexcept>
#include <vector>

#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-help.h"

constexpr unsigned maxNbLoops = 1000;
const std::string shave_peak_remix_storage_error_msg_start = "Remix storage input : ";

namespace Antares::Solver::Simulation
{

void checkInput(const std::vector<double>& Load,
                const std::vector<double>& UnsupE,
                const std::vector<double>& Spillage,
                const std::vector<double>& DTG_MRG,
                const ListStorageForRemix& storagesForRemix)
{
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {Load.size(), UnsupE.size(), Spillage.size(), DTG_MRG.size()};
    std::ranges::transform(storagesForRemix,
                           std::back_inserter(sizes),
                           [&](auto& s) { return s->initWithdrawal().size(); });

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(shave_peak_remix_storage_error_msg_start
                                    + "arrays of different sizes");
    }

    if (!Load.size())
    {
        throw std::invalid_argument(shave_peak_remix_storage_error_msg_start
                                    + "all arrays of sizes 0");
    }
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
    CyclicIterator<std::shared_ptr<IStorageForRemix>> cyclic_it(listStorage);
    while (!listStorage.empty() && nbLoops != maxNbLoops)
    {
        if (nbLoops++ == maxNbLoops)
        {
            throw std::runtime_error("storage remix > max nb of iterations was reached");
        }

        updateValidHours(validHours, **cyclic_it, UnsupEinit);
        auto exchange = searchForExhange(validHours, TotalGen, UnsupE, **cyclic_it);

        if (!exchange.isPossible())
        {
            delete_current(cyclic_it);
        }
        else
        {
            update(exchange, **cyclic_it, UnsupE, TotalGen);
            cyclic_it++;
        }
    }
}

} // End namespace Antares::Solver::Simulation
