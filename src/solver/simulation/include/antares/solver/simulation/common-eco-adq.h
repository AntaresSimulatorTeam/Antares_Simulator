// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__
#define __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__

#include <vector>

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/random.h"
#include "antares/solver/simulation/solver.h" // for definition of type yearRandomNumbers
#include "antares/solver/variable/economy/all.h"
#include "antares/solver/variable/economy/dispatchable-generation-margin.h" // for OP.MRG
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Simulation
{
// We use the namespace 'economy' here. That means it is mandatory
// that adequacy has the same variable (to get the same type)
typedef Solver::Variable::Economy::VCardBalance AvgExchangeVCardBalance;
typedef Variable::Storage<AvgExchangeVCardBalance>::ResultsType AvgExchangeResults;

/*!
** \brief Compute then random unserved energy cost and the new random hydro virtual cost for all
*areas
**
** This method must be called at the begining of each year even if no calculations
** are performed to be able to make a jump to a given year.
** (hot start)
*/
void PrepareRandomNumbers(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          yearRandomNumbers& randomForYear);

void SetInitialHydroLevel(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          const HYDRO_VENTILATION_RESULTS& hydroVentilationResults);

void BuildThermalPartOfWeeklyProblem(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     const int PasDeTempsDebut,
                                     const std::vector<std::vector<double>>& thermalNoises,
                                     unsigned int year);

/*!
** \brief Get if the quadratic optimization should be used according
**  to the input data (eco+adq)
**
** This method check for non-null impedances in links. If a non-null impedance
** is found, we have to launch the quadratic optimisation.
** \return True if the quadratic optimisation should be used, false otherwise
*/
bool ShouldUseQuadraticOptimisation(const Data::Study& study);

/*!
** \brief Perform the quadratic optimization (links) (eco+adq)
*/
void ComputeFlowQuad(Data::Study& study,
                     PROBLEME_HEBDO& problem,
                     const std::vector<AvgExchangeResults*>& balance,
                     unsigned int nbWeeks);

/*!
** \brief Hydro Remix
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
** \param hourInYear The hour in the year of the first hour in the current week
*/
void RemixHydroForAllAreas(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Data::Parameters& params,
                           uint numSpace,
                           uint hourInYear,
                           IResultWriter& resultWriter);

/*
** \brief Interpolates water values related to reservoir levels for outputs only
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
*point of weekly simulation)
** \param hourInYear The hour in the year of the first hour in the current week
**
** For any hour, the computed water values are related to the beginning of the hour, not the end.
*/
void interpolateWaterValue(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Date::Calendar& calendar,
                           int hourInTheYear);

/*
** \brief Updating the weekly simulation final reservoir level, to be used as a start for the next
*week.
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
*/
void updatingWeeklyFinalHydroLevel(const Data::AreaList& areas, PROBLEME_HEBDO& problem);

/*
** \brief Compute the MC-years weighted average NTC of a link, over a range of hours
**
** \param study The study
** \param capacities NTC time series of the link, for one direction
** \param tsNumbers TS numbers of the link
** \param firstHour First hour of the range, in [0, HOURS_PER_YEAR[
** \param hourCount Number of hours of the range
** \param avg Weighted average NTC, resized to hourCount. avg[h] refers to hour firstHour + h
*/
void retrieveAverageNTC(const Data::Study& study,
                        const Matrix<>& capacities,
                        const Data::TimeSeriesNumbers& tsNumbers,
                        uint firstHour,
                        uint hourCount,
                        std::vector<double>& avg);

void finalizeOptimizationStatistics(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state);

void prepareClustersInMustRunMode(Data::Study& study,
                                  Data::Area::ScratchMap& scratchmap,
                                  uint year,
                                  Data::SimulationMode mode);

//! Build the reserve-participation index maps in the study runtime from a weekly problem
void buildReserveIndexMaps(Data::Study& study, const PROBLEME_HEBDO& problem);

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__
