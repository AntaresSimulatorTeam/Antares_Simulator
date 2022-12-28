#include <sstream>
#include <antares/logs/logs.h>
#include "filename.h"

using namespace Antares;


// ------------------------------------
// Daily optimization
// ------------------------------------
optDaily::optDaily(unsigned int day, unsigned int week, unsigned int year) :
    day_(day), week_(week), year_(year)
{}

std::string optDaily::to_string() const
{
    std::string to_return = std::to_string(day_ + 1) + "-";
    to_return += std::to_string(week_ + 1) + "-";
    to_return += std::to_string(year_ + 1);
    return to_return;
}


// ------------------------------------
// Weekly optimization
// ------------------------------------
optWeekly::optWeekly(unsigned int week, unsigned int year) :
    week_(week), year_(year)
{}

std::string optWeekly::to_string() const
{
    std::string to_return = std::to_string(week_ + 1) + "-";
    to_return += std::to_string(year_ + 1);
    return to_return;
}


// ------------------------------------
// Optimization period factory
// ------------------------------------
std::shared_ptr<optPeriod> createOptimizationPeriod(bool isOptimizationWeekly,
                                                    unsigned int day,
                                                    unsigned int week,
                                                    unsigned int year)
{
    if (isOptimizationWeekly)
        return std::make_shared<optWeekly>(week, year);
    else
        return std::make_shared<optDaily>(day, week, year);

}


std::string createOptimizationFilename(const std::string& title,
                                       std::shared_ptr<optPeriod> opt_period,
                                       unsigned int optNumber,
                                       const std::string& extension)
{
    std::ostringstream outputFile;
    outputFile << title.c_str() << "-";
    outputFile << opt_period->to_string();
    outputFile << "--optim-nb-" << std::to_string(optNumber);
    outputFile << "." << extension.c_str();

    logs.info() << "Solver output File: `" << outputFile.str() << "'";
    return outputFile.str();
}

std::string createCriterionFilename(std::shared_ptr<optPeriod> opt_period, const unsigned int optNumber)
{
    return createOptimizationFilename("criterion", opt_period, optNumber, "txt");
}

std::string createMPSfilename(std::shared_ptr<optPeriod> opt_period, const unsigned int optNumber)
{
    return createOptimizationFilename("problem", opt_period, optNumber, "mps");
}