#pragma once

#include "base_weekly_optimization.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "antares/study/area/area.h"

using Antares::Data::AreaList;

namespace Antares::Solver::Simulation
{
class AdequacyPatchOptimization : public interfaceWeeklyOptimization
{
public:
    explicit AdequacyPatchOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    virtual ~AdequacyPatchOptimization() = default;
    void solve(uint weekInTheYear, int hourInTheYear) override;
    void postProcess(Antares::Data::AreaList& areas, uint year, uint week) override;

private:
    double calculateDensNewAndTotalLmrViolation(AreaList& areas);
    std::vector<double> calculateENSoverAllAreasForEachHour() const;
    std::set<int> identifyHoursForCurtailmentSharing(std::vector<double> sumENS) const;
    std::set<int> getHoursRequiringCurtailmentSharing() const;
};
} // namespace Antares::Solver::Simulation
