// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/utils/filename.h"

#include <sstream>

// ------------------------------------
// Optimization period factory
// ------------------------------------
std::shared_ptr<OptPeriodStringGenerator> createOptPeriodAsString(bool isOptimizationWeekly,
                                                                  unsigned int day,
                                                                  unsigned int week,
                                                                  unsigned int year)
{
    if (isOptimizationWeekly)
    {
        return std::make_shared<OptWeeklyStringGenerator>(week, year);
    }
    else
    {
        return std::make_shared<OptDailyStringGenerator>(day, week, year);
    }
}

std::string createOptimizationFilename(const std::string& title,
                                       const OptPeriodStringGenerator& optPeriodStringGenerator,
                                       unsigned int optNumber,
                                       const std::string& extension)
{
    std::ostringstream outputFile;
    outputFile << title.c_str() << "-";
    outputFile << optPeriodStringGenerator.to_string();
    outputFile << "--optim-nb-" << std::to_string(optNumber);
    outputFile << "." << extension.c_str();

    return outputFile.str();
}

std::string createCriterionFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                    const unsigned int optNumber)
{
    return createOptimizationFilename("criterion", optPeriodStringGenerator, optNumber, "txt");
}

std::string createMPSfilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                              const unsigned int optNumber)
{
    return createOptimizationFilename("problem", optPeriodStringGenerator, optNumber, "mps");
}

std::string createSolutionFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                   const unsigned int optNumber)
{
    return createOptimizationFilename("optimal-values", optPeriodStringGenerator, optNumber, "txt");
}

std::string createMarginalCostFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                       const unsigned int optNumber)
{
    return createOptimizationFilename("marginal-costs", optPeriodStringGenerator, optNumber, "txt");
}

std::string createReducedCostFilename(const OptPeriodStringGenerator& optPeriodStringGenerator,
                                      const unsigned int optNumber)
{
    return createOptimizationFilename("reduced-costs", optPeriodStringGenerator, optNumber, "txt");
}
