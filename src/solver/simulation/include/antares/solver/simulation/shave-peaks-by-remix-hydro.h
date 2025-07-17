
#pragma once

#include <tuple>
#include <vector>

namespace Antares::Solver::Simulation
{

struct RemixHydroOutput
{
    std::vector<double> HydroGen;
    std::vector<double> UnsupE;
    std::vector<double> levels;

    // Allow std::tie(a, b, c) = remixHydroOutput;
    inline operator std::tuple<std::vector<double>&, std::vector<double>&, std::vector<double>&>()

    {
        return {HydroGen, UnsupE, levels};
    }
};

// class ExchangeBounds
//{
// public:
//     virtual double atPeak(unsigned hourPeak) = 0;
//     virtual double atBottom(unsigned hourPeak) = 0;
// };
//
// std::shared_ptr<ExchangeBounds> ExchangeBoundsFactory()
//{
// }

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<double>& HydroGen,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           const double initialLevel,
                                           const double capacity,
                                           const double efficiency,
                                           const bool reservoirManagement,
                                           const std::vector<double>& inflow,
                                           const std::vector<double>& overflow,
                                           const std::vector<double>& pump,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG);

} // namespace Antares::Solver::Simulation
