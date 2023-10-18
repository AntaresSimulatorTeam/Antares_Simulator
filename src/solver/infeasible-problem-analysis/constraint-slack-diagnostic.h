#pragma once

#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

class ConstraintSlackDiagnostic : public UnfeasibilityAnalysis
{
    using UnfeasibilityAnalysis::UnfeasibilityAnalysis;
public:
    void run() override;
    void printReport() override;
    std::string title() override { return "Slack variables diagnostic"; }

private:
    void buildObjective() const;
    void addSlackVariables();
    operations_research::MPSolver::ResultStatus Solve() const;

    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::";
};

} // namespace Antares::Optimization
