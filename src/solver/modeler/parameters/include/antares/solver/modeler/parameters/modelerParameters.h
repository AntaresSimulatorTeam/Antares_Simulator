#pragma once

#include <string>

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
};
} // namespace Antares::Solver
