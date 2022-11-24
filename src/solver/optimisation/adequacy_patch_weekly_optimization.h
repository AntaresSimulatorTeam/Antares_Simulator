#pragma once

#include "base_weekly_optimization.h"
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{
class AdequacyPatchOptimization : public interfaceWeeklyOptimization
{
public:
    explicit AdequacyPatchOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    void solve(uint weekInTheYear, int hourInTheYear) override;
    void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace) override;

private:
    vector<double> calculateENSoverAllAreasForEachHour(uint numSpace) const;
    std::set<int> identifyHoursForCurtailmentSharing(vector<double> sumENS, uint numSpace) const;
    std::set<int> getHoursRequiringCurtailmentSharing(uint numSpace) const;
};
} // namespace Simulation
} // namespace Solver
} // namespace Antares
