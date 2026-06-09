// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
