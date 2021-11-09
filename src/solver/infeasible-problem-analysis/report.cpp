#include "report.h"
#include "constraint.h"
#include <antares/logs.h>
#include <algorithm>

static bool compareSlackSolutions(const Antares::Optimization::Constraint& a,
                                  const Antares::Optimization::Constraint& b)
{
    return a.getSlackValue() > b.getSlackValue();
}

namespace Antares
{
namespace Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(
  const std::vector<const operations_research::MPVariable*>& slackVariables)
{
    for (const operations_research::MPVariable* slack : slackVariables)
    {
        append(slack->name(), slack->solution_value());
    }
    trim();
}

void InfeasibleProblemReport::append(const std::string& constraintName, double value)
{
    // TODO check prior existence of constraintName
    mConstraints.emplace_back(constraintName, value);
}

void InfeasibleProblemReport::extractItems()
{
    for (auto& c : mConstraints)
    {
        if (c.extractItems() == 0)
        {
            return;
        }
        mTypes[c.getType()]++;
    }
}

void InfeasibleProblemReport::logSuspiciousConstraints()
{
    Antares::logs.error() << "The following constraints are suspicious (first = most suspicious)";
    for (const auto& c : mConstraints)
    {
        Antares::logs.error() << c.prettyPrint();
    }
    Antares::logs.error() << "Possible causes of infeasibility:";
    if (mTypes[ConstraintType::hydro_reservoir_level] > 0)
    {
        Antares::logs.error() << "* Hydro reservoir impossible to manage with cumulative options "
                                 "\"hard bounds without heuristic\"";
    }
    if (mTypes[ConstraintType::fictitious_load] > 0)
    {
        Antares::logs.error() << "* Last resort shedding status,";
    }
    const unsigned int bcCount = mTypes[ConstraintType::binding_constraint_hourly]
                                 + mTypes[ConstraintType::binding_constraint_daily]
                                 + mTypes[ConstraintType::binding_constraint_weekly];
    if (bcCount > 0)
    {
        Antares::logs.error() << "* Binding constraints,";
    }

    Antares::logs.error() << "* Negative hurdle costs on lines with infinite capacity (rare).";
}

void InfeasibleProblemReport::prettyPrint()
{
    extractItems();
    logSuspiciousConstraints();
}

void InfeasibleProblemReport::trim()
{
    std::sort(std::begin(mConstraints), std::end(mConstraints), ::compareSlackSolutions);
    if (nbVariables <= mConstraints.size())
    {
        mConstraints.resize(nbVariables);
    }
}

} // namespace Optimization
} // namespace Antares
