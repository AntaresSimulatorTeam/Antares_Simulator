// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    const StateForAnArea& operator[](size_t areaIndex) const;

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
    ** \param clusterEnabledIndex Index of the thermal cluster for the current area
    */
    void initFromThermalClusterIndex(const unsigned int clusterEnabledIndex);

    /*!
    ** \brief Initialize some variable according a short term storage cluster index
    **
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param areaWideIndex Index of the short term storage cluster for the current area
    */
    void initFromShortTermStorageClusterIndex(const unsigned int areaWideIndex);

    /*!
    ** \brief Initialize some variable according a Hydro index
    **
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param areaWideIndex Index of the Hydro for the current area
    */
    void initFromHydroIndex(const unsigned int areaWideIndex);

    /*!
    ** \brief End the year by smoothing the thermal units run
    ** and computing costs.
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param clusterEnabledIndex Index of the thermal cluster for the current area
    */

    void yearEndBuildFromThermalClusterIndex(const unsigned int clusterEnabledIndex);

    void calculateReserveParticipationCosts();

    void initFromHydro();

private:
    /*!
    ** \brief Initialize some variable according a thermal cluster index
    **
    ** Called in initFromAreaIndex to split code
    **
    ** \param clusterEnabledIndex Index of the thermal cluster for the current area
    */
    void initFromThermalClusterIndexProduction(const unsigned int clusterEnabledIndex);

    void yearEndBuildThermalClusterCalculateStartupCosts(
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
    ** \param clusterEnabledIndex Index of the thermal cluster for the current area
    */
    void yearEndSmoothDispatchedUnitsCount(const unsigned int clusterEnabledIndex, uint numSpace);

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

    //! The current Short Term Storage cluster
    Data::ShortTermStorage::STStorageCluster* STStorageCluster;

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

    struct ReserveData
    {
        struct DetailledParticipation
        {
            double totalParticipation = 0;
            double onUnitsParticipation = 0;
            double offUnitsParticipation = 0;

            void addParticipation(double participation)
            {
                totalParticipation += participation;
            }

            void addOffParticipation(double participation)
            {
                offUnitsParticipation += participation;
                totalParticipation += participation;
            }

            void addOnParticipation(double participation)
            {
                onUnitsParticipation += participation;
                totalParticipation += participation;
            }
        };

        struct ReserveParticipationPerGroupForYear
        {
            //! Reserve Participation for all thermal group types (nuclear / coal / ...) for
            //! the whole year per reserve
            std::map<std::string, std::map<ReserveID, double>> thermalGroupsReserveParticipation;

            //! Reserve Participation for all thermal Short Term storages types (PSP /
            //! Battery / ...) for the whole year per reserve
            std::map<std::string, std::map<ReserveID, double>>
              shortTermStorageGroupsReserveParticipation;
        };

        //! All type of clusters reserves participations
        std::vector<ReserveParticipationPerGroupForYear> reserveParticipationPerGroupForYear{
          HOURS_PER_YEAR};

        //! Reserve Participation for each thermal cluster per reserve
        std::vector<std::map<std::string, std::map<ReserveID, DetailledParticipation>>>
          reserveParticipationPerThermalClusterForYear{HOURS_PER_YEAR};

        //! Reserve Participation for each STStorage cluster per reserve
        std::vector<std::map<std::string, std::map<ReserveID, double>>>
          reserveParticipationPerSTStorageClusterForYear{HOURS_PER_YEAR};

        //! Reserve Participation for each Hydro per reserve
        std::vector<std::map<std::string, std::map<ReserveID, double>>>
          reserveParticipationPerHydroForYear{HOURS_PER_YEAR};

        //! Reserve Participation cost for the whole year
        std::vector<double> reserveParticipationCostForYear;

        //! Reserves participation cost of the thermal cluster for the whole year
        std::vector<double> thermalClusterReserveParticipationCostForYear;

        //! Reserves participation cost of the Short Term Storage cluster for the whole year
        std::vector<double> STStorageClusterReserveParticipationCostForYear;

        //! Reserves participation cost of the Hydro for the whole year
        std::vector<double> HydroReserveParticipationCostForYear;

        ReserveData()
        {
            reserveParticipationCostForYear.resize(HOURS_PER_YEAR, 0);
            thermalClusterReserveParticipationCostForYear.resize(HOURS_PER_YEAR, 0);
            STStorageClusterReserveParticipationCostForYear.resize(HOURS_PER_YEAR, 0);
            HydroReserveParticipationCostForYear.resize(HOURS_PER_YEAR, 0);
            reserveParticipationPerSTStorageClusterForYear.clear();
            reserveParticipationPerSTStorageClusterForYear.resize(HOURS_PER_YEAR);
            reserveParticipationPerHydroForYear.clear();
            reserveParticipationPerHydroForYear.resize(HOURS_PER_YEAR);
            reserveParticipationPerThermalClusterForYear.clear();
            reserveParticipationPerThermalClusterForYear.resize(HOURS_PER_YEAR);
        }
    };

    ReserveOpt<std::vector<ReserveData>> reserveData;

    //! Number of unit dispatched for all clusters for the whole year for ucHeruistic (fast) or
    //! ucMILP (accurate)
    uint thermalClusterDispatchedUnitsCountForYear[HOURS_PER_YEAR];

    //! Thermal operating cost for the current thermal cluster for the whole year
    double thermalClusterOperatingCostForYear[HOURS_PER_YEAR];
    //! Thermal NP Cost for the current thermal cluster for the whole year
    double thermalClusterNonProportionalCostForYear[HOURS_PER_YEAR];
    //! Minimum power of the cluster for the whole year
    double thermalClusterPMinOfTheClusterForYear[HOURS_PER_YEAR];

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
