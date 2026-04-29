// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
#define __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__

#include <unordered_map>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/area/area.h>
#include <antares/study/fwd.h>
#include <antares/study/parts/hydro/container.h>
#include "antares/date/date.h"
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
using HydroSpecificMap = std::unordered_map<const Antares::Data::Area*,
                                            Antares::Data::TimeDependantHydroManagementData>;

class HydroManagement final
{
public:
    HydroManagement(const Data::AreaList& areas,
                    const Data::Parameters& params,
                    const Date::Calendar& calendar,
                    Solver::IResultWriter& resultWriter);

    //! Perform the hydro ventilation
    void makeVentilation(const std::vector<double>& randomReservoirLevel,
                         uint y,
                         Antares::Data::Area::ScratchMap& scratchmap);

    const HYDRO_VENTILATION_RESULTS& ventilationResults()
    {
        return ventilationResults_;
    }

private:
    //! Prepare the net demand for each area
    void prepareNetDemand(uint year,
                          Data::SimulationMode mode,
                          const Antares::Data::Area::ScratchMap& scratchmap,
                          HydroSpecificMap& hydro_specific_map);
    //! Prepare the effective demand for each area
    void prepareEffectiveDemand(uint year, HydroSpecificMap& hydro_specific_map) const;
    //! Monthly Optimal generations
    void prepareMonthlyOptimalGenerations(const std::vector<double>& random_reservoir_level,
                                          uint y,
                                          Antares::Data::Area::ScratchMap& scratchmap,
                                          HydroSpecificMap& hydro_specific_map);

    void prepareDailyOptimalGenerations(uint y,
                                        Antares::Data::Area::ScratchMap& scratchmap,
                                        HydroSpecificMap& hydro_specific_map);

    void prepareDailyOptimalGenerations(
      Data::Area& area,
      uint y,
      Antares::Data::Area::ScratchMap& scratchmap,
      Antares::Data::TimeDependantHydroManagementData& hydro_specific);

private:
    const Data::AreaList& areas_;
    const Date::Calendar& calendar_;
    const Data::Parameters& parameters_;
    Solver::IResultWriter& resultWriter_;

    HYDRO_VENTILATION_RESULTS ventilationResults_;
}; // class HydroManagement
} // namespace Antares

#endif // __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
