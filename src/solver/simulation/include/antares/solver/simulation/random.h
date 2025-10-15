/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/solver/simulation/solver_utils.h>
#include <antares/study/study.h>

namespace Antares::Solver::Simulation
{
class randomNumbers
{
public:
    randomNumbers(uint maxNbPerformedYearsInAset, Data::PowerFluctuations powerFluctuations);
    ~randomNumbers() = default;

    void allocate(const Antares::Data::Study& study);
    void compute(Antares::Data::Study& study,
                 unsigned years,
                 std::map<unsigned int, bool>& isYearPerformed,
                 MersenneTwister& randomHydro);
    void reset();

    std::vector<yearRandomNumbers> pYears;

    // Associates :
    //		year number (0, ..., total nb of years to compute - 1) --> index of the year's space
    //(0,
    //..., max nb of parallel years - 1)
    std::map<uint, uint> yearNumberToIndex;

private:
    uint pMaxNbPerformedYears;
};
} // namespace Antares::Solver::Simulation
