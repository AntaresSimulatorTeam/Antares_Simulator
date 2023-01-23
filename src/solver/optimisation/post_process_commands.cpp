
#include "post_process_commands.h"
#include "../simulation/common-eco-adq.h"

namespace Antares::Solver::Simulation
{

DispatchableMarginPostProcessCmd::DispatchableMarginPostProcessCmd(
        PROBLEME_HEBDO* problemeHebdo,
        unsigned int thread_number,
        AreaList& areas) 
    : basePostProcessCommand(problemeHebdo), 
      thread_number_(thread_number), 
      area_list_(areas)
{
}

void DispatchableMarginPostProcessCmd::acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data)
{
    hourInYear_ = opt_runtime_data.hourInTheYear;
}

void DispatchableMarginPostProcessCmd::run()
{
    DispatchableMarginForAllAreas(area_list_, *problemeHebdo_, thread_number_, hourInYear_);
}

} // namespace Antares::Solver::Simulation