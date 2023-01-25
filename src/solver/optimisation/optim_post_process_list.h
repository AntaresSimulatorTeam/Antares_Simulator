#pragma once

#include "../simulation/base_post_process.h"


namespace Antares::Solver::Simulation
{

class OptPostProcessList : public interfacePostProcessList
{
public:
    OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                       uint thread_number,
                       AreaList& areas,
                       SheddingPolicy sheddingPolicy,
                       SimplexOptimization splxOptimization,
                       Calendar& calendar);

    virtual ~OptPostProcessList() = default;
};

} // namespace Antares::Solver::Simulation
