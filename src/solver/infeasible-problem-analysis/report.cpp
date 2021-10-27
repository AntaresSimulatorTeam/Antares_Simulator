#include "report.h"
#include "constraint.h"
#include <antares/logs.h>
#include <algorithm>
#include <regex>

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
    for (auto& c : mConstraints)
    {
        if (!c.extractItems())
            return;
        Antares::logs.info() << c.prettyPrint();
    }
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
