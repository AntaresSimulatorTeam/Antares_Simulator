#pragma once

#include <yuni/yuni.h> // for "uint" definition
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
class interfaceWeeklyOptimization
{
public:
    virtual void solve(uint weekInTheYear, int hourInTheYear) = 0;
    virtual void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace);
    virtual ~interfaceWeeklyOptimization() = default;
    static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled,
                                                               PROBLEME_HEBDO* problemesHebdo,
                                                               uint numSpace);

protected:
    explicit interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint numSpace);
    PROBLEME_HEBDO* problemeHebdo_ = nullptr;
    uint thread_number_ = 0;
};
} // namespace Antares::Solver::Simulation
