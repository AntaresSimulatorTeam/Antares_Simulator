/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
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
class FinalLevelValidator
{
public:
    FinalLevelValidator(Antares::Data::PartHydro& hydro,
                        unsigned int areaIndex,
                        const Antares::Data::AreaName areaName,
                        double initialLevel,
                        double finalLevel,
                        const unsigned int year,
                        const unsigned int lastSimulationDay,
                        const unsigned int firstMonthOfSimulation,
                        HydroErrorsCollector& errorCollector);
    bool check();
    bool finalLevelFineForUse();

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
    Antares::Data::PartHydro& hydro_;
    unsigned int areaIndex_;
    const Antares::Data::AreaName areaName_;
    double initialLevel_;
    double finalLevel_;

    bool finalLevelFineForUse_ = false;

    // area input errors
    HydroErrorsCollector& errorCollector_;
};
} // namespace Solver
} // namespace Antares
