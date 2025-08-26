/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __SOLVER_SIMULATION_SOLVER_HXX__
#define __SOLVER_SIMULATION_SOLVER_HXX__

#include <yuni/io/io.h>

#include <antares/antares/fatal-error.h>
#include <antares/date/date.h>
#include <antares/exception/InitializationError.hpp>
#include <antares/logs/logs.h>
#include "antares/concurrency/concurrency.h"
#include "antares/io/outputs/SimulationTableCsv.h"
#include "antares/solver/hydro/management/HydroInputsChecker.h"
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/simulation/numspace_manager.h"
#include "antares/solver/simulation/opt_time_writer.h"
#include "antares/solver/simulation/timeseries-numbers.h"
#include "antares/solver/ts-generator/generator.h"
#include "antares/solver/variable/print.h"

namespace Antares::Solver::Simulation
{

template<class Impl>
class yearJob
{
public:
    yearJob(ISimulation<Impl>* simulation,
            unsigned int pY,
            std::map<uint, bool>& pYearsFailed,
            NumSpaceManager& numspaceManager,
            randomNumbers& pRandomForParallelYears,
            Data::Study& pStudy,
            std::vector<Variable::State>& pStates,
            bool pYearByYear,
            Benchmarking::DurationCollector& durationCollector,
            IResultWriter& resultWriter,
            ISimulationObserver& simulationObserver,
            std::mutex& aggregationMutex):
        simulation_(simulation),
        y(pY),
        yearsFailed(pYearsFailed),
        numspaceManager(numspaceManager),
        randomForParallelYears(pRandomForParallelYears),
        study(pStudy),
        states(pStates),
        yearByYear(pYearByYear),
        pDurationCollector(durationCollector),
        pResultWriter(resultWriter),
        simulationObserver_(simulationObserver),
        hydroManagement(study.areas, study.parameters, study.calendar, resultWriter),
        aggregationMutex(aggregationMutex)
    {
    }

    yearJob(const yearJob&) = delete;
    yearJob& operator=(const yearJob&) = delete;
    ~yearJob() = default;

private:
    ISimulation<Impl>* simulation_;
    unsigned int y;
    std::map<uint, bool>& yearsFailed;
    NumSpaceManager& numspaceManager;
    randomNumbers& randomForParallelYears;
    Data::Study& study;
    std::vector<Variable::State>& states;
    bool yearByYear;
    Benchmarking::DurationCollector& pDurationCollector;
    IResultWriter& pResultWriter;
    std::reference_wrapper<ISimulationObserver> simulationObserver_;
    HydroManagement hydroManagement;
    std::mutex& aggregationMutex;

private:
    /*
    ** \brief Log failed week
    **
    ** \param y     MC Year
    ** \param study The Antares study
    ** \param failedWeek List of failing week
    */
    void logFailedWeek(int y, const Data::Study& study, const std::list<uint>& failedWeekList)
    {
        if (!failedWeekList.empty())
        {
            std::stringstream failedWeekStr;
            std::ranges::copy(failedWeekList, std::ostream_iterator<int>(failedWeekStr, " "));

            std::string s = failedWeekStr.str();
            s = s.substr(0, s.length() - 1); // get rid of the trailing space

            std::string failedStr = failedWeekList.size() != 1 ? " failed at weeks "
                                                               : " failed at week ";

            logs.info(); // empty line

            if (Data::stopSimulation(study.parameters.include.unfeasibleProblemBehavior))
            {
                logs.fatal() << "Year " << y + 1 << failedStr << s << ".";
            }
            else
            {
                logs.warning() << "Year " << y + 1 << failedStr << s << ".";
            }
        }
    }

public:
    void operator()()
    {
        Progression::Task progression(study, y, Solver::Progression::sectYear);

        // Index of the current year in the list of structures
        uint indexYear = randomForParallelYears.yearNumberToIndex[y];

        // Getting random tables for this year
        yearRandomNumbers& randomForCurrentYear = randomForParallelYears.pYears[indexYear];

        // 1 - Applying random levels for current year
        auto randomReservoirLevel = randomForCurrentYear.pReservoirLevels;

        // 2 - Getting the numpspace and scratchMap associated to the current year
        unsigned numSpace = numspaceManager.getAvailableNumSpace();
        logs.info() << "Year " << y + 1 << " started";
        logs.debug() << "year " << y + 1 << " received numSpace " << numSpace;

        Antares::Data::Area::ScratchMap scratchmap = study.areas.buildScratchMap(numSpace);

        // 3 - Preparing data related to Clusters in 'must-run' mode
        simulation_->prepareClustersInMustRunMode(scratchmap, y);

        // 4 - Hydraulic ventilation
        pDurationCollector("hydro_ventilation") << [this, &scratchmap, &randomReservoirLevel]
        { hydroManagement.makeVentilation(randomReservoirLevel, y, scratchmap); };

        // Updating the state
        auto& state = states[numSpace];
        state.year = y;

        // 5 - Resetting all variables for the output
        simulation_->variables.yearBegin(y, numSpace);

        // 6 - The Solver itself
        std::list<uint> failedWeekList;

        OptimizationStatisticsWriter optWriter(pResultWriter, y);
        bool yearFailed = !simulation_->year(progression,
                                             state,
                                             numSpace,
                                             randomForCurrentYear,
                                             failedWeekList,
                                             hydroManagement.ventilationResults(),
                                             optWriter,
                                             scratchmap);
        auto& simTable = simulation_->getSimulationTable(numSpace);

        auto buffers = simTable.buffers();
        simTable.clear();

        simulation_->storeYearBuffers(y, std::move(buffers.first), std::move(buffers.second));

        // Log failing weeks
        logFailedWeek(y, study, failedWeekList);

        simulation_->variables.yearEndBuild(state, y, numSpace);

        // 7 - End of the year, this is the last stade where the variables can retrieve
        // their data for this year.
        simulation_->variables.yearEnd(y, numSpace);

        // 8 - Spatial clusters
        // Notifying all variables to perform spatial aggregates.
        // This must be done only when all variables have finished to compute their
        // data for the year.
        simulation_->variables.yearEndSpatialAggregates(simulation_->variables, y, numSpace);

        // 9 - Write results for the current year
        if (yearByYear)
        {
            pDurationCollector("yby_export") << [this, &numSpace]
            {
                // Before writing, some variable may require minor modifications
                simulation_->variables.beforeYearByYearExport(y, numSpace);
                // writing the results for the current year into the output
                simulation_->writeResults(false, y, numSpace); // false for synthesis
            };
        }

        // 10 - Synthesis results
        // Computing the summary : adding the contribution of MC years
        // previously computed in parallel
        aggregationMutex.lock();
        yearsFailed[y] = yearFailed;

        simulation_->variables.computeSummary(y, numSpace);

        // Computing summary of spatial aggregations
        simulation_->variables.computeSpatialAggregatesSummary(simulation_->variables, y, numSpace);

        // Computes statistics on annual (system and solution) costs, to be printed in output
        // into separate files
        simulation_->computeAnnualCostsStatistics(state);

        logs.debug() << "year " << y + 1 << " ended and returned numSpace " << numSpace;
        numspaceManager.freeNumSpace(numSpace);
        simulation_->incrementProgression(progression);

        aggregationMutex.unlock();

    } // End of onExecute() method
};

template<class ImplementationType>
inline ISimulation<ImplementationType>::ISimulation(
  Data::Study& study,
  const ::Settings& settings,
  Benchmarking::DurationCollector& duration_collector,
  IResultWriter& resultWriter,
  Simulation::ISimulationObserver& simulationObserver):
    ImplementationType(study, resultWriter, simulationObserver),
    study(study),
    settings(settings),
    pNbMaxPerformedYearsInParallel(0),
    pYearByYear(study.parameters.yearByYear),
    pDurationCollector(duration_collector),
    pQueueService(study.pQueueService),
    pResultWriter(resultWriter),
    simulationObserver_(simulationObserver)
{
    // Ask to the interface to show the messages
    logs.info();
    logs.info() << LOG_UI_DISPLAY_MESSAGES_ON;

    // Running !
    logs.checkpoint() << "Running the simulation (" << ImplementationType::Name() << ')';
    logs.info() << "Allocating resources...";

    if (pYearByYear && (settings.noOutput || settings.tsGeneratorsOnly))
    {
        pYearByYear = false;
    }
}

template<class ImplementationType>
inline void ISimulation<ImplementationType>::checkWriter() const
{
    // The zip writer needs a queue service (async mutexed write)
    if (!pQueueService && pResultWriter.needsTheJobQueue())
    {
        throw Solver::Initialization::Error::NoQueueService();
    }
}

template<class ImplementationType>
inline ISimulation<ImplementationType>::~ISimulation()
{
}

template<class ImplementationType>
void ISimulation<ImplementationType>::run()
{
    pNbMaxPerformedYearsInParallel = study.maxNbYearsInParallel;

    // Initialize all data
    ImplementationType::variables.initializeFromStudy(study);
    // Computing the max number columns a report of any kind can contain.
    study.parameters.variablesPrintInfo.computeMaxColumnsCountInReports();

    logs.info() << "Allocating resources...";

    // Memory usage
    {
        Variable::PrintInfosStdCout c;
        ImplementationType::variables.template provideInformations<Variable::PrintInfosStdCout>(c);
    }

    ImplementationType::setNbPerformedYearsInParallel(pNbMaxPerformedYearsInParallel);

    if (settings.tsGeneratorsOnly)
    {
        // Only the preprocessors can be used
        // We only have to regenerate time-series according the settings
        // in general data of the study.
        logs.info() << " Only the preprocessors are enabled.";

        regenerateTimeSeries();

        // Destroy the TS Generators if any
        // It will export the time-series into the output at the same time
        TSGenerator::DestroyAll(study);
    }
    else
    {
        // Export ts-numbers into output
        TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(study, pResultWriter);

        if (not ImplementationType::simulationBegin())
        {
            return;
        }
        // Allocating the memory
        ImplementationType::variables.simulationBegin();

        // For beauty
        logs.info();

        // Launching the simulation for all years
        logs.info() << "MC-Years : [" << (study.runtime.rangeLimits.year[Data::rangeBegin] + 1)
                    << " .. " << (1 + study.runtime.rangeLimits.year[Data::rangeEnd])
                    << "], total: " << study.runtime.rangeLimits.year[Data::rangeCount];

        // Current state
        std::vector<Variable::State> state(pNbMaxPerformedYearsInParallel, Variable::State(study));
        // Initializing states for parallel actually performed years
        for (uint numSpace = 0; numSpace != pNbMaxPerformedYearsInParallel; ++numSpace)
        {
            ImplementationType::initializeState(state[numSpace], numSpace);
        }

        uint finalYear = 1 + study.runtime.rangeLimits.year[Data::rangeEnd];
        {
            pDurationCollector("mc_years")
              << [finalYear, &state, this] { loopThroughYears(0, finalYear, state); };
        }
        // Destroy the TS Generators if any
        // It will export the time-series into the output in the same time
        TSGenerator::DestroyAll(study);

        // Post operations
        pDurationCollector("post_processing") << [this] { ImplementationType::simulationEnd(); };

        ImplementationType::variables.simulationEnd();

        // Spatial clusters
        // Notifying all variables to perform the final spatial clusters.
        // This must be done only when all variables have finished to compute their
        // own data.
        ImplementationType::variables.simulationEndSpatialAggregates(ImplementationType::variables);
    }
}

template<class ImplementationType>
void ISimulation<ImplementationType>::writeResults(bool synthesis, uint year, uint numSpace)
{
    using namespace Yuni;

    // The writer might need the job queue, after it's been stopped
    // this is the case e.g if synthesis == true (writing mc-all)
    // Don't restart the queue if the writer doesn't need it

    assert(!settings.noOutput);
    assert(!settings.tsGeneratorsOnly);

    if (!pNbYearsReallyPerformed)
    {
        logs.info();
        logs.info() << "The writing of the output results is disabled.";
        logs.info();
    }
    else
    {
        if (synthesis)
        {
            const auto& parameters = study.parameters;
            if (not parameters.synthesis) // disabled by parameters
            {
                logs.info() << "The simulation synthesis is disabled.";
                return;
            }
        }

        // The target folder
        String newPath;
        newPath << ImplementationType::Name() << IO::Separator;
        if (synthesis)
        {
            newPath << "mc-all";
        }
        else
        {
            CString<10, false> tmp;
            tmp = (year + 1);
            newPath << "mc-ind" << IO::Separator << "00000";
            newPath.overwriteRight(tmp);
        }

        // Dumping
        ImplementationType::variables.exportSurveyResults(synthesis,
                                                          newPath,
                                                          numSpace,
                                                          pResultWriter);
    }
}

template<class ImplementationType>
void ISimulation<ImplementationType>::regenerateTimeSeries()
{
    // A preprocessor can be launched for several reasons:
    // * The option "Preprocessor" is checked in the interface _and_ year == 0
    // * Both options "Preprocessor" and "Refresh" are checked in the interface
    //   _and_ the refresh must be done for the given year (always done for the first year).
    using namespace TSGenerator;

    const Data::Parameters& p = study.parameters;
    // Load
    if (Data::timeSeriesLoad & p.timeSeriesToGenerate)
    {
        pDurationCollector("tsgen_load")
          << [this] { GenerateTimeSeries<Data::timeSeriesLoad>(study, pResultWriter); };
    }
    // Solar
    if (Data::timeSeriesSolar & p.timeSeriesToGenerate)
    {
        pDurationCollector("tsgen_solar")
          << [this] { GenerateTimeSeries<Data::timeSeriesSolar>(study, pResultWriter); };
    }
    // Wind
    if (Data::timeSeriesWind & p.timeSeriesToGenerate)
    {
        pDurationCollector("tsgen_wind")
          << [this] { GenerateTimeSeries<Data::timeSeriesWind>(study, pResultWriter); };
    }
    // Hydro
    if (Data::timeSeriesHydro & p.timeSeriesToGenerate)
    {
        pDurationCollector("tsgen_hydro")
          << [this] { GenerateTimeSeries<Data::timeSeriesHydro>(study, pResultWriter); };
    }

    pDurationCollector("tsgen_thermal") << [this]
    {
        bool globalThermalTSgeneration = study.parameters.timeSeriesToGenerate
                                         & Data::timeSeriesThermal;
        auto clusters = getAllClustersToGen(study.areas, globalThermalTSgeneration);
        generateThermalTimeSeries(study, clusters, study.runtime.random[Data::seedTsGenThermal]);

        bool archive = study.parameters.timeSeriesToArchive & Data::timeSeriesThermal;
        bool doWeWrite = archive && !study.parameters.noOutput;
        if (doWeWrite)
        {
            fs::path savePath = study.folderOutput / "ts-generator" / "thermal" / "mc-" / "0";
            writeThermalTimeSeries(clusters, savePath);
        }

        // apply the spinning if we generated some in memory clusters
        for (auto* cluster: clusters)
        {
            cluster->calculationOfSpinning();
        }
    };
}

template<class ImplementationType>
void ISimulation<ImplementationType>::allocateMemoryForRandomNumbers(
  randomNumbers& randomForParallelYears)
{
    uint maxNbPerformedYears = randomForParallelYears.pMaxNbPerformedYears;
    uint nbAreas = study.areas.size();

    for (uint y = 0; y < maxNbPerformedYears; y++)
    {
        // General :
        randomForParallelYears.pYears[y].setNbAreas(nbAreas);
        randomForParallelYears.pYears[y].pNbClustersByArea.resize(nbAreas);

        // Thermal noises :
        randomForParallelYears.pYears[y].pThermalNoisesByArea.resize(nbAreas);

        for (uint a = 0; a != nbAreas; ++a)
        {
            // logs.info() << "   area : " << a << " :";
            auto& area = *(study.areas.byIndex[a]);
            size_t nbClusters = area.thermal.list.allClustersCount();
            randomForParallelYears.pYears[y].pThermalNoisesByArea[a].resize(nbClusters);
            randomForParallelYears.pYears[y].pNbClustersByArea[a] = nbClusters;
        }

        // Reservoir levels
        randomForParallelYears.pYears[y].pReservoirLevels.resize(nbAreas);

        // Noises on unsupplied and spilled energy
        randomForParallelYears.pYears[y].pUnsuppliedEnergy.resize(nbAreas);
        randomForParallelYears.pYears[y].pSpilledEnergy.resize(nbAreas);

        // Hydro costs noises
        switch (study.parameters.power.fluctuations)
        {
        case Data::lssFreeModulations:
        {
            randomForParallelYears.pYears[y].pHydroCostsByArea_freeMod.resize(nbAreas);
            for (uint a = 0; a != nbAreas; ++a)
            {
                randomForParallelYears.pYears[y].pHydroCostsByArea_freeMod[a].resize(8784);
            }
            break;
        }
        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            randomForParallelYears.pYears[y].pHydroCosts_rampingOrExcursion.resize(nbAreas);
            break;
        }
        case Data::lssUnknown:
        {
            logs.error() << "Power fluctuation unknown";
            break;
        }
        }
    } // End loop over years
}

template<class ImplementationType>
void ISimulation<ImplementationType>::computeRandomNumbers(
  randomNumbers& randomForYears,
  unsigned nbYears,
  std::map<unsigned int, bool>& isYearPerformed,
  MersenneTwister& randomHydroGenerator)
{
    uint indexYear = 0;

    for (unsigned y = 0; y < nbYears; ++y)
    {
        bool isPerformed = isYearPerformed[y];
        if (isPerformed)
        {
            randomForYears.yearNumberToIndex[y] = indexYear;
        }

        // General
        const unsigned int nbAreas = study.areas.size();

        // ... Thermal noise ...
        for (unsigned int a = 0; a != nbAreas; ++a)
        {
            const auto& area = *(study.areas.byIndex[a]);

            for (auto& cluster: area.thermal.list.all())
            {
                uint clusterIndex = cluster->areaWideIndex;
                double thermalNoise = study.runtime.random[Data::seedThermalCosts].next();
                if (isPerformed)
                {
                    randomForYears.pYears[indexYear].pThermalNoisesByArea[a][clusterIndex]
                      = thermalNoise;
                }
            }
        }

        // ... Reservoir levels ...
        uint areaIndex = 0;
        study.areas.each(
          [&areaIndex, &indexYear, &randomForYears, &randomHydroGenerator, &y, &isPerformed, this](
            Data::Area& area)
          {
              // looking for the initial reservoir level (begining of the year)
              auto& min = area.hydro.reservoirLevel[Data::PartHydro::minimum];
              auto& avg = area.hydro.reservoirLevel[Data::PartHydro::average];
              auto& max = area.hydro.reservoirLevel[Data::PartHydro::maximum];

              // Month the reservoir level is initialized according to.
              // This month number is given in the civil calendar, from january to december (0 is
              // january).
              int initResLevelOnMonth = area.hydro.initializeReservoirLevelDate;

              // Conversion of the previous month into simulation calendar
              int initResLevelOnSimMonth = study.calendar.mapping.months[initResLevelOnMonth];

              // Previous month's first day in the year
              int firstDayOfMonth = study.calendar.months[initResLevelOnSimMonth].daysYear.first;

              double randomLevel = randomReservoirLevel(min[firstDayOfMonth],
                                                        avg[firstDayOfMonth],
                                                        max[firstDayOfMonth],
                                                        randomHydroGenerator);

              // Possibly update the intial level from scenario builder
              if (study.parameters.useCustomScenario)
              {
                  double levelFromScenarioBuilder = study.scenarioInitialHydroLevels[areaIndex][y];
                  if (levelFromScenarioBuilder >= 0.)
                  {
                      randomLevel = levelFromScenarioBuilder;
                  }
              }

              // Current area's hydro starting (or initial) level computation
              // (no matter if the year is performed or not, we always draw a random initial
              // reservoir level to ensure the same results)
              if (isPerformed)
              {
                  randomForYears.pYears[indexYear].pReservoirLevels[areaIndex] = randomLevel;
              }

              areaIndex++;
          }); // each area

        // ... Unsupplied and spilled energy costs noises (french : bruits sur la defaillance
        // positive et negatives) ... references to the random number generators
        auto& randomUnsupplied = study.runtime.random[Data::seedUnsuppliedEnergyCosts];
        auto& randomSpilled = study.runtime.random[Data::seedSpilledEnergyCosts];

        int currentSpilledEnergySeed = study.parameters.seed[Data::seedSpilledEnergyCosts];
        int defaultSpilledEnergySeed = Data::antaresSeedDefaultValue
                                       + Data::seedSpilledEnergyCosts * Data::antaresSeedIncrement;
        bool SpilledEnergySeedIsDefault = (currentSpilledEnergySeed == defaultSpilledEnergySeed);
        areaIndex = 0;
        study.areas.each(
          [&isPerformed,
           &areaIndex,
           &randomUnsupplied,
           &randomSpilled,
           &randomForYears,
           &indexYear,
           &SpilledEnergySeedIsDefault](Data::Area& area)
          {
              (void)area; // Avoiding warnings at compilation (unused variable) on linux
              if (isPerformed)
              {
                  double randomNumber = randomUnsupplied();
                  randomForYears.pYears[indexYear].pUnsuppliedEnergy[areaIndex] = randomNumber;
                  randomForYears.pYears[indexYear].pSpilledEnergy[areaIndex] = randomNumber;
                  if (!SpilledEnergySeedIsDefault)
                  {
                      randomForYears.pYears[indexYear].pSpilledEnergy[areaIndex] = randomSpilled();
                  }
              }
              else
              {
                  randomUnsupplied();
                  if (!SpilledEnergySeedIsDefault)
                  {
                      randomSpilled();
                  }
              }

              areaIndex++;
          }); // each area

        // ... Hydro costs noises ...
        auto& randomHydro = study.runtime.random[Data::seedHydroCosts];

        Data::PowerFluctuations powerFluctuations = study.parameters.power.fluctuations;
        switch (powerFluctuations)
        {
        case Data::lssFreeModulations:
        {
            areaIndex = 0;
            auto end = study.areas.end();

            // Computing hourly hydro costs noises so that they are homogeneously spread into :
            // [-1.e-3, -5*1.e-4] U [+5*1.e-4, +1.e-3]
            if (isPerformed)
            {
                for (auto i = study.areas.begin(); i != end; ++i)
                {
                    auto& noise = randomForYears.pYears[indexYear]
                                    .pHydroCostsByArea_freeMod[areaIndex];
                    std::set<hydroCostNoise, compareHydroCostsNoises> setHydroCostsNoises;
                    for (uint j = 0; j != 8784; ++j)
                    {
                        noise[j] = randomHydro();
                        noise[j] -= 0.5; // Now we have : -0.5 < noise[j] < +0.5

                        // This std::set naturally sorts the hydro costs noises into increasing
                        // absolute values order
                        setHydroCostsNoises.insert(hydroCostNoise(noise[j], j));
                    }

                    uint rank = 0;
                    std::set<hydroCostNoise, compareHydroCostsNoises>::iterator it;
                    for (it = setHydroCostsNoises.begin(); it != setHydroCostsNoises.end(); it++)
                    {
                        uint index = it->getIndex();
                        double value = it->getValue();

                        if (value < 0.)
                        {
                            noise[index] = -5 * 1.e-4 * (1 + rank / 8784.);
                        }
                        else
                        {
                            noise[index] = 5 * 1.e-4 * (1 + rank / 8784.);
                        }

                        rank++;
                    }

                    areaIndex++;
                }
            }
            else
            {
                for (auto i = study.areas.begin(); i != end; ++i)
                {
                    for (uint j = 0; j != 8784; ++j)
                    {
                        randomHydro();
                    }
                }
            }

            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            areaIndex = 0;
            auto end = study.areas.end();
            for (auto i = study.areas.begin(); i != end; ++i)
            {
                if (isPerformed)
                {
                    randomForYears.pYears[indexYear].pHydroCosts_rampingOrExcursion[areaIndex]
                      = randomHydro();
                }
                else
                {
                    randomHydro();
                }

                areaIndex++;
            }
            break;
        }

        case Data::lssUnknown:
        {
            logs.error() << "Power fluctuation unknown";
            break;
        }

        } // end of switch

        if (isPerformed)
        {
            indexYear++;
        }

    } // End loop over years
} // End function

template<class ImplementationType>
void ISimulation<ImplementationType>::computeAnnualCostsStatistics(Variable::State s)
{
    pAnnualStatistics.systemCost.addCost(s.annualSystemCost);
    pAnnualStatistics.criterionCost1.addCost(s.optimalSolutionCost1);
    pAnnualStatistics.criterionCost2.addCost(s.optimalSolutionCost2);
    pAnnualStatistics.optimizationTime1.addCost(s.averageOptimizationTime1);
    pAnnualStatistics.optimizationTime2.addCost(s.averageOptimizationTime2);
    pAnnualStatistics.updateTime.addCost(s.averageUpdateTime);
}

template<class ImplementationType>
void ISimulation<ImplementationType>::loopThroughYears(uint firstYear,
                                                       uint endYear,
                                                       std::vector<Variable::State>& state)
{
    assert(endYear <= study.parameters.nbYears);

    // Init random hydro
    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study.parameters.seed[Data::seedHydroManagement]);

    // List of parallel years sets
    std::vector<setOfParallelYears> setsOfParallelYears;

    // Number of threads to perform the jobs waiting in the queue
    pQueueService->maximumThreadCount(pNbMaxPerformedYearsInParallel);

    regenerateTimeSeries();

    HydroInputsChecker hydroInputsChecker(study);
    logs.info() << " Doing hydro validation";

    // Selecting the years to be performed
    std::map<unsigned, bool> yearsFailed;
    std::map<unsigned, bool> isYearPerformed;
    pNbYearsReallyPerformed = 0;
    for (uint year = firstYear; year < endYear; ++year)
    {
        isYearPerformed[year] = study.parameters.yearsFilter[year];
        if (study.parameters.yearsFilter[year])
        {
            ++pNbYearsReallyPerformed;
        }
        else
        {
            logs.debug() << "Ignoring year " << year + 1 << ": not in the playlist";
            yearsFailed[year] = false;
        }
    }

    if (pNbYearsReallyPerformed != endYear)
    {
        logs.info() << "Playlist detected, " << pNbYearsReallyPerformed
                    << " years to be performed out of " << endYear << " years.";
    }

    logs.info() << " Starting the simulation";

    // Related to annual costs statistics (printed in output into separate files)
    pAnnualStatistics.setNbPerformedYears(pNbYearsReallyPerformed);

    // Container for random numbers of parallel years (to be executed or not)
    randomNumbers randomForParallelYears(pNbYearsReallyPerformed,
                                         study.parameters.power.fluctuations);

    allocateMemoryForRandomNumbers(randomForParallelYears);
    computeRandomNumbers(randomForParallelYears, endYear, isYearPerformed, randomHydroGenerator);

    // hydro checks
    for (uint year = firstYear; year < endYear; ++year)
    {
        if (study.parameters.yearsFilter[year])
        {
            hydroInputsChecker.Execute(year);
        }
    }
    hydroInputsChecker.CheckForErrors();

    NumSpaceManager numspaceManager(pNbMaxPerformedYearsInParallel);

    Concurrency::FutureSet results;
    std::mutex aggregationMutex;
    for (uint year = firstYear; year < endYear; ++year)
    {
        if (study.parameters.yearsFilter[year])
        {
            // If the year has not to be rerun, we skip the computation of the year.
            // Note that, when we enter for the first time in the "for" loop, all years of the set
            // have to be rerun (meaning : they must be run once). if(!batch.yearsFailed[y])
            // continue;

            auto task = std::make_shared<yearJob<ImplementationType>>(this,
                                                                      year,
                                                                      yearsFailed,
                                                                      numspaceManager,
                                                                      randomForParallelYears,
                                                                      study,
                                                                      state,
                                                                      pYearByYear,
                                                                      pDurationCollector,
                                                                      pResultWriter,
                                                                      simulationObserver_.get(),
                                                                      aggregationMutex);
            results.add(Concurrency::AddTask(*pQueueService, task));
        }
    }

    pQueueService->start();

    pQueueService->wait(Yuni::qseIdle);
    pQueueService->stop();
    aggregateAndWriteSimulationTables();
    results.join();
    pResultWriter.flush();
    // On regarde si au moins une année du lot n'a pas trouvé de solution
    for (auto& [year, failed]: yearsFailed)
    {
        // Si une année du lot d'années n'a pas trouvé de solution, on arrête tout
        if (failed)
        {
            std::ostringstream msg;
            msg << "Year " << year + 1 << " has failed in the previous set of parallel year.";
            throw FatalError(msg.str());
        }
    }

    // Set to zero the random numbers of all parallel years
    randomForParallelYears.reset();

    // Writing annual costs statistics
    pAnnualStatistics.endStandardDeviations();
    pAnnualStatistics.writeToOutput(pResultWriter);
}

template<class ImplementationType>
void ISimulation<ImplementationType>::storeYearBuffers(uint year,
                                                       std::string&& firstBuffer,
                                                       std::string&& secondBuffer)
{
    std::lock_guard lock(buffersMutex_);
    yearSimulationBuffers_.emplace(year,
                                   std::pair{std::move(firstBuffer), std::move(secondBuffer)});
}

template<class ImplementationType>
void ISimulation<ImplementationType>::aggregateAndWriteSimulationTables()
{
    std::lock_guard lock(buffersMutex_);
    std::string globalFirstBuffer;
    std::string globalSecondBuffer;
    // dans l'ordre des années
    for (uint year = 0; year < study.parameters.nbYears; ++year)
    {
        auto it = yearSimulationBuffers_.find(year);
        if (it != yearSimulationBuffers_.end())
        {
            globalFirstBuffer += it->second.first;
            globalSecondBuffer += it->second.second;
        }
    }
    const auto header = ImplementationType::getSimulationTableHeader() + "\n";
    if (!globalFirstBuffer.empty())
    {
        std::string writerEntry = header + std::move(globalFirstBuffer);
        pResultWriter.addEntryFromBuffer("simulation_table--optim-nb-1.csv", writerEntry);
    }
    if (!globalSecondBuffer.empty())
    {
        std::string writerEntry = header + std::move(globalSecondBuffer);
        pResultWriter.addEntryFromBuffer("simulation_table--optim-nb-2.csv", writerEntry);
    }

    yearSimulationBuffers_.clear();
}

template<class ImplementationType>
OptimisationsSimulationTable& ISimulation<ImplementationType>::getSimulationTable(uint numSpace)
{
    // Cette méthode doit être implémentée dans la classe dérivée (Economy)
    // pour retourner la table spécifique à l'espace numérique
    return ImplementationType::getSimulationTable(numSpace);
}
} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_SOLVER_HXX__
