#define WIN32_LEAN_AND_MEAN

#include <string>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/converter.h>
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/study/system-model/library.h"

using namespace std::string_literals;
using namespace Antares::IO::Inputs;
using namespace Antares::ModelerStudy;

static const auto libraryYaml = R"(
library:
  id: my_lib
  description: test model library

  port-types:
    - id:  power_flow
      description: power flow
      fields:
        - id: flow

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
    )"s;

BOOST_AUTO_TEST_CASE(two_components_connected_by_ports_of_same_type_but_different)
{
    YmlModel::Parser parserModel;
    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml)));

    YmlSystem::Parser parserSystem;
    YmlSystem::System system = parserSystem.parse(systemYaml);
    auto systemModel = SystemConverter::convert(system, libraries);

    const auto component_N = systemModel.Components().at("N");

    const std::string port_id = "injection_port";
    auto connections_to_N = component_N.connexionsViaPort(port_id);

    BOOST_CHECK_EQUAL(connections_to_N.size(), 1);
    BOOST_CHECK_EQUAL(connections_to_N[0].port()->Id(), "some_other_port");
    BOOST_CHECK_EQUAL(connections_to_N[0].component()->Id(), "NG");

    // Symmetrically, check connexions of NG
    const auto component_NG = systemModel.Components().at("NG");
    auto connections_to_NG = component_NG.connexionsViaPort("some_other_port");

    BOOST_CHECK_EQUAL(connections_to_NG.size(), 1);
    BOOST_CHECK_EQUAL(connections_to_NG[0].port()->Id(), "injection_port");
    BOOST_CHECK_EQUAL(connections_to_NG[0].component()->Id(), "N");
}
