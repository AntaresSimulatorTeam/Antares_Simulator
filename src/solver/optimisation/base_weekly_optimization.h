#pragma once

#include <yuni/yuni.h> // for "uint" definition
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
class interfaceWeeklyOptimization
{
public:
    virtual void solve(uint weekInTheYear, int hourInTheYear) = 0;
    virtual void postProcess(Antares::Data::AreaList& areas, uint year, uint week) = 0;
    virtual ~interfaceWeeklyOptimization() = default;
    static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled,
                                                               PROBLEME_HEBDO* problemesHebdo,
                                                               uint numSpace);

protected:
    explicit interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint numSpace);
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    const uint thread_number_ = 0;
};
} // namespace Antares::Solver::Simulation
