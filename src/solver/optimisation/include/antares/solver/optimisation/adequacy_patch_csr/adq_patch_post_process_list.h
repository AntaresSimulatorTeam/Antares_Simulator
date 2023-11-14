#pragma once


#include "../simulation/base_post_process.h"
#include "antares/study/parameters/adq-patch-params.h"

namespace Antares::Solver::Simulation
{

class AdqPatchPostProcessList : public interfacePostProcessList
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;
public:
    AdqPatchPostProcessList(const AdqPatchParams& adqPatchParams,
                            PROBLEME_HEBDO* problemeHebdo,
                            uint thread_number,
                            AreaList& areas,
                            SheddingPolicy sheddingPolicy,
                            SimplexOptimization splxOptimization,
                            Calendar& calendar);

    virtual ~AdqPatchPostProcessList() = default;
};

} // namespace Antares::Solver::Simulation
