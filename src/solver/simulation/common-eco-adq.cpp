// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/common-eco-adq.h"

#include <cassert>
#include <cmath>
#include <map>
#include <memory>
#include <ranges>

#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/logs/logs.h>
#include <antares/solver/simulation/reserve-index-maps.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/study/study.h>

namespace Antares::Solver::Simulation
{
static void RecalculDesEchangesMoyens(Data::Study& study,
                                      PROBLEME_HEBDO& problem,
                                      const std::vector<AvgExchangeResults*>& balance,
                                      int PasDeTempsDebut)
{
    const uint linkCount = study.runtime.interconnectionsCount();
    const uint hourCount = static_cast<uint>(problem.NombreDePasDeTemps);

    // The average NTC only depends on the link and on the hour, not on the time step being filled
    // in : compute it once per link for the whole week.
    std::vector<std::vector<double>> avgDirect(linkCount);
    std::vector<std::vector<double>> avgIndirect(linkCount);
    for (uint j = 0; j < linkCount; ++j)
    {
        const auto* link = study.runtime.areaLink[j];
        retrieveAverageNTC(study,
                           link->directCapacities.timeSeries,
                           link->timeseriesNumbers,
                           PasDeTempsDebut,
                           hourCount,
                           avgDirect[j]);
        retrieveAverageNTC(study,
                           link->indirectCapacities.timeSeries,
                           link->timeseriesNumbers,
                           PasDeTempsDebut,
                           hourCount,
                           avgIndirect[j]);
    }

    for (uint i = 0; i < hourCount; i++)
    {
        auto& ntcValues = problem.ValeursDeNTC[i];
        uint decalPasDeTemps = PasDeTempsDebut + i;

        for (uint j = 0; j < study.areas.size(); ++j)
        {
            assert(balance[j] && "Impossible to find the variable");
            if (balance[j])
            {
                problem.SoldeMoyenHoraire[i].SoldeMoyenDuPays[j] = balance[j]
                                                                     ->avgdata()
                                                                     .hourly[decalPasDeTemps];
            }
            else
            {
                assert(false && "invalid balance");
                problem.SoldeMoyenHoraire[i].SoldeMoyenDuPays[j] = 0.0;
            }
        }

        for (uint j = 0; j < linkCount; ++j)
        {
            ntcValues.ValeurDeNTCOrigineVersExtremite[j] = avgDirect[j][i];
            ntcValues.ValeurDeNTCExtremiteVersOrigine[j] = avgIndirect[j][i];

            const auto& mtxParamaters = study.runtime.areaLink[j]->parameters;
            ntcValues.ResistanceApparente[j] = mtxParamaters[Data::fhlImpedances][decalPasDeTemps];
        }
    }

    try
    {
        OPT_OptimisationHebdomadaireQuadratique(study.parameters.optOptions, &problem);
    }
    catch (Data::UnfeasibleProblemError&)
    {
        study.runtime.quadraticOptimizationHasFailed = true;
    }

    for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
    {
        const uint indx = i + PasDeTempsDebut;
        auto& ntcValues = problem.ValeursDeNTC[i];

        for (uint j = 0; j < study.runtime.interconnectionsCount(); ++j)
        {
            study.runtime.transitMoyenInterconnexionsRecalculQuadratique[j][indx] = ntcValues
                                                                                      .ValeurDuFlux
                                                                                        [j];
        }
    }
}

bool ShouldUseQuadraticOptimisation(const Data::Study& study)
{
    const bool flowQuadEnabled = study.parameters.variablesPrintInfo.isPrinted("FLOW QUAD.");
    if (!flowQuadEnabled)
    {
        return false;
    }

    for (uint j = 0; j < study.runtime.interconnectionsCount(); ++j)
    {
        auto& lnk = *(study.runtime.areaLink[j]);
        auto& impedances = lnk.parameters[Data::fhlImpedances];

        for (uint hour = 0; hour < HOURS_PER_YEAR; ++hour)
        {
            if (std::abs(impedances[hour]) >= 1e-100)
            {
                return true;
            }
        }
    }
    return false;
}

void ComputeFlowQuad(Data::Study& study,
                     PROBLEME_HEBDO& problem,
                     const std::vector<AvgExchangeResults*>& balance,
                     uint nbWeeks)
{
    uint startTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;

    if (ShouldUseQuadraticOptimisation(study))
    {
        logs.info() << "Post-processing... (quadratic optimisation)";

        problem.LeProblemeADejaEteInstancie = false;
        for (uint w = 0; w != nbWeeks; ++w)
        {
            int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
            RecalculDesEchangesMoyens(study, problem, balance, PasDeTempsDebut);
        }
    }
    else
    {
        logs.info() << "  The quadratic optimisation has been skipped";

        for (uint j = 0; j < study.runtime.interconnectionsCount(); ++j)
        {
            for (uint w = 0; w != nbWeeks; ++w)
            {
                int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
                for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
                {
                    const uint indx = i + PasDeTempsDebut;
                    study.runtime.transitMoyenInterconnexionsRecalculQuadratique[j][indx] = 0;
                }
            }
        }
    }
}

void PrepareRandomNumbers(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          yearRandomNumbers& randomForYear)
{
    uint indexArea = 0;
    study.areas.each(
      [&study, &problem, &randomForYear, &indexArea](const Data::Area& area)
      {
          double rnd = 0.;

          rnd = randomForYear.pUnsuppliedEnergy[indexArea];

          double alea;

          if (area.spreadUnsuppliedEnergyCost == 0)
          {
              if (rnd < 0.5)
              {
                  alea = 1.e-4 * (5 + 2 * rnd);
              }
              else
              {
                  alea = -1.e-4 * (5 + 2 * (rnd - 0.5));
              }
          }
          else
          {
              alea = (rnd - 0.5) * (area.spreadUnsuppliedEnergyCost);

              if (std::abs(alea) < 5.e-4)
              {
                  if (alea >= 0)
                  {
                      alea += 5.e-4;
                  }
                  else
                  {
                      alea -= 5.e-4;
                  }
              }
          }
          problem.CoutDeDefaillancePositive[area.index] = area.thermal.unsuppliedEnergyCost + alea;

          rnd = randomForYear.pSpilledEnergy[indexArea];

          if (area.spreadSpilledEnergyCost == 0)
          {
              if (rnd < 0.5)
              {
                  alea = 1.e-4 * (5 + 2 * rnd);
              }
              else
              {
                  alea = -1.e-4 * (5 + 2 * (rnd - 0.5));
              }
          }
          else
          {
              alea = (rnd - 0.5) * (area.spreadSpilledEnergyCost);

              if (std::abs(alea) < 5.e-4)
              {
                  if (alea >= 0)
                  {
                      alea += 5.e-4;
                  }
                  else
                  {
                      alea -= 5.e-4;
                  }
              }
          }
          problem.CoutDeDefaillanceNegative[area.index] = area.thermal.spilledEnergyCost + alea;

          //-----------------------------
          // Thermal noises
          //-----------------------------
          for (auto& cluster: area.thermal.list.each_enabled())
          {
              // we use the areaWideIndex because the thermal noises are randomly calculated
              // for every cluster to avoid different results if a cluster is deactivated
              uint clusterIndex = cluster->areaWideIndex;
              double& rnd = randomForYear.pThermalNoisesByArea[indexArea][clusterIndex];
              double randomClusterProdCost(0.);
              if (cluster->spreadCost == 0) // 5e-4 < |randomClusterProdCost| < 6e-4
              {
                  if (rnd < 0.5)
                  {
                      randomClusterProdCost = 1e-4 * (5 + 2 * rnd);
                  }
                  else
                  {
                      randomClusterProdCost = -1e-4 * (5 + 2 * (rnd - 0.5));
                  }
              }
              else
              {
                  randomClusterProdCost = (rnd - 0.5) * (cluster->spreadCost);

                  if (std::abs(randomClusterProdCost) < 5.e-4)
                  {
                      if (std::abs(randomClusterProdCost) >= 0)
                      {
                          randomClusterProdCost += 5.e-4;
                      }
                      else
                      {
                          randomClusterProdCost -= 5.e-4;
                      }
                  }
              }
              rnd = randomClusterProdCost;
          }

          //-----------------------------
          // Hydro noises
          //-----------------------------
          auto& noise = problem.BruitSurCoutHydraulique[area.index];
          switch (study.parameters.power.fluctuations)
          {
          case Data::lssFreeModulations:
          {
              for (uint j = 0; j != 8784; ++j)
              {
                  noise[j] = randomForYear.pHydroCostsByArea_freeMod[indexArea][j];
              }

              auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 5.e-4;
              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 5.e-4;
              break;
          }

          case Data::lssMinimizeRamping:
          case Data::lssMinimizeExcursions:
          {
              std::fill(noise.begin(), noise.end(), 0);

              auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
              double rnd = randomForYear.pHydroCosts_rampingOrExcursion[indexArea];

              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations
                = 0.01 * (1. + rnd / 10.);
              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax
                = 0.1 * (1. + rnd / 100.);
              break;
          }

          case Data::lssUnknown:
          {
              assert(false && "invalid power fluctuations");
              std::fill(noise.begin(), noise.end(), 0);

              auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 1e-4;
              penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 1e-4;
              break;
          }
          }
          indexArea++;
      });
}

void SetInitialHydroLevel(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          const HYDRO_VENTILATION_RESULTS& hydroVentilationResults)
{
    uint firstDaySimu = study.parameters.simulationDays.first;
    study.areas.each(
      [&problem, &firstDaySimu, &hydroVentilationResults](const Data::Area& area)
      {
          bool updatePreviousLevel = area.hydro.reservoirManagement
                                     && (!area.hydro.useHeuristicTarget || area.hydro.useLeeway);
          if (updatePreviousLevel)
          {
              double capacity = area.hydro.reservoirCapacity;

              problem.previousSimulationFinalLevel[area.index] = hydroVentilationResults[area.index]
                                                                   .NiveauxReservoirsDebutJours
                                                                     [firstDaySimu]
                                                                 * capacity;
          }
      });
}

void BuildThermalPartOfWeeklyProblem(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     const int PasDeTempsDebut,
                                     const std::vector<std::vector<double>>& thermalNoises,
                                     unsigned int year)
{
    int hourInYear = PasDeTempsDebut;
    const uint nbPays = study.areas.size();
    for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps;
         ++hourInWeek, ++hourInYear)
    {
        for (uint areaIdx = 0; areaIdx < nbPays; ++areaIdx)
        {
            auto& area = *study.areas.byIndex[areaIdx];
            for (auto& cluster: area.thermal.list.each_enabled_and_not_mustrun())
            {
                auto& Pt = problem.PaliersThermiquesDuPays[areaIdx]
                             .PuissanceDisponibleEtCout[cluster->index];

                Pt.CoutHoraireDeProductionDuPalierThermique[hourInWeek]
                  = cluster->getCostProvider().getMarketBidCost(hourInYear, year)
                    + thermalNoises[areaIdx][cluster->areaWideIndex];

                Pt.PuissanceDisponibleDuPalierThermique[hourInWeek] = cluster->series
                                                                        .getCoefficient(year,
                                                                                        hourInYear);

                Pt.PuissanceMinDuPalierThermique[hourInWeek]
                  = (Pt.PuissanceDisponibleDuPalierThermique[hourInWeek]
                     < cluster->PthetaInf[hourInYear])
                      ? Pt.PuissanceDisponibleDuPalierThermique[hourInWeek]
                      : cluster->PthetaInf[hourInYear];
            }
        }
    }

    for (uint k = 0; k < nbPays; ++k)
    {
        auto& area = *study.areas.byIndex[k];

        for (uint l = 0; l != area.thermal.list.enabledAndNotMustRunCount(); ++l)
        {
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[l]
              .PuissanceDisponibleDuPalierThermiqueRef
              = problem.PaliersThermiquesDuPays[k]
                  .PuissanceDisponibleEtCout[l]
                  .PuissanceDisponibleDuPalierThermique;
        }
    }
}

void retrieveAverageNTC(const Data::Study& study,
                        const Matrix<>& capacities,
                        const Data::TimeSeriesNumbers& tsNumbers,
                        uint firstHour,
                        uint hourCount,
                        std::vector<double>& avg)
{
    const auto& parameters = study.parameters;

    const auto& yearsWeight = parameters.getYearsWeight();
    const auto yearsWeightSum = parameters.getYearsWeightSum();
    const auto& yearsFilter = parameters.yearsFilter;
    const bool singleTS = (capacities.width == 1);

    avg.assign(hourCount, 0.);

    std::map<uint32_t, double> weightOfTS;

    for (uint y = 0; y < parameters.nbYears; y++)
    {
        if (!yearsFilter[y])
        {
            continue;
        }

        uint32_t tsIndex = singleTS ? 0 : tsNumbers[y];
        weightOfTS[tsIndex] += yearsWeight[y];
    }

    // No need for the year number, only the TS index is required
    for (const auto& [tsIndex, weight]: weightOfTS)
    {
        const auto* column = capacities[tsIndex];

        for (uint h = 0; h < hourCount; h++)
        {
            avg[h] += column[firstHour + h] * weight;
        }
    }

    for (auto& value: avg)
    {
        value /= yearsWeightSum;
    }
}

void finalizeOptimizationStatistics(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state)
{
    auto& firstOptStat = problem.optimizationStatistics[0];
    state.averageOptimizationTime1 = firstOptStat.getAverageSolveTime();

    auto& secondOptStat = problem.optimizationStatistics[1];
    state.averageOptimizationTime2 = secondOptStat.getAverageSolveTime();

    state.averageUpdateTime = firstOptStat.getAverageUpdateTime()
                              + secondOptStat.getAverageUpdateTime();

    firstOptStat.reset();
    secondOptStat.reset();
}

void prepareClustersInMustRunMode(Data::Study& study,
                                  Data::Area::ScratchMap& scratchmap,
                                  uint year,
                                  Data::SimulationMode mode)
{
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *study.areas[i];
        auto& scratchpad = scratchmap.at(&area);

        std::ranges::fill(scratchpad.mustrunSum, 0);
        if (mode == Data::SimulationMode::Adequacy)
        {
            std::ranges::fill(scratchpad.originalMustrunSum, 0);
        }

        auto& mrs = scratchpad.mustrunSum;
        auto& adq = scratchpad.originalMustrunSum;

        for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
            {
                mrs[h] += availableProduction[h];
            }
            if (cluster->mustrunOrigin && mode == Data::SimulationMode::Adequacy)
            {
                for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
                {
                    adq[h] += 2 * availableProduction[h]; // Why do we add the available production
                                                          // twice ?
                }
            }
        }
    }
}

void buildReserveIndexMaps(Data::Study& study, const PROBLEME_HEBDO& problem)
{
    auto maps = std::make_shared<ReserveIndexMaps>();
    auto& participationIndexMaps = maps->participationIndexMaps;
    auto& idToName = maps->idToName;

    auto loadReserveParticipations =
      [&](const Data::Area* area, const CAPACITY_RESERVATION& reserve)
    {
        // Thermal clusters
        for (auto& [clusterId, reserveParticipation]: reserve.AllThermalReservesParticipation)
        {
            participationIndexMaps.at(area->id).thermalClusters.insert(
              {{reserve.reserveID, reserveParticipation.clusterName},
               reserveParticipation.areaIndexClusterParticipation});
        }

        // Short Term Storage
        for (auto& [clusterId, reserveParticipation]: reserve.AllSTStorageReservesParticipation)
        {
            participationIndexMaps.at(area->id).STStorageClusters.insert(
              {{reserve.reserveID, reserveParticipation.clusterName},
               reserveParticipation.areaIndexClusterParticipation});
        }

        // Hydro
        for (auto& reserveParticipation: reserve.AllHydroReservesParticipation)
        {
            participationIndexMaps.at(area->id).Hydro.insert(
              {reserve.reserveID, reserveParticipation.areaIndexClusterParticipation});
        }
    };

    for (const auto& area: study.areas | std::views::values)
    {
        participationIndexMaps.emplace(area->id, ReserveIndexMaps::AreaReserveIndexMap{});
        for (const auto& reserve: problem.allReserves.value()[area->index].areaCapacityReservations)
        {
            idToName.try_emplace(reserve.reserveID, reserve.reserveName);
            loadReserveParticipations(area.get(), reserve);
        }
    }

    study.reserveMaps = std::move(maps);
}

} // namespace Antares::Solver::Simulation
