// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"
#include "watched-constraints.h"

namespace operations_research
{
class MPVariable;
}

namespace Antares::Optimization
{
class InfeasibleProblemReport
{
public:
    InfeasibleProblemReport() = delete;
    explicit InfeasibleProblemReport(const std::vector<const operations_research::MPVariable*>&);
    void storeSuspiciousConstraints();
    void storeInfeasibilityCauses();
    const std::vector<std::string>& getLogs() const;

private:
    void filterConstraintsToOneByType();

    std::vector<std::shared_ptr<WatchedConstraint>> constraints_;
    std::vector<std::string> report_;
};
} // namespace Antares::Optimization
