// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/Modeler.h>
#include "antares/solver/modeler/fileWriter/FileWriter.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/simulation/solver.h"
#include "antares/writer/table_format.h"

using namespace Antares;

static void usage()
{
    std::cout << "Usage:\n"
              << "antares-modeler <path/to/study> [--parquet]\n";
}

Writer::TableFormat getTableFormat(int argc, const char** argv)
{
    if (argc <= 2)
    {
        return Writer::TableFormat::CSV;
    }

    std::string parquetOption = argv[2];
    if (parquetOption == "--parquet")
    {
        return Writer::TableFormat::Parquet;
    }

    return Writer::TableFormat::CSV;
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
    Writer::TableFormat tableFormat = getTableFormat(argc, argv);

    logs.info() << "Study path: " << studyPath;

    if (!std::filesystem::is_directory(studyPath))
    {
        logs.error() << "The path provided isn't a valid directory, exiting";
        return EXIT_FAILURE;
    }

    try
    {
        LoadFiles::FileLoader loader(studyPath);
        FileWriter writer(studyPath, tableFormat);
        Modeler modeler(loader, writer);
        modeler.run();
    }
    catch (const LoadFiles::ErrorLoadingYaml& e)
    {
        logs.error() << "Modeler loading error: " << e.what() << "\nExiting simulation.";
        return EXIT_FAILURE;
    }
    catch (const Modeler::ModelerError& e)
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
