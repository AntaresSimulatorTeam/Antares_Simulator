// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "antares/solver/modeler/parameters/scenarioScope.h"

namespace Antares::Solver
{
struct ModelerParameters
{
    // OR-Tools solver to be used for the simulation
    std::string solver;
    // Display solver logs ON/OFF
    bool solverLogs = false;
    // Specific solver parameters
    std::string solverParameters;
    // Write output results
    bool noOutput = false;
    // Write output results
    bool exportMps = false;
    // time steps
    unsigned int firstTimeStep;
    unsigned int lastTimeStep;
    // Monte-Carlo scenarios to simulate
    ScenarioScope scenarioScope;
};
} // namespace Antares::Solver
