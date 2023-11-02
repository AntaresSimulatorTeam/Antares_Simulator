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
#ifndef __SOLVER_SIMULATION_SOLVER_HXX__
#define __SOLVER_SIMULATION_SOLVER_HXX__

#include "../variable/constants.h"
#include <antares/logs/logs.h>
#include <antares/date/date.h>
#include <antares/benchmarking/timer.h>
#include <antares/exception/InitializationError.hpp>
#include "../variable/print.h"
#include <yuni/io/io.h>
#include "timeseries-numbers.h"
#include "apply-scenario.h"
#include <antares/fatal-error.h>
#include "../ts-generator/generator.h"
#include "opt_time_writer.h"
#include "../hydro/management.h" // Added for use of randomReservoirLevel(...)

#include <yuni/core/system/suspend.h>
#include <yuni/job/job.h>

#include "antares/concurrency/concurrency.h"

namespace Antares::Solver::Simulation
{

template<class Impl>
class yearJob
{
public:
    yearJob(ISimulation<Impl>* simulation,
            unsigned int pY,
            std::map<uint, bool>& pYearFailed,
            std::map<uint, bool>& pIsFirstPerformedYearOfASet,
            bool pFirstSetParallelWithAPerformedYearWasRun,
            unsigned int pNumSpace,
            randomNumbers& pRandomForParallelYears,
            bool pPerformCalculations,
            Data::Study& pStudy,
            std::vector<Variable::State>& pState,
            bool pYearByYear,
            Benchmarking::IDurationCollector& durationCollector,
            IResultWriter& resultWriter) :
     simulation_(simulation),
     y(pY),
     yearFailed(pYearFailed),
     isFirstPerformedYearOfASet(pIsFirstPerformedYearOfASet),
     firstSetParallelWithAPerformedYearWasRun(pFirstSetParallelWithAPerformedYearWasRun),
     numSpace(pNumSpace),
     randomForParallelYears(pRandomForParallelYears),
     performCalculations(pPerformCalculations),
     study(pStudy),
     state(pState),
     yearByYear(pYearByYear),
     pDurationCollector(durationCollector),
     pResultWriter(resultWriter),
    hydroManagement(study.areas,
                    study.parameters,
                    study.calendar,
                    study.maxNbYearsInParallel,
                    resultWriter)
    {
        hydroHotStart = (study.parameters.initialReservoirLevels.iniLevels == Data::irlHotStart);
    }

private:
    ISimulation<Impl>* simulation_;
    unsigned int y;
    std::map<uint, bool>& yearFailed;
    std::map<uint, bool>& isFirstPerformedYearOfASet;
    bool firstSetParallelWithAPerformedYearWasRun;
    unsigned int numSpace;
    randomNumbers& randomForParallelYears;
    bool performCalculations;
    Data::Study& study;
    std::vector<Variable::State>& state;
    bool yearByYear;
    bool hydroHotStart;
    Benchmarking::IDurationCollector& pDurationCollector;
    IResultWriter& pResultWriter;
    HydroManagement hydroManagement;
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
        if (failedWeekList.size() != 0)
        {
            std::stringstream failedWeekStr;
            std::copy(failedWeekList.begin(),
                      failedWeekList.end(),
                      std::ostream_iterator<int>(failedWeekStr, " "));

            std::string s = failedWeekStr.str();
            s = s.substr(0, s.length() - 1); // get rid of the trailing space

            std::string failedStr
              = failedWeekList.size() != 1 ? " failed at weeks " : " failed at week ";

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

        if (performCalculations)
        {
            // Index of the current year in the list of structures
            uint indexYear = randomForParallelYears.yearNumberToIndex[y];

            // Getting random tables for this year
            yearRandomNumbers& randomForCurrentYear = randomForParallelYears.pYears[indexYear];
            double* randomReservoirLevel = nullptr;

            // 1 - Applying random levels for current year
            if (hydroHotStart && firstSetParallelWithAPerformedYearWasRun)
                randomReservoirLevel = state[numSpace].problemeHebdo->previousYearFinalLevels.data();
            else
                randomReservoirLevel = randomForCurrentYear.pReservoirLevels;

            // 2 - Preparing the Time-series numbers
            // removed

            // 3 - Preparing data related to Clusters in 'must-run' mode
            simulation_->prepareClustersInMustRunMode(numSpace, y);

            // 4 - Hydraulic ventilation
            {
                Benchmarking::Timer timer;
                hydroManagement.makeVentilation(randomReservoirLevel,
                                                             state[numSpace],
                                                             y,
                                                             numSpace);
                timer.stop();
                pDurationCollector.addDuration("hydro_ventilation", timer.get_duration());
            }

            // Updating the state
            state[numSpace].year = y;

            // 5 - Resetting all variables for the output
            simulation_->variables.yearBegin(y, numSpace);

            // 6 - The Solver itself
            bool isFirstPerformedYearOfSimulation
              = isFirstPerformedYearOfASet[y] && not firstSetParallelWithAPerformedYearWasRun;
            std::list<uint> failedWeekList;

            OptimizationStatisticsWriter optWriter(pResultWriter, y);
            yearFailed[y] = !simulation_->year(progression,
                                               state[numSpace],
                                               numSpace,
                                               randomForCurrentYear,
                                               failedWeekList,
                                               isFirstPerformedYearOfSimulation,
                                               hydroManagement.ventilationResults(),
                                               optWriter);

            // Log failing weeks
            logFailedWeek(y, study, failedWeekList);

            simulation_->variables.yearEndBuild(state[numSpace], y, numSpace);

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
                Benchmarking::Timer timerYear;
                // Before writing, some variable may require minor modifications
                simulation_->variables.beforeYearByYearExport(y, numSpace);
                // writing the results for the current year into the output
                simulation_->writeResults(false, y, numSpace); // false for synthesis
                timerYear.stop();
                pDurationCollector.addDuration("yby_export", timerYear.get_duration());
            }
        }
        else
        {
            simulation_->incrementProgression(progression);

            logs.info() << "  playlist: ignoring the year " << (y + 1);

            yearFailed[y] = false;

        } // End if(performCalculations)

    } // End of onExecute() method
};

template<class Impl>
inline ISimulation<Impl>::ISimulation(Data::Study& study,
    const ::Settings& settings,
    Benchmarking::IDurationCollector& duration_collector,
    IResultWriter& resultWriter) :
    ImplementationType(study, resultWriter),
    study(study),
    settings(settings),
    pNbYearsReallyPerformed(0),
    pNbMaxPerformedYearsInParallel(0),
    pYearByYear(study.parameters.yearByYear),
    pFirstSetParallelWithAPerformedYearWasRun(false),
    pDurationCollector(duration_collector),
    pQueueService(study.pQueueService),
    pResultWriter(resultWriter)
{
    // Ask to the interface to show the messages
    logs.info();
    logs.info() << LOG_UI_DISPLAY_MESSAGES_ON;

    // Running !
    logs.checkpoint() << "Running the simulation (" << ImplementationType::Name() << ')';
    logs.info() << "Allocating resources...";

    if (pYearByYear && (settings.noOutput || settings.tsGeneratorsOnly))
        pYearByYear = false;

    pHydroHotStart = (study.parameters.initialReservoirLevels.iniLevels == Data::irlHotStart);
}

template<class Impl>
inline void ISimulation<Impl>::checkWriter() const
{
    // The zip writer needs a queue service (async mutexed write)
    if (!pQueueService && pResultWriter.needsTheJobQueue())
    {
        throw Solver::Initialization::Error::NoQueueService();
    }
}

template<class Impl>
inline ISimulation<Impl>::~ISimulation()
{
}

template<class Impl>
void ISimulation<Impl>::run()
{
    pNbMaxPerformedYearsInParallel = study.maxNbYearsInParallel;

    // Initialize all data
    ImplementationType::variables.initializeFromStudy(study);
    // Computing the max number columns a report of any kind can contain.
    study.parameters.variablesPrintInfo.computeMaxColumnsCountInReports();

    logs.info() << "Allocating resources...";

    // Memory usage
    {
        logs.info() << " Variables:  ("
                    << (uint)(ImplementationType::variables.memoryUsage() / 1024 / 1024) << "Mo)";
        Variable::PrintInfosStdCout c;
        ImplementationType::variables.template provideInformations<Variable::PrintInfosStdCout>(c);
    }

    // Preprocessors
    // Determine if we have to use the preprocessors at least one time.
    pData.initialize(study.parameters);

    // Prepro only ?
    ImplementationType::preproOnly = settings.tsGeneratorsOnly;

    ImplementationType::setNbPerformedYearsInParallel(pNbMaxPerformedYearsInParallel);

    if (settings.tsGeneratorsOnly)
    {
        // Only the preprocessors can be used
        // We only have to regenerate time-series according the settings
        // in general data of the study.
        logs.info() << " Only the preprocessors are enabled.";

        regenerateTimeSeries(0);

        // Destroy the TS Generators if any
        // It will export the time-series into the output in the same time
        Solver::TSGenerator::DestroyAll(study);
    }
    else
    {
        if (not ImplementationType::simulationBegin())
            return;
        // Allocating the memory
        ImplementationType::variables.simulationBegin();

        // For beauty
        logs.info();

        // Sampled time-series Numbers
        // We will resize all matrix related to the time-series numbers
        // This operation can be done once since the number of years is constant
        // for a single simulation
        study.resizeAllTimeseriesNumbers(1 + study.runtime->rangeLimits.year[Data::rangeEnd]);
        // Now, we will prepare the time-series numbers
        if (not TimeSeriesNumbers::Generate(study))
        {
            throw FatalError("An unrecoverable error has occured. Can not continue.");
        }

        if (study.parameters.useCustomScenario)
            ApplyCustomScenario(study);

        // Launching the simulation for all years
        logs.info() << "MC-Years : [" << (study.runtime->rangeLimits.year[Data::rangeBegin] + 1)
                    << " .. " << (1 + study.runtime->rangeLimits.year[Data::rangeEnd])
                    << "], total: " << study.runtime->rangeLimits.year[Data::rangeCount];

        // Current state
        std::vector<Variable::State> state(pNbMaxPerformedYearsInParallel, Variable::State(study));
        // Initializing states for parallel actually performed years
        for (uint numSpace = 0; numSpace != pNbMaxPerformedYearsInParallel; ++numSpace)
            ImplementationType::initializeState(state[numSpace], numSpace);

        logs.info() << " Starting the simulation";
        uint finalYear = 1 + study.runtime->rangeLimits.year[Data::rangeEnd];
        {
            Benchmarking::Timer timer;
            loopThroughYears(0, finalYear, state);
            timer.stop();
            pDurationCollector.addDuration("mc_years", timer.get_duration());
        }
        // Destroy the TS Generators if any
        // It will export the time-series into the output in the same time
        Solver::TSGenerator::DestroyAll(study);

        // Post operations
        {
            Benchmarking::Timer timer;
            ImplementationType::simulationEnd();
            timer.stop();
            pDurationCollector.addDuration("post_processing", timer.get_duration());
        }

        ImplementationType::variables.simulationEnd();

        // Export ts-numbers into output
        TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(study, pResultWriter);

        // Spatial clusters
        // Notifying all variables to perform the final spatial clusters.
        // This must be done only when all variables have finished to compute their
        // own data.
        ImplementationType::variables.simulationEndSpatialAggregates(ImplementationType::variables);
    }
}

template<class Impl>
void ISimulation<Impl>::writeResults(bool synthesis, uint year, uint numSpace)
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
            newPath << "mc-all";
        else
        {
            CString<10, false> tmp;
            tmp = (year + 1);
            newPath << "mc-ind" << IO::Separator << "00000";
            newPath.overwriteRight(tmp);
        }

        // Dumping
        ImplementationType::variables.exportSurveyResults(
          synthesis, newPath, numSpace, pResultWriter);
    }
}

template<class Impl>
void ISimulation<Impl>::regenerateTimeSeries(uint year)
{
    // A preprocessor can be launched for several reasons:
    // * The option "Preprocessor" is checked in the interface _and_ year == 0
    // * Both options "Preprocessor" and "Refresh" are checked in the interface
    //   _and_ the refresh must be done for the given year (always done for the first year).
    using namespace Solver::TSGenerator;
    // Load
    if (pData.haveToRefreshTSLoad && (year % pData.refreshIntervalLoad == 0))
    {
        Benchmarking::Timer timer;
        GenerateTimeSeries<Data::timeSeriesLoad>(study, year, pResultWriter);
        timer.stop();
        pDurationCollector.addDuration("tsgen_load", timer.get_duration());
    }
    // Solar
    if (pData.haveToRefreshTSSolar && (year % pData.refreshIntervalSolar == 0))
    {
        Benchmarking::Timer timer;
        GenerateTimeSeries<Data::timeSeriesSolar>(study, year, pResultWriter);
        timer.stop();
        pDurationCollector.addDuration("tsgen_solar", timer.get_duration());
    }
    // Wind
    if (pData.haveToRefreshTSWind && (year % pData.refreshIntervalWind == 0))
    {
        Benchmarking::Timer timer;
        GenerateTimeSeries<Data::timeSeriesWind>(study, year, pResultWriter);
        timer.stop();
        pDurationCollector.addDuration("tsgen_wind", timer.get_duration());
    }
    // Hydro
    if (pData.haveToRefreshTSHydro && (year % pData.refreshIntervalHydro == 0))
    {
        Benchmarking::Timer timer;
        GenerateTimeSeries<Data::timeSeriesHydro>(study, year, pResultWriter);
        timer.stop();
        pDurationCollector.addDuration("tsgen_hydro", timer.get_duration());
    }
    // Thermal
    const bool refreshTSonCurrentYear = (year % pData.refreshIntervalThermal == 0);
    {
        Benchmarking::Timer timer;
        GenerateThermalTimeSeries(
          study, year, pData.haveToRefreshTSThermal, refreshTSonCurrentYear, pResultWriter);
        timer.stop();
        pDurationCollector.addDuration("tsgen_thermal", timer.get_duration());
    }
}

template<class Impl>
uint ISimulation<Impl>::buildSetsOfParallelYears(
  uint firstYear,
  uint endYear,
  std::vector<setOfParallelYears>& setsOfParallelYears)
{
    // Filter on the years
    const auto& yearsFilter = study.parameters.yearsFilter;

    // number max of years (to be executed or not) in a set of parallel years
    uint maxNbYearsPerformed = 0;

    setOfParallelYears* set = nullptr;
    bool buildNewSet = true;
    bool foundFirstPerformedYearOfCurrentSet = false;

    // Gets information on each parallel years set
    for (uint y = firstYear; y < endYear; ++y)
    {
        unsigned int indexSpace = 999999;
        bool performCalculations = yearsFilter[y];

        // Do we refresh just before this year ? If yes a new set of parallel years has to be
        // created
        bool refreshing = false;
        refreshing = pData.haveToRefreshTSLoad && (y % pData.refreshIntervalLoad == 0);
        refreshing
          = refreshing || (pData.haveToRefreshTSSolar && (y % pData.refreshIntervalSolar == 0));
        refreshing
          = refreshing || (pData.haveToRefreshTSWind && (y % pData.refreshIntervalWind == 0));
        refreshing
          = refreshing || (pData.haveToRefreshTSHydro && (y % pData.refreshIntervalHydro == 0));

        // Some thermal clusters may override the global parameter.
        // Therefore, we may want to refresh TS even if pData.haveToRefreshTSThermal == false
        bool haveToRefreshTSThermal
          = pData.haveToRefreshTSThermal || study.runtime->thermalTSRefresh;
        refreshing
          = refreshing || (haveToRefreshTSThermal && (y % pData.refreshIntervalThermal == 0));

        // We build a new set of parallel years if one of these conditions is fulfilled :
        //	- We have to refresh (or regenerate) some or all time series before running the
        //    current year
        //	- This is the first year (to be executed or not) after the previous set is full with
        //    years to be executed. That is : in the previous set filled, the max number of
        //    years to be actually run is reached.
        buildNewSet = buildNewSet || refreshing;

        if (buildNewSet)
        {
            setOfParallelYears setToCreate;
            setsOfParallelYears.push_back(setToCreate);
            set = &(setsOfParallelYears.back());

            // Initializations
            set->nbPerformedYears = 0;
            set->nbYears = 0;
            set->regenerateTS = false;
            set->yearForTSgeneration = 999999;

            // In case we have to regenerate times series before run the current set of parallel
            // years
            if (refreshing)
            {
                set->regenerateTS = true;
                set->yearForTSgeneration = y;
            }
        }

        set->yearsIndices.push_back(y);
        set->nbYears++;
        set->yearFailed[y] = true;
        set->isFirstPerformedYearOfASet[y] = false;

        if (performCalculations)
        {
            // Another year performed
            ++pNbYearsReallyPerformed;

            // Number of actually performed years in the current set (up to now).
            set->nbPerformedYears++;
            // Index of the MC year's space (useful if this year is actually run)
            indexSpace = set->nbPerformedYears - 1;

            set->isYearPerformed[y] = true;
            set->performedYearToSpace[y] = indexSpace;
            set->spaceToPerformedYear[indexSpace] = y;

            if (!foundFirstPerformedYearOfCurrentSet)
            {
                set->isFirstPerformedYearOfASet[y] = true;
                foundFirstPerformedYearOfCurrentSet = true;
            }
        }
        else
        {
            set->isYearPerformed[y] = false;
        }

        // Do we build a new set at next iteration (for years to be executed or not) ?
        if (indexSpace == pNbMaxPerformedYearsInParallel - 1 || y == endYear - 1)
        {
            buildNewSet = true;
            foundFirstPerformedYearOfCurrentSet = false;
            if (set->nbPerformedYears > maxNbYearsPerformed)
                maxNbYearsPerformed = set->nbPerformedYears;
        }
        else
            buildNewSet = false;

    } // End of loop over years

    return maxNbYearsPerformed;
}

template<class Impl>
void ISimulation<Impl>::allocateMemoryForRandomNumbers(randomNumbers& randomForParallelYears)
{
    uint maxNbPerformedYears = randomForParallelYears.pMaxNbPerformedYears;
    uint nbAreas = study.areas.size();

    for (uint y = 0; y < maxNbPerformedYears; y++)
    {
        // General :
        randomForParallelYears.pYears[y].setNbAreas(nbAreas);
        randomForParallelYears.pYears[y].pNbClustersByArea = new size_t[nbAreas];

        // Thermal noises :
        randomForParallelYears.pYears[y].pThermalNoisesByArea = new double*[nbAreas];

        for (uint a = 0; a != nbAreas; ++a)
        {
            // logs.info() << "   area : " << a << " :";
            auto& area = *(study.areas.byIndex[a]);
            size_t nbClusters = area.thermal.list.mapping.size();
            randomForParallelYears.pYears[y].pThermalNoisesByArea[a] = new double[nbClusters];
            randomForParallelYears.pYears[y].pNbClustersByArea[a] = nbClusters;
        }

        // Reservoir levels
        randomForParallelYears.pYears[y].pReservoirLevels = new double[nbAreas];

        // Noises on unsupplied and spilled energy
        randomForParallelYears.pYears[y].pUnsuppliedEnergy = new double[nbAreas];
        randomForParallelYears.pYears[y].pSpilledEnergy = new double[nbAreas];

        // Hydro costs noises
        switch (study.parameters.power.fluctuations)
        {
        case Data::lssFreeModulations:
        {
            randomForParallelYears.pYears[y].pHydroCostsByArea_freeMod = new double*[nbAreas];
            for (uint a = 0; a != nbAreas; ++a)
                randomForParallelYears.pYears[y].pHydroCostsByArea_freeMod[a] = new double[8784];
            break;
        }
        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            randomForParallelYears.pYears[y].pHydroCosts_rampingOrExcursion = new double[nbAreas];
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

template<class Impl>
void ISimulation<Impl>::computeRandomNumbers(randomNumbers& randomForYears,
                                             std::vector<uint>& years,
                                             std::map<unsigned int, bool>& isYearPerformed,
                                             MersenneTwister& randomHydro)
{
    auto& runtime = *study.runtime;

    uint indexYear = 0;
    std::vector<unsigned int>::iterator ity;

    for (ity = years.begin(); ity != years.end(); ++ity)
    {
        uint y = *ity;
        bool isPerformed = isYearPerformed[y];
        if (isPerformed)
            randomForYears.yearNumberToIndex[y] = indexYear;

        // General
        const unsigned int nbAreas = study.areas.size();

        // ... Thermal noise ...
        for (unsigned int a = 0; a != nbAreas; ++a)
        {
            // logs.info() << "   area : " << a << " :";
            auto& area = *(study.areas.byIndex[a]);

            auto end = area.thermal.list.mapping.end();
            for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
            {
                uint clusterIndex = it->second->areaWideIndex;
                double thermalNoise = runtime.random[Data::seedThermalCosts].next();
                if (isPerformed)
                    randomForYears.pYears[indexYear].pThermalNoisesByArea[a][clusterIndex] = thermalNoise;
            }
        }

        // ... Reservoir levels ...
        uint areaIndex = 0;
        study.areas.each([&](Data::Area& area) {
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

            double randomLevel = HydroManagement::randomReservoirLevel(min[firstDayOfMonth],
                                                                       avg[firstDayOfMonth],
                                                                       max[firstDayOfMonth],
                                                                       randomHydro);

            // Possibly update the intial level from scenario builder
            if (study.parameters.useCustomScenario)
            {
                double levelFromScenarioBuilder = study.scenarioHydroLevels[areaIndex][y];
                if (levelFromScenarioBuilder >= 0.)
                    randomLevel = levelFromScenarioBuilder;
            }

            if (pHydroHotStart)
            {
                if (!isPerformed || !area.hydro.reservoirManagement)
                {
                    // This initial level should be unused, so -1, as impossible value, is suitable.
                    randomForYears.pYears[indexYear].pReservoirLevels[areaIndex] = -1.;
                    areaIndex++;
                    return; // Skipping the current area
                }

                if (!pFirstSetParallelWithAPerformedYearWasRun)
                    randomForYears.pYears[indexYear].pReservoirLevels[areaIndex] = randomLevel;
                // Else : means the start levels (multiple areas are affected) of a year are
                // retrieved from a previous year and
                //		  these levels are updated inside the year job (see year job).
            }
            else
            {
                // Current area's hydro starting (or initial) level computation
                // (no matter if the year is performed or not, we always draw a random initial
                // reservoir level to ensure the same results)
                if (isPerformed)
                    randomForYears.pYears[indexYear].pReservoirLevels[areaIndex] = randomLevel;
            }

            areaIndex++;
        }); // each area

        // ... Unsupplied and spilled energy costs noises (french : bruits sur la defaillance
        // positive et negatives) ... references to the random number generators
        auto& randomUnsupplied = study.runtime->random[Data::seedUnsuppliedEnergyCosts];
        auto& randomSpilled = study.runtime->random[Data::seedSpilledEnergyCosts];

        int currentSpilledEnergySeed = study.parameters.seed[Data::seedSpilledEnergyCosts];
        int defaultSpilledEnergySeed = Data::antaresSeedDefaultValue
                                       + Data::seedSpilledEnergyCosts * Data::antaresSeedIncrement;
        bool SpilledEnergySeedIsDefault = (currentSpilledEnergySeed == defaultSpilledEnergySeed);
        areaIndex = 0;
        study.areas.each([&](Data::Area& area) {
            (void)area; // Avoiding warnings at compilation (unused variable) on linux
            if (isPerformed)
            {
                double randomNumber = randomUnsupplied();
                randomForYears.pYears[indexYear].pUnsuppliedEnergy[areaIndex] = randomNumber;
                randomForYears.pYears[indexYear].pSpilledEnergy[areaIndex] = randomNumber;
                if (!SpilledEnergySeedIsDefault)
                    randomForYears.pYears[indexYear].pSpilledEnergy[areaIndex] = randomSpilled();
            }
            else
            {
                randomUnsupplied();
                if (!SpilledEnergySeedIsDefault)
                    randomSpilled();
            }

            areaIndex++;
        }); // each area

        // ... Hydro costs noises ...
        auto& randomHydro = study.runtime->random[Data::seedHydroCosts];

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
                    double* noise
                      = randomForYears.pYears[indexYear].pHydroCostsByArea_freeMod[areaIndex];
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
                            noise[index] = -5 * 1.e-4 * (1 + rank / 8784.);
                        else
                            noise[index] = 5 * 1.e-4 * (1 + rank / 8784.);

                        rank++;
                    }

                    areaIndex++;
                }
            }
            else
                for (auto i = study.areas.begin(); i != end; ++i)
                    for (uint j = 0; j != 8784; ++j)
                        randomHydro();

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
                    randomForYears.pYears[indexYear].pHydroCosts_rampingOrExcursion[areaIndex]
                      = randomHydro();
                else
                    randomHydro();

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
            indexYear++;

    } // End loop over years
} // End function

template<class Impl>
void ISimulation<Impl>::computeAnnualCostsStatistics(
  std::vector<Variable::State>& state,
  std::vector<setOfParallelYears>::iterator& set_it)
{
    // Loop over years contained in the set
    std::vector<unsigned int>::iterator year_it;
    for (year_it = set_it->yearsIndices.begin(); year_it != set_it->yearsIndices.end(); ++year_it)
    {
        // Get the index of the year
        unsigned int y = *year_it;
        if (set_it->isYearPerformed[y])
        {
            // Get space number associated to the performed year
            uint numSpace = set_it->performedYearToSpace[y];
            pAnnualCostsStatistics.systemCost.addCost(state[numSpace].annualSystemCost);
            pAnnualCostsStatistics.criterionCost1.addCost(state[numSpace].optimalSolutionCost1);
            pAnnualCostsStatistics.criterionCost2.addCost(state[numSpace].optimalSolutionCost2);
            pAnnualCostsStatistics.optimizationTime1.addCost(
              state[numSpace].averageOptimizationTime1);
            pAnnualCostsStatistics.optimizationTime2.addCost(
              state[numSpace].averageOptimizationTime2);
        }
    }
}

static void logPerformedYearsInAset(setOfParallelYears& set)
{
    logs.info() << "parallel batch size : " << set.nbYears << " (" << set.nbPerformedYears
                << " perfomed)";

    std::string performedYearsToLog = "";
    std::for_each(std::begin(set.yearsIndices), std::end(set.yearsIndices), [&](uint const& y) {
        if (set.isYearPerformed[y])
            performedYearsToLog += std::to_string(y + 1) + " ";
    });

    logs.info() << "Year(s) " << performedYearsToLog;
}

template<class Impl>
void ISimulation<Impl>::loopThroughYears(uint firstYear,
                                         uint endYear,
                                         std::vector<Variable::State>& state)
{
    assert(endYear <= study.parameters.nbYears);

    // Init random hydro
    MersenneTwister randomHydro;
    randomHydro.reset(study.parameters.seed[Data::seedHydroManagement]);

    // List of parallel years sets
    std::vector<setOfParallelYears> setsOfParallelYears;

    // Gets information on each set of parallel years and returns the max number of years performed
    // in a set The variable "maxNbYearsPerformedInAset" is the maximum numbers of years to be
    // actually executed in a set. A set contains some years to be actually executed (at most
    // "pNbMaxPerformedYearsInParallel" years) and some others to skip.
    uint maxNbYearsPerformedInAset
      = buildSetsOfParallelYears(firstYear, endYear, setsOfParallelYears);
    // Related to annual costs statistics (printed in output into separate files)
    pAnnualCostsStatistics.setNbPerformedYears(pNbYearsReallyPerformed);

    // Container for random numbers of parallel years (to be executed or not)
    randomNumbers randomForParallelYears(maxNbYearsPerformedInAset,
                                         study.parameters.power.fluctuations);

    // Allocating memory to store random numbers of all parallel years
    allocateMemoryForRandomNumbers(randomForParallelYears);

    // Number of threads to perform the jobs waiting in the queue
    pQueueService->maximumThreadCount(pNbMaxPerformedYearsInParallel);

    // Loop over sets of parallel years
    std::vector<setOfParallelYears>::iterator set_it;
    for (set_it = setsOfParallelYears.begin(); set_it != setsOfParallelYears.end(); ++set_it)
    {
        // 1 - We may want to regenerate the time-series this year.
        // This is the case when the preprocessors are enabled from the
        // interface and/or the refresh is enabled.
        if (set_it->regenerateTS)
            regenerateTimeSeries(set_it->yearForTSgeneration);

        computeRandomNumbers(randomForParallelYears, set_it->yearsIndices, set_it->isYearPerformed,
                             randomHydro);

        std::vector<unsigned int>::iterator year_it;

        bool yearPerformed = false;
        Concurrency::FutureSet results;
        for (year_it = set_it->yearsIndices.begin(); year_it != set_it->yearsIndices.end();
             ++year_it)
        {
            // Get the index of the year
            unsigned int y = *year_it;

            bool performCalculations = set_it->isYearPerformed[y];
            unsigned int numSpace = 999999;
            if (performCalculations)
            {
                yearPerformed = true;
                numSpace = set_it->performedYearToSpace[y];
            }

            // If the year has not to be rerun, we skip the computation of the year.
            // Note that, when we enter for the first time in the "for" loop, all years of the set
            // have to be rerun (meaning : they must be run once). if(!set_it->yearFailed[y])
            // continue;

            Concurrency::Task task = yearJob<ImplementationType>(this,
                                                                 y,
                                                                 set_it->yearFailed,
                                                                 set_it->isFirstPerformedYearOfASet,
                                                                 pFirstSetParallelWithAPerformedYearWasRun,
                                                                 numSpace,
                                                                 randomForParallelYears,
                                                                 performCalculations,
                                                                 study,
                                                                 state,
                                                                 pYearByYear,
                                                                 pDurationCollector,
                                                                 pResultWriter);
            results.add(Concurrency::AddTask(*pQueueService, task));
        } // End loop over years of the current set of parallel years

        logPerformedYearsInAset(*set_it);

        pQueueService->start();

        pQueueService->wait(Yuni::qseIdle);
        pQueueService->stop();
        results.join();
        pResultWriter.flush();

        // At this point, the first set of parallel year(s) was run with at least one year performed
        if (!pFirstSetParallelWithAPerformedYearWasRun && yearPerformed)
            pFirstSetParallelWithAPerformedYearWasRun = true;

        // On regarde si au moins une année du lot n'a pas trouvé de solution
        for (auto& [year, failed] : set_it->yearFailed)
        {
            // Si une année du lot d'années n'a pas trouvé de solution, on arrête tout
            if (failed)
            {
                std::ostringstream msg;
                msg << "Year " << year + 1 << " has failed in the previous set of parallel year.";
                throw FatalError(msg.str());
            }
        }
        // Computing the summary : adding the contribution of MC years
        // previously computed in parallel
        ImplementationType::variables.computeSummary(set_it->spaceToPerformedYear,
                                                     set_it->nbPerformedYears);

        // Computing summary of spatial aggregations
        ImplementationType::variables.computeSpatialAggregatesSummary(
          ImplementationType::variables, set_it->spaceToPerformedYear, set_it->nbPerformedYears);

        // Computes statistics on annual (system and solution) costs, to be printed in output into
        // separate files
        computeAnnualCostsStatistics(state, set_it);

        // Set to zero the random numbers of all parallel years
        randomForParallelYears.reset();

    } // End loop over sets of parallel years

    // Writing annual costs statistics
    pAnnualCostsStatistics.endStandardDeviations();
    pAnnualCostsStatistics.writeToOutput(pResultWriter);
}

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_SOLVER_HXX__
