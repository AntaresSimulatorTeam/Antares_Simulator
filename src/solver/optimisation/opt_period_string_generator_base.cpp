#include "opt_period_string_generator_base.h"
#include <sstream>

std::string OptPeriodStringGenerator::createOptimizationFilename(
  const std::string& title,
  unsigned int optNumber,
  const std::optional<unsigned int> adqPatchOptNumber,
  const std::string& extension) const
{
    std::ostringstream outputFile;
    outputFile << title.c_str() << "-";
    outputFile << this->to_string();
    outputFile << "--optim-nb-";

    // Special case: Local matching rule (LMR) in adequacy patch performs 2 weekly optimizations
    if (adqPatchOptNumber)
        outputFile << std::to_string(*adqPatchOptNumber) << "-";

    outputFile << std::to_string(optNumber);
    outputFile << "." << extension.c_str();
    return outputFile.str();
}
