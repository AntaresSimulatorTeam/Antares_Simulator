/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once
#define WIN32_LEAN_AND_MEAN
#include "antares/study/study.h"
#include "simulation/economy.h"
#include "antares/study/scenario-builder/rules.h"
#include "antares/study/scenario-builder/sets.h"
#include "simulation.h"

using namespace Antares::Solver;
using namespace Antares::Solver::Simulation;
using namespace Antares::Data::ScenarioBuilder;


void initializeStudy(Study::Ptr study);
void configureLinkCapacities(AreaLink* link);


class TimeSeriesConfigurer
{
public:
    TimeSeriesConfigurer() = default;
    TimeSeriesConfigurer(Matrix<>& matrix) : ts_(&matrix) {}
    TimeSeriesConfigurer& setColumnCount(unsigned int columnCount);
    TimeSeriesConfigurer& fillColumnWith(unsigned int column, double value);
private:
    Matrix<>* ts_ = nullptr;
};


class ThermalClusterConfig
{
public:
    ThermalClusterConfig() = default;
    ThermalClusterConfig(ThermalCluster* cluster);
    ThermalClusterConfig& setNominalCapacity(double nominalCapacity);
    ThermalClusterConfig& setUnitCount(unsigned int unitCount);
    ThermalClusterConfig& setCosts(double cost);
    ThermalClusterConfig& setAvailablePowerNumberOfTS(unsigned int columnCount);
    ThermalClusterConfig& setAvailablePower(unsigned int column, double value);

private:
    ThermalCluster* cluster_ = nullptr;
    TimeSeriesConfigurer tsAvailablePowerConfig_;
};

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName);
void addScratchpadToEachArea(Study& study);

// -------------------------------
// Simulation results retrieval
// -------------------------------
class averageResults
{
public:
    averageResults(Variable::R::AllYears::AverageData& averageResults) : averageResults_(averageResults)
    {}

    double hour(unsigned int hour) { return averageResults_.hourly[hour]; }
    double day(unsigned int day) { return averageResults_.daily[day]; }
    double week(unsigned int week) { return averageResults_.weekly[week]; }

private:
    Variable::R::AllYears::AverageData& averageResults_;
};


class OutputRetriever
{
public:
    OutputRetriever(ISimulation<Economy>& simulation) : simulation_(simulation) {}
    averageResults overallCost(Area* area);
    averageResults load(Area* area);
    averageResults flow(AreaLink* link);
    averageResults thermalGeneration(ThermalCluster* cluster);
    averageResults thermalNbUnitsON(ThermalCluster* cluster);

private:
    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveAreaResults(Area* area);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveLinkResults(AreaLink* link);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveResultsForThermalCluster(ThermalCluster* cluster);

    ISimulation<Economy>& simulation_;
};

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveAreaResults(Area* area)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForArea<VCard>(&result, area);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveLinkResults(AreaLink* link)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForLink<VCard>(&result, link);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveResultsForThermalCluster(ThermalCluster* cluster)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForThermalCluster<VCard>(&result, cluster);
    return result;
}

class ScenarioBuilderRule
{
public:
    ScenarioBuilderRule(Study& study);
    loadTSNumberData& load() { return rules_->load; }
    BindingConstraintsTSNumberData& bcGroup() { return rules_->binding_constraints; }

private:
    Rules::Ptr rules_;
};

// =====================
// Simulation handler
// =====================
using namespace Benchmarking;

class SimulationHandler
{
public:
    SimulationHandler(Study& study)
        : study_(study)
    {}
    ~SimulationHandler() = default;
    void create();
    void run() { simulation_->run(); }
    ISimulation<Economy>& rawSimu() { return *simulation_; }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    NullDurationCollector nullDurationCollector_;
    Settings settings_;
    Study& study_;
    NullResultWriter resultWriter_;
};


// =========================
// Basic study builder
// =========================

struct StudyBuilder
{
    StudyBuilder();

    void simulationBetweenDays(const unsigned int firstDay, const unsigned int lastDay);
    Area* addAreaToStudy(const std::string& areaName);
    void setNumberMCyears(unsigned int nbYears);
    void playOnlyYear(unsigned int year);
    void giveWeightToYear(float weight, unsigned int year);

    // Data members
    std::shared_ptr<Study> study;
    std::shared_ptr<SimulationHandler> simulation;
};

std::shared_ptr<Antares::Data::BindingConstraint> addBindingConstraints(Antares::Data::Study& study, std::string name, std::string group);
