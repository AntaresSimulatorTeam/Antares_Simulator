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
#ifndef __SOLVER_VARIABLE_STATE_H__
#define __SOLVER_VARIABLE_STATE_H__

#include <yuni/yuni.h>
#include "constants.h"
#include <antares/study/fwd.h>
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_extern_variables_globales.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
class State
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit State(Data::Study& s);
    //@}
    //! \name Destructor
    //@{
    /*!
    ** \brief Default Destructor
    */
    //	~State();
    //@}

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
    void initFromThermalClusterIndex(const unsigned int areaWideIndex, uint numSpace);

    /*!
    ** \brief End the year by smoothing the thermal units run
    ** We assume here that the variables related to an area
    ** are properly initialized.
    **
    ** \param areaWideIndex Index of the thermal cluster for the current area
    */
    void yearEndBuildFromThermalClusterIndex(const unsigned int areaWideIndex, uint numSpace);

    /*!
    ** \brief Reset internal data
    */
    void startANewYear();

    /*!
    ** \brief Reset internal data for end of year calculations
    */
    void yearEndReset();

public:
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
    //! The current thermal cluster
    Data::ThermalCluster* thermalCluster;
    //! The current renewable cluster
    Data::RenewableCluster* renewableCluster;
    //! The Scratchpad for the current area
    Data::AreaScratchpad* scratchpad;
    //! The current link
    Data::AreaLink* link;

    //! \name Adequacy Draft
    //@{
    RESULTATS_HORAIRES_ADEQUATION* hourlyAdequacyResults;
    double eensSystemIS;
    double eensSystemCN;
    bool loleSystemIS_HasFailure;
    bool loleSystemCN_HasFailure;
    //@}

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
    VALEURS_DE_NTC_ET_RESISTANCES* ntc;
    //! Thermal production for the current thermal cluster for the current hour in the year
    double thermalClusterProduction;
    //! Thermal available production for the current thermal cluster for the current hour in the
    //! year
    double thermalClusterAvailableProduction;
    //! The operating cost for the current cluster of the current hour (production level*production
    //! cost + NP Cost)
    double thermalClusterOperatingCost;
    //! The non propostional cost for the current cluster of the current hour (startupCost *
    //! (newUnitCount - previousUnitCount)) + (fixed cost * newUnitCount) - MBO - 13/05/2014 - #21
    double thermalClusterNonProportionalCost;
    //! Number of groups turned ON by cluster for the current hour in the year with the ucMILP
    //! (accurate) unit commitment mode
    uint thermalClusterNumberON;
    //! Minimum power of a group of the cluster for the current hour in the year
    double thermalClusterPMinOfAGroup;
    //! Minimum power of the cluster for the current hour in the year
    double thermalClusterPMinOfTheCluster;

    //! Thermal production for the current thermal cluster for the whole year
    double thermalClusterProductionForYear[Variable::maxHoursInAYear];
    //! Number of unit dispatched for all clusters for the whole year for ucHeruistic (fast) or
    //! ucMILP (accurate)
    uint thermalClusterDispatchedUnitsCountForYear[Variable::maxHoursInAYear];

    //! Thermal operating cost for the current thermal cluster for the whole year
    double thermalClusterOperatingCostForYear[Variable::maxHoursInAYear];
    //! Thermal NP Cost for the current thermal cluster for the whole year
    double thermalClusterNonProportionalCostForYear[Variable::maxHoursInAYear];
    //! Minimum power of the cluster for the whole year
    double thermalClusterPMinOfTheClusterForYear[Variable::maxHoursInAYear];

    //! Dispatchable margin for the current area (valid only from weekForEachArea)
    const double* dispatchableMargin;
    //@}

    //! Probleme Hebdo
    PROBLEME_HEBDO* problemeHebdo;

    //! The timeseries indexes for the current area
    NUMERO_CHRONIQUES_TIREES_PAR_PAYS* timeseriesIndex;
    //! Study mode: economy / adequacy / draft
    Data::StudyMode studyMode;
    //! Study unit commitment mode: fast(ucHeuristic) / accurate(ucMILP)
    Data::UnitCommitmentMode unitCommitmentMode;
    //! Reference to the original study
    Data::Study& study;

    /*!
    ** \brief Flag to know if the simplex has been used for the current week
    **
    ** This flag is always true in economy mode, and always false in adequacy-draft
    */
    bool simplexHasBeenRan;

    // Working variable for output water values computation
    Antares::Data::h2oValueWorkVarsType h2oValueWorkVars;

    // Annual costs to be printed in output into separate files
    // -----------------------------------------------------------------
    // Sum over all year's hours of  :
    // - sum over all areas of variable "overall cost"
    // - sum over all links of variable "hurdle cost".
    // That is : Somme(h in Y)[ Somme(a in areas)(OV. COST[h,a])  +  Somme(l in links)(HURDLE
    // COST[h,l]) ]
    double annualSystemCost;
    // Sum of the weekly optimal costs over the year (first optimisation step)
    double optimalSolutionCost1;
    // Sum of the weekly optimal costs over the year (second optimisation step)
    double optimalSolutionCost2;
    // -----------------------------------------------------------------
}; // class State

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "state.hxx"

#endif // __SOLVER_VARIABLE_STATE_H__
