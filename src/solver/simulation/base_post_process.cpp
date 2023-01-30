
#include <memory>
#include "base_post_process.h"
#include "../optimisation/optim_post_process_list.h"
#include "../optimisation/adq_patch_post_process_list.h"

namespace Antares::Solver::Simulation
{

basePostProcessCommand::basePostProcessCommand(PROBLEME_HEBDO* problemeHebdo) 
    : problemeHebdo_(problemeHebdo)
{
}

interfacePostProcessList::interfacePostProcessList(PROBLEME_HEBDO* problemesHebdo, uint numSpace) :
    problemeHebdo_(problemesHebdo), thread_number_(numSpace)
{
}


std::unique_ptr<interfacePostProcessList> interfacePostProcessList::create(
    bool adqPatchEnabled,
    PROBLEME_HEBDO* problemeHebdo,
    uint thread_number,
    AreaList& areas,
    SheddingPolicy sheddingPolicy,
    SimplexOptimization splxOptimization,
    Calendar& calendar)
{
    if (adqPatchEnabled)
        return std::make_unique<AdqPatchPostProcessList>(
            problemeHebdo, thread_number, areas, sheddingPolicy, splxOptimization, calendar);
    else
        return std::make_unique<OptPostProcessList>(
            problemeHebdo, thread_number, areas, sheddingPolicy, splxOptimization, calendar);
}

void interfacePostProcessList::runAll(const optRuntimeData& opt_runtime_data)
{
    for (auto& post_process : post_process_list)
    {
        post_process->execute(opt_runtime_data);
    }
}

} // namespace Antares::Solver::Simulation