/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <antares/modeler/data.h>
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace Antares::Solver::LoadFiles
{

Study::SystemModel::Data loadAll(const fs::path& studyPath)
{
    try
    {
        Study::SystemModel::Data data;
        const auto libraries = LoadFiles::loadLibraries(studyPath);
        logs.info() << "Libraries loaded";
        const auto system = LoadFiles::loadSystem(studyPath, libraries);
        logs.info() << "System loaded";
        auto dataSeries = LoadFiles::loadDataSeries(studyPath);
        return Study::SystemModel::Data {libraries, system, dataSeries};
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        logs.error() << "Error while loading files, exiting";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        logs.error() << e.what();
        logs.error() << "Error during the execution, exiting";
        return EXIT_FAILURE;
    }
}

} // namespace Antares::Solver::LoadFiles
