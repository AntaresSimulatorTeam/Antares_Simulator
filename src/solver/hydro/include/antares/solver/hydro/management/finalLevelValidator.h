// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "antares/solver/hydro/management/HydroErrorsCollector.h"
#include "antares/study/parts/hydro/container.h"

namespace Antares
{
namespace Data
{
class PartHydro;
}

namespace Solver
{
class FinalLevelValidator final
{
public:
    FinalLevelValidator(const Antares::Data::PartHydro& hydro,
                        const Antares::Data::AreaName areaName,
                        double initialLevel,
                        double finalLevel,
                        const unsigned int year,
                        const unsigned int lastSimulationDay,
                        const unsigned int firstMonthOfSimulation,
                        HydroErrorsCollector& errorCollector);
    bool check();
    bool finalLevelFineForUse() const;

private:
    bool wasSetInScenarioBuilder();
    bool compatibleWithReservoirProperties();
    bool skippingFinalLevelUse();
    bool checkForInfeasibility();
    bool hydroAllocationStartMatchesSimulation() const;
    bool isFinalLevelReachable() const;
    double calculateTotalInflows() const;
    bool isBetweenRuleCurves() const;

    // Data from simulation
    unsigned int year_ = 0;
    unsigned int lastSimulationDay_ = 0;
    unsigned int firstMonthOfSimulation_ = 0;

    // Data from area
    const Antares::Data::PartHydro& hydro_;
    const Antares::Data::AreaName areaName_;
    double initialLevel_;
    double finalLevel_;

    bool finalLevelFineForUse_ = false;

    // area input errors
    HydroErrorsCollector& errorCollector_;
};
} // namespace Solver
} // namespace Antares
