#pragma once

#include "base_weekly_optimization.h"
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
class weeklyOptimization : public interfaceWeeklyOptimization
{
public:
    explicit weeklyOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    virtual ~weeklyOptimization() = default;
    void solve(uint, int) override;
    void postProcess(Antares::Data::AreaList&, uint, uint) override;
};
} // namespace Antares::Solver::Simulation
