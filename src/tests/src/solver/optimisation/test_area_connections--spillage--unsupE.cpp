// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/converter.h>
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"
#include "antares/study/system-model/library.h"

#include "unit_test_utils.h"

using namespace std::string_literals;

using namespace Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Optimisation;
using namespace LinearProblemApi;
using namespace LinearProblemDataImpl;

static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: area_conn_port_type
      description: some port type for area connection
      fields:
        - id: to-area-bound
      area-connection:
        - to-area-bound-field: to-area-bound

  models:
    - id: model_with_vars
      variables:
        - id: var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: area_conn_port
          type: area_conn_port_type
      port-field-definitions:
        - port: area_conn_port
          field: to-area-bound
          definition: 2 * var_1
)"s;

static const auto systemYaml = R"(
system:
  id: my_system
  model-libraries: my_lib
  components:
  - id: component_with_vars
    model: my_lib.model_with_vars
    scenario-group: sg

  area-connections:
    - component: component_with_vars
      port: area_conn_port
      area: Area1
)"s;

struct AreaConnectionFixture
{
    // public function members :
    // -----------------------
    AreaConnectionFixture();
    std::unique_ptr<Solver::ModelerData> buildModelerSystem();
    void addComponentsVariablesToLP();
    void addNamedConstraintsToLP(std::vector<std::string>& constraintNames, double rhs);

    // public data members :
    // -------------------
    // ... GEMS system
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<Library> libraries;
    // ... GEMS paramaters
    FillContext ctx = {0, 0, 0, 0, 0};
    LinearProblemData data;                          // Empty
    ScenarioGroupRepository scenarioGroupRepository; // Empty

    // ... Hybrid / Legacy linear problem
    LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem;
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<OptimEntityContainer> optimContainer;

private:
    void addConstraintsToLinearProblem(std::vector<std::string>& names, double rhs);
};

AreaConnectionFixture::AreaConnectionFixture():
    linearProblem(true, "scip") //,
    // optimEntityContainer(linearProblem, &data, &scenarioGroupRepository)
{
    modelerData = buildModelerSystem();

    problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
    problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
    problemeHebdo->modelerData = modelerData.get();

    auto scenario = std::make_unique<Scenario>("SG");
    scenarioGroupRepository.addScenario("SG", std::move(scenario));
    optimContainer = std::make_unique<OptimEntityContainer>(linearProblem, &data, &scenarioGroupRepository);
    optimContainer->addFromSystemComponents(modelerData->system->Components());
}

std::unique_ptr<Solver::ModelerData> AreaConnectionFixture::buildModelerSystem()
{
    auto to_return = std::make_unique<Solver::ModelerData>();

    IO::Inputs::YmlModel::Parser parserModel;
    libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));

    IO::Inputs::YmlSystem::Parser parserSystem;
    auto ymlSystem = parserSystem.parse(systemYaml);
    auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);

    to_return->system = std::make_unique<System>(std::move(system));
    // std::string id = to_return->system->Id();
    return to_return;
}

void AreaConnectionFixture::addComponentsVariablesToLP()
{
    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            optimContainer->addStartColumn();

            // All variables are time-dependent here
            for (auto t = 0; t <= ctx.getLocalLastTimeStep(); ++t)
            {
                auto name = buildVariableName(component.Id(), variable.Id(), {}, t);
                linearProblem.addVariable(-999, 999, false, name);
            }
        }
    }
}

void AreaConnectionFixture::addNamedConstraintsToLP(std::vector<std::string>& constraintNames,
                                                    double rhs)
{
    problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;
    problemeHebdo->ProblemeAResoudre->NombreDeContraintes = constraintNames.size();

    addConstraintsToLinearProblem(constraintNames, rhs);
}

void AreaConnectionFixture::addConstraintsToLinearProblem(std::vector<std::string>& names,
                                                          double rhs)
{
    for (const auto& name: names)
    {
        linearProblem.addConstraint(rhs, rhs, name);
    }
}

BOOST_AUTO_TEST_SUITE(_area_connections___spillage_)

BOOST_FIXTURE_TEST_CASE(dummy_test, AreaConnectionFixture)
{
    
    // Linear problem before addition from GEMS
    // ------------------------------------------
    // LP is filled with constraints before GEMS comes into play.
    std::vector<std::string> constraintNames({"dummy constaint", "AreaBalance::area<area1>::hour<0>"});
    addNamedConstraintsToLP(constraintNames, 10 /* rhs */);

    // Contraints numbering in linear problem before GEMS commes into play. 
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);


    // Adding GEMS variables (replaces the work of ComponentFiller, for variables only)
    addComponentsVariablesToLP();

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
