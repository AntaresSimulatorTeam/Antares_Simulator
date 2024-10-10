/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/simulation/solver_utils.h"

#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace fs = std::filesystem;

static const std::string systemCostFilename = "annualSystemCost.txt";
static const std::string criterionsCostsFilename = "checkIntegrity.txt";
static const fs::path optimizationTimeFilename = fs::path("optimization") / "solve-durations.txt";
static const fs::path updateTimeFilename = fs::path("optimization") / "update-durations.txt";

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
    {
        costMin = cost;
    }
    if (cost > costMax)
    {
        costMax = cost;
    }
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
}

void annualCostsStatistics::endStandardDeviations()
{
    systemCost.endStandardDeviation();
    criterionCost1.endStandardDeviation();
    criterionCost2.endStandardDeviation();
    optimizationTime1.endStandardDeviation();
    optimizationTime2.endStandardDeviation();
    updateTime.endStandardDeviation();
}

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
    buffer << toScientific << criterionCost1.costAverage << endl
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
    writer.addEntryFromBuffer(updateTimeFilename.string(), s);
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
    writer.addEntryFromBuffer(optimizationTimeFilename.string(), s);
}

} // namespace Antares::Solver::Simulation
