#include "solver.utils.h"
#include <yuni/yuni.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <yuni/io/file.h>

#define SEP Yuni::IO::Separator

static const std::string systemCostFilename = "annualSystemCost.txt";
static const std::string criterionsCostsFilename = "checkIntegrity.txt";
static const std::string optimizationTimeFilename
  = std::string("optimization") + SEP + "solve-durations.txt";
static const std::string updateTimeFilename
  = std::string("optimization") + SEP + "update-durations.txt";

static std::string to_scientific(const double d)
{
    std::ostringstream stream;
    stream << std::scientific;
    stream << std::setprecision(14);
    stream << d;

    return stream.str();
}

namespace Antares::Solver::Simulation
{
// costStatistics
costStatistics::costStatistics() :
 costAverage(0.),
 costStdDeviation(0.),
 costMin(std::numeric_limits<double>::max()),
 costMax(0.),
 nbPerformedYears(0)
{
}

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

void annualCostsStatistics::writeToOutput(IResultWriter::Ptr writer)
{
    if (!writer)
        return;

    writeSystemCostToOutput(writer);
    writeCriterionCostsToOutput(writer);
    writeOptimizationTimeToOutput(writer);
    writeUpdateTimes(writer);
}

void annualCostsStatistics::writeSystemCostToOutput(IResultWriter::Ptr writer)
{
    Yuni::Clob buffer;
    buffer << "EXP : " << round_to_closer_int(systemCost.costAverage) << "\n";
    buffer << "STD : " << round_to_closer_int(systemCost.costStdDeviation) << "\n";
    buffer << "MIN : " << round_to_closer_int(systemCost.costMin) << "\n";
    buffer << "MAX : " << round_to_closer_int(systemCost.costMax) << "\n";

    writer->addEntryFromBuffer(systemCostFilename, buffer);
}

void annualCostsStatistics::writeCriterionCostsToOutput(IResultWriter::Ptr writer)
{
    Yuni::Clob buffer;
    buffer << to_scientific(criterionCost1.costAverage) << "\n";
    buffer << to_scientific(criterionCost1.costStdDeviation) << "\n";
    buffer << to_scientific(criterionCost1.costMin) << "\n";
    buffer << to_scientific(criterionCost1.costMax) << "\n";

    buffer << to_scientific(criterionCost2.costAverage) << "\n";
    buffer << to_scientific(criterionCost2.costStdDeviation) << "\n";
    buffer << to_scientific(criterionCost2.costMin) << "\n";
    buffer << to_scientific(criterionCost2.costMax) << "\n";

    writer->addEntryFromBuffer(criterionsCostsFilename, buffer);
}

void annualCostsStatistics::writeUpdateTimes(IResultWriter::Ptr writer)
{
    Yuni::Clob buffer;
    buffer << "EXP (ms) : " << updateTime.costAverage << "\n";
    buffer << "STD (ms) : " << updateTime.costStdDeviation << "\n";
    buffer << "MIN (ms) : " << updateTime.costMin << "\n";
    buffer << "MAX (ms) : " << updateTime.costMax << "\n";

    writer->addEntryFromBuffer(updateTimeFilename, buffer);
}

void annualCostsStatistics::writeOptimizationTimeToOutput(IResultWriter::Ptr writer)
{
    Yuni::Clob buffer;
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

    writer->addEntryFromBuffer(optimizationTimeFilename, buffer);
}
std::string annualCostsStatistics::round_to_closer_int(const double d)
{
#ifdef YUNI_OS_MSVC
    ::sprintf_s(conversionBuffer, sizeof(conversionBuffer), "%.0f", d);
#else
    ::snprintf(conversionBuffer, sizeof(conversionBuffer), "%.0f", d);
#endif
    std::string rnd(conversionBuffer);
    return rnd;
}
} // namespace Antares::Solver::Simulation
