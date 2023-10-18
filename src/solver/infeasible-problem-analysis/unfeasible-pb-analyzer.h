#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../utils/ortools_utils.h"
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

class VariablesBoundsConsistency : public UnfeasibilityAnalysis
{
    using UnfeasibilityAnalysis::UnfeasibilityAnalysis;
public:
    void run() override;
    void printReport() override;
    std::string title() override { return "Variables bounds check"; }

private:
    void storeIncorrectVariable(std::string name, double lowBound, double upBound);
    bool foundIncorrectVariables();

    std::vector<VariableBounds> incorrectVars_;
};

class UnfeasiblePbAnalyzer
{
public:
    UnfeasiblePbAnalyzer() = delete;
    explicit UnfeasiblePbAnalyzer(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx);
    void run();
    void printReport();

private:
    std::shared_ptr<operations_research::MPSolver> problem_;
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList_;
};
} // namespace Antares::Optimization