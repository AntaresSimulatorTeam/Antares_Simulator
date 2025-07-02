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

#include "include/antares/optimisation/linear-problem-data-impl/Scenario.h"

#include "antares/exception/RuntimeError.hpp"

#include "fmt/format.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
LinearProblemApi::IScenario::Chronicle Scenario::getData(Year year) const
{
    auto it = chronicleData_.find(year);
    if (it == chronicleData_.end())
    {
        throw Error::RuntimeError(
          fmt::format("In scenario group '{}', chronicle for year {} does not exist.",
                      group(),
                      year));
    }
    return it->second;
}

void Scenario::setChronicle(Year year, Chronicle chronicle)
{
    if (chronicleData_.contains(year))
    {
        throw Error::RuntimeError("Chronicle for year " + std::to_string(year)
                                  + " already exists.");
    }
    chronicleData_[year] = chronicle;
}
} // namespace Antares::Optimisation::LinearProblemDataImpl
