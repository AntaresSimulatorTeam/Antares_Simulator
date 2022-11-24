#pragma once

#include "base_weekly_optimization.h"
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{
class weeklyOptimization : public interfaceWeeklyOptimization
{
public:
    explicit weeklyOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    void solve(uint, int) override;
};
} // namespace Simulation
} // namespace Solver
} // namespace Antares
