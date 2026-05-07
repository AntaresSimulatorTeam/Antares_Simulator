// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <optional>
#include <string>

#include "antares/optimisation/linear-problem-api/hasStatus.h"

namespace Antares::IO::Outputs
{
struct SimulationTableEntry
{
    unsigned block;
    std::optional<std::string> component;
    std::string output;
    std::optional<unsigned> absolute_time_index;
    std::optional<unsigned> block_time_index;
    unsigned scenario_index;
    std::optional<double> value;
    std::optional<Antares::Optimisation::LinearProblemApi::MipBasisStatus> status;
};
} // namespace Antares::IO::Outputs
