/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
    writer.addEntryFromBuffer(path.string(), buffer);
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
