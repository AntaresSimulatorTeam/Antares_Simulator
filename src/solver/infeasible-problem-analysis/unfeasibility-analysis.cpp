#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{
UnfeasibilityAnalysis::UnfeasibilityAnalysis(std::shared_ptr<operations_research::MPSolver> problem)
    : problem_(problem)
{}
}