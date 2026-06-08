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

static const auto libraryYamlWithSpillageAndUnsuppliedEnergyBound = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: area_conn_port_type
      description: some port type for area connection
      fields:
        - id: to-area-bound
        - id: from-area-bound
      area-connection:
        injection-to-balance: 
        spillage-bound: to-area-bound
        unsupplied-energy-bound: from-area-bound

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
          definition: 2 * var_1 + 30
        - port: area_conn_port
          field: from-area-bound
          definition: var_1 / 2 - 10
)"s;

static const auto systemYamlAreaConnection = R"(
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
    void addNamedConstraintsToLP(std::vector<std::string>& constraintNames,
                                 const double low_bound,
                                 const double up_bound);

    // public data members :
    // -------------------
    // ... GEMS system
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<Library> libraries;
    // ... GEMS paramaters
    FillContext fill_ctx = {0, 0, 0, 0, 0};
    LinearProblemData data;                          // Empty
    ScenarioGroupRepository scenarioGroupRepository; // Empty

    // ... Hybrid / Legacy linear problem
    LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem;
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<OptimEntityContainer> optimContainer;

private:
    void addConstraintsToLinearProblem(std::vector<std::string>& names,
                                       const double low_bound,
                                       const double up_bound);
};

AreaConnectionFixture::AreaConnectionFixture():
    linearProblem(true, "scip")
{
    modelerData = buildModelerSystem();

    problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
    problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
    problemeHebdo->modelerData = modelerData.get();

    auto scenario = std::make_unique<Scenario>("SG");
    scenarioGroupRepository.addScenario("SG", std::move(scenario));
    optimContainer = std::make_unique<OptimEntityContainer>(linearProblem);
    optimContainer->addFromSystemComponents(modelerData->system->Components());
}

std::unique_ptr<Solver::ModelerData> AreaConnectionFixture::buildModelerSystem()
{
    auto to_return = std::make_unique<Solver::ModelerData>();

    IO::Inputs::YmlModel::Parser parserModel;
    libraries.push_back(IO::Inputs::ModelConverter::convert(
      parserModel.parse(libraryYamlWithSpillageAndUnsuppliedEnergyBound)));

    IO::Inputs::YmlSystem::Parser parserSystem;
    auto ymlSystem = parserSystem.parse(systemYamlAreaConnection, "");
    auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);

    to_return->system = std::make_unique<System>(std::move(system));
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
            for (unsigned t = 0; t <= fill_ctx.getLocalLastTimeStep(); ++t)
            {
                auto name = buildVariableName(component.Id(), variable.Id(), {}, t);
                linearProblem.addVariable(-999, 999, false, name);
            }
        }
    }
}

void AreaConnectionFixture::addNamedConstraintsToLP(std::vector<std::string>& constraintNames,
                                                    const double low_bound,
                                                    const double up_bound)
{
    problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;
    problemeHebdo->ProblemeAResoudre->NombreDeContraintes = constraintNames.size();

    addConstraintsToLinearProblem(constraintNames, low_bound, up_bound);
}

void AreaConnectionFixture::addConstraintsToLinearProblem(std::vector<std::string>& names,
                                                          const double low_bound,
                                                          const double up_bound)
{
    for (const auto& name: names)
    {
        linearProblem.addConstraint(low_bound, up_bound, name);
    }
}

BOOST_AUTO_TEST_SUITE(_area_connections___injecting_spillage_and_unsupplied_energy_bounds)

BOOST_FIXTURE_TEST_CASE(injecting_spillage_and_unsupplied_energy_bounds, AreaConnectionFixture)
{
    // ------------------------------------------
    // Linear problem before addition from GEMS
    // ------------------------------------------
    // 1. LP is filled with constraints before GEMS comes into play.
    std::vector<std::string> constraintNames;
    std::string fictiveLoadsConstrName = "FictiveLoads::area<area1>::hour<0>";
    std::string maxUnsupEconstrName = "MaxUnsupEnergy::area<area1>::hour<0>";

    constraintNames.push_back("dummy constaint");
    constraintNames.push_back(fictiveLoadsConstrName);
    constraintNames.push_back("another dummy constaint");
    constraintNames.push_back(maxUnsupEconstrName);

    addNamedConstraintsToLP(constraintNames, 0. /* low rhs */, 50. /* up rhs */);

    // 2. Contraints numbering in linear problem before GEMS comes into play.
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    // ... In LP, fictive loads constraints are contiguous.
    //     The start number for fictive loads is 1.
    problemeHebdo->CorrespondanceCntNativesCntOptim[0]
      .NumeroDeContraintePourEviterLesChargesFictives.push_back(1);
    // ... The start number for upper-bounding the unsupplied energy is 3.
    problemeHebdo->CorrespondanceCntNativesCntOptim[0]
      .NumeroDeContraintePourBornerLaDefaillance.push_back(3);

    // -------------------------------------------------------
    // Adding GEMS variables and constraints to linear problem
    // -------------------------------------------------------
    // 1. Adding variables (replaces the work of ComponentFiller, but for variables)
    addComponentsVariablesToLP();

    // 2. Adding constraints from GEMS area connections
    ComponentToAreaConnectionFiller filler(problemeHebdo.get(),
                                           *optimContainer,
                                           &data,
                                           scenarioGroupRepository);
    filler.addConstraints(fill_ctx);

    // ---------
    // Checks
    // ---------
    const auto* var1_t0 = linearProblem.lookupVariable("component_with_vars.var_1_t0");

    const auto* fictive_load_ct_t0 = linearProblem.lookupConstraint(fictiveLoadsConstrName);
    BOOST_CHECK_EQUAL(fictive_load_ct_t0->getCoefficient(var1_t0), -2.);
    BOOST_CHECK_EQUAL(fictive_load_ct_t0->getLb(), 0. + 30.);
    BOOST_CHECK_EQUAL(fictive_load_ct_t0->getUb(), 50. + 30.);

    const auto* max_unsupE_ct_t0 = linearProblem.lookupConstraint(maxUnsupEconstrName);
    BOOST_CHECK_EQUAL(max_unsupE_ct_t0->getCoefficient(var1_t0), -0.5);
    BOOST_CHECK_EQUAL(max_unsupE_ct_t0->getLb(), 0. - 10.);
    BOOST_CHECK_EQUAL(max_unsupE_ct_t0->getUb(), 50. - 10.);

    auto dummy_ct = linearProblem.lookupConstraint("dummy constaint");
    auto other_dummy_ct = linearProblem.lookupConstraint("dummy constaint");
    BOOST_CHECK_EQUAL(dummy_ct->getCoefficient(var1_t0), 0);
    BOOST_CHECK_EQUAL(other_dummy_ct->getCoefficient(var1_t0), 0);
}

BOOST_AUTO_TEST_SUITE_END()
