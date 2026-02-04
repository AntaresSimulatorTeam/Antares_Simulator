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
)";

static const auto thermalConnectionSystem = R"(
system:
  id: my_system
  description: some descrition for my system

  model-libraries: my_lib

  components:
  - id: my_thermal_invest
    model: invest_lib.thermal_invest
    parameters:
    - id: investment_cost
      scenario-dependent: false
      time-dependent: false
      value: 0
    - id: max_investment
      scenario-dependent: false
      time-dependent: false
      value: 0
    - id: availability_factor
      scenario-dependent: true
      time-dependent: true
      value: dd
    - id: already_installed_capacity
      scenario-dependent: false
      time-dependent: false
      value: 0
    - id: already_installed_availability_factor
      scenario-dependent: true
      time-dependent: true
      value: 0
        # Integer investment (do not define max_investment in this case)
    - id: unit_size
      scenario-dependent: false
      time-dependent: false
      value: 0
    - id: max_units
      scenario-dependent: false
      time-dependent: false
      value: 0


  thermal-capacity-connections:
  - component : my_thermal_invest
    port: capacity_port
    thermal-component:
      area: fr
      cluster-id: nuclear1)";
