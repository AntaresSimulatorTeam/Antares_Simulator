#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{
UnfeasibilityAnalysis::UnfeasibilityAnalysis(operations_research::MPSolver* problem)
    : problem_(problem)
{}
}