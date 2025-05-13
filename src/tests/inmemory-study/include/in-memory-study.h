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
#pragma once
#define WIN32_LEAN_AND_MEAN
#include "antares/solver/simulation/economy.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/study/scenario-builder/rules.h"
#include "antares/study/scenario-builder/sets.h"
#include "antares/study/study.h"

using namespace Antares::Solver;
using namespace Antares::Solver::Simulation;
using namespace Antares::Data::ScenarioBuilder;

void initializeStudy(Data::Study* study);
void configureLinkCapacities(AreaLink* link);

class TimeSeriesConfigurer
{
public:
    TimeSeriesConfigurer() = default;

    TimeSeriesConfigurer(Matrix<>& matrix):
        ts_(&matrix)
    {
    }

    TimeSeriesConfigurer& setDimensions(unsigned columnCount, unsigned rowCount = HOURS_PER_YEAR);
    TimeSeriesConfigurer& fillColumnWith(unsigned column, double value);
    TimeSeriesConfigurer& fillColumnWith(unsigned column, const std::vector<double>& values);

private:
    Matrix<>* ts_ = nullptr;
};

class ThermalClusterConfig
{
public:
    ThermalClusterConfig() = default;
    ThermalClusterConfig(ThermalCluster* cluster);
    ThermalClusterConfig& setNominalCapacity(double nominalCapacity);
    ThermalClusterConfig& setUnitCount(unsigned unitCount);
    ThermalClusterConfig& setCosts(double cost);
    ThermalClusterConfig& setAvailablePowerNumberOfTS(unsigned columnCount);
    ThermalClusterConfig& setAvailablePower(unsigned column, double value);

private:
    ThermalCluster* cluster_ = nullptr;
    TimeSeriesConfigurer tsAvailablePowerConfig_;
};

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName);
void addScratchpadToEachArea(Data::Study& study);

// -------------------------------
// Simulation results retrieval
// -------------------------------
class averageResults
{
public:
    averageResults(Variable::R::AllYears::AverageData& averageResults):
        averageResults_(averageResults)
    {
    }

    double* hours()
    {
        return averageResults_.hourly;
    }

    double hour(unsigned hour)
    {
        return averageResults_.hourly[hour];
    }

    double* days()
    {
        return averageResults_.daily;
    }

    double day(unsigned day)
    {
        return averageResults_.daily[day];
    }

    double* weeks()
    {
        return averageResults_.weekly;
    }

    double week(unsigned week)
    {
        return averageResults_.weekly[week];
    }

private:
    Variable::R::AllYears::AverageData& averageResults_;
};

class OutputRetriever
{
public:
    OutputRetriever(ISimulation<Economy>& simulation):
        simulation_(simulation)
    {
    }

    averageResults overallCost(Area* area);
    averageResults levelForSTSgroup(Area* area, unsigned groupNb);
    averageResults load(Area* area);
    averageResults hydroStorage(Area* area);
    averageResults flow(AreaLink* link);
    averageResults thermalGeneration(ThermalCluster* cluster);
    averageResults thermalNbUnitsON(ThermalCluster* cluster);

private:
    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveAreaResults(Area* area);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveLinkResults(AreaLink* link);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveResultsForThermalCluster(
      ThermalCluster* cluster);

    ISimulation<Economy>& simulation_;
};

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveAreaResults(Area* area)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForArea<VCard>(&result, area);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveLinkResults(AreaLink* link)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForLink<VCard>(&result, link);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveResultsForThermalCluster(
  ThermalCluster* cluster)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForThermalCluster<VCard>(&result, cluster);
    return result;
}

class ScenarioBuilderRule
{
public:
    ScenarioBuilderRule(Data::Study& study);

    loadTSNumberData& load()
    {
        return rules_->load;
    }

    BindingConstraintsTSNumberData& bcGroup()
    {
        return rules_->binding_constraints;
    }

    hydroTSNumberData& hydro()
    {
        return rules_->hydro;
    }

private:
    Rules::Ptr rules_;
};

// =====================
// Simulation handler
// =====================

class SimulationHandler
{
public:
    SimulationHandler(Data::Study& study):
        study_(study)
    {
    }

    ~SimulationHandler() = default;
    void create();

    void run()
    {
        simulation_->run();
    }

    ISimulation<Economy>& rawSimu()
    {
        return *simulation_;
    }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    Benchmarking::DurationCollector durationCollector_;
    Settings settings_;
    Data::Study& study_;
    NullResultWriter resultWriter_;
    NullSimulationObserver observer_;
};

// =========================
// Basic study builder
// =========================

struct StudyBuilder
{
    StudyBuilder();

    void simulationBetweenDays(const unsigned firstDay, const unsigned lastDay);
    Area* addAreaToStudy(const std::string& areaName);
    void setNumberMCyears(unsigned nbYears);
    void playOnlyYear(unsigned year);
    void giveWeightToYear(float weight, unsigned year);

    // Data members
    std::unique_ptr<Data::Study> study;
    std::shared_ptr<SimulationHandler> simulation;
};

std::shared_ptr<Antares::Data::BindingConstraint> addBindingConstraints(Antares::Data::Study& study,
                                                                        std::string name,
                                                                        std::string group);
