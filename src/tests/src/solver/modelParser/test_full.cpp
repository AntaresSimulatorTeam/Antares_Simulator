
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

#include <yaml-cpp/exceptions.h>

#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/modelConverter/modelConverter.h"
#include "antares/solver/modelParser/model.h"
#include "antares/solver/modelParser/parser.h"
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

using namespace std::string_literals;
using namespace Antares::Solver;

void checkParameter(const ObjectModel::Parameter& parameter,
                    const std::string& name,
                    bool timeDependent,
                    bool scenarioDependent,
                    ObjectModel::ValueType type)
{
    BOOST_CHECK_EQUAL(parameter.Name(), name);
    BOOST_CHECK_EQUAL(parameter.isTimeDependent(), timeDependent);
    BOOST_CHECK_EQUAL(parameter.isScenarioDependent(), scenarioDependent);
    BOOST_CHECK_EQUAL(parameter.Type(), type);
}

void checkVariable(const ObjectModel::Variable& variable,
                   const std::string& name,
                   const std::string& lowerBound,
                   const std::string& upperBound,
                   ObjectModel::ValueType type)
{
    BOOST_CHECK_EQUAL(variable.Name(), name);
    BOOST_CHECK_EQUAL(variable.LowerBound().Value(), lowerBound);
    BOOST_CHECK_EQUAL(variable.UpperBound().Value(), upperBound);
    BOOST_CHECK_EQUAL(variable.Type(), type);
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
        - name: flow

  models:
    - id: generator
      description: A basic generator model
      parameters:
        - name: cost
          time-dependent: false
          scenario-dependent: false
        - name: p_max
          time-dependent: false
          scenario-dependent: false
      variables:
        - name: generation
          lower-bound: 0
          upper-bound: p_max
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      objective: expec(sum(cost * generation))

    - id: node
      description: A basic balancing node model
      ports:
        - name: injection_port
          type: flow
      binding-constraints:
        - name: balance
          expression:  sum_connections(injection_port.flow) = 0

    - id: spillage
      description: A basic spillage model
      parameters:
        - name: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - name: spillage
          lower-bound: 0
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -spillage

    - id: unsupplied
      description: A basic unsupplied model
      parameters:
        - name: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - name: unsupplied_energy
          lower-bound: 0
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: unsupplied_energy

    - id: demand
      description: A basic fixed demand model
      parameters:
        - name: demand
          time-dependent: true
          scenario-dependent: true
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -demand

    - id: short-term-storage
      description: A short term storage
      parameters:
        - name: efficiency
        - name: level_min
        - name: level_max
        - name: p_max_withdrawal
        - name: p_max_injection
        - name: inflows
      variables:
        - name: injection
          lower-bound: 0
          upper-bound: p_max_injection
        - name: withdrawal
          lower-bound: 0
          upper-bound: p_max_withdrawal
        - name: level
          lower-bound: level_min
          upper-bound: level_max
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: injection - withdrawal
      constraints:
        - name: Level equation
          expression: level[t] - level[t-1] - efficiency * injection + withdrawal = inflows

    - id: thermal-cluster-dhd
      description: DHD model for thermal cluster
      parameters:
        - name: cost
        - name: p_min
        - name: p_max
        - name: d_min_up
        - name: d_min_down
        - name: nb_units_max
        - name: nb_failures
          time-dependent: true
          scenario-dependent: true
      variables:
        - name: generation
          lower-bound: 0
          upper-bound: nb_units_max * p_max
          time-dependent: true
          scenario-dependent: true
        - name: nb_on
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - name: nb_stop
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - name: nb_start
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
      ports:
        - name: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      constraints:
        - name: Max generation
          expression: generation <= nb_on * p_max
        - name: Min generation
          expression: generation >= nb_on * p_min
        - name: Number of units variation
          expression: nb_on = nb_on[t-1] + nb_start - nb_stop
        - name: Min up time
          expression: sum(t-d_min_up + 1 .. t, nb_start) <= nb_on
        - name: Min down time
          expression: sum(t-d_min_down + 1 .. t, nb_stop) <= nb_units_max[t-d_min_down] - nb_on
      objective: expec(sum(cost * generation))
    )"s;

    try
    {
        ModelParser::Parser parser;
        ModelParser::Library libraryObj = parser.parse(library);
        ObjectModel::Library lib = ModelConverter::convert(libraryObj);
        BOOST_CHECK_EQUAL(lib.id(), "basic");
        BOOST_CHECK_EQUAL(lib.description(), "Basic library");

        BOOST_REQUIRE_EQUAL(lib.portTypes().size(), 1);
        auto& portType = lib.portTypes().at("flow");
        BOOST_CHECK_EQUAL(portType.id(), "flow");
        BOOST_CHECK_EQUAL(portType.description(), "A port which transfers power flow");

        BOOST_REQUIRE_EQUAL(portType.fields().size(), 1);
        auto& portTypeField = portType.fields().at(0);
        BOOST_CHECK_EQUAL(portTypeField.Name(), "flow");

        BOOST_REQUIRE_EQUAL(lib.models().size(), 7);
        auto& model0 = lib.models().at("generator");
        BOOST_CHECK_EQUAL(model0.Id(), "generator");
        BOOST_CHECK_EQUAL(model0.Objective().Value(), "expec(sum(cost * generation))");

        BOOST_REQUIRE_EQUAL(model0.getConstraints().size(), 0);
        BOOST_REQUIRE_EQUAL(model0.Parameters().size(), 2);
        BOOST_REQUIRE_EQUAL(model0.Variables().size(), 1);
        // BOOST_REQUIRE_EQUAL(model0.Ports().size(), 1); Unsuported
        //  BOOST_REQUIRE_EQUAL(model0.PortFieldDefinitions().size(), 1); Unsuported

        checkParameter(model0.Parameters().at("cost"),
                       "cost",
                       false,
                       false,
                       ObjectModel::ValueType::FLOAT);
        checkParameter(model0.Parameters().at("p_max"),
                       "p_max",
                       false,
                       false,
                       ObjectModel::ValueType::FLOAT);

        checkVariable(model0.Variables().at("generation"),
                      "generation",
                      "0",
                      "p_max",
                      ObjectModel::ValueType::FLOAT);
    }
    catch (const YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        BOOST_FAIL(e.what());
    }
}
