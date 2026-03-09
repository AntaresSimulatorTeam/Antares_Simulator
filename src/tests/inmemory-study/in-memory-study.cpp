// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

Antares::Data::ShortTermStorage::STStorageCluster* addSTSToArea(Area* area,
                                                                const std::string& stsName)
{
    Antares::Data::ShortTermStorage::STStorageCluster sts;
    sts.properties.name = stsName;
    auto& storages = area->shortTermStorage.storagesByIndex;
    storages.push_back(sts);
    return &storages.back();
}

void addScratchpadToEachArea(Study& study)
{
    for (auto& [_, area]: study.areas)
    {
        for (unsigned i = 0; i < study.maxNbYearsInParallel; ++i)
        {
            area->scratchpad.emplace_back(study.runtime, *area);
        }
    }
}

TimeSeriesConfigurer& TimeSeriesConfigurer::setDimensions(unsigned columnCount, unsigned rowCount)
{
    ts_->resize(columnCount, rowCount);
    return *this;
}

TimeSeriesConfigurer& TimeSeriesConfigurer::fillColumnWith(unsigned column, double value)
{
    ts_->fillColumn(column, value);
    return *this;
}

TimeSeriesConfigurer& TimeSeriesConfigurer::fillColumnWith(unsigned column,
                                                           const std::vector<double>& values)
{
    ts_->pasteToColumn(column, values.data());
    return *this;
}

ThermalClusterConfig::ThermalClusterConfig(std::shared_ptr<ThermalCluster> cluster):
    cluster_(cluster),
    tsAvailablePowerConfig_(cluster_->series.timeSeries)
{
}

ThermalClusterConfig& ThermalClusterConfig::setNominalCapacity(double nominalCapacity)
{
    cluster_->nominalCapacity = nominalCapacity;
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setUnitCount(unsigned unitCount)
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

ThermalClusterConfig& ThermalClusterConfig::setAvailablePowerNumberOfTS(unsigned columnCount)
{
    tsAvailablePowerConfig_.setDimensions(columnCount);
    return *this;
}

ThermalClusterConfig& ThermalClusterConfig::setAvailablePower(unsigned column, double value)
{
    tsAvailablePowerConfig_.fillColumnWith(column, value);
    return *this;
}

// -------------------------------
// Short-term storage
// -------------------------------
ShortTermStorageConfig::ShortTermStorageConfig(
  Antares::Data::ShortTermStorage::STStorageCluster& storage):
    storage(storage),
    constraintConfig(storage)
{
}

ShortTermStorageConfig& ShortTermStorageConfig::setInjectionNominalCapacity(
  double injectionNominalCapacity)
{
    storage.properties.injectionNominalCapacity = injectionNominalCapacity;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setWithdrawalNominalCapacity(
  double withdrawalNominalCapacity)
{
    storage.properties.withdrawalNominalCapacity = withdrawalNominalCapacity;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setReservoirCapacity(double reservoirCapacity)
{
    storage.properties.reservoirCapacity = reservoirCapacity;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setInitialLevel(double initialLevel)
{
    storage.properties.initialLevel = initialLevel;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setInitialLevelOptim(bool initialLevelOptim)
{
    storage.properties.initialLevelOptim = initialLevelOptim;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setAllowOverflow(bool allowOverflow)
{
    storage.properties.allowOverflow = allowOverflow;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setInjectionEfficiency(double injectionEfficiency)
{
    storage.properties.injectionEfficiency = injectionEfficiency;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setWithdrawalEfficiency(double withdrawalEfficiency)
{
    storage.properties.withdrawalEfficiency = withdrawalEfficiency;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setGroupName(const std::string& groupName)
{
    storage.properties.groupName = groupName;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setName(const std::string& name)
{
    storage.properties.name = name;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setPenalizeVariationWithdrawal(
  bool penalizeVariationWithdrawal)
{
    storage.properties.penalizeVariationWithdrawal = penalizeVariationWithdrawal;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::setPenalizeVariationInjection(
  bool penalizeVariationInjection)
{
    storage.properties.penalizeVariationInjection = penalizeVariationInjection;
    return *this;
}

ShortTermStorageConfig& ShortTermStorageConfig::ShortTermStorageConfig::setEnabled(bool enabled)
{
    storage.properties.enabled = enabled;
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

averageResults OutputRetriever::levelForSTSgroup(Area* area, unsigned groupNb)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardSTSbyGroup>(area);
    unsigned levelIndex = groupNb * 3 + 2;
    return result[area->index][levelIndex].avgdata;
}

averageResults OutputRetriever::withdrawalForSTSgroup(Area* area, unsigned groupNb)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardSTSbyGroup>(area);
    unsigned withdrawalIndex = groupNb * 3 + 1;
    return result[area->index][withdrawalIndex].avgdata;
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
// Simulation observer
// =====================
void TestingSimulationObserver::notifyHebdoProblem(const PROBLEME_HEBDO& problemeHebdo,
                                                   int optimizationNumber,
                                                   std::string_view name)
{
    auto* pb = problemeHebdo.ProblemeAResoudre.get();
    std::string nameStr(name.begin(), name.end());
    auto& toInsert = problems[std::make_pair(optimizationNumber, nameStr)];

    // Variables
    for (int varIdx = 0; varIdx < pb->NombreDeVariables; varIdx++)
    {
        const std::string& varName = pb->NomDesVariables[varIdx];
        auto& insertedVariable = toInsert.variables[varName];
        insertedVariable = {.Xmin = pb->Xmin[varIdx],
                            .Xmax = pb->Xmax[varIdx],
                            .objectiveCoefficient = pb->CoutLineaire[varIdx]};
    }

    // Constraints
    for (int ctIdx = 0; ctIdx < pb->NombreDeContraintes; ctIdx++)
    {
        const std::string& ctName = pb->NomDesContraintes[ctIdx];
        auto& insertedConstraint = toInsert.constraints[ctName];
        int debutLigne = pb->IndicesDebutDeLigne[ctIdx];
        for (int coefIdx = 0; coefIdx < pb->NombreDeTermesDesLignes[ctIdx]; ++coefIdx)
        {
            int pos = debutLigne + coefIdx;
            int varIdx = pb->IndicesColonnes[pos];
            const std::string& varName = pb->NomDesVariables[varIdx];
            insertedConstraint.coefficients[varName] = pb->CoefficientsDeLaMatriceDesContraintes
                                                         [pos];
            insertedConstraint.rhs = pb->SecondMembre[ctIdx];
        }
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
StudyBuilder::StudyBuilder():
    study(std::make_unique<Study>()),
    simulation(*study)
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;
    study->parameters.namedProblems = true;
    initializeStudy(study.get());
}

void StudyBuilder::simulationBetweenDays(const unsigned firstDay, const unsigned lastDay)
{
    study->parameters.simulationDays.first = firstDay;
    study->parameters.simulationDays.end = lastDay;
}

void StudyBuilder::setNumberMCyears(unsigned nbYears)
{
    study->parameters.resetPlaylist(nbYears);
    study->areas.resizeAllTimeseriesNumbers(nbYears);
    study->areas.each([&](Data::Area& area)
                      { area.hydro.deltaBetweenFinalAndInitialLevels.resize(nbYears); });
}

void StudyBuilder::playOnlyYear(unsigned year)
{
    auto& params = study->parameters;

    params.userPlaylist = true;
    std::fill(params.yearsFilter.begin(), params.yearsFilter.end(), false);
    params.yearsFilter[year] = true;
}

void StudyBuilder::giveWeightToYear(float weight, unsigned year)
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
