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
    post_process_list.push_back(std::make_unique<DispatchableMarginPostProcessCmd>(
        problemeHebdo_, 
        thread_number_, 
        areas));
    post_process_list.push_back(std::make_unique<HydroLevelsUpdatePostProcessCmd>(
        problemeHebdo_, 
        areas, 
        false, 
        false));
    post_process_list.push_back(std::make_unique<RemixHydroPostProcessCmd>(
        problemeHebdo_,
        areas,
        sheddingPolicy,
        splxOptimization,
        thread_number));
    post_process_list.push_back(std::make_unique<HydroLevelsUpdatePostProcessCmd>(
        problemeHebdo_,
        areas,
        true,
        false));
    post_process_list.push_back(std::make_unique<InterpolateWaterValuePostProcessCmd>(
        problemeHebdo_,
        areas,
        calendar));

    // post_process_list.push_back(std::make_unique<something>());
}

} // namespace Antares::Solver::Simulation