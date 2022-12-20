#include <sstream>
#include <antares/study.h>
#include "filename.h"

std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     int year,
                                     int week,
                                     int optNumber)
{
    std::ostringstream outputFile;
    outputFile << prefix.c_str() << "-"
               << std::to_string(year + 1) << "-"
               << std::to_string(week + 1);

    if (optNumber)
        outputFile << "--optim-nb-" << std::to_string(optNumber);

    outputFile << "." << extension.c_str();

    logs.info() << "Solver output File: `" << outputFile.str() << "'";
    return outputFile.str();
}
