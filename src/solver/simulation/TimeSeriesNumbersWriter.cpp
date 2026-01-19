// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 17/03/23.
//

#include <filesystem>
#include <utility>

#include "antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h"
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"

namespace Antares::Solver::Simulation
{
BindingConstraintsTimeSeriesNumbersWriter::BindingConstraintsTimeSeriesNumbersWriter(
  IResultWriter& writer):
    writer_(writer)
{
}

// TODO : remove duplication
static void genericStoreTimeseriesNumbers(Solver::IResultWriter& writer,
                                          const Data::TimeSeriesNumbers& timeseriesNumbers,
                                          const std::string& id,
                                          const std::string& directory)
{
    std::filesystem::path path = std::filesystem::path() / "ts-numbers" / directory.c_str()
                                 / id.c_str();
    path.replace_extension("txt");

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer);
    writer.addEntryFromBuffer(path, buffer);
}

void BindingConstraintsTimeSeriesNumbersWriter::write(
  const Data::BindingConstraintGroupRepository& bindingConstraintGroupRepository)
{
    for (const auto& group: bindingConstraintGroupRepository)
    {
        genericStoreTimeseriesNumbers(writer_,
                                      group->timeseriesNumbers,
                                      group->name(),
                                      "bindingconstraints");
    }
}
} // namespace Antares::Solver::Simulation
