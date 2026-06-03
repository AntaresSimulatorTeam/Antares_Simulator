// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <fstream>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/Modeler.h>
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/simulation/solver.h"
#include "antares/writer/table_format.h"

using namespace Antares;
using namespace Antares::Writer;
namespace fs = std::filesystem;

static void usage()
{
    std::cout << "Usage:\n"
              << "antares-modeler <path/to/study> [--parquet]\n";
}

TableFormat getTableFormat(int argc, const char** argv)
{
    if (argc <= 2)
    {
        return TableFormat::CSV;
    }

    std::string parquetOption = argv[2];
    if (parquetOption == "--parquet")
    {
        return TableFormat::Parquet;
    }

    return TableFormat::CSV;
}

fs::path makeOutputPath(fs::path studyPath)
{
    const auto simulationId = formatTime(getCurrentTime(), "%Y%m%d-%H%M");
    fs::path outputPath = studyPath / "output" / simulationId;

    // avoid overwriting existing output by adding a suffix (-2, -3, etc.)
    if (!Utils::generatePathWithSuffix(outputPath))
    {
        throw Modeler::ModelerError("Output folder already exists: " + outputPath.string());
    }

    logs.info() << "Output folder : " << outputPath;
    if (!fs::is_directory(outputPath) && !fs::create_directories(outputPath))
    {
        throw Modeler::ModelerError("Failed to create output directory. Exiting simulation.");
    }
    return outputPath;
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
    fs::path studyPath(argv[1]);
    TableFormat tableFormat = getTableFormat(argc, argv);

    logs.info() << "Study path: " << studyPath;

    if (!fs::is_directory(studyPath))
    {
        logs.error() << "The path provided isn't a valid directory, exiting";
        return EXIT_FAILURE;
    }

    try
    {
        LoadFiles::FileLoader loader(studyPath);
        fs::path outputPath = makeOutputPath(studyPath);
        Modeler modeler(loader, outputPath, tableFormat);
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
