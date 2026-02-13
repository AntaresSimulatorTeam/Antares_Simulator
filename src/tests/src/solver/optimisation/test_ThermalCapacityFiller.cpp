// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

#include <boost/test/unit_test.hpp>

#include "FillerFixture.h"

using namespace std::string_literals;

using namespace Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Optimisation;
using namespace LinearProblemApi;
using namespace LinearProblemDataImpl;

static const auto thermalConnectionLib = R"(
library:
  id: invest_lib

  port-types:
    - id: capacity_port
      fields:
        - id: capacity
      thermal-capacity-connection: # Explicitly use "capacity" in the name as later on there might be other thermal hybrid connections involving thermal generation
        - capacity-field: capacity

    - id: asset_management
      fields:
        - id: gold
      thermal-capacity-connection:
        - capacity-field: gold
    - id: my_port_type
      fields:
        - id: my_field
      thermal-capacity-connection:
        - capacity-field: my_field
  models:
    - id: thermal_invest
      parameters:
        - id: investment_cost
          scenario-dependent: false
          time-dependent: false
        - id: max_investment
          scenario-dependent: false
          time-dependent: false
        - id: availability_factor
          scenario-dependent: true
          time-dependent: true
        - id: already_installed_capacity
          scenario-dependent: false
          time-dependent: false
        - id: already_installed_availability_factor
          scenario-dependent: true
          time-dependent: true
        # Integer investment (do not define max_investment in this case)
        - id: unit_size
          scenario-dependent: false
          time-dependent: false
        - id: max_units
          scenario-dependent: false
          time-dependent: false

      variables:
        - id: invested_capacity
          lower-bound: 0
          upper-bound: max_investment # or unit_size * max_units
          variable-type: continuous
        # Integer investment
        - id: invested_units
          lower-bound: 0
          upper-bound: max_units
          variable-type: integer

      constraints:
        # Integer investment
        - id: units_capa_relationship
          expression: invested_capacity = unit_size * invested_units

      ports:
        - id: capacity_port
          type: capacity_port

      port-field-definitions:
        - port: capacity_port
          field: capacity
          definition: availability_factor * invested_capacity + already_installed_availability_factor * already_installed_capacity

      objective-contributions:
        - id: objective
          expression: investment_cost * invested_capacity
    - id: bitcoin
      parameters:
        - id: satoshi
          scenario-dependent: false
          time-dependent: false

      variables:
        - id: oil
          lower-bound: 0
          upper-bound: satoshi

      ports:
        - id: share_hold
          type: asset_management

      port-field-definitions:
        - port: share_hold
          field: gold
          definition: -satoshi*oil
    - id: modelForCompo
      parameters:
        - id: modelForCompoPar
          scenario-dependent: false
          time-dependent: false

      variables:
        - id: modelForCompo
          lower-bound: 0
          upper-bound: 156

      ports:
        - id: model_port
          type: my_port_type

      port-field-definitions:
        - port: model_port
          field: my_field
          definition: modelForCompo*modelForCompoPar
)";

static const auto thermalConnectionSystem = R"(
system:
  id: my_system
  description: some description for my system

  model-libraries: my_lib

  components:
  - id: crypto_invest
    model: invest_lib.bitcoin
    parameters:
    - id: satoshi
      scenario-dependent: false
      time-dependent: false
      value: -1110
    scenario-group: sg
  - id: compo
    model: invest_lib.modelForCompo
    parameters:
    - id: modelForCompoPar
      scenario-dependent: false
      time-dependent: false
      value: 43
    scenario-group: sg
  - id: my_thermal_invest
    model: invest_lib.thermal_invest
    parameters:
    - id: investment_cost
      scenario-dependent: false
      time-dependent: false
      value: 502
    - id: max_investment
      scenario-dependent: false
      time-dependent: false
      value: 2000
    - id: availability_factor
      scenario-dependent: true
      time-dependent: true
      value: availability_factor
    - id: already_installed_capacity
      scenario-dependent: false
      time-dependent: false
      value: -140
    - id: already_installed_availability_factor
      scenario-dependent: true
      time-dependent: true
      value: already_installed_availability_factor
        # Integer investment (do not define max_investment in this case)
    - id: unit_size
      scenario-dependent: false
      time-dependent: false
      value: -658
    - id: max_units
      scenario-dependent: false
      time-dependent: false
      value: 210
    scenario-group: sg



  thermal-capacity-connections:
  - component : crypto_invest
    port: share_hold
    thermal-component:
      area: Carthage
      cluster-id: assets
  - component : compo
    port: model_port
    thermal-component:
      area: Persia
      cluster-id: east
  - component : my_thermal_invest
    port: capacity_port
    thermal-component:
      area: Rome
      cluster-id: short)";

static constexpr std::string_view dispatchableProductionVariableFormat
  = "DispatchableProduction::area<{}>::ThermalCluster<{}"
    ">::hour<{}>";
static constexpr std::string_view portVariableFormat = "{}_t{}";

constexpr std::string_view maxGenerationFromCapacityConstraintFormat = "MaxGenerationFromCapacity::"
                                                                       "area<{}>::ThermalCluster<{}"
                                                                       ">::hour<{}>";

struct ExpectedConstraint
{
    std::vector<std::pair<std::string, double>> terms;
    std::vector<double> upperBounds;
};
BOOST_FIXTURE_TEST_SUITE(_ComponentToThermalCapacityFiller_, FillerFixture)

BOOST_AUTO_TEST_CASE(add_two_max_generation_from_capacity_constraints)
{
    init(thermalConnectionSystem, thermalConnectionLib);
    setData("availability_factor", {4.0, -389});
    setData("already_installed_availability_factor", {1250., -153});

    OptimEntityContainer optimEntityContainer(linearProblem,
                                              &linearProblemData,
                                              &scenarioGroupRepository);

    optimEntityContainer.addFromSystemComponents(modelerData->system->Components());
    unsigned int nbTimeStep = 2;
    auto tsEnd = nbTimeStep - 1;

    const char* areas[] = {"carthage", "persia", "rome"};
    size_t nbAreas = std::size(areas);
    problemeHebdo->NomsDesPays.resize(nbAreas);
    // problemeHebdo->CorrespondanceCntNativesCntOptim.resize(nbTimeStep);
    problemeHebdo->CorrespondanceVarNativesVarOptim.resize(nbTimeStep);
    problemeHebdo->PaliersThermiquesDuPays.resize(nbAreas);

    std::ranges::transform(areas,
                           problemeHebdo->NomsDesPays.begin(),
                           [](const char* area) { return area; });
    std::vector<std::vector<std::string>> allClusters = {{"assets"},
                                                         {"north", "east", "south"},
                                                         {"short"}};
    const std::vector<std::string> connectedClusterPerAreas = {
      {allClusters.at(0).front() /*== assets*/},
      {allClusters.at(1).at(1) /*== east*/},
      {allClusters.at(2).front() /*== short*/}};
    int totalNbClusters = 0;
    for (int i = 0; i < nbAreas; i++)
    {
        PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[i];
        const auto& areaClusters = allClusters[i];
        PaliersThermiquesDuPays.NomsDesPaliersThermiques = areaClusters;
        auto& NumeroDuPalierDansLEnsembleDesPaliersThermiques
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques;
        NumeroDuPalierDansLEnsembleDesPaliersThermiques.resize(areaClusters.size());
        std::iota(NumeroDuPalierDansLEnsembleDesPaliersThermiques.begin(),
                  NumeroDuPalierDansLEnsembleDesPaliersThermiques.end(),
                  totalNbClusters);
        totalNbClusters += areaClusters.size();
    }

    for (int pdt = 0; pdt < nbTimeStep; ++pdt)
    {
        auto& numeroDeVariableDuPalierThermique = problemeHebdo
                                                    ->CorrespondanceVarNativesVarOptim[pdt]
                                                    .NumeroDeVariableDuPalierThermique;
        numeroDeVariableDuPalierThermique.resize(totalNbClusters);
        std::iota(numeroDeVariableDuPalierThermique.begin(),
                  numeroDeVariableDuPalierThermique.end(),
                  pdt * totalNbClusters);
        std::vector<std::string> dispatchableProductionVariables(totalNbClusters);
        int clusterGlobalIndex = 0;
        for (int areaIndex = 0; areaIndex < nbAreas; areaIndex++)
        {
            const auto& areaName = areas[areaIndex];

            for (const auto& clusterName: allClusters[areaIndex])
            {
                dispatchableProductionVariables[clusterGlobalIndex] = fmt::format(
                  dispatchableProductionVariableFormat,
                  areaName,
                  clusterName,
                  pdt);
                ++clusterGlobalIndex;
            }
        }
        addLegacyVariables(dispatchableProductionVariables);
    }
    addModelerVariables(0, tsEnd, optimEntityContainer);
    fillProblemWithThermalCapacityConnectionFiller({0, tsEnd, 0, tsEnd, 0}, optimEntityContainer);

    std::vector<ExpectedConstraint> expectedGemsVariableCoefPerConnection;
    std::pair<std::string, double> crypto_invest_oil = {"crypto_invest.oil",
                                                        -1110 /* -(-satoshi) */};
    expectedGemsVariableCoefPerConnection.emplace_back(std::vector{crypto_invest_oil,
                                                                   crypto_invest_oil},
                                                       std::vector{0., 0.});

    std::pair<std::string, double> compo_modelForCompo = {"compo.modelForCompo",
                                                          -43 /* -(modelForCompoPar)*/};
    expectedGemsVariableCoefPerConnection.emplace_back(std::vector{compo_modelForCompo,
                                                                   compo_modelForCompo},
                                                       std::vector{0., 0.});
    std::pair<std::string, double> my_thermal_invest_invested_capacity_t0 = {
      "my_thermal_invest.invested_capacity",
      -4 /*-(availability_factor)*/};
    std::pair<std::string, double> my_thermal_invest_invested_capacity_t1 = {
      "my_thermal_invest.invested_capacity",
      389 /*-(availability_factor)*/};
    expectedGemsVariableCoefPerConnection.emplace_back(
      std::vector{my_thermal_invest_invested_capacity_t0, my_thermal_invest_invested_capacity_t1},
      std::vector{-175000., 21420.}); // == already_installed_availability_factor *
                                      // already_installed_capacity ==(1250.,-153)*(-140)
    for (int pdt = 0; pdt < nbTimeStep; ++pdt)
    {
        for (int areaIndex = 0; areaIndex < nbAreas; areaIndex++)
        {
            const auto& area = areas[areaIndex];
            const auto& cluster = connectedClusterPerAreas[areaIndex];

            auto maxGenConstraint = linearProblem.lookupConstraint(
              fmt::format(maxGenerationFromCapacityConstraintFormat, area, cluster, pdt));
            const auto* dispatchableVar = linearProblem.lookupVariable(
              fmt::format(dispatchableProductionVariableFormat, area, cluster, pdt));
            BOOST_CHECK_EQUAL(maxGenConstraint->getCoefficient(dispatchableVar), 1);

            const auto& areaExpectedResult = expectedGemsVariableCoefPerConnection[areaIndex];
            const auto* portVariable = linearProblem.lookupVariable(
              fmt::format(portVariableFormat, areaExpectedResult.terms[pdt].first, pdt));
            BOOST_CHECK_EQUAL(maxGenConstraint->getCoefficient(portVariable),
                              areaExpectedResult.terms[pdt].second);
            BOOST_CHECK_EQUAL(maxGenConstraint->getLb(), -linearProblem.infinity());
            BOOST_CHECK_EQUAL(maxGenConstraint->getUb(), areaExpectedResult.upperBounds[pdt]);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
