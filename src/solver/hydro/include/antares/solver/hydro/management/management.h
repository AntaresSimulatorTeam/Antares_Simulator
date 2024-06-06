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
#ifndef __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
#define __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__

#include <unordered_map>

#include <yuni/yuni.h>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/area/area.h>
#include <antares/study/fwd.h>
#include "antares/date/date.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/writer/i_writer.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
class State;
}

double randomReservoirLevel(double min, double avg, double max, MersenneTwister& random);
double BetaVariable(double a, double b, MersenneTwister& random);
double GammaVariable(double a, MersenneTwister& random);

} // namespace Solver

typedef struct
{
    std::vector<double> HydrauliqueModulableQuotidien; /* indice par jour */
    std::vector<double> NiveauxReservoirsDebutJours; // Niveaux (quotidiens) du reservoir de début
    // de jour (en cas de gestion des reservoirs).
    std::vector<double> NiveauxReservoirsFinJours; // Niveaux (quotidiens) du reservoir de fin
    // de jour (en cas de gestion des reservoirs).
} VENTILATION_HYDRO_RESULTS_BY_AREA;

using HYDRO_VENTILATION_RESULTS = std::vector<VENTILATION_HYDRO_RESULTS_BY_AREA>;

class HydroManagement final
{
public:
    HydroManagement(const Data::AreaList& areas,
                    const Data::Parameters& params,
                    const Date::Calendar& calendar,
                    unsigned int maxNbYearsInParallel,
                    Solver::IResultWriter& resultWriter);

    //! Perform the hydro ventilation
    void makeVentilation(double* randomReservoirLevel,
                         Solver::Variable::State& state,
                         uint y,
                         Antares::Data::Area::ScratchMap& scratchmap);

    const HYDRO_VENTILATION_RESULTS& ventilationResults()
    {
        return ventilationResults_;
    }

private:
    //! check Monthly minimum generation is lower than available inflows
    bool checkMonthlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Yearly minimum generation is lower than available inflows
    bool checkYearlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Weekly minimum generation is lower than available inflows
    bool checkWeeklyMinGeneration(uint year, const Data::Area& area) const;
    //! check Hourly minimum generation is lower than available inflows
    bool checkGenerationPowerConsistency(uint year) const;
    //! return false if checkGenerationPowerConsistency or checkMinGeneration returns false
    bool checksOnGenerationPowerBounds(uint year) const;
    //! check minimum generation is lower than available inflows
    bool checkMinGeneration(uint year) const;
    //! Prepare the net demand for each area
    void prepareNetDemand(uint year,
                          Data::SimulationMode mode,
                          const Antares::Data::Area::ScratchMap& scratchmap);
    //! Prepare the effective demand for each area
    void prepareEffectiveDemand(uint year);
    //! Monthly Optimal generations
    void prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y);

    //! Monthly target generations
    // note: inflows may have two different types, if in swap mode or not
    // \return The total inflow for the whole year
    double prepareMonthlyTargetGenerations(Data::Area& area, Antares::Data::TmpDataByArea& data);

    void prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                        uint y,
                                        Antares::Data::Area::ScratchMap& scratchmap);

    void prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                        Data::Area& area,
                                        uint y,
                                        Antares::Data::Area::ScratchMap& scratchmap);

private:
    const Data::AreaList& areas_;
    const Date::Calendar& calendar_;
    const Data::Parameters& parameters_;
    unsigned int maxNbYearsInParallel_ = 0;
    Solver::IResultWriter& resultWriter_;

    HYDRO_VENTILATION_RESULTS ventilationResults_;
}; // class HydroManagement
} // namespace Antares

#endif // __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
