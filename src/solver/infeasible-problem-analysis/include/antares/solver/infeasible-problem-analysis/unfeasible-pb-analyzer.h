// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "unfeasibility-analysis.h"

namespace operations_research
{
class MPSolver;
}

namespace Antares::Optimization
{

/*!
 * In charge of anayzing the possible reasons for the unfeasibility of an optimization problem.
 *
 * The analyzer relies on the execution of possibly multiple UnfeasibilityAnalysis.
 */
class UnfeasiblePbAnalyzer
{
public:
    UnfeasiblePbAnalyzer() = delete;
    explicit UnfeasiblePbAnalyzer(std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList);
    void run(operations_research::MPSolver* problem);
    void printReport() const;

private:
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList_;
};

std::unique_ptr<UnfeasiblePbAnalyzer> makeUnfeasiblePbAnalyzer();

} // namespace Antares::Optimization
