// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace operations_research
{
class MPSolver;
}

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

    bool hasDetectedInfeasibilityCause() const
    {
        return hasDetectedInfeasibilityCause_;
    }

protected:
    bool hasDetectedInfeasibilityCause_ = false;
};

} // namespace Antares::Optimization
