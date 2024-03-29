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

#include "antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h"
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"
#include <cstdint>
#include <filesystem>
#include <utility>

namespace Antares::Solver::Simulation {
BindingConstraintsTimeSeriesNumbersWriter::BindingConstraintsTimeSeriesNumbersWriter(IResultWriter& writer)
: writer_(writer)
{

}

namespace // anonymous
{
    struct TSNumbersPredicate
    {
        uint32_t operator()(uint32_t value) const
        {
            return value + 1;
        }
    };
} // anonymous namespace

// TODO : remove duplication
static void genericStoreTimeseriesNumbers(Solver::IResultWriter& writer,
                                          const Matrix<uint32_t>& timeseriesNumbers,
                                          const std::string& id,
                                          const std::string& directory)
{
    TSNumbersPredicate predicate;
    std::filesystem::path path = std::filesystem::path() / "ts-numbers" / directory.c_str() / id.c_str();
    path.replace_extension("txt");

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer,
                                   0,         // precision
                                   true,      // print_dimensions
                                   predicate, // predicate
                                   true);     // save even if all coeffs are zero

    writer.addEntryFromBuffer(path.string(), buffer);
}

void BindingConstraintsTimeSeriesNumbersWriter::write(const Data::BindingConstraintGroupRepository &bindingConstraintGroupRepository) {
    for (auto const& group: bindingConstraintGroupRepository) {
        genericStoreTimeseriesNumbers(writer_,
                                      group->timeseriesNumbers,
                                      group->name(),
                                      "bindingconstraints");
    }

}
} // Simulation
