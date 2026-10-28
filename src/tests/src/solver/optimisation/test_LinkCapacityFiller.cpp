// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

#include <boost/test/unit_test.hpp>

#include "FillerFixture.h"

using namespace std::string_literals;

using namespace Antares::Optimization;
using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::Api;

static const auto linkConnectionLib = R"(
library:
  id: invest_lib

  port-types:
    - id: bidirectional_capacity_port
      fields:
        - id: capacity_direct
        - id: capacity_indirect
      link-capacity-connection:
        capacity-direct-field: capacity_direct
        capacity-indirect-field: capacity_indirect
  models:
    - id: link_invest
      parameters:
        - id: availability_factor_direct
          scenario-dependent: true
          time-dependent: true
        - id: availability_factor_indirect
          scenario-dependent: true
          time-dependent: true
        - id: already_installed_capacity
          scenario-dependent: false
          time-dependent: false
        - id: already_installed_availability_factor_direct
          scenario-dependent: true
          time-dependent: true
        - id: already_installed_availability_factor_indirect
          scenario-dependent: true
          time-dependent: true
      variables:
        - id: invested_capacity
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: capacity_port
          type: bidirectional_capacity_port
      port-field-definitions:
        - port: capacity_port
          field: capacity_direct
          definition: availability_factor_direct * invested_capacity + already_installed_availability_factor_direct * already_installed_capacity
        - port: capacity_port
          field: capacity_indirect
          definition: availability_factor_indirect * invested_capacity + already_installed_availability_factor_indirect * already_installed_capacity
)"s;

static const auto linkConnectionSystem = R"(
system:
  id: my_system

  components:
    - id: my_link_invest
      model: invest_lib.link_invest
      parameters:
        - id: availability_factor_direct
          scenario-dependent: true
          time-dependent: true
          value: availability_factor_direct
        - id: availability_factor_indirect
          scenario-dependent: true
          time-dependent: true
          value: availability_factor_indirect
        - id: already_installed_capacity
          scenario-dependent: false
          time-dependent: false
          value: 20
        - id: already_installed_availability_factor_direct
          scenario-dependent: true
          time-dependent: true
          value: already_installed_availability_factor_direct
        - id: already_installed_availability_factor_indirect
          scenario-dependent: true
          time-dependent: true
          value: already_installed_availability_factor_indirect
      scenario-group: sg

  link-capacity-connections:
    - component: my_link_invest
      port: capacity_port
      link-component:
        from: de
        to: fr
)"s;

BOOST_FIXTURE_TEST_SUITE(_ComponentToLinkCapacityFiller_, FillerFixture)

BOOST_AUTO_TEST_CASE(add_link_capacity_constraints_and_override_legacy_bounds)
{
    init(linkConnectionSystem, linkConnectionLib);
    setData("availability_factor_direct", {4.0});
    setData("availability_factor_indirect", {5.0});
    setData("already_installed_availability_factor_direct", {6.0});
    setData("already_installed_availability_factor_indirect", {7.0});

    OptimEntityContainer optimEntityContainer(linearProblem);
    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());

    problemeHebdo->NomsDesPays = {"de", "fr"};
    problemeHebdo->NombreDePays = 2;
    problemeHebdo->NombreDInterconnexions = 1;
    problemeHebdo->PaysOrigineDeLInterconnexion = {0};
    problemeHebdo->PaysExtremiteDeLInterconnexion = {1};
    problemeHebdo->CoutDeTransport.resize(1);
    problemeHebdo->CoutDeTransport[0].IntercoGereeAvecDesCouts = true;
    problemeHebdo->CorrespondanceVarNativesVarOptim.resize(1);
    auto& varMap = problemeHebdo->CorrespondanceVarNativesVarOptim[0];
    varMap.NumeroDeVariableDuFluxDirect = {0};
    varMap.NumeroDeVariableDuFluxDirectPositif = {1};
    varMap.NumeroDeVariableDuFluxIndirectPositif = {2};

    addLegacyVariables({"direct_flow_t0", "direct_cost_t0", "indirect_cost_t0"});
    addModelerVariables(0, 0, optimEntityContainer);
    fillProblemWithLinkCapacityConnectionFiller({0, 0, 0, 0, 0}, optimEntityContainer);

    const auto* directFlow = linearProblem.lookupVariable("direct_flow_t0");
    const auto* directCost = linearProblem.lookupVariable("direct_cost_t0");
    const auto* indirectCost = linearProblem.lookupVariable("indirect_cost_t0");
    BOOST_REQUIRE(directFlow);
    BOOST_REQUIRE(directCost);
    BOOST_REQUIRE(indirectCost);
    BOOST_CHECK_EQUAL(directFlow->getLb(), -linearProblem.infinity());
    BOOST_CHECK_EQUAL(directFlow->getUb(), linearProblem.infinity());
    BOOST_CHECK_EQUAL(directCost->getLb(), 0.);
    BOOST_CHECK_EQUAL(directCost->getUb(), linearProblem.infinity());
    BOOST_CHECK_EQUAL(indirectCost->getLb(), 0.);
    BOOST_CHECK_EQUAL(indirectCost->getUb(), linearProblem.infinity());

    const auto* investedCapacity = linearProblem.lookupVariable("my_link_invest.invested_capacity_t0");
    BOOST_REQUIRE(investedCapacity);

    const auto* ntcUpper = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::NTCDirectUpper::hour<0>");
    BOOST_REQUIRE(ntcUpper);
    BOOST_CHECK_EQUAL(ntcUpper->getCoefficient(directFlow), 1.0);
    BOOST_CHECK_EQUAL(ntcUpper->getCoefficient(investedCapacity), -4.0);
    BOOST_CHECK_EQUAL(ntcUpper->getUb(), 120.0);

    const auto* ntcLower = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::NTCDirectLower::hour<0>");
    BOOST_REQUIRE(ntcLower);
    BOOST_CHECK_EQUAL(ntcLower->getCoefficient(directFlow), -1.0);
    BOOST_CHECK_EQUAL(ntcLower->getCoefficient(investedCapacity), -5.0);
    BOOST_CHECK_EQUAL(ntcLower->getUb(), 140.0);

    const auto* directCostCt = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::IntercoDirectCostUpper::hour<0>");
    BOOST_REQUIRE(directCostCt);
    BOOST_CHECK_EQUAL(directCostCt->getCoefficient(directCost), 1.0);
    BOOST_CHECK_EQUAL(directCostCt->getCoefficient(investedCapacity), -4.0);
    BOOST_CHECK_EQUAL(directCostCt->getUb(), 120.0);

    const auto* indirectCostCt = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::IntercoIndirectCostUpper::hour<0>");
    BOOST_REQUIRE(indirectCostCt);
    BOOST_CHECK_EQUAL(indirectCostCt->getCoefficient(indirectCost), 1.0);
    BOOST_CHECK_EQUAL(indirectCostCt->getCoefficient(investedCapacity), -5.0);
    BOOST_CHECK_EQUAL(indirectCostCt->getUb(), 140.0);
}

BOOST_AUTO_TEST_SUITE_END()
