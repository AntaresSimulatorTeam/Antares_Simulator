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
#define WIN32_LEAN_AND_MEAN

#include "in-memory-study.h"

#include "antares/application/ScenarioBuilderOwner.h"

void initializeStudy(Study* study)
{
    study->parameters.reset();
}

void configureLinkCapacities(AreaLink* link)
{
    const double linkCapacityInfinite = +std::numeric_limits<double>::infinity();
    link->directCapacities.resize(1, 8760);
    link->directCapacities.fill(linkCapacityInfinite);

    link->indirectCapacities.resize(1, 8760);
    link->indirectCapacities.fill(linkCapacityInfinite);
}

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName(clusterName);
    cluster->reset();

    area->thermal.list.addToCompleteList(cluster);

    return cluster;
}

void addScratchpadToEachArea(Study& study)
{
    for (auto& [_, area]: study.areas)
    {
        for (unsigned int i = 0; i < study.maxNbYearsInParallel; ++i)
        {
            area->scratchpad.emplace_back(study.runtime, *area);
        }
    }
}

// Name should be changed to setTSSize
TimeSeriesConfigurer& TimeSeriesConfigurer::setColumnCount(unsigned int columnCount,
                                                           unsigned rowCount)
{
    ts_->resize(columnCount, rowCount);
    return *this;
}

TimeSeriesConfigurer& TimeSeriesConfigurer::fillColumnWith(unsigned int column, double value)
{
    ts_->fillColumn(column, value);
    return *this;
}

ThermalClusterConfig::ThermalClusterConfig(ThermalCluster* cluster):
    cluster_(cluster),
    tsAvailablePowerConfig_(cluster_->series.timeSeries)
{
}

ThermalClusterConfig& ThermalClusterConfig::setNominalCapacity(double nominalCapacity)
{
    cluster_->nominalCapacity = nominalCapacity;
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setUnitCount(unsigned int unitCount)
{
    cluster_->unitCount = unitCount;
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setCosts(double cost)
{
    cluster_->marginalCost = cost;
    cluster_->marketBidCost = cost; // Must define market bid cost otherwise all production is used
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setAvailablePowerNumberOfTS(unsigned int columnCount)
{
    tsAvailablePowerConfig_.setColumnCount(columnCount);
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setAvailablePower(unsigned int column, double value)
{
    tsAvailablePowerConfig_.fillColumnWith(column, value);
    return *this;
}

// -------------------------------
// Simulation results retrieval
// -------------------------------
averageResults OutputRetriever::overallCost(Area* area)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardOverallCost>(area);
    return averageResults(result->avgdata);
}

averageResults OutputRetriever::levelForSTSgroup(Area* area, unsigned int groupNb)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardSTSbyGroup>(area);
    unsigned int levelIndex = groupNb * 3 + 2;
    return result[area->index][levelIndex].avgdata;
}

averageResults OutputRetriever::load(Area* area)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardTimeSeriesValuesLoad>(area);
    return averageResults(result->avgdata);
}

averageResults OutputRetriever::hydroStorage(Area* area)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardHydroStorage>(area);
    return averageResults(result->avgdata);
}

averageResults OutputRetriever::flow(AreaLink* link)
{
    // There is a problem here :
    //    we cannot easly retrieve the hourly flow for a link and a year :
    //    - Functions retrieveHourlyResultsForCurrentYear are not coded everywhere it should.
    //    - Even if those functions were correctly implemented, there is another problem :
    //      Each year results erase results of previous year, how can we retrieve results of year 1
    //      if 2 year were run ?
    //    We should be able to run each year independently, which is not possible now.
    //    A workaround is to retrieve syntheses, and that's what we do here.

    auto result = retrieveLinkResults<Variable::Economy::VCardFlowLinear>(link);
    return averageResults(result->avgdata);
}

averageResults OutputRetriever::thermalGeneration(ThermalCluster* cluster)
{
    auto result = retrieveResultsForThermalCluster<
      Variable::Economy::VCardProductionByDispatchablePlant>(cluster);
    return averageResults((*result)[cluster->enabledIndex].avgdata);
}

averageResults OutputRetriever::thermalNbUnitsON(ThermalCluster* cluster)
{
    auto result = retrieveResultsForThermalCluster<
      Variable::Economy::VCardNbOfDispatchedUnitsByPlant>(cluster);
    return averageResults((*result)[cluster->enabledIndex].avgdata);
}

ScenarioBuilderRule::ScenarioBuilderRule(Study& study)
{
    study.scenarioRulesCreate();
    auto sets = study.scenarioRules.get();
    if (sets && !sets->empty())
    {
        rules_ = sets->createNew("Custom");

        study.parameters.useCustomScenario = true;
        study.parameters.activeRulesScenario = "Custom";
    }
}

// =====================
// Simulation handler
// =====================

void SimulationHandler::create()
{
    study_.initializeRuntimeInfos();
    addScratchpadToEachArea(study_);
    simulation_ = std::make_shared<ISimulation<Economy>>(study_,
                                                         settings_,
                                                         durationCollector_,
                                                         resultWriter_,
                                                         observer_);
    Antares::Solver::ScenarioBuilderOwner(study_).callScenarioBuilder();
}

// =========================
// Basic study builder
// =========================
StudyBuilder::StudyBuilder()
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;

    study = std::make_unique<Study>(true);
    simulation = std::make_shared<SimulationHandler>(*study);

    initializeStudy(study.get());
}

void StudyBuilder::simulationBetweenDays(const unsigned int firstDay, const unsigned int lastDay)
{
    study->parameters.simulationDays.first = firstDay;
    study->parameters.simulationDays.end = lastDay;
}

void StudyBuilder::setNumberMCyears(unsigned int nbYears)
{
    study->parameters.resetPlaylist(nbYears);
    study->areas.resizeAllTimeseriesNumbers(nbYears);
    study->areas.each([&](Data::Area& area)
                      { area.hydro.deltaBetweenFinalAndInitialLevels.resize(nbYears); });
}

void StudyBuilder::playOnlyYear(unsigned int year)
{
    auto& params = study->parameters;

    params.userPlaylist = true;
    std::fill(params.yearsFilter.begin(), params.yearsFilter.end(), false);
    params.yearsFilter[year] = true;
}

void StudyBuilder::giveWeightToYear(float weight, unsigned int year)
{
    study->parameters.setYearWeight(year, weight);

    // Activate playlist, otherwise previous sets won't have any effect
    study->parameters.userPlaylist = true;
}

Area* StudyBuilder::addAreaToStudy(const std::string& areaName)
{
    Area* area = addAreaToListOfAreas(study->areas, areaName);

    // Default values for the area
    area->createMissingData();
    area->resetToDefaultValues();

    // Temporary : we want to give a high unsupplied or spilled energy costs.
    // Which cost should we give ?
    area->thermal.unsuppliedEnergyCost = 1000.0;
    area->thermal.spilledEnergyCost = 1000.0;

    study->areas.rebuildIndexes();

    return area;
}

std::shared_ptr<BindingConstraint> addBindingConstraints(Study& study,
                                                         std::string name,
                                                         std::string group)
{
    auto bc = study.bindingConstraints.add(name);
    bc->group(group);
    auto g = study.bindingConstraintsGroups.add(group);
    g->add(bc);
    return bc;
}
