#include "report.h"
#include <antares/logs.h>
#include <algorithm>

void InfeasibleProblemReport::append(const std::string& constraintName,
                                double value) {
    // TODO check prior existence of constraintName
    mConstraints.push_back({constraintName, value});
}

void InfeasibleProblemReport::printLargest(int n)
{
  std::sort(std::begin(mConstraints),
            std::end(mConstraints),
            [](const std::pair<std::string, double>& a,
               const std::pair<std::string, double>& b)
            {
              return a.second > b.second;
            });
  for (int ii = 0; ii < n; ++ii)
    Antares::logs.info() << mConstraints[ii].first << ": " << mConstraints[ii].second;
}

