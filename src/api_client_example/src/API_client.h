
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include <antares/api/SimulationResults.h>
#include <antares/api/solver.h>

Antares::API::SimulationResults solve(std::filesystem::path study_path,
                                      std::filesystem::path output);
