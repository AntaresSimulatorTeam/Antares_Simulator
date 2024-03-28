#pragma once

namespace Antares::Solver::Optimization {
struct OptimizationOptions
{
    //! Force ortools use
    bool ortoolsUsed;
    //! THe solver name, sirius is the default
    std::string ortoolsSolver = "sirius";
    std::string solver_parameters;
    bool useBasisOptim1 = false;
    bool useBasisOptim2 = true;
};
}
