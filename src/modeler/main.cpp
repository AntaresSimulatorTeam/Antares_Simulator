// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/Modeler.h>
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/solver/simulation/solver.h"

#include "FileWriter.h"

using namespace Antares;

static void usage()
{
    std::cout << "Usage:\n"
              << "antares-modeler <path/to/study>\n";
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

    std::filesystem::path studyPath(argv[1]);
    logs.info() << "Study path: " << studyPath;

    if (!std::filesystem::is_directory(studyPath))
    {
        logs.error() << "The path provided isn't a valid directory, exiting";
        return EXIT_FAILURE;
    }

    try
    {
        LoadFiles::FileLoader loader(studyPath);
        Antares::Modeler::FileWriter writer(studyPath);
        Antares::Solver::Modeler modeler(loader, writer);
        modeler.solve();
    }
    catch (const Antares::Solver::Modeler::ModelerError& e)
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
