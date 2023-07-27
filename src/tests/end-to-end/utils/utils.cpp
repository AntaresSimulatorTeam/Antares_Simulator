#define WIN32_LEAN_AND_MEAN
#include "utils.h"


void initializeStudy(Study::Ptr study)
{
    study->resultWriter = std::make_shared<NullResultWriter>();
    study->parameters.reset();
    Data::Study::Current::Set(study);
}

void configureLinkCapacities(AreaLink* link)
{
    const double linkCapacityInfinite = +std::numeric_limits<double>::infinity();
    link->directCapacities.resize(1, 8760);
    link->directCapacities.fill(linkCapacityInfinite);

    link->indirectCapacities.resize(1, 8760);
    link->indirectCapacities.fill(linkCapacityInfinite);
}

void addLoadToArea(Area* area, double loadInArea)
{
    unsigned int loadNumberTS = 1;
    area->load.series->timeSeries.resize(loadNumberTS, HOURS_PER_YEAR);
    area->load.series->timeSeries.fill(loadInArea);
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

void addScratchpadToEachArea(Study::Ptr study)
{
    for (auto [_, area] : study->areas) {
        for (unsigned int i = 0; i < study->maxNbYearsInParallel; ++i) {
            area->scratchpad.emplace_back(*study->runtime, *area);
        }
    }
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

ScenarioBuilderRule::ScenarioBuilderRule(Study::Ptr study)
{
    study->scenarioRulesCreate();
    ScenarioBuilder::Sets* sets = study->scenarioRules;
    if (sets && !sets->empty())
    {
        rules_ = sets->createNew("Custom");

        study->parameters.useCustomScenario = true;
        study->parameters.activeRulesScenario = "Custom";
    }
 }


// =====================
// Simulation handler
// =====================

void SimulationHandler::create()
{
    study_->initializeRuntimeInfos();
    addScratchpadToEachArea(study_);

    simulation_ = std::make_shared<ISimulation<Economy>>(*study_,
                                                         settings_,
                                                         &nullDurationCollector_);
    SIM_AllocationTableaux();
}


// =========================
// Basic study builder
// =========================
StudyBuilder::StudyBuilder()
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;

    study = std::make_shared<Study>();
    simulation = std::make_shared<SimulationHandler>(study);

    initializeStudy(study);
    output = std::make_shared<OutputRetriever>(simulation->get());
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

std::shared_ptr<BindingConstraint> addBindingConstraints(Study::Ptr study, std::string name, std::string group) {
    auto bc = study->bindingConstraints.add(name);
    bc->group(group);
    return bc;
}

void NullResultWriter::addEntryFromBuffer(const std::string&, Clob&)
{

}
void NullResultWriter::addEntryFromBuffer(const std::string&, std::string&)
{

}
void NullResultWriter::addEntryFromFile(const std::string&, const std::string&)
{

}
bool NullResultWriter::needsTheJobQueue() const
{
    return false;
}
void NullResultWriter::finalize(bool)
{

}
