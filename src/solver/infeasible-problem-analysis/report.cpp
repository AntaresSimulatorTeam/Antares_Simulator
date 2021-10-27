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
    mConstraints.push_back({constraintName, value});
}

void InfeasibleProblemReport::printLargest(unsigned int n)
{
    std::sort(std::begin(mConstraints), std::end(mConstraints), compareSlackSolutions);

    if (n > mConstraints.size())
        n = mConstraints.size();

    Antares::logs.info() << "Infeasibility analysis gives the following results :";
    for (unsigned int ii = 0; ii < n; ++ii)
        Antares::logs.info() << mConstraints[ii].first << ": " << mConstraints[ii].second;
}
} // namespace Optimization
} // namespace Antares
