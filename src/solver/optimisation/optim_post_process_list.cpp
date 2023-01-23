#include "optim_post_process_list.h"
#include "post_process_commands.h"

namespace Antares::Solver::Simulation
{

OptPostProcessList::OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                                       uint thread_number,
                                       AreaList& areas,
                                       SheddingPolicy sheddingPolicy,
                                       SimplexOptimization splxOptimization,
                                       Calendar& calendar)

    : interfacePostProcessList(problemeHebdo, thread_number)
{
    post_process_list.push_back(std::make_unique<DispatchableMarginPostProcessCmd>(problemeHebdo_, thread_number_, areas));
    // post_process_list.push_back(std::make_unique<something>());
}

} // namespace Antares::Solver::Simulation