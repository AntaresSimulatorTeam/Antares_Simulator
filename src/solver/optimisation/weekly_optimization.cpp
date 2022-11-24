#include "weekly_optimization.h"
#include "opt_fonctions.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{

weeklyOptimization::weeklyOptimization(PROBLEME_HEBDO* problemeHebdo, uint thread_number) :
 interfaceWeeklyOptimization(problemeHebdo, thread_number)
{
}

void weeklyOptimization::solve(uint, int)
{
    OPT_OptimisationHebdomadaire(problemeHebdo_, thread_number_);
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
