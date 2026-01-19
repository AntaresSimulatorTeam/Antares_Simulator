// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_SOLVER_UTILS_H__
#define __SOLVER_SIMULATION_SOLVER_UTILS_H__

#include <limits> // For std numeric_limits
#include <map>
#include <vector>

#include <antares/study/fwd.h>
#include <antares/writer/i_writer.h>

#include "sim_structure_probleme_economique.h"

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

void addTimeMeasure(Benchmarking::DurationCollector&, const TIME_MEASURES&);

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_SOLVER_H__
