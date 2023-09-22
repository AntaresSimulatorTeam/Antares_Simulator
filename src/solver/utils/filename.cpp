#include <sstream>
#include "filename.h"
#include "opt_period_string_generator.h"

// ------------------------------------
// Optimization period factory
// ------------------------------------
std::shared_ptr<OptPeriodStringGenerator> createOptPeriodAsString(bool isOptimizationWeekly,
                                                                  unsigned int day,
                                                                  unsigned int week,
                                                                  unsigned int year)
{
    if (isOptimizationWeekly)
        return std::make_shared<OptWeeklyStringGenerator>(week, year);
    else
        return std::make_shared<OptDailyStringGenerator>(day, week, year);
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
