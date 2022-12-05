#include <sstream>
#include <antares/study.h>
#include "filename.h"

std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     uint numSpace,
                                     int optNumber)
{
    auto study = Data::Study::Current::Get();
    std::ostringstream outputFile;
    outputFile << prefix.c_str() << "-" // problem ou criterion
               << std::to_string(study->runtime->currentYear[numSpace] + 1) << "-"
               << std::to_string(study->runtime->weekInTheYear[numSpace] + 1);

    if (optNumber)
        outputFile << "--optim-nb-" << std::to_string(optNumber);

    outputFile << "." << extension.c_str();

    logs.info() << "Solver output File: `" << outputFile.str() << "'";
    return outputFile.str();
}
