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
#pragma once
#include <optional>
#include <string>

#include "antares/optimisation/linear-problem-api/hasStatus.h"

inline std::string StatusToString(
  const std::optional<Antares::Optimisation::LinearProblemApi::MipBasisStatus>& status)
{
    using namespace Antares::Optimisation::LinearProblemApi;
    // TODO shorten returns
    if (status.has_value())
    {
        switch (*status)
        {
        case MipBasisStatus::FREE:
            return "Free";
        case MipBasisStatus::AT_LOWER_BOUND:
            return "At lower bound";
        case MipBasisStatus::AT_UPPER_BOUND:
            return "At upper bound";
        case MipBasisStatus::FIXED_VALUE:
            return "Fixed value";
        case MipBasisStatus::BASIC:
            return "Basic";
        case MipBasisStatus::NOT_AVAILABLE:
        default:
            return "None";
        }
    }
    else
    {
        return "None";
    }
}

struct SimulationTableEntry
{
    unsigned int block;
    std::string component;
    std::string output;
    std::optional<unsigned int> absolute_time_index;
    std::optional<unsigned int> block_time_index;
    std::optional<unsigned int> scenario_index;
    std::optional<double> value;
    Antares::Optimisation::LinearProblemApi::MipBasisStatus status;
};

