// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include <antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h>

/**
 * Reads a DataSeriesRepo from a directory
 * Every csv file found represents a DataSeries, its id will be the name of the file
 */
namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
class DataSeriesRepoImporter final
{
public:
    DataSeriesRepoImporter() = delete; // must not be used
    static Optimisation::LinearProblemDataImpl::DataSeriesRepository importFromDirectory(
      const std::filesystem::path&,
      char csvSeparator = '\t');
};

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
