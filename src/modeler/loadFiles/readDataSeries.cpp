/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace Antares::Solver::LoadFiles
{
std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> loadDataSeries(
  const std::filesystem::path& studyPath)
{
    try
    {
        Optimisation::LinearProblemDataImpl::DataSeriesRepository dataSeriesRepository = IO::
          Inputs::DataSeriesCsvImporter::DataSeriesRepoImporter::importFromDirectory(
            studyPath / "input" / "data-series",
            '\t');
        logs.info() << "Data-series loaded";
        return std::make_unique<Optimisation::LinearProblemDataImpl::LinearProblemData>(
          std::move(dataSeriesRepository));
    }
    catch (const std::exception& e)
    {
        // data-series are not mandatory
        logs.warning() << "Data-series could not be loaded: " << e.what();
        return std::make_unique<Optimisation::LinearProblemDataImpl::LinearProblemData>();
    }
}
} // namespace Antares::Solver::LoadFiles
