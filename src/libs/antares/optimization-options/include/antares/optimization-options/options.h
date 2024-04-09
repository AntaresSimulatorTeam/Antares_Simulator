#pragma once

namespace Antares::Solver::Optimization {
struct OptimizationOptions
{
    //! Force ortools use
    bool ortoolsUsed = false;
    //! THe solver name, sirius is the default
    std::string ortoolsSolver = "sirius";
    bool solverLogs = false;
    std::string solverParameters = "";
};
}
