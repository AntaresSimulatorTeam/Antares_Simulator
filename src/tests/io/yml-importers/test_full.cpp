/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <yaml-cpp/exceptions.h>

#include <boost/test/unit_test.hpp>

#include "antares/expressions/nodes/Node.h"
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/study/system-model/library.h"

#include "enum_operators.h"

using namespace std::string_literals;
using namespace Antares::Solver;
using namespace Antares::Study;

void checkParameter(const SystemModel::Parameter& parameter,
                    const std::string& name,
                    bool timeDependent,
                    bool scenarioDependent)
{
    std::cout << "Parameter: " << parameter.Id() << std::endl;
    BOOST_CHECK_EQUAL(parameter.Id(), name);
    BOOST_CHECK_EQUAL(parameter.isTimeDependent(), timeDependent);
    BOOST_CHECK_EQUAL(parameter.isScenarioDependent(), scenarioDependent);
}

void checkVariable(const SystemModel::Variable& variable,
                   const std::string& name,
                   const std::string& lowerBound,
                   const std::string& upperBound,
                   SystemModel::ValueType type,
                   SystemModel::TimeDependent timeDependent,
                   SystemModel::ScenarioDependent scenarioDependent)
{
    std::cout << "Variable: " << variable.Id() << std::endl;
    BOOST_CHECK_EQUAL(variable.Id(), name);
    BOOST_CHECK_EQUAL(variable.LowerBound().Value(), lowerBound);
    BOOST_CHECK_EQUAL(variable.UpperBound().Value(), upperBound);
    BOOST_CHECK_EQUAL(variable.Type(), type);
}

void checkConstraint(const SystemModel::Constraint& constraint,
                     const std::string& name,
                     const std::string& expression)
{
    std::cout << "Constraint: " << constraint.Id() << std::endl;
    BOOST_CHECK_EQUAL(constraint.Id(), name);
    BOOST_CHECK_EQUAL(constraint.expression().Value(), expression);
}

BOOST_AUTO_TEST_CASE(test_full)
{
    auto library = R"(
# Copyright (c) 2024, RTE (https://www.rte-france.com)
#
# See AUTHORS.txt
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# SPDX-License-Identifier: MPL-2.0
#
# This file is part of the Antares project.
library:
  id: basic
  description: Basic library

  port-types:
    - id: flow
      description: A port which transfers power flow
      fields:
        - id: flow

  models:
    - id: generator
      description: A basic generator model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
        - id: p_max
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: generation
          lower-bound: 0
          upper-bound: p_max
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      objective: cost * generation

    - id: node
      description: A basic balancing node model
      ports:
        - id: injection_port
          type: flow
      binding-constraints:
        - id: balance
          expression:  injection_port = 0

    - id: spillage
      description: A basic spillage model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: spillage
          lower-bound: 0
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -spillage

    - id: unsupplied
      description: A basic unsupplied model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: unsupplied_energy
          lower-bound: 0
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: unsupplied_energy

    - id: demand
      description: A basic fixed demand model
      parameters:
        - id: demand
          time-dependent: true
          scenario-dependent: true
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -demand

    - id: short-term-storage
      description: A short term storage
      parameters:
        - id: efficiency
        - id: level_min
        - id: level_max
        - id: p_max_withdrawal
        - id: p_max_injection
        - id: inflows
      variables:
        - id: injection
          lower-bound: 0
          upper-bound: p_max_injection
        - id: withdrawal
          lower-bound: 0
          upper-bound: p_max_withdrawal
        - id: level
          lower-bound: level_min
          upper-bound: level_max
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: injection - withdrawal
      constraints:
        - id: Level equation
          expression: level - level - efficiency * injection + withdrawal = inflows

    - id: thermal-cluster-dhd
      description: DHD model for thermal cluster
      parameters:
        - id: cost
        - id: p_min
        - id: p_max
        - id: d_min_up
        - id: d_min_down
        - id: nb_units_max
        - id: nb_failures
          time-dependent: true
          scenario-dependent: true
      variables:
        - id: generation
          lower-bound: 0
          upper-bound: nb_units_max * p_max
          time-dependent: true
          scenario-dependent: true
        - id: nb_on
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - id: nb_stop
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - id: nb_start
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      constraints:
        - id: Max generation
          expression: generation <= nb_on * p_max
        - id: Min generation
          expression: generation >= nb_on * p_min
        - id: Number of units variation
          expression: nb_on = nb_on + nb_start - nb_stop
        - id: Min up time
          expression: t-d_min_up + 1 <= nb_on
        - id: Min down time
          expression: t-d_min_down + 1 <= nb_units_max - nb_on
      objective: cost * generation
    )"s;

    try
    {
        YmlModel::Parser parser;
        YmlModel::Library libraryObj = parser.parse(library);
        SystemModel::Library lib = ModelConverter::convert(libraryObj);
        BOOST_CHECK_EQUAL(lib.Id(), "basic");
        BOOST_CHECK_EQUAL(lib.Description(), "Basic library");

        BOOST_REQUIRE_EQUAL(lib.PortTypes().size(), 1);
        auto& portType = lib.PortTypes().at("flow");
        BOOST_CHECK_EQUAL(portType.Id(), "flow");
        BOOST_CHECK_EQUAL(portType.Description(), "A port which transfers power flow");

        BOOST_REQUIRE_EQUAL(portType.Fields().size(), 1);
        auto& portTypeField = portType.Fields().at(0);
        BOOST_CHECK_EQUAL(portTypeField.Id(), "flow");

        BOOST_REQUIRE_EQUAL(lib.Models().size(), 7);
        auto& model0 = lib.Models().at("generator");
        BOOST_CHECK_EQUAL(model0.Id(), "generator");
        BOOST_CHECK_EQUAL(model0.Objective().Value(), "cost * generation");

        BOOST_REQUIRE_EQUAL(model0.getConstraints().size(), 0);
        BOOST_REQUIRE_EQUAL(model0.Parameters().size(), 2);
        BOOST_REQUIRE_EQUAL(model0.Variables().size(), 1);
        // BOOST_REQUIRE_EQUAL(model0.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model0.PortFieldDefinitions().size(), 1); Unsuported

        checkParameter(model0.Parameters().at("cost"), "cost", false, false);
        checkParameter(model0.Parameters().at("p_max"), "p_max", false, false);

        checkVariable(model0.Variables().at("generation"),
                      "generation",
                      "0",
                      "p_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::YES,
                      SystemModel::ScenarioDependent::YES);

        // auto& port = model0.Ports().at("injection_port");
        // BOOST_CHECK_EQUAL(port.Id(), "injection_port");
        //  other properties

        auto& model1 = lib.Models().at("node");
        BOOST_CHECK_EQUAL(model1.Id(), "node");
        // BOOST_REQUIRE_EQUAL(model1.getConstraints().size(), 1);
        BOOST_REQUIRE_EQUAL(model1.Parameters().size(), 0);
        BOOST_REQUIRE_EQUAL(model1.Variables().size(), 0);
        // BOOST_REQUIRE_EQUAL(model1.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model1.PortFieldDefinitions().size(), 0); Unsuported

        auto& model2 = lib.Models().at("spillage");
        BOOST_CHECK_EQUAL(model2.Id(), "spillage");
        BOOST_REQUIRE_EQUAL(model2.getConstraints().size(), 0);
        BOOST_REQUIRE_EQUAL(model2.Parameters().size(), 1);
        BOOST_REQUIRE_EQUAL(model2.Variables().size(), 1);
        // BOOST_REQUIRE_EQUAL(model2.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model2.PortFieldDefinitions().size(), 1); Unsuported

        checkParameter(model2.Parameters().at("cost"), "cost", false, false);
        checkVariable(model2.Variables().at("spillage"),
                      "spillage",
                      "0",
                      "",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::NO,
                      SystemModel::ScenarioDependent::NO);

        auto& model3 = lib.Models().at("unsupplied");
        BOOST_CHECK_EQUAL(model3.Id(), "unsupplied");
        BOOST_REQUIRE_EQUAL(model3.getConstraints().size(), 0);
        BOOST_REQUIRE_EQUAL(model3.Parameters().size(), 1);
        BOOST_REQUIRE_EQUAL(model3.Variables().size(), 1);
        // BOOST_REQUIRE_EQUAL(model3.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model3.PortFieldDefinitions().size(), 1); Unsuported
        checkParameter(model3.Parameters().at("cost"), "cost", false, false);
        checkVariable(model3.Variables().at("unsupplied_energy"),
                      "unsupplied_energy",
                      "0",
                      "",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::NO,
                      SystemModel::ScenarioDependent::NO);

        auto& model4 = lib.Models().at("demand");
        BOOST_CHECK_EQUAL(model4.Id(), "demand");
        BOOST_REQUIRE_EQUAL(model4.getConstraints().size(), 0);
        BOOST_REQUIRE_EQUAL(model4.Parameters().size(), 1);
        BOOST_REQUIRE_EQUAL(model4.Variables().size(), 0);
        // BOOST_REQUIRE_EQUAL(model4.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model4.PortFieldDefinitions().size(), 1); Unsuported
        checkParameter(model4.Parameters().at("demand"), "demand", true, true);

        auto& model5 = lib.Models().at("short-term-storage");
        BOOST_CHECK_EQUAL(model5.Id(), "short-term-storage");
        BOOST_REQUIRE_EQUAL(model5.getConstraints().size(), 1);
        BOOST_REQUIRE_EQUAL(model5.Parameters().size(), 6);
        BOOST_REQUIRE_EQUAL(model5.Variables().size(), 3);
        // BOOST_REQUIRE_EQUAL(model5.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model5.PortFieldDefinitions().size(), 1); Unsuported
        checkParameter(model5.Parameters().at("efficiency"), "efficiency", true, true);
        checkParameter(model5.Parameters().at("level_min"), "level_min", true, true);
        checkParameter(model5.Parameters().at("level_max"), "level_max", true, true);
        checkParameter(model5.Parameters().at("p_max_withdrawal"), "p_max_withdrawal", true, true);
        checkParameter(model5.Parameters().at("p_max_injection"), "p_max_injection", true, true);
        checkParameter(model5.Parameters().at("inflows"), "inflows", true, true);
        checkVariable(model5.Variables().at("injection"),
                      "injection",
                      "0",
                      "p_max_injection",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::NO,
                      SystemModel::ScenarioDependent::NO);
        checkVariable(model5.Variables().at("withdrawal"),
                      "withdrawal",
                      "0",
                      "p_max_withdrawal",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::NO,
                      SystemModel::ScenarioDependent::NO);
        checkVariable(model5.Variables().at("level"),
                      "level",
                      "level_min",
                      "level_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::NO,
                      SystemModel::ScenarioDependent::NO);
        checkConstraint(model5.getConstraints().at("Level equation"),
                        "Level equation",
                        "level - level - efficiency * injection + withdrawal = inflows");

        auto& model6 = lib.Models().at("thermal-cluster-dhd");
        BOOST_CHECK_EQUAL(model6.Id(), "thermal-cluster-dhd");
        BOOST_REQUIRE_EQUAL(model6.getConstraints().size(), 5);
        BOOST_REQUIRE_EQUAL(model6.Parameters().size(), 7);
        BOOST_REQUIRE_EQUAL(model6.Variables().size(), 4);
        // BOOST_REQUIRE_EQUAL(model6.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model6.PortFieldDefinitions().size(), 1); Unsuported
        checkParameter(model6.Parameters().at("cost"), "cost", true, true);
        checkParameter(model6.Parameters().at("p_min"), "p_min", true, true);
        checkParameter(model6.Parameters().at("p_max"), "p_max", true, true);
        checkParameter(model6.Parameters().at("d_min_up"), "d_min_up", true, true);
        checkParameter(model6.Parameters().at("d_min_down"), "d_min_down", true, true);
        checkParameter(model6.Parameters().at("nb_units_max"), "nb_units_max", true, true);
        checkParameter(model6.Parameters().at("nb_failures"), "nb_failures", true, true);
        checkVariable(model6.Variables().at("generation"),
                      "generation",
                      "0",
                      "nb_units_max * p_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::YES,
                      SystemModel::ScenarioDependent::YES);
        checkVariable(model6.Variables().at("nb_on"),
                      "nb_on",
                      "0",
                      "nb_units_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::YES,
                      SystemModel::ScenarioDependent::NO);
        checkVariable(model6.Variables().at("nb_stop"),
                      "nb_stop",
                      "0",
                      "nb_units_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::YES,
                      SystemModel::ScenarioDependent::NO);
        checkVariable(model6.Variables().at("nb_start"),
                      "nb_start",
                      "0",
                      "nb_units_max",
                      SystemModel::ValueType::FLOAT,
                      SystemModel::TimeDependent::YES,
                      SystemModel::ScenarioDependent::NO);
        checkConstraint(model6.getConstraints().at("Max generation"),
                        "Max generation",
                        "generation <= nb_on * p_max");
        checkConstraint(model6.getConstraints().at("Min generation"),
                        "Min generation",
                        "generation >= nb_on * p_min");
        checkConstraint(model6.getConstraints().at("Number of units variation"),
                        "Number of units variation",
                        "nb_on = nb_on + nb_start - nb_stop");
        checkConstraint(model6.getConstraints().at("Min up time"),
                        "Min up time",
                        "t-d_min_up + 1 <= nb_on");
        checkConstraint(model6.getConstraints().at("Min down time"),
                        "Min down time",
                        "t-d_min_down + 1 <= nb_units_max - nb_on");
        BOOST_CHECK_EQUAL(model6.Objective().Value(), "cost * generation");
    }
    catch (const YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        BOOST_FAIL(e.what());
    }
}
