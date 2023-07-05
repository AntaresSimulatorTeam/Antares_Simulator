#define WIN32_LEAN_AND_MEAN
#include "utils.h"

#include <utility>
#include "simulation/simulation.h"

using namespace Antares::Data;

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

std::shared_ptr<BindingConstraint> addBindingConstraints(const Study::Ptr& study, const std::string& name, const std::string& group) {
    auto bc = study->bindingConstraints.add(name);
    bc->group(group);
    if (auto groupOfConstraint = study->bindingConstraintsGroups[group]; groupOfConstraint != nullptr) {
        groupOfConstraint->add(bc);
    } else {
        study->bindingConstraintsGroups.add(group)->add(bc);
    }
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

void NoOPResultWriter::addEntryFromBuffer(const std::string&, Clob&)
{

}
void NoOPResultWriter::addEntryFromBuffer(const std::string&, std::string&)
{

}
void NoOPResultWriter::addEntryFromFile(const std::string&, const std::string&)
{

}
bool NoOPResultWriter::needsTheJobQueue() const
{
    return false;
}
void NoOPResultWriter::finalize(bool)
{

}
