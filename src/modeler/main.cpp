// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/Modeler.h>
#include "antares/solver/modeler/fileWriter/FileWriter.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/solver/simulation/solver.h"

using namespace Antares;

static void usage()
{
    std::cout << "Usage:\n"
              << "antares-modeler <path/to/study> [--parquet]\n";
}

bool parquetFormatRequired(int argc, const char** argv)
{
    std::string parquetOption = argv[2];
    if (argc == 3 && parquetOption == "--parquet")
    {
        return true;
    }
    return false;
}

int main(int argc, const char** argv)
{
    logs.applicationName("modeler");
    if (argc <= 1)
    {
        logs.error() << "No study path provided, exiting.";
        usage();
        return EXIT_FAILURE;
    }

    // Options parsing
    std::filesystem::path studyPath(argv[1]);
    bool parquetFormat = parquetFormatRequired(argc, argv);

    logs.info() << "Study path: " << studyPath;

    if (!std::filesystem::is_directory(studyPath))
    {
        logs.error() << "The path provided isn't a valid directory, exiting";
        return EXIT_FAILURE;
    }

    try
    {
        LoadFiles::FileLoader loader(studyPath);
        Solver::FileWriter writer(studyPath, parquetFormat);
        Solver::Modeler modeler(loader, writer);
        modeler.run();
    }
    catch (const Solver::Modeler::ModelerError& e)
    {
        logs.error() << "Modeler error: " << e.what() << "\nExiting simulation.";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        logs.error() << e.what() << "\nError during the execution, exiting";
        return EXIT_FAILURE;
    }

    return 0;
}
