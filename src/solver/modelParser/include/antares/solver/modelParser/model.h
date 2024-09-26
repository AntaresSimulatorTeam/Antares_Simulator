
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

#pragma once
#include <string>
#include <vector>

namespace Antares::Solver::ModelParser
{
static std::string yaml_string = R"(
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
)";

// Define structures
struct Parameter
{
    std::string name;
    bool time_dependent;
    bool scenario_dependent;
};

struct Variable
{
    std::string name;
    double lower_bound;
    double upper_bound;
};

struct Port
{
    std::string name;
    std::string type;
};

struct PortFieldDefinition
{
    std::string port;
    std::string field;
    std::string definition;
};

struct Constraint
{
    std::string name;
    std::string expression;
};

struct Model
{
    std::string id;
    std::string description;
    std::vector<Parameter> parameters;
    std::vector<Variable> variables;
    std::vector<Port> ports;
    std::vector<PortFieldDefinition> port_field_definitions;
    std::vector<Constraint> constraints;
    std::string objective;
};

struct PortType
{
    std::string id;
    std::string description;
    std::vector<std::string> fields;
};

struct Library
{
    std::string id;
    std::string description;
    std::vector<PortType> port_types;
    std::vector<Model> models;
};
} // namespace Antares::Solver::ModelParser
