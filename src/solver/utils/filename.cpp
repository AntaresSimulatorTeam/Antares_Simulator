#include <sstream>
#include "filename.h"
#include "opt-period-as-string.h"

// ------------------------------------
// Optimization period factory
// ------------------------------------
std::shared_ptr<optPeriodAsString> createOptPeriodAsString(bool isOptimizationWeekly,
                                                    unsigned int day,
                                                    unsigned int week,
                                                    unsigned int year)
{
    if (isOptimizationWeekly)
        return std::make_shared<optWeeklyAsString>(week, year);
    else
        return std::make_shared<optDailyAsString>(day, week, year);

}


std::string createOptimizationFilename(const std::string& title,
                                       std::shared_ptr<optPeriodAsString> opt_period_as_string,
                                       unsigned int optNumber,
                                       const std::string& extension)
{
    std::ostringstream outputFile;
    outputFile << title.c_str() << "-";
    outputFile << opt_period_as_string->to_string();
    outputFile << "--optim-nb-" << std::to_string(optNumber);
    outputFile << "." << extension.c_str();

    return outputFile.str();
}

std::string createCriterionFilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber)
{
    return createOptimizationFilename("criterion", opt_period_as_string, optNumber, "txt");
}

std::string createMPSfilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber)
{
    return createOptimizationFilename("problem", opt_period_as_string, optNumber, "mps");
}
