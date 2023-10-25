#pragma once

#include <memory>
#include "ortools/linear_solver/linear_solver.h"

namespace Antares::Optimization
{

/*!
 * Interface for all elementary analysis.
 */
class UnfeasibilityAnalysis
{
public:
    UnfeasibilityAnalysis() = default;
    virtual void run(operations_research::MPSolver* problem) = 0;
    virtual void printReport() = 0;
    virtual std::string title() = 0;
    bool hasDetectedInfeasibilityCause() { return hasDetectedInfeasibilityCause_; }

protected:
    bool hasDetectedInfeasibilityCause_ = false;
};

}
