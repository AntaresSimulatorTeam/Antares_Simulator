// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

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
    - id: some_port_type
      description: port type with area connection
      fields:
        - id: some_field
        - id: connected_field
      area-connection:
        injection-to-balance: connected_field
        spillage-bound: 
        unsupplied-energy-bound: 

  models:
    - id: model_with_vars
      variables:
        - id: no_connect_var
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: var_2
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: connection_port_var
          type: some_port_type
      port-field-definitions:
        - port: connection_port_var
          field: some_field
          definition: no_connect_var
        - port: connection_port_var
          field: connected_field
          definition: 5 * var_1 - 37 * var_2

    - id: model_with_param
      parameters:
        - id: some_param
          time-dependent: true
          scenario-dependent: false
      ports:
        - id: connection_port_param
          type: some_port_type
      port-field-definitions:
        - port: connection_port_param
          field: some_field
          definition: some_param
        - port: connection_port_param
          field: connected_field
          definition: 2 * some_param - 6
    )"s;

static const auto systemYaml = R"(
system:
  id: my_system

  model-libraries: my_lib

  components:
  - id: component_with_vars
    model: my_lib.model_with_vars
    scenario-group: sg
  - id: connected_component_param
    model: my_lib.model_with_param
    scenario-group: sg
    parameters:
    - id: some_param
      time-dependent: true
      scenario-dependent: false
      value: some_param_value

  area-connections:
    - component: component_with_vars
      port: connection_port_var
      area: Area1
    - component: connected_component_param
      port: connection_port_param
      area: Area1
    )"s;

struct ComponentToAreaConnectionFillerFixture
{
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<Library> libraries;
    LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem;
    ScenarioGroupRepository scenarioGroupRepository;
    std::remove_reference_t<LinearProblemData&> data;

    ComponentToAreaConnectionFillerFixture():
        linearProblem(true, "scip")
    {
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
        modelerData = buildModelerSystem();
        problemeHebdo->modelerData = modelerData.get();

        auto scenarioPtr = std::make_unique<Scenario>("SG");
        scenarioPtr->setTimeSerieNumber(0, 1);
        scenarioGroupRepository.addScenario("SG", std::move(scenarioPtr));
    }

    void setLinearProblemData(const std::vector<double>& some_param_value)
    {
        auto tss = std::make_unique<TimeSeriesSet>("some_param_value", some_param_value.size());
        tss->add(some_param_value);
        DataSeriesRepository ds;
        ds.addDataSeries(std::move(tss));
        LinearProblemData d(std::move(ds));
        data = std::move(d);
    }

    std::unique_ptr<Solver::ModelerData> buildModelerSystem()
    {
        auto to_return = std::make_unique<Solver::ModelerData>();

        IO::Inputs::YmlModel::Parser parserModel;
        libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));

        IO::Inputs::YmlSystem::Parser parserSystem;
        auto ymlSystem = parserSystem.parse(systemYaml, "");
        auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);

        to_return->system = std::make_unique<System>(std::move(system));
        return to_return;
    }

    void addAllComponentsVariablesToLP(unsigned int ts_start,
                                       unsigned int ts_end,
                                       OptimEntityContainer& optimEntityContainer)
    {
        for (const auto& component: modelerData->system->Components())
        {
            for (const auto& variable: component.getModel()->Variables())
            {
                optimEntityContainer.addStartColumn();
                if (variable.isTimeDependent())
                {
                    for (auto t = ts_start; t <= ts_end; ++t)
                    {
                        auto name = buildVariableName(component.Id(), variable.Id(), {}, t);
                        linearProblem.addVariable(-999, 999, false, name);
                    }
                }
                else
                {
                    auto name = buildVariableName(component.Id(),
                                                  variable.Id(),
                                                  std::nullopt,
                                                  std::nullopt);
                    linearProblem.addVariable(-999, 999, false, name);
                }
            }
        }
    }

    void addConstraintsToLinearProblem(std::vector<std::string>& names, double rhs)
    {
        for (const auto& name: names)
        {
            linearProblem.addConstraint(rhs, rhs, name);
        }
    }

    std::vector<std::string> makeAutomaticConstraintNames(unsigned nb_of_constraints)
    {
        std::vector<std::string> constraintNames;
        for (unsigned i = 0; i < nb_of_constraints; ++i)
        {
            constraintNames.push_back("c" + std::to_string(i));
        }
        return constraintNames;
    }

    void addAutomaticConstraintsToLP(unsigned nb_of_constraints, const double rhs)
    {
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes = nb_of_constraints;
        auto constraintNames = makeAutomaticConstraintNames(nb_of_constraints);
        problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;

        addConstraintsToLinearProblem(constraintNames, rhs);
    }

    void addNamedConstraintsToLP(std::vector<std::string>& constraintNames, double rhs)
    {
        problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes = constraintNames.size();

        addConstraintsToLinearProblem(constraintNames, rhs);
    }

    void addConstraintsFromConnectionsToLP(const FillContext& fillCtx,
                                           OptimEntityContainer& optimEntityContainer)
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = fillCtx.getLocalNumberOfTimeSteps();

        ComponentToAreaConnectionFiller filler(problemeHebdo.get(),
                                               optimEntityContainer,
                                               &data,
                                               scenarioGroupRepository);
        filler.addVariables(fillCtx);
        filler.addConstraints(fillCtx);
        filler.addObjectives(fillCtx);
    }
};

BOOST_FIXTURE_TEST_SUITE(_ComponentToAreaConnectionFiller_, ComponentToAreaConnectionFillerFixture)

BOOST_AUTO_TEST_CASE(add_one_term_to_balance_constraint_named)
{
    setLinearProblemData({4.0});

    OptimEntityContainer optimEntityContainer(linearProblem);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    addAllComponentsVariablesToLP(0, 0, optimEntityContainer);

    std::vector<std::string> constraints({"whatever", "AreaBalance::area<area1>::hour<0>"});
    addNamedConstraintsToLP(constraints, 10);

    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);

    addConstraintsFromConnectionsToLP({0, 0, 0, 0, 0}, optimEntityContainer);

    const auto* balance_ct = linearProblem.lookupConstraint("AreaBalance::area<area1>::hour<0>");

    const auto* nc_var_t0 = linearProblem.lookupVariable("component_with_vars.no_connect_var_t0");
    const auto* var1_t0 = linearProblem.lookupVariable("component_with_vars.var_1_t0");
    const auto* var2_t0 = linearProblem.lookupVariable("component_with_vars.var_2_t0");

    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(nc_var_t0), 0);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(var1_t0), -5);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(var2_t0), 37);
    BOOST_CHECK_EQUAL(balance_ct->getLb(), 10 + 2 * 4 - 6);
    BOOST_CHECK_EQUAL(balance_ct->getUb(), 10 + 2 * 4 - 6);

    auto other_ct = linearProblem.lookupConstraint("whatever");
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(nc_var_t0), 0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(var1_t0), 0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(var2_t0), 0);
    BOOST_CHECK_EQUAL(other_ct->getLb(), 10);
    BOOST_CHECK_EQUAL(other_ct->getUb(), 10);
}

BOOST_AUTO_TEST_CASE(add_two_terms_to_balance_constraint_not_named)
{
    setLinearProblemData({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -51.0, 8.3});

    OptimEntityContainer optimEntityContainer(linearProblem);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    addAllComponentsVariablesToLP(10, 11, optimEntityContainer);

    addAutomaticConstraintsToLP(3 /* nb of constraints */, -100);

    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[1].NumeroDeContrainteDesBilansPays.push_back(2);

    addConstraintsFromConnectionsToLP({0, 1, 10, 11, 0}, optimEntityContainer);

    auto balance_ct_t10 = linearProblem.lookupConstraint("c1");
    auto balance_ct_t11 = linearProblem.lookupConstraint("c2");

    const auto* nc_var_t10 = linearProblem.lookupVariable("component_with_vars.no_connect_var_t10");
    const auto* var1_t10 = linearProblem.lookupVariable("component_with_vars.var_1_t10");
    const auto* var2_t10 = linearProblem.lookupVariable("component_with_vars.var_2_t10");
    const auto* nc_var_t11 = linearProblem.lookupVariable("component_with_vars.no_connect_var_t11");
    const auto* var1_t11 = linearProblem.lookupVariable("component_with_vars.var_1_t11");
    const auto* var2_t11 = linearProblem.lookupVariable("component_with_vars.var_2_t11");

    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(nc_var_t10), 0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(var1_t10), -5);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(var2_t10), 37);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(nc_var_t11), 0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(var1_t11), 0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(var2_t11), 0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getLb(), -100 + 2 * -51 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t10->getUb(), -100 + 2 * -51 - 6);

    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(nc_var_t10), 0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(var1_t10), 0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(var2_t10), 0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(nc_var_t11), 0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(var1_t11), -5);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(var2_t11), 37);
    BOOST_CHECK_EQUAL(balance_ct_t11->getLb(), -100 + 2 * 8.3 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t11->getUb(), -100 + 2 * 8.3 - 6);
}

BOOST_AUTO_TEST_CASE(failure_if_a_connection_references_a_not_existing_area)
{
    OptimEntityContainer optimEntityContainer(linearProblem);

    std::string errMsg = "Component 'component_with_vars' is connected to a non existing area ";
    errMsg += ": area1";
    BOOST_CHECK_EXCEPTION(addConstraintsFromConnectionsToLP({0, 0, 0, 0, 0}, optimEntityContainer),
                          std::runtime_error,
                          checkMessage(errMsg));
}

BOOST_AUTO_TEST_SUITE_END()
