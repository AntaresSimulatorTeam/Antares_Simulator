#include "adq_patch_post_process_list.h"

namespace Antares::Solver::Simulation
{

AdqPatchPostProcessList::AdqPatchPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                                                 uint thread_number,
                                                 AreaList& areas,
                                                 SheddingPolicy sheddingPolicy,
                                                 SimplexOptimization splxOptimization,
                                                 Calendar& calendar) 
    : interfacePostProcessList(problemeHebdo, thread_number)
{
    // post_process_list.push_back(std::make_unique<DispatchableMargin>());
    // post_process_list.push_back(std::make_unique<DispatchableMargin>());
}

} // namespace Antares::Solver::Simulation