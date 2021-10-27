#include "report.h"
#include <antares/logs.h>
#include <algorithm>

using SlackSolution = Antares::Optimization::InfeasibleProblemReport::SlackSolution;

static bool compareSlackSolutions(const SlackSolution& a, const SlackSolution& b)
{
    return a.second > b.second;
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

void InfeasibleProblemReport::print() const
{
    for (const auto& c : mConstraints)
        Antares::logs.info() << c.first << ": " << c.second;
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
