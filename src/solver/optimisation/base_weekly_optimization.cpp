#include <memory>
#include "base_weekly_optimization.h"
#include "weekly_optimization.h"
#include "adequacy_patch_weekly_optimization.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{
interfaceWeeklyOptimization::interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint thread_number) :
    problemeHebdo_(problemesHebdo),
    thread_number_(thread_number)
{
}

std::unique_ptr<interfaceWeeklyOptimization> interfaceWeeklyOptimization::create(
    bool adqPatchEnabled,
    PROBLEME_HEBDO* problemeHebdo,
    uint thread_number)
{
    if (adqPatchEnabled)
        return std::make_unique<AdequacyPatchOptimization>(problemeHebdo, thread_number);
    else
        return std::make_unique<weeklyOptimization>(problemeHebdo, thread_number);

    return nullptr;
}
} // namespace Simulation
} // namespace Solver
} // namespace Antares