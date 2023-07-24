#define WIN32_LEAN_AND_MEAN
#include "utils.h"
#include "simulation/simulation.h"

using namespace Antares::Data;


void initializeStudy(Study::Ptr study)
{
    study->resultWriter = std::make_shared<NullResultWriter>();
    study->parameters.reset();
    Data::Study::Current::Set(study);
}

void setNumberMCyears(Study::Ptr study, unsigned int nbYears)
{
    study->parameters.resetPlaylist(nbYears);
    study->bindingConstraints.resizeAllTimeseriesNumbers(nbYears);
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

void addScratchpadToEachArea(Study::Ptr study)
{
    for (auto [_, area] : study->areas) {
        for (unsigned int i = 0; i < study->maxNbYearsInParallel; ++i) {
            area->scratchpad.push_back(AreaScratchpad(*study->runtime, *area));
        }
    }
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

    auto result = retrieveLinkFlowResults(link);
    return averageResults(result->avgdata);
}

averageResults OutputRetriever::thermalGeneration(ThermalCluster* cluster)
{
    auto result = retrieveThermalClusterGenerationResults(cluster);
    return averageResults((*result)[cluster->areaWideIndex].avgdata);
}

Variable::Storage<Variable::Economy::VCardFlowLinear>::ResultsType*
OutputRetriever::retrieveLinkFlowResults(AreaLink* link)
{
    typename Variable::Storage<Variable::Economy::VCardFlowLinear>::ResultsType* result = nullptr;
    simulation_->variables.retrieveResultsForLink<Variable::Economy::VCardFlowLinear>(&result, link);
    return result;
}

Variable::Storage<Variable::Economy::VCardProductionByDispatchablePlant>::ResultsType*
OutputRetriever::retrieveThermalClusterGenerationResults(ThermalCluster* cluster)
{
    typename Variable::Storage<Variable::Economy::VCardProductionByDispatchablePlant>::ResultsType* result = nullptr;
    simulation_->variables.retrieveResultsForThermalCluster<Variable::Economy::VCardProductionByDispatchablePlant>(&result, cluster);
    return result;
}

// ===========================================================

void prepareStudy(Study::Ptr pStudy, int nbYears)
{
    //Define study parameters
    pStudy->parameters.reset();
    pStudy->parameters.resetPlaylist(nbYears);

    //Prepare parameters for simulation
    Data::StudyLoadOptions options;
    pStudy->parameters.prepareForSimulation(options);

    // Logical cores
    // -------------------------
    // Getting the number of logical cores to use before loading and creating the areas :
    // Areas need this number to be up-to-date at construction.
    pStudy->getNumberOfCores(false, 0);

    // Define as current study
    Data::Study::Current::Set(pStudy);
}

std::shared_ptr<BindingConstraint> addBindingConstraints(Study::Ptr study, std::string name, std::string group) {
    auto bc = study->bindingConstraints.add(name);
    bc->group(group);
    return bc;
}

Antares::Data::ScenarioBuilder::Rules::Ptr createScenarioRules(Study::Ptr study)
{
    ScenarioBuilder::Rules::Ptr rules;

    study->scenarioRulesCreate();
    ScenarioBuilder::Sets* sets = study->scenarioRules;
    if (sets && !sets->empty())
    {
        rules = sets->createNew("Custom");

        study->parameters.useCustomScenario  = true;
        study->parameters.activeRulesScenario = "Custom";
    }

    return rules;
}

float defineYearsWeight(Study::Ptr pStudy, const std::vector<float>& yearsWeight)
{
    pStudy->parameters.userPlaylist = true;

    for (uint i = 0; i < yearsWeight.size(); i++)
    {
        pStudy->parameters.setYearWeight(i, yearsWeight[i]);
    }

    return pStudy->parameters.getYearsWeightSum();
}

void cleanSimulation(Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation)
{
    delete simulation;
}

void cleanStudy(Study::Ptr pStudy)
{
    pStudy->clear();

    // Remove any global reference
    Data::Study::Current::Set(nullptr);
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
