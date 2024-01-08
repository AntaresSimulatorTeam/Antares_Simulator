#include "solver_utils.h"
#include <yuni/yuni.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <yuni/io/file.h>
#include <cmath>

#define SEP Yuni::IO::Separator

static const std::string systemCostFilename = "annualSystemCost.txt";
static const std::string criterionsCostsFilename = "checkIntegrity.txt";
static const std::string optimizationTimeFilename
  = std::string("optimization") + SEP + "solve-durations.txt";
static const std::string updateTimeFilename
  = std::string("optimization") + SEP + "update-durations.txt";

static std::ostream& toScientific(std::ostream& os)
{
    return os << std::scientific << std::setprecision(14);
}

namespace Antares::Solver::Simulation
{

void costStatistics::setNbPerformedYears(uint n)
{
    assert(n);
    nbPerformedYears = n;
}

void costStatistics::addCost(const double cost)
{
    // Average
    costAverage += cost / nbPerformedYears;

    // Standard deviation
    costStdDeviation += cost * cost / nbPerformedYears;

    // Min and Max
    if (cost < costMin)
        costMin = cost;
    if (cost > costMax)
        costMax = cost;
}

void costStatistics::endStandardDeviation()
{
    costStdDeviation = std::sqrt(costStdDeviation - costAverage * costAverage);
}

// annualCostsStatistics
annualCostsStatistics::annualCostsStatistics() = default;

void annualCostsStatistics::setNbPerformedYears(uint n)
{
    systemCost.setNbPerformedYears(n);
    criterionCost1.setNbPerformedYears(n);
    criterionCost2.setNbPerformedYears(n);
    optimizationTime1.setNbPerformedYears(n);
    optimizationTime2.setNbPerformedYears(n);
    updateTime.setNbPerformedYears(n);
};

void annualCostsStatistics::endStandardDeviations()
{
    systemCost.endStandardDeviation();
    criterionCost1.endStandardDeviation();
    criterionCost2.endStandardDeviation();
    optimizationTime1.endStandardDeviation();
    optimizationTime2.endStandardDeviation();
    updateTime.endStandardDeviation();
};

void annualCostsStatistics::writeToOutput(IResultWriter& writer)
{
    writeSystemCostToOutput(writer);
    writeCriterionCostsToOutput(writer);
    writeOptimizationTimeToOutput(writer);
    writeUpdateTimes(writer);
}

void annualCostsStatistics::writeSystemCostToOutput(IResultWriter& writer)
{
    std::ostringstream buffer;
    buffer << "EXP : " << std::round(systemCost.costAverage) << '\n';
    buffer << "STD : " << std::round(systemCost.costStdDeviation) << '\n';
    buffer << "MIN : " << std::round(systemCost.costMin) << '\n';
    buffer << "MAX : " << std::round(systemCost.costMax) << '\n';

    std::string s = buffer.str();
    writer.addEntryFromBuffer(systemCostFilename, s);
}

void annualCostsStatistics::writeCriterionCostsToOutput(IResultWriter& writer) const
{
    using std::endl;
    std::ostringstream buffer;
    buffer << toScientific
           << criterionCost1.costAverage << endl
           << criterionCost1.costStdDeviation << endl
           << criterionCost1.costMin << endl
           << criterionCost1.costMax << endl
           << criterionCost2.costAverage << endl
           << criterionCost2.costStdDeviation << endl
           << criterionCost2.costMin << endl
           << criterionCost2.costMax << endl;

    std::string s = buffer.str(); // TODO allow std::string&& in addEntryFromBuffer
    writer.addEntryFromBuffer(criterionsCostsFilename, s);
}

void annualCostsStatistics::writeUpdateTimes(IResultWriter& writer) const
{
    std::ostringstream buffer;
    buffer << "EXP (ms) : " << updateTime.costAverage << "\n";
    buffer << "STD (ms) : " << updateTime.costStdDeviation << "\n";
    buffer << "MIN (ms) : " << updateTime.costMin << "\n";
    buffer << "MAX (ms) : " << updateTime.costMax << "\n";

    std::string s = buffer.str();
    writer.addEntryFromBuffer(updateTimeFilename, s);
}

void annualCostsStatistics::writeOptimizationTimeToOutput(IResultWriter& writer) const
{
    std::ostringstream buffer;
    buffer << "First optimization :\n";
    buffer << "EXP (ms) : " << optimizationTime1.costAverage << "\n";
    buffer << "STD (ms) : " << optimizationTime1.costStdDeviation << "\n";
    buffer << "MIN (ms) : " << optimizationTime1.costMin << "\n";
    buffer << "MAX (ms) : " << optimizationTime1.costMax << "\n";

    buffer << "Second optimization :\n";
    buffer << "EXP (ms) : " << optimizationTime2.costAverage << "\n";
    buffer << "STD (ms) : " << optimizationTime2.costStdDeviation << "\n";
    buffer << "MIN (ms) : " << optimizationTime2.costMin << "\n";
    buffer << "MAX (ms) : " << optimizationTime2.costMax << "\n";

    std::string s = buffer.str();
    writer.addEntryFromBuffer(optimizationTimeFilename, s);
}

} // namespace Antares::Solver::Simulation
