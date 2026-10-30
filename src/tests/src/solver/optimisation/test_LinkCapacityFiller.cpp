// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cmath>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/exception/LoadingError.hpp"
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optimisation/LinkCapacityFiller.h"
#include "antares/study/InactiveComponentsAnalyzerBuilder.h"
#include "antares/study/categories.h"
#include "antares/study/study.h"

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

namespace
{
struct LinkFixture: FillerFixture
{
    LinkFixture():
        FillerFixture(false, "sirius")
    {
    }

    void setUpLink(OptimEntityContainer& optimEntityContainer)
    {
        init(linkConnectionSystem, linkConnectionLib);
        setData("availability_factor_direct", {4.0});
        setData("availability_factor_indirect", {5.0});
        setData("already_installed_availability_factor_direct", {6.0});
        setData("already_installed_availability_factor_indirect", {7.0});

        optimEntityContainer.addFromSystemComponents(modelerData->system->Components());

        problemeHebdo->NomsDesPays = {"de", "fr"};
        problemeHebdo->NombreDePays = 2;
        problemeHebdo->NombreDInterconnexions = 1;
        problemeHebdo->PaysOrigineDeLInterconnexion = {0};
        problemeHebdo->PaysExtremiteDeLInterconnexion = {1};
        problemeHebdo->CoutDeTransport.resize(1);
        problemeHebdo->CorrespondanceVarNativesVarOptim.resize(1);
        auto& varMap = problemeHebdo->CorrespondanceVarNativesVarOptim[0];
        varMap.NumeroDeVariableDuFluxDirect = {0};
        varMap.NumeroDeVariableDuFluxDirectPositif = {1};
        varMap.NumeroDeVariableDuFluxIndirectPositif = {2};
        problemeHebdo->ValeursDeNTC.resize(1);
        problemeHebdo->ValeursDeNTC[0].ValeurDeNTCOrigineVersExtremite = {1000.};
        problemeHebdo->ValeursDeNTC[0].ValeurDeNTCExtremiteVersOrigine = {1000.};
        problemeHebdo->ValeursDeNTC[0].CapaciteEffectiveOrigineVersExtremite = {std::nan("")};
        problemeHebdo->ValeursDeNTC[0].CapaciteEffectiveExtremiteVersOrigine = {std::nan("")};
        problemeHebdo->ProblemeAResoudre->CoutsReduits.assign(3, 0.);

        addLegacyVariables({"direct_flow_t0", "direct_cost_t0", "indirect_cost_t0"});
        addModelerVariables(0, 0, optimEntityContainer);
        fillProblemWithLinkCapacityConnectionFiller({0, 0, 0, 0, 0}, optimEntityContainer);

        // Invested capacity 10: direct capacity 4 * 10 + 6 * 20, indirect 5 * 10 + 7 * 20
        linearProblem.lookupVariable("my_link_invest.invested_capacity_t0")->setBounds(10., 10.);
    }
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(_LinkCapacityResults_, LinkFixture)

BOOST_AUTO_TEST_CASE(binding_direct_capacity_is_reported_on_legacy_link)
{
    OptimEntityContainer optimEntityContainer(linearProblem);
    setUpLink(optimEntityContainer);
    // Maximize the direct flow
    linearProblem.setObjectiveCoefficient(linearProblem.lookupVariable("direct_flow_t0"), -1.);
    linearProblem.solve(false);

    applyLinkCapacityResults(*problemeHebdo, linearProblem, 0);

    const auto& ntc = problemeHebdo->ValeursDeNTC[0];
    BOOST_CHECK_CLOSE(ntc.directCapacity(0), 160., 1e-6);
    BOOST_CHECK_CLOSE(ntc.indirectCapacity(0), 190., 1e-6);
    // The study NTC is left untouched
    BOOST_CHECK_EQUAL(ntc.ValeurDeNTCOrigineVersExtremite[0], 1000.);
    BOOST_CHECK_EQUAL(ntc.ValeurDeNTCExtremiteVersOrigine[0], 1000.);
    // Same sign as the reduced cost of a flow variable at its legacy upper bound
    BOOST_CHECK_CLOSE(problemeHebdo->ProblemeAResoudre->CoutsReduits[0], -1., 1e-6);
}

BOOST_AUTO_TEST_CASE(binding_indirect_capacity_is_reported_on_legacy_link)
{
    OptimEntityContainer optimEntityContainer(linearProblem);
    setUpLink(optimEntityContainer);
    // Maximize the indirect flow
    linearProblem.setObjectiveCoefficient(linearProblem.lookupVariable("direct_flow_t0"), 1.);
    linearProblem.solve(false);

    applyLinkCapacityResults(*problemeHebdo, linearProblem, 0);

    BOOST_CHECK_CLOSE(linearProblem.lookupVariable("direct_flow_t0")->solutionValue(), -190., 1e-6);
    BOOST_CHECK_CLOSE(problemeHebdo->ProblemeAResoudre->CoutsReduits[0], 1., 1e-6);
}

BOOST_AUTO_TEST_CASE(legacy_bounds_reapplied_on_flow_are_detected)
{
    OptimEntityContainer optimEntityContainer(linearProblem);
    setUpLink(optimEntityContainer);
    // Simulates a legacy filler running after LinkCapacityFiller
    linearProblem.lookupVariable("direct_flow_t0")->setBounds(-1000., 1000.);
    linearProblem.solve(false);

    BOOST_CHECK_THROW(applyLinkCapacityResults(*problemeHebdo, linearProblem, 0),
                      Antares::Error::RuntimeError);
}

BOOST_AUTO_TEST_SUITE_END()

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

    const auto* investedCapacity = linearProblem.lookupVariable(
      "my_link_invest.invested_capacity_t0");
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
    BOOST_CHECK_CLOSE(directCostCt->getUb(), 120.01, 1e-9);

    const auto* indirectCostCt = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::IntercoIndirectCostUpper::hour<0>");
    BOOST_REQUIRE(indirectCostCt);
    BOOST_CHECK_EQUAL(indirectCostCt->getCoefficient(indirectCost), 1.0);
    BOOST_CHECK_EQUAL(indirectCostCt->getCoefficient(investedCapacity), -5.0);
    BOOST_CHECK_CLOSE(indirectCostCt->getUb(), 140.01, 1e-9);
}

BOOST_AUTO_TEST_CASE(hurdle_cost_bounds_preserve_legacy_loop_flow_offsets)
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
    problemeHebdo->CoutDeTransport[0].IntercoGereeAvecLoopFlow = true;
    problemeHebdo->ValeursDeNTC.resize(1);
    problemeHebdo->ValeursDeNTC[0].ValeurDeLoopFlowOrigineVersExtremite = {20.0};
    problemeHebdo->CorrespondanceVarNativesVarOptim.resize(1);
    auto& varMap = problemeHebdo->CorrespondanceVarNativesVarOptim[0];
    varMap.NumeroDeVariableDuFluxDirect = {0};
    varMap.NumeroDeVariableDuFluxDirectPositif = {1};
    varMap.NumeroDeVariableDuFluxIndirectPositif = {2};

    addLegacyVariables({"direct_flow_t0", "direct_cost_t0", "indirect_cost_t0"});
    addModelerVariables(0, 0, optimEntityContainer);
    fillProblemWithLinkCapacityConnectionFiller({0, 0, 0, 0, 0}, optimEntityContainer);

    const auto* directCostCt = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::IntercoDirectCostUpper::hour<0>");
    BOOST_REQUIRE(directCostCt);
    BOOST_CHECK_CLOSE(directCostCt->getUb(), 100.01, 1e-9);

    const auto* indirectCostCt = linearProblem.lookupConstraint(
      "LinkCapacity::from<de>::to<fr>::IntercoIndirectCostUpper::hour<0>");
    BOOST_REQUIRE(indirectCostCt);
    BOOST_CHECK_CLOSE(indirectCostCt->getUb(), 160.01, 1e-9);
}

BOOST_AUTO_TEST_CASE(loop_flow_and_results_use_the_same_hour_for_an_unaligned_daily_interval)
{
    constexpr unsigned localFirst = 25;
    constexpr unsigned intervalLength = 24;
    constexpr unsigned weekFirst = 168;
    constexpr unsigned firstPdt = 24;
    constexpr unsigned globalFirst = weekFirst + firstPdt;

    init(linkConnectionSystem, linkConnectionLib);
    setData("availability_factor_direct", std::vector<double>(globalFirst + intervalLength, 4.));
    setData("availability_factor_indirect", std::vector<double>(globalFirst + intervalLength, 5.));
    setData("already_installed_availability_factor_direct",
            std::vector<double>(globalFirst + intervalLength, 6.));
    setData("already_installed_availability_factor_indirect",
            std::vector<double>(globalFirst + intervalLength, 7.));

    OptimEntityContainer optimEntityContainer(linearProblem);
    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());

    problemeHebdo->HeureDansLAnnee = weekFirst;
    problemeHebdo->weekInTheYear = 1;
    problemeHebdo->NomsDesPays = {"de", "fr"};
    problemeHebdo->NombreDePays = 2;
    problemeHebdo->NombreDInterconnexions = 1;
    problemeHebdo->PaysOrigineDeLInterconnexion = {0};
    problemeHebdo->PaysExtremiteDeLInterconnexion = {1};
    problemeHebdo->CoutDeTransport.resize(1);
    problemeHebdo->CoutDeTransport[0].IntercoGereeAvecDesCouts = true;
    problemeHebdo->CoutDeTransport[0].IntercoGereeAvecLoopFlow = true;
    problemeHebdo->ValeursDeNTC.resize(168);
    for (unsigned hour = 0; hour < problemeHebdo->ValeursDeNTC.size(); ++hour)
    {
        auto& ntc = problemeHebdo->ValeursDeNTC[hour];
        ntc.ValeurDeLoopFlowOrigineVersExtremite = {static_cast<double>(hour)};
        ntc.CapaciteEffectiveOrigineVersExtremite = {std::nan("")};
        ntc.CapaciteEffectiveExtremiteVersOrigine = {std::nan("")};
    }

    problemeHebdo->CorrespondanceVarNativesVarOptim.resize(intervalLength);
    std::vector<std::string> variableNames;
    for (unsigned pdt = 0; pdt < intervalLength; ++pdt)
    {
        auto& varMap = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        varMap.NumeroDeVariableDuFluxDirect = {static_cast<int>(3 * pdt)};
        varMap.NumeroDeVariableDuFluxDirectPositif = {static_cast<int>(3 * pdt + 1)};
        varMap.NumeroDeVariableDuFluxIndirectPositif = {static_cast<int>(3 * pdt + 2)};
        variableNames.push_back("direct_flow_t" + std::to_string(pdt));
        variableNames.push_back("direct_cost_t" + std::to_string(pdt));
        variableNames.push_back("indirect_cost_t" + std::to_string(pdt));
    }
    problemeHebdo->ProblemeAResoudre->CoutsReduits.assign(variableNames.size(), 0.);
    addLegacyVariables(variableNames);
    addModelerVariables(localFirst, localFirst + intervalLength - 1, optimEntityContainer);
    fillProblemWithLinkCapacityConnectionFiller({localFirst,
                                                 localFirst + intervalLength - 1,
                                                 globalFirst,
                                                 globalFirst + intervalLength - 1,
                                                 0},
                                                optimEntityContainer);

    BOOST_REQUIRE_EQUAL(problemeHebdo->linkCapacityConstraints.size(), intervalLength);
    for (unsigned pdt = 0; pdt < intervalLength; ++pdt)
    {
        const auto& link = problemeHebdo->linkCapacityConstraints[pdt];
        BOOST_CHECK_EQUAL(link.pdt, pdt);
        BOOST_CHECK_EQUAL(link.flowVariable, 3 * pdt);

        auto* investedCapacity = linearProblem.lookupVariable("my_link_invest.invested_capacity_t"
                                                              + std::to_string(localFirst + pdt));
        BOOST_REQUIRE(investedCapacity);
        investedCapacity->setBounds(10., 10.);
        BOOST_CHECK_EQUAL(link.direct->getCoefficient(investedCapacity), -4.);
        BOOST_CHECK_EQUAL(link.indirect->getCoefficient(investedCapacity), -5.);

        const auto suffix = "::hour<" + std::to_string(weekFirst + pdt) + ">";
        const auto* directCostCt = linearProblem.lookupConstraint(
          "LinkCapacity::from<de>::to<fr>::IntercoDirectCostUpper" + suffix);
        const auto* indirectCostCt = linearProblem.lookupConstraint(
          "LinkCapacity::from<de>::to<fr>::IntercoIndirectCostUpper" + suffix);
        BOOST_REQUIRE(directCostCt);
        BOOST_REQUIRE(indirectCostCt);
        BOOST_CHECK_EQUAL(directCostCt->getCoefficient(
                            linearProblem.lookupVariable("direct_cost_t" + std::to_string(pdt))),
                          1.);
        BOOST_CHECK_EQUAL(indirectCostCt->getCoefficient(
                            linearProblem.lookupVariable("indirect_cost_t" + std::to_string(pdt))),
                          1.);
        const double loopFlow = problemeHebdo->ValeursDeNTC[firstPdt + pdt]
                                  .ValeurDeLoopFlowOrigineVersExtremite[0];
        BOOST_CHECK_CLOSE(directCostCt->getUb(), 120.01 - loopFlow, 1e-9);
        BOOST_CHECK_CLOSE(indirectCostCt->getUb(), 140.01 + loopFlow, 1e-9);
    }

    linearProblem.solve(false);
    applyLinkCapacityResults(*problemeHebdo, linearProblem, 1);
    for (unsigned pdt = 0; pdt < intervalLength; ++pdt)
    {
        const auto& ntc = problemeHebdo->ValeursDeNTC[firstPdt + pdt];
        BOOST_CHECK_CLOSE(ntc.directCapacity(0), 160., 1e-6);
        BOOST_CHECK_CLOSE(ntc.indirectCapacity(0), 190., 1e-6);
    }
    BOOST_CHECK(std::isnan(
      problemeHebdo->ValeursDeNTC[firstPdt - 1].CapaciteEffectiveOrigineVersExtremite[0]));
    BOOST_CHECK(std::isnan(problemeHebdo->ValeursDeNTC[firstPdt + intervalLength]
                             .CapaciteEffectiveOrigineVersExtremite[0]));
}

BOOST_AUTO_TEST_SUITE_END()

namespace
{
struct CompatibilityCheckStudy: Antares::Data::Study
{
    using Study::checkModelerDataCompatibility;
};

struct TransmissionCapacitiesFixture: FillerFixture
{
    CompatibilityCheckStudy study;
    Antares::Data::AreaLink* link = nullptr;

    TransmissionCapacitiesFixture()
    {
        init(linkConnectionSystem, linkConnectionLib);
        auto* de = addAreaToListOfAreas(study.areas, "de");
        auto* fr = addAreaToListOfAreas(study.areas, "fr");
        link = AreaAddLinkBetweenAreas(de, fr);
        problemeHebdo->modelerData = nullptr;
        study.setModelerData(std::move(modelerData));
    }
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(_LinkCapacityTransmissionCapacities_, TransmissionCapacitiesFixture)

BOOST_AUTO_TEST_CASE(link_using_transmission_capacities_is_accepted)
{
    link->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::enabled;
    BOOST_CHECK_NO_THROW(study.checkModelerDataCompatibility());
}

BOOST_AUTO_TEST_CASE(flow_quad_is_rejected_with_link_capacity_connections)
{
    const Antares::Data::VariablePrintInfo flowQuadInfo(
      Antares::Solver::Variable::Category::FileLevel::va,
      Antares::Solver::Variable::Category::DataLevel::link);
    study.parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuadInfo);
    link->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::enabled;

    // The quadratic optimisation only runs when a link has an impedance
    link->parameters[Antares::Data::fhlImpedances][0] = 1.;

    study.parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", true);
    BOOST_CHECK_THROW(study.checkModelerDataCompatibility(), Antares::Error::LoadingError);

    study.parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", false);
    BOOST_CHECK_NO_THROW(study.checkModelerDataCompatibility());

    study.parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", true);
    link->parameters[Antares::Data::fhlImpedances][0] = 0.;
    BOOST_CHECK_NO_THROW(study.checkModelerDataCompatibility());
}

BOOST_AUTO_TEST_CASE(modeler_capacity_retains_zero_legacy_capacity_link)
{
    link->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::enabled;
    link->directCapacities.fill(0.);
    link->indirectCapacities.fill(0.);

    auto* gb = addAreaToListOfAreas(study.areas, "gb");
    auto* unconnectedLink = AreaAddLinkBetweenAreas(link->from, gb);
    unconnectedLink->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::enabled;
    unconnectedLink->directCapacities.fill(0.);
    unconnectedLink->indirectCapacities.fill(0.);
    study.areas.rebuildIndexes();
    for (unsigned index = 0; index < study.areas.size(); ++index)
    {
        study.areas[index]->createMissingData();
    }

    const auto analyzer = Antares::Data::BuildInactiveComponentsAnalyzer(study);

    BOOST_CHECK(!analyzer->linkIsAllZero(0));
    BOOST_CHECK(analyzer->linkIsAllZero(1));

    study.setModelerData(nullptr);
    BOOST_CHECK(Antares::Data::BuildInactiveComponentsAnalyzer(study)->linkIsAllZero(0));
}

BOOST_AUTO_TEST_CASE(link_ignoring_transmission_capacities_is_rejected)
{
    link->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::null;
    BOOST_CHECK_THROW(study.checkModelerDataCompatibility(), Antares::Error::LoadingError);
}

BOOST_AUTO_TEST_CASE(copper_plate_link_is_rejected)
{
    link->transmissionCapacities = Antares::Data::LocalTransmissionCapacities::infinite;
    BOOST_CHECK_THROW(study.checkModelerDataCompatibility(), Antares::Error::LoadingError);
}

BOOST_AUTO_TEST_SUITE_END()
