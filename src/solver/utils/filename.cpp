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

std::string createCriterionFilename(OptPeriodStringGenerator& optPeriodStringGenerator,
                                    const unsigned int optNumber)
{
    return optPeriodStringGenerator.createOptimizationFilename("criterion", optNumber, "txt");
}

std::string createMPSfilename(OptPeriodStringGenerator& optPeriodStringGenerator,
                              const unsigned int optNumber)
{
    return optPeriodStringGenerator.createOptimizationFilename("problem", optNumber, "mps");
}
// TODO[FOM] Remove this function
std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     uint year,
                                     uint week)
{
    std::ostringstream outputFile;
    outputFile << prefix.c_str() << "-" << std::to_string(year + 1) << "-"
               << std::to_string(week + 1);

    outputFile << "." << extension.c_str();
    return outputFile.str();
}
