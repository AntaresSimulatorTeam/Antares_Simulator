#pragma once


#include "../simulation/base_post_process.h"

namespace Antares::Solver::Simulation
{

class AdqPatchPostProcessList : public interfacePostProcessList
{
public:
    AdqPatchPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                            uint thread_number,
                            AreaList& areas,
                            SheddingPolicy sheddingPolicy,
                            SimplexOptimization splxOptimization,
                            Calendar& calendar);

    virtual ~AdqPatchPostProcessList() = default;
};

} // namespace Antares::Solver::Simulation
