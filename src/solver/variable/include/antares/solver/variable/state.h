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
#ifndef __SOLVER_VARIABLE_STATE_H__
#define __SOLVER_VARIABLE_STATE_H__

#include <array>
#include <vector>

#include <yuni/yuni.h>

#include <antares/study/area/scratchpad.h>
#include <antares/study/fwd.h>
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Variable
{
class ThermalState
{
public:
    explicit ThermalState(const Data::AreaList& areas);

    class StateForAnArea
    {
    public:
        void initializeFromArea(const Data::Area& area);
        //! Thermal production for thermal clusters for the current hour in the year
        std::vector<double> thermalClustersProductions;

        //! The operating cost for all clusters at the current hour (production level*production
        //! cost + NP Cost)
        std::vector<double> thermalClustersOperatingCost;

        //! Number of units turned ON by cluster for the current hour in the year with the ucMILP
        //! (accurate) unit commitment mode
        std::vector<uint> numberOfUnitsONbyCluster;

        //! Minimum power of all clusters for the current hour in the year
        std::vector<double> PMinOfClusters;

        std::vector<unsigned int> unitCountLastHour;
        std::vector<double> productionLastHour;
        std::vector<double> pminOfAGroup;
    };

    StateForAnArea& operator[](size_t areaIndex);

private:
    std::vector<StateForAnArea> thermal;
};

class State
{
public:
    explicit State(Data::Study& s);

    /*!
    ** \brief Initialize some variables according an area index
    **
    ** \param areaIndex Index of the area
    */
    void initFromAreaIndex(const unsigned int areaIndex, uint numSpace);

    /*!
    ** \brief Initialize some variable according a thermal cluster index
    **
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param areaWideIndex Index of the thermal cluster for the current area
    */
    void initFromThermalClusterIndex(const unsigned int areaWideIndex);

    /*!
    ** \brief End the year by smoothing the thermal units run
    ** and computing costs.
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param areaWideIndex Index of the thermal cluster for the current area
    */

    void yearEndBuildFromThermalClusterIndex(const unsigned int areaWideIndex);


private:
    /*!
    ** \brief Initialize some variable according a thermal cluster index
    **
    ** Called in initFromAreaIndex to split code
    **
    ** \param areaWideIndex Index of the thermal cluster for the current area
    */
    void initFromThermalClusterIndexProduction(const unsigned int areaWideIndex);

    void yearEndBuildThermalClusterCalculateStartupCosts(
      const uint& maxDurationON,
      const std::array<uint, HOURS_PER_YEAR>& ON_min,
      const std::array<uint, HOURS_PER_YEAR>& ON_opt,
      const Data::ThermalCluster* currentCluster);

    void yearEndBuildCalculateRampingCosts(
      const uint& maxDurationON,
      const std::array<uint, HOURS_PER_YEAR>& ON_min,
      const std::array<uint, HOURS_PER_YEAR>& ON_opt,
      const Data::ThermalCluster* currentCluster);

    std::array<uint, HOURS_PER_YEAR> computeEconomicallyOptimalNbClustersONforEachHour(

      const uint& maxDurationON,
      const std::array<uint, HOURS_PER_YEAR>& ON_min,
      const std::array<uint, HOURS_PER_YEAR>& ON_max) const;

    /*!
    ** \brief Smooth the thermal units run after resolutions
    ** using heuristics
    **
    ** \param areaWideIndex Index of the thermal cluster for the current area
    */
    void yearEndSmoothDispatchedUnitsCount(const unsigned int areaWideIndex, uint numSpace);

public:
    /*!
    ** \brief Reset internal data
    */
    void startANewYear();

    /*!
    ** \brief Reset thermal internal data for end of year calculations
    */
    void yearEndResetThermal();

    //! Current year
    unsigned int year;
    //! Current week for current year (zero-based)
    unsigned int weekInTheYear;
    //! Current hour in the year (zero-based)
    unsigned int hourInTheYear;
    //! Current hour in the week (zero-based)
    unsigned int hourInTheWeek;
    //! Current hour from the begining of the simulation
    unsigned int hourInTheSimulation;

    //! The current area
    Data::Area* area;

    //! The current thermal cluster (used in yearEndBuildForEachThermalCluster functions)
    Data::ThermalCluster* thermalCluster;

    //! The current renewable cluster
    Data::RenewableCluster* renewableCluster;
    //! The Scratchpad for the current area
    Data::AreaScratchpad* scratchpad;
    //! The current link
    Data::AreaLink* link;

    //! \name Adequacy
    //@{
    /*!
    ** \brief Spilled energy for all areas in the week
    */
    Matrix<> resSpilled;
    //@}

    //! \name Economy
    //@{
    RESULTATS_HORAIRES* hourlyResults;
    //! NTC Values
    VALEURS_DE_NTC_ET_RESISTANCES ntc;

    //! Thermal production for the current thermal cluster for the whole year
    double thermalClusterProductionForYear[HOURS_PER_YEAR];
    //! Number of unit dispatched for all clusters for the whole year for ucHeruistic (fast) or
    //! ucMILP (accurate)
    uint thermalClusterDispatchedUnitsCountForYear[HOURS_PER_YEAR];

    //! Thermal operating cost for the current thermal cluster for the whole year
    double thermalClusterOperatingCostForYear[HOURS_PER_YEAR];
    //! Thermal NP Cost for the current thermal cluster for the whole year
    double thermalClusterNonProportionalCostForYear[HOURS_PER_YEAR];
    //! Minimum power of the cluster for the whole year
    double thermalClusterPMinOfTheClusterForYear[HOURS_PER_YEAR];
    //! Ramping cost of the thermal cluster for the whole year
    double thermalClusterRampingCostForYear[HOURS_PER_YEAR];

    double renewableClusterProduction;

    //! Dispatchable margin for the current area (valid only from weekForEachArea)
    const double* dispatchableMargin;
    //@}

    //! Probleme Hebdo
    PROBLEME_HEBDO* problemeHebdo;

    //! Simulation mode: economy / adequacy / expansion
    Data::SimulationMode simulationMode;
    //! Study unit commitment mode: fast(ucHeuristic) / accurate(ucMILP)
    Data::UnitCommitmentMode unitCommitmentMode;
    //! Reference to the original study
    Data::Study& study;
    // Thermal data, used to compute overall cost, etc.
    ThermalState thermal;
    //! Index of the state in the state vector
    unsigned int numSpace;
    /*!
    ** \brief Flag to know if the simplex has been used for the current week
    */
    bool simplexRunNeeded;

    // Annual costs to be printed in output into separate files
    // -----------------------------------------------------------------
    // Sum over all year's hours of  :
    // - sum over all areas of variable "overall cost"
    // - sum over all links of variable "hurdle cost".
    // That is : Somme(h in Y)[ Somme(a in areas)(OV. COST[h,a])  +  Somme(l in links)(HURDLE
    // COST[h,l]) ]
    double annualSystemCost = 0.;
    /// Sum of the weekly optimal costs over the year (first optimisation step)
    double optimalSolutionCost1 = 0.;
    /// Sum of the weekly optimal costs over the year (second optimisation step)
    double optimalSolutionCost2 = 0.;
    /// Average time spent in first optimization over the year (ms)
    double averageOptimizationTime1 = 0.;
    /// Average time spent in second optimization over the year (ms)
    double averageOptimizationTime2 = 0.;
    /// Average time spent updating the problem over the year (ms)
    double averageUpdateTime = 0.;
    // -----------------------------------------------------------------
}; // class State

} // namespace Antares::Solver::Variable

#include "state.hxx"

#endif // __SOLVER_VARIABLE_STATE_H__
