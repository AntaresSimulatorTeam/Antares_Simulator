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

#include <map>
#include <vector>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/study.h>

namespace Antares::Solver::Simulation
{
class yearRandomNumbers
{
public:
    yearRandomNumbers();
    ~yearRandomNumbers() = default;

    void setNbAreas(uint nbAreas);
    void setPowerFluctuations(Data::PowerFluctuations powerFluctuations);
    void reset();

    // General data
    uint pNbAreas;
    Data::PowerFluctuations pPowerFluctuations;

    // Data for thermal noises
    std::vector<std::vector<double>> pThermalNoisesByArea;
    std::vector<size_t> pNbClustersByArea;

    // Data for reservoir levels
    std::vector<double> pReservoirLevels;

    // Data for unsupplied and spilled energy costs
    std::vector<double> pUnsuppliedEnergy;
    std::vector<double> pSpilledEnergy;

    // Hydro costs noises
    std::vector<std::vector<double>> pHydroCostsByArea_freeMod;
    std::vector<double> pHydroCosts_rampingOrExcursion;
};

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
