#pragma once

#include "../simulation/base_post_process.h"

namespace Antares::Solver::Simulation
{

class DispatchableMarginPostProcessCmd : public basePostProcessCommand
{
public:
    DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                     unsigned int thread_number,
                                     AreaList& areas);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    unsigned int thread_number_ = 0;
    const AreaList& area_list_;
    unsigned int hourInYear_ = 0;
};

} // namespace Antares::Solver::Simulation