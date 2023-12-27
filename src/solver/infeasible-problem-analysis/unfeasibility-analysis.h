#pragma once

#include <memory>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

namespace Antares::Optimization
{

/*!
 * Interface for all elementary analysis.
 */
class UnfeasibilityAnalysis
{
public:
    UnfeasibilityAnalysis() = default;
    virtual ~UnfeasibilityAnalysis() = default;

    virtual void run(operations_research::MPSolver* problem) = 0;
    virtual void printReport() const = 0;
    virtual std::string title() const = 0;
    bool hasDetectedInfeasibilityCause() const { return hasDetectedInfeasibilityCause_; }

protected:
    bool hasDetectedInfeasibilityCause_ = false;
};

}
