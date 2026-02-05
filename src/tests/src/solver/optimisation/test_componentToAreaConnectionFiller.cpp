// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

#include <boost/test/unit_test.hpp>

#include "FillerFixture.h"
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
        - injection-field: connected_field

  models:
    - id: connected_model_through_vars
      variables:
        - id: not_connected_var
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: connected_var_1
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
        - id: connected_var_2
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: connection_port_var
          type: some_port_type
      port-field-definitions:
        - port: connection_port_var
          field: some_field
          definition: not_connected_var
        - port: connection_port_var
          field: connected_field
          definition: 5 * connected_var_1 - 37 * connected_var_2

    - id: connected_model_through_param
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
  - id: connected_component_var
    model: my_lib.connected_model_through_vars
    scenario-group: sg
  - id: connected_component_param
    model: my_lib.connected_model_through_param
    scenario-group: sg
    parameters:
    - id: some_param
      time-dependent: true
      scenario-dependent: false
      value: some_param_value

  area-connections:
    - component: connected_component_var
      port: connection_port_var
      area: Area1
    - component: connected_component_param
      port: connection_port_param
      area: Area1
    )"s;

BOOST_FIXTURE_TEST_SUITE(_ComponentToAreaConnectionFiller_, FillerFixture)

BOOST_AUTO_TEST_CASE(add_one_term_to_balance_constraint_named)
{
    init(systemYaml, libraryYaml);
    setData("some_param_value", {4.0});

    OptimEntityContainer optimEntityContainer(linearProblem,
                                              &linearProblemData,
                                              &scenarioGroupRepository);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    setUpModelerVariables(0, 0, optimEntityContainer);
    std::vector<std::string> constraints({"whatever", "AreaBalance::area<area1>::hour<0>"});
    setUpLegacyLp(constraints, true, 10);
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);
    fillProblemWithAreaConnectionFiller({0, 0, 0, 0, 0}, optimEntityContainer);

    auto balance_ct = linearProblem.lookupConstraint("AreaBalance::area<area1>::hour<0>");
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t0")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_1_t0")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_2_t0")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct->getLb(), 10 + 2 * 4 - 6);
    BOOST_CHECK_EQUAL(balance_ct->getUb(), 10 + 2 * 4 - 6);

    auto other_ct = linearProblem.lookupConstraint("whatever");
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_1_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getCoefficient(
                        linearProblem.lookupVariable("connected_component_var.connected_var_2_t0")),
                      0);
    BOOST_CHECK_EQUAL(other_ct->getLb(), 10);
    BOOST_CHECK_EQUAL(other_ct->getUb(), 10);
}

BOOST_AUTO_TEST_CASE(add_two_terms_to_balance_constraint_not_named)
{
    init(systemYaml, libraryYaml);
    setData("some_param_value", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -51.0, 8.3});

    OptimEntityContainer optimEntityContainer(linearProblem,
                                              &linearProblemData,
                                              &scenarioGroupRepository);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    setUpModelerVariables(10, 11, optimEntityContainer);
    // Legacy indexing of TS always starts at 1
    std::vector<std::string> constraints(
      {"whatever", "AreaBalance::area<area1>::hour<0>", "AreaBalance::area<area1>::hour<1>"});
    setUpLegacyLp(constraints, false, -100);
    problemeHebdo->NomsDesPays.push_back("area1");
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[0].NumeroDeContrainteDesBilansPays.push_back(1);
    problemeHebdo->CorrespondanceCntNativesCntOptim.push_back({});
    problemeHebdo->CorrespondanceCntNativesCntOptim[1].NumeroDeContrainteDesBilansPays.push_back(2);
    fillProblemWithAreaConnectionFiller({0, 1, 10, 11, 0}, optimEntityContainer);

    auto balance_ct_t10 = linearProblem.lookupConstraint("c1");
    auto balance_ct_t11 = linearProblem.lookupConstraint("c2");

    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t10")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t10")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t10->getLb(), -100 + 2 * -51 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t10->getUb(), -100 + 2 * -51 - 6);
    ;

    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t10")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.not_connected_var_t11")),
                      0);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_1_t11")),
                      -5);
    BOOST_CHECK_EQUAL(balance_ct_t11->getCoefficient(linearProblem.lookupVariable(
                        "connected_component_var.connected_var_2_t11")),
                      37);
    BOOST_CHECK_EQUAL(balance_ct_t11->getLb(), -100 + 2 * 8.3 - 6);
    BOOST_CHECK_EQUAL(balance_ct_t11->getUb(), -100 + 2 * 8.3 - 6);
}

BOOST_AUTO_TEST_CASE(fail_if_constraint_not_defined)
{
    init(systemYaml, libraryYaml);
    setData("some_param_value", {4.0});

    OptimEntityContainer optimEntityContainer(linearProblem,
                                              &linearProblemData,
                                              &scenarioGroupRepository);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    setUpModelerVariables(0, 0, optimEntityContainer);
    std::vector<std::string> constraints({"whatever"});
    setUpLegacyLp(constraints, true, 0);
    BOOST_CHECK_EXCEPTION(fillProblemWithAreaConnectionFiller({0, 0, 0, 0, 0},
                                                              optimEntityContainer),
                          std::runtime_error,
                          checkMessage("A component is connected to area \"area1\", that does not "
                                       "have a balance constraint defined for timestep 0"));
}

BOOST_AUTO_TEST_SUITE_END()
