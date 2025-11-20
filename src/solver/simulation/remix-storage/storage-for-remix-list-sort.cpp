#include <algorithm>
#include <ranges>

#include "antares/solver/simulation/remix-storage/storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{
void StorageListSort::add(const double capacity, const std::shared_ptr<IStorageForRemix> sts)
{
    pairs_capa_storage_.push_back({capacity, sts});
}

ListStorageForRemix StorageListSort::makeSortedList()
{
    std::ranges::sort(pairs_capa_storage_, std::ranges::greater{}, [](auto& p) { return p.first; });

    ListStorageForRemix list_to_return;
    std::ranges::transform(pairs_capa_storage_,
                           std::back_inserter(list_to_return),
                           [](auto& p) { return p.second; });
    return list_to_return;
}
} // namespace Antares::Solver::Simulation
