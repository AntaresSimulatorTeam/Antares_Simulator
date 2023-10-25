#pragma once

#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

struct VariableBounds
{
    VariableBounds(std::string& var_name, double& low_bound, double& up_bound)
        : name(var_name), lowBound(low_bound), upBound(up_bound)
    {}

    const std::string& name;
    const double& lowBound;
    const double& upBound;
};

class VariablesBoundsConsistency : public UnfeasibilityAnalysis
{
public:
    VariablesBoundsConsistency() = default;
    void run(operations_research::MPSolver* problem) override;
    void printReport() const override;
    std::string title() const override { return "Variables bounds consistency check"; }

private:
    void storeIncorrectVariable(std::string name, double lowBound, double upBound);
    bool foundIncorrectVariables();

    std::vector<VariableBounds> incorrectVars_;
};
}
