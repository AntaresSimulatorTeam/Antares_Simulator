// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "API_client.h"

#include <utility>

Antares::API::SimulationResults solve(std::filesystem::path study_path,
                                      std::filesystem::path output)
{
    return Antares::API::PerformSimulation(std::move(study_path), std::move(output), {});
}
