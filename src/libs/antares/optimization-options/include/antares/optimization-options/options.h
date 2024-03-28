#pragma once

namespace Antares::Solver::Optimization {
struct OptimizationOptions
{
    //! Force ortools use
    bool ortoolsUsed;
    //! THe solver name, sirius is the default
    std::string ortoolsSolver = "sirius";
    bool presolve = false;
    bool scaling = false;
    bool useBasisOptim1 = false;
    bool useBasisOptim2 = true;
};
}
