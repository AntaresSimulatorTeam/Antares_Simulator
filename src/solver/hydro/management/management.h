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
#ifndef __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
#define __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>
#include <antares/mersenne-twister/mersenne-twister.h>
#include "../../simulation/sim_structure_donnees.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
class State;
}
} // namespace Solver

enum
{
    //! The maximum number of days in a year
    dayYearCount = 366
};

//! Temporary data
struct TmpDataByArea
{
    //! Monthly local effective demand
    double MLE[12];
    //! Monthly optimal generation
    double MOG[12];
    //! Monthly optimal level
    double MOL[12];
    //! Monthly target generations
    double MTG[12];
    //! inflows
    double inflows[12];
    //! monthly minimal generation
    std::array<double, 12> mingens;

    //! Net demand, for each day of the year, for each area
    double DLN[dayYearCount];
    //! Daily local effective load
    double DLE[dayYearCount];
    //! Daily optimized Generation
    double DOG[dayYearCount];
    //! daily minimal generation
    std::array<double, dayYearCount> dailyMinGen;

    // Data for minGen<->inflows preChecks
    //! monthly total mingen
    std::array<double, 12> totalMonthMingen;
    //! monthly total inflows
    std::array<double, 12> totalMonthInflows;
    //! yearly total mingen
    double totalYearMingen;
    //! yearly total inflows
    double totalYearInflows;

}; // struct TmpDataByArea

typedef struct
{
    std::vector<double> HydrauliqueModulableQuotidien; /* indice par jour */
    std::vector<double> NiveauxReservoirsDebutJours;   //Niveaux (quotidiens) du reservoir de début
    //de jour (en cas de gestion des reservoirs).
    std::vector<double> NiveauxReservoirsFinJours; //Niveaux (quotidiens) du reservoir de fin
    //de jour (en cas de gestion des reservoirs).
} VENTILATION_HYDRO_RESULTS_BY_AREA;

// vector of [numSpace][area]
using HYDRO_VENTILATION_RESULTS = std::vector<VENTILATION_HYDRO_RESULTS_BY_AREA>;


class HydroManagement final
{
public:
    HydroManagement(const Data::AreaList& areas,
                    const Data::Parameters& params,
                    const Date::Calendar& calendar,
                    unsigned int maxNbYearsInParallel,
                    Solver::IResultWriter& resultWriter);

    static double randomReservoirLevel(double min, double avg, double max, MersenneTwister& random);

    //! Perform the hydro ventilation
    void makeVentilation(double* randomReservoirLevel,
                        Solver::Variable::State& state,
                        uint y,
                        uint numSpace);

    HYDRO_VENTILATION_RESULTS& ventilationResults() { return ventilationResults_; }

private:
    //! Prepare inflows scaling for each area
    void prepareInflowsScaling(uint year);
    //! Prepare minimum generation scaling for each area
    void minGenerationScaling(uint year);
    //! check Monthly minimum generation is lower than available inflows
    bool checkMonthlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Yearly minimum generation is lower than available inflows
    bool checkYearlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Weekly minimum generation is lower than available inflows
    bool checkWeeklyMinGeneration(uint year, const Data::Area& area) const;
    //! check Hourly minimum generation is lower than available inflows
    bool checkHourlyMinGeneration(uint year, const Data::Area& area) const;
    //! check minimum generation is lower than available inflows
    bool checkMinGeneration(uint year) const;
    //! Prepare the net demand for each area
    void prepareNetDemand(uint numSpace, uint year, Data::StudyMode mode);
    //! Prepare the effective demand for each area
    void prepareEffectiveDemand();
    //! Monthly Optimal generations
    void prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y);

    //! Monthly target generations
    // note: inflows may have two different types, if in swap mode or not
    // \return The total inflow for the whole year
    double prepareMonthlyTargetGenerations(Data::Area& area, TmpDataByArea& data);

    void prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                        uint y,
                                        uint numSpace);

    void prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                        Data::Area& area,
                                        uint y,
                                        uint numSpace);
    //@}

    //! \name Utilities
    //@{
    //! Beta variable
    static double BetaVariable(double a, double b, MersenneTwister &random);
    //! Gamma variable
    static double GammaVariable(double a, MersenneTwister &random);
    //@}

private:
    std::vector<TmpDataByArea> tmpDataByArea_;
    const Data::AreaList& areas_;
    const Date::Calendar& calendar_;
    const Data::Parameters& parameters_;
    unsigned int maxNbYearsInParallel_ = 0;
    Solver::IResultWriter& resultWriter_;

    HYDRO_VENTILATION_RESULTS ventilationResults_;
}; // class HydroManagement
} // namespace Antares

#endif // __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
