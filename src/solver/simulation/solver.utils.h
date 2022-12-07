/*
** Copyright 2007-2018 RTE
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
#ifndef __SOLVER_SIMULATION_SOLVER_UTILS_H__
#define __SOLVER_SIMULATION_SOLVER_UTILS_H__

#include <vector>
#include <iostream> // For std namespace
#include <limits>   // For std numeric_limits
#include <sstream>  // For ostringstream
#include <iomanip>  // For setprecision
// #include <stdio.h>
#include <yuni/yuni.h>

#include <i_writer.h>

#define SEP Yuni::IO::Separator

namespace Antares
{
namespace Solver
{
namespace Simulation
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
    costStatistics() :
     costAverage(0.),
     costStdDeviation(0.),
     costMin(std::numeric_limits<double>::max()),
     costMax(0.),
     nbPerformedYears(0)
    {
    }

    void setNbPerformedYears(uint n)
    {
        assert(n);
        nbPerformedYears = n;
    }

    void addCost(const double cost)
    {
        // Average
        costAverage += cost / nbPerformedYears;

        // Standard deviation
        costStdDeviation += cost * cost / nbPerformedYears;

        // Min and Max
        if (cost < costMin)
            costMin = cost;
        if (cost > costMax)
            costMax = cost;
    }

    void endStandardDeviation()
    {
        costStdDeviation = Yuni::Math::SquareRoot(costStdDeviation - costAverage * costAverage);
    }

public:
    // System costs statistics
    double costAverage;
    double costStdDeviation;
    double costMin;
    double costMax;

private:
    // Total number of performed years in the study
    uint nbPerformedYears;
};

class annualCostsStatistics
{
public:
    annualCostsStatistics() :
     systemCostFilename("annualSystemCost.txt"),
     criterionsCostsFilename("checkIntegrity.txt"),
     optimizationTimeFilename("timeStatistics.txt")
    {
    }

    void setNbPerformedYears(uint n)
    {
        systemCost.setNbPerformedYears(n);
        criterionCost1.setNbPerformedYears(n);
        criterionCost2.setNbPerformedYears(n);
        optimizationTime1.setNbPerformedYears(n);
        optimizationTime2.setNbPerformedYears(n);
    };

    void endStandardDeviations()
    {
        systemCost.endStandardDeviation();
        criterionCost1.endStandardDeviation();
        criterionCost2.endStandardDeviation();
        optimizationTime1.endStandardDeviation();
        optimizationTime2.endStandardDeviation();
    };

    void writeToOutput(IResultWriter::Ptr writer)
    {
        if (!writer)
            return;

        writeSystemCostToOutput(writer);
        writeCriterionCostsToOutput(writer);
        writeOptimizationTimeToOutput(writer);
    }

    std::string to_scientific(const double d)
    {
        std::ostringstream stream;
        stream << std::scientific;
        stream << std::setprecision(14);
        stream << d;

        return stream.str();
    }

    std::string round_to_closer_int(const double d)
    {
#ifdef YUNI_OS_MSVC
        ::sprintf_s(conversionBuffer, sizeof(conversionBuffer), "%.0f", d);
#else
        ::snprintf(conversionBuffer, sizeof(conversionBuffer), "%.0f", d);
#endif
        std::string rnd(conversionBuffer);
        return rnd;
    }

private:
    void writeSystemCostToOutput(IResultWriter::Ptr writer)
    {
        Yuni::Clob buffer;
        buffer << "EXP : " << round_to_closer_int(systemCost.costAverage) << "\n";
        buffer << "STD : " << round_to_closer_int(systemCost.costStdDeviation) << "\n";
        buffer << "MIN : " << round_to_closer_int(systemCost.costMin) << "\n";
        buffer << "MAX : " << round_to_closer_int(systemCost.costMax) << "\n";

        writer->addEntryFromBuffer(systemCostFilename, buffer);
    }

    void writeCriterionCostsToOutput(IResultWriter::Ptr writer)
    {
        Yuni::Clob buffer;
        buffer << to_scientific(criterionCost1.costAverage) << "\n";
        buffer << to_scientific(criterionCost1.costStdDeviation) << "\n";
        buffer << to_scientific(criterionCost1.costMin) << "\n";
        buffer << to_scientific(criterionCost1.costMax) << "\n";

        buffer << to_scientific(criterionCost2.costAverage) << "\n";
        buffer << to_scientific(criterionCost2.costStdDeviation) << "\n";
        buffer << to_scientific(criterionCost2.costMin) << "\n";
        buffer << to_scientific(criterionCost2.costMax) << "\n";

        writer->addEntryFromBuffer(criterionsCostsFilename, buffer);
    }

    void writeOptimizationTimeToOutput(IResultWriter::Ptr writer)
    {
        Yuni::Clob buffer;
        buffer << "First optimization :\n";
        buffer << "EXP (ms) : " << optimizationTime1.costAverage << "\n";
        buffer << "STD (ms) : " << optimizationTime1.costStdDeviation << "\n";
        buffer << "MIN (ms) : " << optimizationTime1.costMin << "\n";
        buffer << "MAX (ms) : " << optimizationTime1.costMax << "\n";

        buffer << "Second optimization :\n";
        buffer << "EXP (ms) : " << optimizationTime2.costAverage << "\n";
        buffer << "STD (ms) : " << optimizationTime2.costStdDeviation << "\n";
        buffer << "MIN (ms) : " << optimizationTime2.costMin << "\n";
        buffer << "MAX (ms) : " << optimizationTime2.costMax << "\n";

        writer->addEntryFromBuffer(optimizationTimeFilename, buffer);
    }

public:
    // Costs
    costStatistics systemCost;
    costStatistics criterionCost1;
    costStatistics criterionCost2;
    costStatistics optimizationTime1;
    costStatistics optimizationTime2;

private:
    const std::string systemCostFilename;
    const std::string criterionsCostsFilename;
    const std::string optimizationTimeFilename;
    char conversionBuffer[256]; // Used to round a double to the closer integer
};

class yearRandomNumbers
{
public:
    yearRandomNumbers()
    {
        pThermalNoisesByArea = nullptr;
        pNbClustersByArea = nullptr;
        pNbAreas = 0;
    }

    ~yearRandomNumbers()
    {
        // General
        delete[] pNbClustersByArea;

        // Thermal noises
        for (uint a = 0; a != pNbAreas; a++)
            delete[] pThermalNoisesByArea[a];
        delete[] pThermalNoisesByArea;

        // Reservoir levels, spilled and unsupplied energy
        delete[] pReservoirLevels;
        delete[] pUnsuppliedEnergy;
        delete[] pSpilledEnergy;

        // Hydro costs noises
        switch (pPowerFluctuations)
        {
        case Data::lssFreeModulations:
        {
            for (uint a = 0; a != pNbAreas; a++)
                delete[] pHydroCostsByArea_freeMod[a];
            delete[] pHydroCostsByArea_freeMod;
            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            delete[] pHydroCosts_rampingOrExcursion;
            break;
        }

        case Data::lssUnknown:
            break;
        }
    }

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
        // General
        memset(pNbClustersByArea, 0, pNbAreas * sizeof(size_t));

        // Thermal noises
        for (uint a = 0; a != pNbAreas; a++)
            memset(pThermalNoisesByArea[a], 0, pNbClustersByArea[a] * sizeof(double));

        // Reservoir levels, spilled and unsupplied energy costs
        memset(pReservoirLevels, 0, pNbAreas * sizeof(double));
        memset(pUnsuppliedEnergy, 0, pNbAreas * sizeof(double));
        memset(pSpilledEnergy, 0, pNbAreas * sizeof(double));

        // Hydro costs noises
        switch (pPowerFluctuations)
        {
        case Data::lssFreeModulations:
        {
            for (uint a = 0; a != pNbAreas; a++)
                memset(pHydroCostsByArea_freeMod[a], 0, 8784 * sizeof(double));
            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            memset(pHydroCosts_rampingOrExcursion, 0, pNbAreas * sizeof(double));
            break;
        }

        case Data::lssUnknown:
            break;
        }
    }

public:
    // General data
    uint pNbAreas;
    Data::PowerFluctuations pPowerFluctuations;

    // Data for thermal noises
    double** pThermalNoisesByArea;
    size_t* pNbClustersByArea;

    // Data for reservoir levels
    double* pReservoirLevels;

    // Data for unsupplied and spilled energy costs
    double* pUnsuppliedEnergy;
    double* pSpilledEnergy;

    // Hydro costs noises
    double** pHydroCostsByArea_freeMod;
    double* pHydroCosts_rampingOrExcursion;
};

class randomNumbers
{
public:
    randomNumbers(uint maxNbPerformedYearsInAset, Data::PowerFluctuations powerFluctuations) :
     pMaxNbPerformedYears(maxNbPerformedYearsInAset)
    {
        // Allocate a table of parallel years structures
        pYears = new yearRandomNumbers[maxNbPerformedYearsInAset];

        // Tells these structures their power fluctuations mode
        for (uint y = 0; y < maxNbPerformedYearsInAset; ++y)
            pYears[y].setPowerFluctuations(powerFluctuations);
    }

    ~randomNumbers()
    {
        delete[] pYears;
    }

    void reset()
    {
        for (uint i = 0; i < pMaxNbPerformedYears; i++)
            pYears[i].reset();

        yearNumberToIndex.clear();
    }

public:
    uint pMaxNbPerformedYears;
    yearRandomNumbers* pYears;

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
    hydroCostNoise(double v, uint i) : value(v), index(i)
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

private:
    double value;
    uint index;
};

// Used to sort the hydro costs noises into increasing absolute values order in a std::set
struct compareHydroCostsNoises
{
    inline bool operator()(const hydroCostNoise& hcnr1, const hydroCostNoise& hcnr2) const
    {
        return (std::abs(hcnr1.getValue()) < std::abs(hcnr2.getValue())) ? true : false;
    }
};
} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_SOLVER_H__
