#pragma once

namespace Antares::Solver::Optimization {
struct OptimizationOptions
{
    bool useOrtools = false;
    std::string solverName = "sirius";
    bool presolve = false;
    bool scaling = false;
    bool useBasisOptim1 = false;
    bool useBasisOptim2 = true;
};
}
