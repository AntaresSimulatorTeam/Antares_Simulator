#pragma once

#include <memory>
#include "ortools/linear_solver/linear_solver.h"

namespace Antares::Optimization
{
class UnfeasibilityAnalysis
{
public:
    UnfeasibilityAnalysis() = default;
    virtual void run(operations_research::MPSolver* problem) = 0;
    virtual void printReport() const = 0;
    virtual std::string title() const = 0;
    bool hasDetectedInfeasibilityCause() const { return hasDetectedInfeasibilityCause_; }

protected:
    bool hasDetectedInfeasibilityCause_ = false;
};

}
