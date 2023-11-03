#define WIN32_LEAN_AND_MEAN
#include "utils.h"


void initializeStudy(Study::Ptr study)
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

    auto added = area->thermal.list.add(cluster);

    area->thermal.list.mapping[cluster->id()] = added;
    area->thermal.prepareAreaWideIndexes();

    return cluster;
}

void addScratchpadToEachArea(Study& study)
{
    for (auto [_, area] : study.areas) {
        area->scratchpad.init(*study.runtime, *area);
    }
}

TimeSeriesConfigurer& TimeSeriesConfigurer::setColumnCount(unsigned int columnCount)
{
    ts_->resize(columnCount, HOURS_PER_YEAR);
    return *this;
}

TimeSeriesConfigurer& TimeSeriesConfigurer::fillColumnWith(unsigned int column, double value)
{
    ts_->fillColumn(column, value);
    return *this;
}

ThermalClusterConfig::ThermalClusterConfig(ThermalCluster* cluster) : cluster_(cluster), tsAvailablePowerConfig_(cluster_->series.timeSeries)
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

averageResults OutputRetriever::load(Area* area)
{
    auto result = retrieveAreaResults<Variable::Economy::VCardTimeSeriesValuesLoad>(area);
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
    auto result = retrieveResultsForThermalCluster<Variable::Economy::VCardProductionByDispatchablePlant>(cluster);
    return averageResults((*result)[cluster->areaWideIndex].avgdata);
}

averageResults OutputRetriever::thermalNbUnitsON(ThermalCluster* cluster)
{
    auto result = retrieveResultsForThermalCluster<Variable::Economy::VCardNbOfDispatchedUnitsByPlant>(cluster);
    return averageResults((*result)[cluster->areaWideIndex].avgdata);
}

ScenarioBuilderRule::ScenarioBuilderRule(Study& study)
{
    study.scenarioRulesCreate();
    ScenarioBuilder::Sets* sets = study.scenarioRules;
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
                                                         nullDurationCollector_,
                                                         resultWriter_);
    SIM_AllocationTableaux(study_);
}


// =========================
// Basic study builder
// =========================
StudyBuilder::StudyBuilder()
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;

    study = std::make_shared<Study>();
    simulation = std::make_shared<SimulationHandler>(*study);

    initializeStudy(study);
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

std::shared_ptr<BindingConstraint> addBindingConstraints(Study& study, std::string name, std::string group) {
    auto bc = study.bindingConstraints.add(name);
    bc->group(group);
    auto g = study.bindingConstraintsGroups.add(group);
    g->add(bc);
    return bc;
}
