#include <antares/study.h>
#include "filename.h"

std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     uint numSpace,
                                     int optNumber)
{
    auto study = Data::Study::Current::Get();
    std::string outputFile;
    outputFile.append(prefix.c_str())
      .append("-") // problem ou criterion
      .append(std::to_string(study->runtime->currentYear[numSpace] + 1))
      .append("-")
      .append(std::to_string(study->runtime->weekInTheYear[numSpace] + 1));

    if (optNumber)
        outputFile.append("--optim-nb-").append(std::to_string(optNumber));

    outputFile.append(".").append(extension.c_str());

    logs.info() << "Solver output File: `" << outputFile << "'";
    return outputFile;
}
