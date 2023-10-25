#pragma once

#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

/*!
 * That particular analysis relaxes all constraints by
 * adding slack variables for each one.
 */
class ConstraintSlackAnalysis : public UnfeasibilityAnalysis
{
public:
    ConstraintSlackAnalysis() = default;
    void run(operations_research::MPSolver* problem) override;
    void printReport() override;
    std::string title() override { return "Slack variables analysis"; }

private:
    void buildObjective(operations_research::MPSolver* problem) const;
    void addSlackVariables(operations_research::MPSolver* problem);

    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::";
};

} // namespace Antares::Optimization
