/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_SIMULATION_SOLVER_UTILS_H__
#define __SOLVER_SIMULATION_SOLVER_UTILS_H__

#include <iomanip> // For setprecision
#include <limits>  // For std numeric_limits
#include <map>
#include <sstream> // For ostringstream
#include <vector>

#include <yuni/yuni.h>

#include <antares/study/fwd.h>
#include <antares/writer/i_writer.h>

namespace Antares::Solver::Simulation
{
struct setOfParallelYears
{
    // Un lot d'année à exécuter en parallèle.
    // En fonction d'une éventuelle play-list, certaines seront jouées et d'autres non.

public:
    // Numeros des annees en parallele pour ce lot (certaines ne seront pas jouées en cas de
    // play-list "trouée")
    std::vector<unsigned int> yearsIndices;

    // Une annee doit-elle être rejouée ?
    std::map<uint, bool> yearFailed;

    // Associe le numero d'une année jouée à l'indice de l'espace
    std::map<unsigned int, unsigned int> performedYearToSpace;

    // L'inverse : pour une année jouée, associe l'indice de l'espace au numero de l'année
    std::map<unsigned int, unsigned int> spaceToPerformedYear;

    // Pour chaque année, est-elle la première à devoir être jouée dans son lot d'années ?
    std::map<unsigned int, bool> isFirstPerformedYearOfASet;

    // Pour chaque année du lot, est-elle jouée ou non ?
    std::map<unsigned int, bool> isYearPerformed;

    // Nbre d'années en parallele vraiment jouées pour ce lot
    unsigned int nbPerformedYears;

    // Nbre d'années en parallele jouées ou non pour ce lot
    unsigned int nbYears;

    // Regenere-t-on des times series avant de jouer les annees du lot courant
    bool regenerateTS;

    // Annee a passer a la fonction "regenerateTimeSeries<false>(y)" (si regenerateTS is "true")
    unsigned int yearForTSgeneration;
};

class costStatistics
{
public:
    void setNbPerformedYears(uint n);
    void addCost(const double cost);
    void endStandardDeviation();

    // System costs statistics
    double costAverage = 0.;
    double costStdDeviation = 0.;
    double costMin = std::numeric_limits<double>::max();
    double costMax = 0.;

private:
    // Total number of performed years in the study
    uint nbPerformedYears = 0.;
};

class annualCostsStatistics
{
public:
    // Costs
    costStatistics systemCost;
    costStatistics criterionCost1;
    costStatistics criterionCost2;
    costStatistics optimizationTime1;
    costStatistics optimizationTime2;
    costStatistics updateTime;

    annualCostsStatistics();
    void setNbPerformedYears(uint n);
    void endStandardDeviations();
    void writeToOutput(IResultWriter& writer);

private:
    void writeSystemCostToOutput(IResultWriter& writer);
    void writeCriterionCostsToOutput(IResultWriter& writer) const;
    void writeUpdateTimes(IResultWriter& writer) const;
    void writeOptimizationTimeToOutput(IResultWriter& writer) const;
};

class yearRandomNumbers
{
public:
    yearRandomNumbers()
    {
        pNbAreas = 0;
    }

    ~yearRandomNumbers() = default;

    void setNbAreas(uint nbAreas)
    {
        pNbAreas = nbAreas;
    }

    void setPowerFluctuations(Data::PowerFluctuations powerFluctuations)
    {
        pPowerFluctuations = powerFluctuations;
    }

    void reset()
    {
        // Thermal noises
        for (uint a = 0; a != pNbAreas; a++)
        {
            pThermalNoisesByArea[a].assign(pNbClustersByArea[a], 0);
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
    randomNumbers(uint maxNbPerformedYearsInAset, Data::PowerFluctuations powerFluctuations):
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

    ~randomNumbers() = default;

    void reset()
    {
        for (uint i = 0; i < pMaxNbPerformedYears; i++)
        {
            pYears[i].reset();
        }

        yearNumberToIndex.clear();
    }

    uint pMaxNbPerformedYears;
    std::vector<yearRandomNumbers> pYears;

    // Associates :
    //		year number (0, ..., total nb of years to compute - 1) --> index of the year's space
    //(0,
    //..., max nb of parallel years - 1)
    std::map<uint, uint> yearNumberToIndex;
};

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
} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_SOLVER_H__
