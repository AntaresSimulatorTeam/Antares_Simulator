#pragma once

#include <memory>
#include "ortools/linear_solver/linear_solver.h"

namespace Antares::Optimization
{
class InfeasibilityDiagnostic
{
public:
    InfeasibilityDiagnostic(std::shared_ptr<operations_research::MPSolver> problem_);
    virtual void run() = 0;
    virtual void printReport() = 0;
    virtual std::string title() = 0;
    bool hasDetectedInfeasibilityCause() { return hasDetectedInfeasibilityCause_; }

protected:
    std::shared_ptr<operations_research::MPSolver> problem_;
    bool hasDetectedInfeasibilityCause_ = false;
};
}
