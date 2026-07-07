// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>

#include <antares/logs/logs.h>
#include "antares/api/modelerProblems.h"
#include "antares/api/singleProblemGetter.h"

namespace fs = std::filesystem;

using namespace Antares::Solver;

int main(const int argc, const char** argv)
{
    if (argc < 2)
    {
        Antares::logs.error() << "Please provide valid study path.";
        Antares::logs.error() << "Usage:" << argv[0] << "/path/to/study";
        return 1;
    }

    const fs::path studyPath(argv[1]);

    const fs::path antaresMarker = studyPath / "study.antares";
    const fs::path modelerMarker = studyPath / "parameters.yml";

    if (fs::exists(antaresMarker))
    {
        Antares::logs.info() << "Loading Antares study...";
        const SingleProblemGetter getter(studyPath);
        getter.printProblems();
    }
    else if (fs::exists(modelerMarker))
    {
        Antares::logs.info() << "Loading Modeler study...";
        const ModelerProblems modelerProblems(studyPath);
        modelerProblems.logSize();
    }
    else
    {
        Antares::logs.error() << "Invalid study path: neither Antares nor Modeler study found.";
        return 1;
    }

    return 0;
}
