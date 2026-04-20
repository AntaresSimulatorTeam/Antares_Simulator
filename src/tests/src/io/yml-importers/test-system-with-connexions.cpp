#define WIN32_LEAN_AND_MEAN

#include <string>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/converter.h>
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/study/system-model/library.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>

#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/forbidden-nodes/ForbiddenNodesVisitor.h"
// clang-format on

using namespace std::string_literals;
using namespace Antares::IO::Inputs;
using namespace Antares::ModelerStudy;
using namespace Antares::IO::Inputs::ForbidNodes;

BOOST_AUTO_TEST_SUITE(readYamlInput)
// This yaml lib contains only a port type.
// Note that : an empty model is added to prevent parser from failing when reading the lib
static const auto onlyPortTypeYmlLib = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id:  power_flow
      description: power flow
      fields:
        - id: flow
      area-connection:
        injection-to-balance: flow
        spillage-bound: 
        unsupplied-energy-bound: 
  models:
    - id: empty model
      description: we need this empty model, otherwise parser fails !
)"s;

BOOST_AUTO_TEST_CASE(port_type_area_connection_is_read_correctly)
{
    YmlModel::Parser parserModel;
    SystemModel::Library library = ModelConverter::convert(parserModel.parse(onlyPortTypeYmlLib));

    BOOST_CHECK_EQUAL(library.PortTypes().size(), 1);

    SystemModel::PortType portType = library.PortTypes().begin()->second;

    auto fields = portType.Fields();
    BOOST_CHECK_EQUAL(fields.size(), 1);
    BOOST_CHECK_EQUAL(fields[0].Id(), "flow");

    auto area_connection = portType.areaConnection();
    BOOST_CHECK(area_connection.has_value());
    BOOST_CHECK_EQUAL(area_connection->inject_to_balance, "flow");
    BOOST_CHECK(area_connection->spillage_bound.empty());
    BOOST_CHECK(area_connection->unsupplied_energy_bound.empty());
}

// This yaml lib contains only a port type, but with a more complete area connection.
// Note that : as previously (and for same reason), an empty model is added.
static const auto onlyPortTypeYmlLib2 = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id: power_flow
      description: power flow
      fields:
        - id: flow
        - id: to-area-bound
        - id: from-area-bound
      area-connection:
        injection-to-balance: flow
        spillage-bound: to-area-bound
        unsupplied-energy-bound: from-area-bound

  models:
    - id: empty model
      description: we need this empty model, otherwise parser fails !
)"s;

BOOST_AUTO_TEST_CASE(port_type_area_connection_is_more_complete_and_is_still_read_correctly)
{
    YmlModel::Parser parserModel;
    SystemModel::Library library = ModelConverter::convert(parserModel.parse(onlyPortTypeYmlLib2));

    BOOST_CHECK_EQUAL(library.PortTypes().size(), 1);

    SystemModel::PortType portType = library.PortTypes().begin()->second;

    auto fields = portType.Fields();
    BOOST_CHECK_EQUAL(fields.size(), 3);
    BOOST_CHECK_EQUAL(fields[0].Id(), "flow");
    BOOST_CHECK_EQUAL(fields[1].Id(), "to-area-bound");
    BOOST_CHECK_EQUAL(fields[2].Id(), "from-area-bound");

    auto area_connection = portType.areaConnection();
    BOOST_CHECK(area_connection.has_value());
    BOOST_CHECK_EQUAL(area_connection->inject_to_balance, "flow");
    BOOST_CHECK_EQUAL(area_connection->spillage_bound, "to-area-bound");
    BOOST_CHECK_EQUAL(area_connection->unsupplied_energy_bound, "from-area-bound");
}

static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id:  power_flow
      description: power flow
      fields:
        - id: flow
      area-connection:
        injection-to-balance: flow
        spillage-bound: 
        unsupplied-energy-bound: 

  models:
    - id: balance_node
      description: A basic balancing node model
      ports:
        - id: injection_port
          type: power_flow
      binding-constraints:
        - id: balance
          expression:  sum_connections(injection_port.flow) = 0

    - id: node_gen
      description: A simple node with a generator
      variables:
        - id: total_gen
          lower-bound: 0
          upper-bound: 1000
          variable-type: continuous
      ports:
        - id: some_other_port
          type: power_flow
      port-field-definitions:
        - port: some_other_port
          field: flow
          definition: total_gen

    - id: node_load
      description: A simple node with one load
      parameters:
       - id: load
         time-dependent: false
         scenario-dependent: false
      ports:
        - id: injection_port
          type: power_flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -load
    )"s;

static const auto systemYaml = R"(
system:
  id: my_system
  description: some descrition for my system

  model-libraries: my_lib

  components:
  - id: N
    model: my_lib.balance_node
    scenario-group: sg
  - id: NG
    model: my_lib.node_gen
    scenario-group: sg
  - id: NL
    model: my_lib.node_load
    scenario-group: sg
    parameters:
    - id: load
      time-dependent: false
      scenario-dependent: false
      value: 100

  connections:
    - component1: N
      port1: injection_port
      component2: NG
      port2: some_other_port

  area-connections:
    - component: NG
      port: some_other_port
      area: some_area
    - component: NL
      port: injection_port
      area: some_other_area
    )"s;

BOOST_AUTO_TEST_CASE(two_components_connected_by_ports_of_same_type_but_different)
{
    YmlModel::Parser parserModel;
    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml)));

    YmlSystem::Parser parserSystem;
    YmlSystem::System system = parserSystem.parse(systemYaml);
    auto systemModel = SystemConverter::convert(system, libraries);

    const auto& components = systemModel.Components();
    const auto component_N = std::ranges::find_if(components,
                                                  [](const auto& component)
                                                  { return component.Id() == "N"; });

    const std::string port_id = "injection_port";
    auto connections_to_N = component_N->componentConnectionsViaPort(port_id);

    BOOST_CHECK_EQUAL(connections_to_N.size(), 1);
    BOOST_CHECK_EQUAL(connections_to_N[0].port()->Id(), "some_other_port");
    BOOST_CHECK_EQUAL(connections_to_N[0].component()->Id(), "NG");

    // Symmetrically, check connexions of NG
    const auto component_NG = std::ranges::find_if(components,
                                                   [](const auto& component)
                                                   { return component.Id() == "NG"; });
    auto connections_to_NG = component_NG->componentConnectionsViaPort("some_other_port");

    BOOST_CHECK_EQUAL(connections_to_NG.size(), 1);
    BOOST_CHECK_EQUAL(connections_to_NG[0].port()->Id(), "injection_port");
    BOOST_CHECK_EQUAL(connections_to_NG[0].component()->Id(), "N");

    // Check area connections
    BOOST_CHECK_EQUAL(component_N->areaConnectedToPort("injection_port").has_value(), false);

    BOOST_CHECK_EQUAL(component_NG->areaConnectedToPort("some_other_port").has_value(), true);
    BOOST_CHECK_EQUAL(component_NG->areaConnectedToPort("some_other_port").value(), "some_area");

    const auto component_NL = std::ranges::find_if(components,
                                                   [](const auto& component)
                                                   { return component.Id() == "NL"; });
    BOOST_CHECK_EQUAL(component_NL->areaConnectedToPort("injection_port").has_value(), true);
    BOOST_CHECK_EQUAL(component_NL->areaConnectedToPort("injection_port").value(),
                      "some_other_area");
}

static const auto thermalConnectionLib = R"(
library:
  id: invest_lib

  port-types:
    - id: capacity_port
      fields:
        - id: capacity
      thermal-capacity-connection: # Explicitly use "capacity" in the name as later on there might be other thermal hybrid connections involving thermal generation
        capacity-field: capacity
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

BOOST_AUTO_TEST_CASE(thermal_capacity_connectivity)
{
    YmlModel::Parser parserModel;
    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(thermalConnectionLib)));
    YmlSystem::Parser parserSystem;
    YmlSystem::System system = parserSystem.parse(thermalConnectionSystem);
    auto systemModel = SystemConverter::convert(system, libraries);
    const auto& thermalInvestComponent = systemModel.Components().at(0);
    const auto& connection = thermalInvestComponent.portToThermalCapacityConnections();
    BOOST_CHECK_EQUAL(connection.size(), 1);
    const auto& [portId, thermalConnection] = *connection.begin();

    BOOST_CHECK_EQUAL(portId, "capacity_port");
    BOOST_CHECK_EQUAL(thermalConnection.areaId, "fr");
    BOOST_CHECK_EQUAL(thermalConnection.clusterId, "nuclear1");
}

BOOST_AUTO_TEST_CASE(binding_constraint_with_sum_connections_op_is_non_linear___exception_raised)
{
    // This library defines :
    // - a port field defintion with a non linear expressions in a model
    // - a binding constraint trying to reach the previous non-linear expression via a port.
    const auto yml_lib = R"(
      library:
        id: my_lib
        description: whatever
      
        port-types:
          - id: transfering_expression
            description: A port to transfer a non linear expression
            fields:
              - id: my_field
      
        models:
          - id: area
            variables:
              - id: var_1
                lower-bound: 0
                upper-bound: 1
                variable-type: continuous
              - id: var_2
                lower-bound: 0
                upper-bound: 1
                variable-type: continuous
            ports:
              - id: convey_non_linear_expr
                type: transfering_expression
            port-field-definitions:
              - port: convey_non_linear_expr
                field: my_field
                definition: dual(my_constraint)
            binding-constraints:
              - id: my_constraint
                expression: var_1 - var_2 = 0
      
          - id: some_model
            ports:
              - id: convey_non_linear_expr
                type: transfering_expression
            binding-constraints:
              - id: my_other_constraint
                expression: sum_connections(convey_non_linear_expr.my_field) # Forbidden ==> causes a failure
    )";

    // This system defines 2 components from previous models, and connects them through a port
    // field.
    const auto yml_system = R"(
      system:
        id: some system
        description: whatever
        components:
          - id: my_area
            model: my_lib.area
      
          - id: model_instance
            model: my_lib.some_model
      
        connections:
          - component1: my_area
            port1: convey_non_linear_expr
            component2: model_instance
            port2: convey_non_linear_expr
    )";

    YmlModel::Parser parserModel;
    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(yml_lib)));

    YmlSystem::Parser parserSystem;
    YmlSystem::System system = parserSystem.parse(yml_system);

    std::string err_msg = "'FunctionNode::dual' is not allowed in expression 'dual(my_constraint)'";
    BOOST_CHECK_EXCEPTION(SystemConverter::convert(system, libraries),
                          InputError,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()
