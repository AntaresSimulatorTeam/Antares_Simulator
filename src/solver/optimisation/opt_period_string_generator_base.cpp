#include "opt_period_string_generator_base.h"
#include <sstream>

std::string OptPeriodStringGenerator::createOptimizationFilename(const std::string& title,
                                                                 unsigned int optNumber,
                                                                 const std::string& extension) const
{
    std::ostringstream outputFile;
    outputFile << title.c_str() << "-";
    outputFile << this->to_string();
    outputFile << "--optim-nb-" << std::to_string(optNumber);
    outputFile << "." << extension.c_str();

    return outputFile.str();
}
