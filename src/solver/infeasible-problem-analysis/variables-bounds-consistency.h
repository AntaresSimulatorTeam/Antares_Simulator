#pragma once

#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

struct VariableBounds
{
    VariableBounds(std::string var_name, double low_bound, double up_bound)
        : name(var_name), lowBound(low_bound), upBound(up_bound)
    {}

    std::string name;
    double lowBound;
    double upBound;
};

/*!
 * That particular analysis simply checks that all variables
 * are within their minimum and maximum bounds.
 */
class VariablesBoundsConsistency : public UnfeasibilityAnalysis
{
public:
    VariablesBoundsConsistency() = default;
    ~VariablesBoundsConsistency() override = default;

    void run(operations_research::MPSolver* problem) override;
    void printReport() override;
    std::string title() override { return "Variables bounds consistency check"; }

    const std::vector<VariableBounds>& incorrectVars() const { return incorrectVars_; }

private:
    void storeIncorrectVariable(std::string name, double lowBound, double upBound);
    bool foundIncorrectVariables();

    std::vector<VariableBounds> incorrectVars_;
};
}
