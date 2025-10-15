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

#include "antares/solver/simulation/random.h"

#include "antares/solver/hydro/management/management.h"

namespace Antares::Solver::Simulation
{
// Class representing a hydro cost noise.
// This class allows sorting hydro costs noises into increasing absolute values order
// when instances are contained in a :
//		std::set<hydroCostNoise, compareHydroCostsNoises>
class hydroCostNoise
{
public:
    hydroCostNoise(double v, uint i):
        value(v),
        index(i)
    {
    }

    inline double getValue() const
    {
        return value;
    }

    inline uint getIndex() const
    {
        return index;
    }

    double value;
    uint index;
};

// Used to sort the hydro costs noises into increasing absolute values order in a std::set
struct compareHydroCostsNoises
{
    inline bool operator()(const hydroCostNoise& hcnr1, const hydroCostNoise& hcnr2) const
    {
        return std::abs(hcnr1.getValue()) < std::abs(hcnr2.getValue());
    }
};

yearRandomNumbers::yearRandomNumbers():
    pNbAreas(0)
{
}

void yearRandomNumbers::setNbAreas(uint nbAreas)
{
    pNbAreas = nbAreas;
}

void yearRandomNumbers::setPowerFluctuations(Data::PowerFluctuations powerFluctuations)
{
    pPowerFluctuations = powerFluctuations;
}

void yearRandomNumbers::reset()
{
    // Thermal noises
    for (uint a = 0; a != pNbAreas; a++)
    {
        pThermalNoisesByArea[a].assign(pThermalNoisesByArea[a].size(), 0);
    }

    // General
    pNbClustersByArea.assign(pNbAreas, 0);

    // Reservoir levels, spilled and unsupplied energy costs
    pReservoirLevels.assign(pNbAreas, 0);
    pUnsuppliedEnergy.assign(pNbAreas, 0);
    pSpilledEnergy.assign(pNbAreas, 0);

    // Hydro costs noises
    switch (pPowerFluctuations)
    {
    case Data::lssFreeModulations:
    {
        for (uint a = 0; a != pNbAreas; a++)
        {
            pHydroCostsByArea_freeMod[a].assign(8784, 0);
        }
        break;
    }

    case Data::lssMinimizeRamping:
    case Data::lssMinimizeExcursions:
    {
        pHydroCosts_rampingOrExcursion.assign(pNbAreas, 0);
        break;
    }

    case Data::lssUnknown:
        break;
    }
}

randomNumbers::randomNumbers(uint maxNbPerformedYearsInAset,
                             Data::PowerFluctuations powerFluctuations):
    pMaxNbPerformedYears(maxNbPerformedYearsInAset)
{
    // Allocate a table of parallel years structures
    pYears.resize(maxNbPerformedYearsInAset);

    // Tells these structures their power fluctuations mode
    for (uint y = 0; y < maxNbPerformedYearsInAset; ++y)
    {
        pYears[y].setPowerFluctuations(powerFluctuations);
    }
}

void randomNumbers::reset()
{
    for (uint i = 0; i < pMaxNbPerformedYears; i++)
    {
        pYears[i].reset();
    }

    yearNumberToIndex.clear();
}

void randomNumbers::allocate(const Antares::Data::Study& study)
{
    uint nbAreas = study.areas.size();

    for (uint y = 0; y < pMaxNbPerformedYears; y++)
    {
        // General :
        pYears[y].setNbAreas(nbAreas);
        pYears[y].pNbClustersByArea.resize(nbAreas);

        // Thermal noises :
        pYears[y].pThermalNoisesByArea.resize(nbAreas);

        for (uint a = 0; a != nbAreas; ++a)
        {
            // logs.info() << "   area : " << a << " :";
            auto& area = *(study.areas.byIndex[a]);
            size_t nbClusters = area.thermal.list.allClustersCount();
            pYears[y].pThermalNoisesByArea[a].resize(nbClusters);
            pYears[y].pNbClustersByArea[a] = nbClusters;
        }

        // Reservoir levels
        pYears[y].pReservoirLevels.resize(nbAreas);

        // Noises on unsupplied and spilled energy
        pYears[y].pUnsuppliedEnergy.resize(nbAreas);
        pYears[y].pSpilledEnergy.resize(nbAreas);

        // Hydro costs noises
        switch (study.parameters.power.fluctuations)
        {
        case Data::lssFreeModulations:
        {
            pYears[y].pHydroCostsByArea_freeMod.resize(nbAreas);
            for (uint a = 0; a != nbAreas; ++a)
            {
                pYears[y].pHydroCostsByArea_freeMod[a].resize(8784);
            }
            break;
        }
        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            pYears[y].pHydroCosts_rampingOrExcursion.resize(nbAreas);
            break;
        }
        case Data::lssUnknown:
        {
            logs.error() << "Power fluctuation unknown";
            break;
        }
        }
    } // End loop over years
}

void randomNumbers::compute(Antares::Data::Study& study, // Mersenne-Twister has non-const methods
                            unsigned nbYears,
                            std::map<unsigned int, bool>& isYearPerformed,
                            MersenneTwister& randomHydroGenerator)
{
    uint indexYear = 0;

    for (unsigned y = 0; y < nbYears; ++y)
    {
        bool isPerformed = isYearPerformed[y];
        if (isPerformed)
        {
            yearNumberToIndex[y] = indexYear;
        }

        // General
        const unsigned int nbAreas = study.areas.size();

        // ... Thermal noise ...
        for (unsigned int a = 0; a != nbAreas; ++a)
        {
            const auto& area = *(study.areas.byIndex[a]);

            for (auto& cluster: area.thermal.list.all())
            {
                uint clusterIndex = cluster->areaWideIndex;
                double thermalNoise = study.runtime.random[Data::seedThermalCosts].next();
                if (isPerformed)
                {
                    pYears[indexYear].pThermalNoisesByArea[a][clusterIndex] = thermalNoise;
                }
            }
        }

        // ... Reservoir levels ...
        uint areaIndex = 0;
        study.areas.each(
          [&areaIndex, &indexYear, &randomHydroGenerator, &y, &isPerformed, &study, this](
            Data::Area& area)
          {
              // looking for the initial reservoir level (begining of the year)
              auto& min = area.hydro.reservoirLevel[Data::PartHydro::minimum];
              auto& avg = area.hydro.reservoirLevel[Data::PartHydro::average];
              auto& max = area.hydro.reservoirLevel[Data::PartHydro::maximum];

              // Month the reservoir level is initialized according to.
              // This month number is given in the civil calendar, from january to december (0 is
              // january).
              int initResLevelOnMonth = area.hydro.initializeReservoirLevelDate;

              // Conversion of the previous month into simulation calendar
              int initResLevelOnSimMonth = study.calendar.mapping.months[initResLevelOnMonth];

              // Previous month's first day in the year
              int firstDayOfMonth = study.calendar.months[initResLevelOnSimMonth].daysYear.first;

              double randomLevel = randomReservoirLevel(min[firstDayOfMonth],
                                                        avg[firstDayOfMonth],
                                                        max[firstDayOfMonth],
                                                        randomHydroGenerator);

              // Possibly update the intial level from scenario builder
              if (study.parameters.useCustomScenario)
              {
                  double levelFromScenarioBuilder = study.scenarioInitialHydroLevels[areaIndex][y];
                  if (levelFromScenarioBuilder >= 0.)
                  {
                      randomLevel = levelFromScenarioBuilder;
                  }
              }

              // Current area's hydro starting (or initial) level computation
              // (no matter if the year is performed or not, we always draw a random initial
              // reservoir level to ensure the same results)
              if (isPerformed)
              {
                  pYears[indexYear].pReservoirLevels[areaIndex] = randomLevel;
              }

              areaIndex++;
          }); // each area

        // ... Unsupplied and spilled energy costs noises (french : bruits sur la defaillance
        // positive et negatives) ... references to the random number generators
        auto& randomUnsupplied = study.runtime.random[Data::seedUnsuppliedEnergyCosts];
        auto& randomSpilled = study.runtime.random[Data::seedSpilledEnergyCosts];

        int currentSpilledEnergySeed = study.parameters.seed[Data::seedSpilledEnergyCosts];
        int defaultSpilledEnergySeed = Data::antaresSeedDefaultValue
                                       + Data::seedSpilledEnergyCosts * Data::antaresSeedIncrement;
        bool SpilledEnergySeedIsDefault = (currentSpilledEnergySeed == defaultSpilledEnergySeed);
        areaIndex = 0;
        study.areas.each(
          [&isPerformed,
           &areaIndex,
           &randomUnsupplied,
           &randomSpilled,
           &indexYear,
           &SpilledEnergySeedIsDefault,
           this](Data::Area& area)
          {
              (void)area; // Avoiding warnings at compilation (unused variable) on linux
              if (isPerformed)
              {
                  double randomNumber = randomUnsupplied();
                  pYears[indexYear].pUnsuppliedEnergy[areaIndex] = randomNumber;
                  pYears[indexYear].pSpilledEnergy[areaIndex] = randomNumber;
                  if (!SpilledEnergySeedIsDefault)
                  {
                      pYears[indexYear].pSpilledEnergy[areaIndex] = randomSpilled();
                  }
              }
              else
              {
                  randomUnsupplied();
                  if (!SpilledEnergySeedIsDefault)
                  {
                      randomSpilled();
                  }
              }

              areaIndex++;
          }); // each area

        // ... Hydro costs noises ...
        auto& randomHydro = study.runtime.random[Data::seedHydroCosts];

        Data::PowerFluctuations powerFluctuations = study.parameters.power.fluctuations;
        switch (powerFluctuations)
        {
        case Data::lssFreeModulations:
        {
            areaIndex = 0;
            auto end = study.areas.end();

            // Computing hourly hydro costs noises so that they are homogeneously spread into :
            // [-1.e-3, -5*1.e-4] U [+5*1.e-4, +1.e-3]
            if (isPerformed)
            {
                for (auto i = study.areas.begin(); i != end; ++i)
                {
                    auto& noise = pYears[indexYear].pHydroCostsByArea_freeMod[areaIndex];
                    std::set<hydroCostNoise, compareHydroCostsNoises> setHydroCostsNoises;
                    for (uint j = 0; j != 8784; ++j)
                    {
                        noise[j] = randomHydro();
                        noise[j] -= 0.5; // Now we have : -0.5 < noise[j] < +0.5

                        // This std::set naturally sorts the hydro costs noises into increasing
                        // absolute values order
                        setHydroCostsNoises.insert(hydroCostNoise(noise[j], j));
                    }

                    uint rank = 0;
                    for (const auto& [value, index]: setHydroCostsNoises)
                    {
                        if (value < 0.)
                        {
                            noise[index] = -5 * 1.e-4 * (1 + rank / 8784.);
                        }
                        else
                        {
                            noise[index] = 5 * 1.e-4 * (1 + rank / 8784.);
                        }
                        rank++;
                    }

                    areaIndex++;
                }
            }
            else
            {
                for (auto i = study.areas.begin(); i != end; ++i)
                {
                    for (uint j = 0; j != 8784; ++j)
                    {
                        randomHydro();
                    }
                }
            }

            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            areaIndex = 0;
            auto end = study.areas.end();
            for (auto i = study.areas.begin(); i != end; ++i)
            {
                if (isPerformed)
                {
                    pYears[indexYear].pHydroCosts_rampingOrExcursion[areaIndex] = randomHydro();
                }
                else
                {
                    randomHydro();
                }

                areaIndex++;
            }
            break;
        }

        case Data::lssUnknown:
        {
            logs.error() << "Power fluctuation unknown";
            break;
        }

        } // end of switch

        if (isPerformed)
        {
            indexYear++;
        }

    } // End loop over years
} // End function
} // namespace Antares::Solver::Simulation
