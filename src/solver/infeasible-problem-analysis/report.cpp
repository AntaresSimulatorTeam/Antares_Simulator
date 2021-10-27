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

void InfeasibleProblemReport::printLargest(std::size_t n)
{
    std::sort(std::begin(mConstraints), std::end(mConstraints), compareSlackSolutions);

    if (n > mConstraints.size())
        n = mConstraints.size();

    for (unsigned int ii = 0; ii < n; ++ii)
        Antares::logs.info() << mConstraints[ii].first << ": " << mConstraints[ii].second;
}
} // namespace Optimization
} // namespace Antares
