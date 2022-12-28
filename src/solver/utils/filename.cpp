#include <sstream>
#include <antares/logs/logs.h>
#include "filename.h"

using namespace Antares;

std::string getFilenameWithExtension(const std::string& prefix,
                                     const std::string& extension,
                                     unsigned int year,
                                     unsigned int week,
                                     unsigned int optNumber)
{
    std::ostringstream outputFile;
    outputFile << prefix.c_str() << "-"
               << std::to_string(year + 1) << "-"
               << std::to_string(week + 1);

    outputFile << "--optim-nb-" << std::to_string(optNumber);

    outputFile << "." << extension.c_str();

    logs.info() << "Solver output File: `" << outputFile.str() << "'";
    return outputFile.str();
}
