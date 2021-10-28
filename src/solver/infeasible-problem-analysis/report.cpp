#include "report.h"
#include "constraint.h"
#include <antares/logs.h>
#include <algorithm>
#include <map>

static bool compareSlackSolutions(const Antares::Optimization::Constraint& a,
                                  const Antares::Optimization::Constraint& b)
{
    return a.getSlackValue() > b.getSlackValue();
}

namespace Antares
{
namespace Optimization
{
void InfeasibleProblemReport::append(const std::string& constraintName, double value)
{
    // TODO check prior existence of constraintName
    mConstraints.emplace_back(constraintName, value);
}

void InfeasibleProblemReport::rawPrint() const
{
    for (const auto& c : mConstraints)
        Antares::logs.info() << c.getInput() << ": " << c.getSlackValue();
}

void InfeasibleProblemReport::prettyPrint()
{
    std::map<ConstraintType, unsigned int> countTypes;
    for (auto& c : mConstraints)
    {
        if (!c.extractItems())
            return;
    }
    Antares::logs.error() << "Infeasible problem encountered. The following constraints are "
                             "suspicious (first = most suspicious)";
    for (const auto& c : mConstraints)
    {
        Antares::logs.error() << c.prettyPrint();
        countTypes[c.getType()]++;
    }
    Antares::logs.error() << "Possible causes of infeasibility:";
    if (countTypes[ConstraintType::hydro_reservoir_level] > 0)
    {
        Antares::logs.error() << "* Hydro reservoir impossible to manage with cumulative options "
                                 "\"hard bounds without heuristic\"";
    }
    if (countTypes[ConstraintType::fictitious_load] > 0)
    {
        Antares::logs.error() << "* Last resort shedding status,";
    }
    if (countTypes[ConstraintType::binding_constraint_hourly] > 0
        || countTypes[ConstraintType::binding_constraint_daily] > 0
        || countTypes[ConstraintType::binding_constraint_weekly] > 0)
    {
        Antares::logs.error() << "* Binding constraints,";
    }

    // TODO: analyse hurdle costs & NTCs to rule out this possibility
    Antares::logs.error() << "* Negative hurdle costs on lines with infinite capacity (rare).";
}

void InfeasibleProblemReport::trimTo(std::size_t n)
{
    std::sort(std::begin(mConstraints), std::end(mConstraints), compareSlackSolutions);

    if (n > mConstraints.size())
        n = mConstraints.size();

    mConstraints.resize(n);
}

} // namespace Optimization
} // namespace Antares
