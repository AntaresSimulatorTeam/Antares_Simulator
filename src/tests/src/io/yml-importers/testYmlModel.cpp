// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE modelParser

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>
#include <yaml-cpp/exceptions.h>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/yml-model/decoders.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/io/inputs/yml-utils/checkFields.h"
#include "antares/io/inputs/yml-utils/path_utils.h"

#include "enum_operators.h"
#include "yml-test-helpers.h"

using namespace std::string_literals;
using namespace Antares::IO::Inputs;
using Antares::UnitTests::CaptureAntaresLogs;
using YmlTest::dedent;
using YmlTest::library;
using YmlTest::libraryWithModel;
using YmlTest::model;

namespace
{
YmlModel::Library parse(const std::string& yaml)
{
    return YmlModel::Parser{}.parse(yaml);
}

struct ParseErrorCase
{
    std::string name;
    std::string yaml;
    std::string expectedMessage;
};

std::ostream& operator<<(std::ostream& os, const ParseErrorCase& c)
{
    return os << c.name;
}

const std::vector<ParseErrorCase> parseErrorCases = {
  {"library_without_id",
   dedent(R"(
       library:
           port-types: []
           models: []
   )"),
   "Missing field: id"},
  {"library_with_null_id",
   dedent(R"(
       library:
           id: null
           port-types: []
           models: []
   )"),
   "Invalid null value for required field: id"},
  {"model_without_id", library({.models = "- description: missing id"}), "Missing field: id"},
  {"parameter_without_id",
   libraryWithModel({.parameters = "- time-dependent: false"}),
   "Missing field: id"},
  {"variable_without_id",
   libraryWithModel({.variables = R"(
       - lower-bound: 0
         upper-bound: 1)"}),
   "Missing field: id"},
  {"port_without_id",
   library({.portTypes = R"(
                - id: port_type
                  fields: [])",
            .models = model({.ports = "- type: port_type"})}),
   "Missing field: id"},
  {"constraint_with_null_id",
   libraryWithModel({.constraints = R"(
       - id: null
         expression: expression)"}),
   "Invalid null value for required field: id"},
  {"misspelled_model_field",
   library({.models = R"(
       - id: model_id
         extra-output:
           - id: profit
             expression: p)"}),
   "Unrecognised field: extra-output"},
  {"misspelled_variable_field",
   libraryWithModel({.variables = R"(
       - id: p
         lower_bound: 0)"}),
   "Unrecognised field: lower_bound"},
  {"area_connection_unknown_field",
   library({.portTypes = R"(
       - id: capacity_port
         fields:
           - id: capacity
         area-connection:
           injection-to-balance: injection
           spillage-bound: spillage
           unsupplied-energy-bound: unsupplied
           not-a-field: value)"}),
   "Unrecognised field: not-a-field"},
  {"area_connection_is_empty",
   library({.portTypes = R"(
       - id: capacity_port
         fields:
           - id: capacity
         area-connection:)"}),
   "expected a YAML mapping in 'library/port-types/capacity_port/area-connection'"},
};
} // namespace

BOOST_DATA_TEST_CASE(invalid_libraries_are_rejected,
                     boost::unit_test::data::make(parseErrorCases),
                     c)
{
    BOOST_CHECK_EXCEPTION(parse(c.yaml), InputError, containsMessage(c.expectedMessage));
}

BOOST_AUTO_TEST_CASE(models_must_be_a_sequence)
{
    BOOST_CHECK_THROW(parse(library({.models = "\"not a map\""})), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(thermal_capacity_connection_should_have_exactly_one_field)
{
    const auto yaml = library({.portTypes = R"(
        - id: capacity_port
          fields:
            - id: capacity
          thermal-capacity-connection:)"});
    BOOST_REQUIRE_THROW(parse(yaml), InputError);
}

BOOST_AUTO_TEST_CASE(checkFields_reports_unexpected_missing_keys_and_tree)
{
    const auto node = YAML::Load("a: 1\nc: 3\n");
    CaptureAntaresLogs logs;
    BOOST_CHECK_EXCEPTION(
      (YmlUtils::checkFields(node, std::vector<YmlUtils::Field>{{"a", true}, {"b", true}})),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("Missing field: b") != std::string::npos);
          BOOST_CHECK(message.find("Unrecognised field: c") != std::string::npos);
          BOOST_CHECK(message.find("|__X c at line 2") != std::string::npos);
          return true;
      });
    BOOST_CHECK(logs.getWarnings().empty());
}

BOOST_AUTO_TEST_CASE(checkFields_accepts_absent_optional_fields_without_warning)
{
    const auto node = YAML::Load("a: 1\n");
    CaptureAntaresLogs logs;
    BOOST_CHECK_NO_THROW(
      (YmlUtils::checkFields(node, std::vector<YmlUtils::Field>{{"a", true}, {"b"}})));
    BOOST_CHECK(logs.getWarnings().empty());
}

BOOST_AUTO_TEST_CASE(checkFields_rejects_duplicated_keys)
{
    const auto node = YAML::Load("id: supply\nid: demand\n");
    BOOST_CHECK_EXCEPTION((YmlUtils::checkFields(node, std::vector<YmlUtils::Field>{{"id", true}})),
                          InputError,
                          containsMessage("Duplicated field: id"));
}

BOOST_AUTO_TEST_CASE(printPathTree_formats_nested_paths)
{
    BOOST_CHECK_EQUAL(YmlUtils::printPathTree(std::filesystem::path("lib/model/port")),
                      std::string("lib\n|__ model\n    |__ port\n"));
}

BOOST_AUTO_TEST_CASE(library_id_and_description_parsed_properly)
{
    const auto lib = parse(library({.id = "test_id", .description = "test_description"}));
    BOOST_CHECK_EQUAL(lib.id, "test_id");
    BOOST_CHECK_EQUAL(lib.description, "test_description");
    BOOST_CHECK(lib.port_types.empty());
    BOOST_CHECK(lib.models.empty());
}

BOOST_AUTO_TEST_CASE(library_with_empty_id_and_description_is_valid)
{
    const auto lib = parse(library({.id = "\"\"", .description = "\"\""}));
    BOOST_CHECK(lib.id.empty());
    BOOST_CHECK(lib.description.empty());
}

BOOST_AUTO_TEST_CASE(library_can_contain_multiple_port_types)
{
    const auto lib = parse(library({.portTypes = R"(
        - id: porttype_id1
          description: porttype_description1
          fields:
            - id: port_name1
        - id: porttype_id2
          description: porttype_description2
          fields:
            - id: port_name2)"}));
    BOOST_REQUIRE_EQUAL(lib.port_types.size(), 2);
    BOOST_CHECK_EQUAL(lib.port_types[0].id, "porttype_id1");
    BOOST_CHECK_EQUAL(lib.port_types[0].description, "porttype_description1");
    BOOST_REQUIRE_EQUAL(lib.port_types[0].fields.size(), 1);
    BOOST_CHECK_EQUAL(lib.port_types[0].fields[0], "port_name1");
    BOOST_CHECK_EQUAL(lib.port_types[1].id, "porttype_id2");
    BOOST_CHECK_EQUAL(lib.port_types[1].description, "porttype_description2");
    BOOST_REQUIRE_EQUAL(lib.port_types[1].fields.size(), 1);
    BOOST_CHECK_EQUAL(lib.port_types[1].fields[0], "port_name2");
}

namespace
{
YmlModel::PortType parseSinglePortType(const std::string& portTypeYaml)
{
    const auto lib = parse(library({.portTypes = portTypeYaml}));
    BOOST_REQUIRE_EQUAL(lib.port_types.size(), 1);
    return lib.port_types[0];
}
} // namespace

BOOST_AUTO_TEST_CASE(port_types_with_thermal_capacity_connection_properly_parsed)
{
    const auto portType = parseSinglePortType(R"(
        - id: capacity_port
          fields:
            - id: capacity
          thermal-capacity-connection:
            capacity-field: capacity)");
    BOOST_CHECK_EQUAL(portType.thermal_capacity_connection_field, "capacity");
}

BOOST_AUTO_TEST_CASE(port_types_with_empty_thermal_capacity_connection_field)
{
    const auto portType = parseSinglePortType(R"(
        - id: capacity_port
          fields:
            - id: capacity
          thermal-capacity-connection:
            capacity-field:)");
    BOOST_CHECK_EQUAL(portType.thermal_capacity_connection_field, "");
}

namespace
{
struct AreaConnectionCase
{
    std::string name;
    std::string yamlFields; // content of the area-connection mapping
    std::string injection;
    std::string spillage;
    std::string unsupplied;
    std::string price;
};

std::ostream& operator<<(std::ostream& os, const AreaConnectionCase& c)
{
    return os << c.name;
}
} // namespace

BOOST_DATA_TEST_CASE(
  area_connection_fields_are_parsed,
  boost::unit_test::data::make<AreaConnectionCase>({
    {"all_empty",
     "injection-to-balance:\nspillage-bound:\nunsupplied-energy-bound:\n",
     "",
     "",
     "",
     ""},
    // 'price' is optional: blocks written before its introduction must keep parsing
    {"only_injection",
     "injection-to-balance: capa\nspillage-bound:\nunsupplied-energy-bound:\n",
     "capa",
     "",
     "",
     ""},
    {"only_spillage_bound",
     "injection-to-balance:\nspillage-bound: f1\nunsupplied-energy-bound:\n",
     "",
     "f1",
     "",
     ""},
    {"only_unsupplied_energy_bound",
     "injection-to-balance:\nspillage-bound:\nunsupplied-energy-bound: f2\n",
     "",
     "",
     "f2",
     ""},
    {"with_price",
     "injection-to-balance: capa\nspillage-bound:\nunsupplied-energy-bound:\nprice: price\n",
     "capa",
     "",
     "",
     "price"},
  }),
  c)
{
    const auto portType = parseSinglePortType("- id: capacity_port\n  fields:\n    - id: capacity\n"
                                              "    - id: price\n  area-connection:\n"
                                              + YmlTest::indent(c.yamlFields, 4));
    BOOST_CHECK_EQUAL(portType.area_connection.inject_to_balance, c.injection);
    BOOST_CHECK_EQUAL(portType.area_connection.spillage_bound, c.spillage);
    BOOST_CHECK_EQUAL(portType.area_connection.unsupplied_energy_bound, c.unsupplied);
    BOOST_CHECK_EQUAL(portType.area_connection.price, c.price);
}

BOOST_AUTO_TEST_CASE(library_can_contain_multiple_models)
{
    const auto lib = parse(library(
      {.models = model({.id = "model_id1", .description = "model_description1", .objectives = R"(
                                                         - id: objective1
                                                           expression: objective1
                                                         - id: objective1b
                                                           expression: objective1b)"})
                 + model({.id = "model_id2", .description = "model_description2", .objectives = R"(
                                                         - id: objective2
                                                           expression: objective2)"})}));
    BOOST_REQUIRE_EQUAL(lib.models.size(), 2);
    BOOST_CHECK_EQUAL(lib.models[0].id, "model_id1");
    BOOST_CHECK_EQUAL(lib.models[0].description, "model_description1");
    BOOST_REQUIRE_EQUAL(lib.models[0].objectives.size(), 2);
    BOOST_CHECK_EQUAL(lib.models[0].objectives[0].id, "objective1");
    BOOST_CHECK_EQUAL(lib.models[0].objectives[0].expression.input_expr, "objective1");
    BOOST_CHECK_EQUAL(lib.models[0].objectives[1].id, "objective1b");
    BOOST_CHECK_EQUAL(lib.models[1].id, "model_id2");
    BOOST_CHECK_EQUAL(lib.models[1].description, "model_description2");
    BOOST_REQUIRE_EQUAL(lib.models[1].objectives.size(), 1);
    BOOST_CHECK_EQUAL(lib.models[1].objectives[0].id, "objective2");
    BOOST_CHECK_EQUAL(lib.models[1].objectives[0].expression.input_expr, "objective2");
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_parameters)
{
    const auto lib = parse(libraryWithModel({.parameters = R"(
        - id: param_name1
          time-dependent: FALSE
          scenario-dependent: false
        - id: param_name2
          time-dependent: true
          scenario-dependent: true)"}));
    const auto& parameters = lib.models.at(0).parameters;
    BOOST_REQUIRE_EQUAL(parameters.size(), 2);
    BOOST_CHECK_EQUAL(parameters[0].id, "param_name1");
    BOOST_CHECK_EQUAL(parameters[0].time_dependent, false);
    BOOST_CHECK_EQUAL(parameters[0].scenario_dependent, false);
    BOOST_CHECK_EQUAL(parameters[1].id, "param_name2");
    BOOST_CHECK_EQUAL(parameters[1].time_dependent, true);
    BOOST_CHECK_EQUAL(parameters[1].scenario_dependent, true);
}

// Time dependent and scenario dependant default value are true
BOOST_AUTO_TEST_CASE(test_library_model_parameters_default_values)
{
    const auto lib = parse(libraryWithModel({.parameters = "- id: param_name"}));
    const auto& parameters = lib.models.at(0).parameters;
    BOOST_REQUIRE_EQUAL(parameters.size(), 1);
    BOOST_CHECK_EQUAL(parameters[0].id, "param_name");
    BOOST_CHECK(parameters[0].time_dependent);
    BOOST_CHECK(parameters[0].scenario_dependent);
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_variables)
{
    const auto lib = parse(libraryWithModel({.variables = R"(
        - id: var_name1
          lower-bound: 0
          upper-bound: 1
        - id: var_name2
          lower-bound: -1
          upper-bound: 2)"}));
    const auto& variables = lib.models.at(0).variables;
    BOOST_REQUIRE_EQUAL(variables.size(), 2);
    BOOST_CHECK_EQUAL(variables[0].id, "var_name1");
    BOOST_CHECK_EQUAL(variables[0].lower_bound.input_expr, "0");
    BOOST_CHECK_EQUAL(variables[0].upper_bound.input_expr, "1");
    BOOST_CHECK_EQUAL(variables[1].id, "var_name2");
    BOOST_CHECK_EQUAL(variables[1].lower_bound.input_expr, "-1");
    BOOST_CHECK_EQUAL(variables[1].upper_bound.input_expr, "2");
}

BOOST_AUTO_TEST_CASE(variables_bounds_are_literals)
{
    const auto lib = parse(libraryWithModel({.variables = R"(
        - id: var_name
          lower-bound: "near-zero"
          upper-bound: "pmax")"}));
    const auto& variable = lib.models.at(0).variables.at(0);
    BOOST_CHECK_EQUAL(variable.lower_bound.input_expr, "near-zero");
    BOOST_CHECK_EQUAL(variable.upper_bound.input_expr, "pmax");
}

BOOST_AUTO_TEST_CASE(variable_types_can_be_integer_bool_float_default_to_float)
{
    const auto lib = parse(libraryWithModel({.variables = R"(
        - id: var1
          lower-bound: 0
          upper-bound: 1
          variable-type: "boolean"
        - id: var2
          lower-bound: 0
          upper-bound: 1
          variable-type: "integer"
        - id: var3
          lower-bound: 0
          upper-bound: 1
          variable-type: "continuous"
        - id: var4
          lower-bound: 0
          upper-bound: 1)"}));
    const auto& variables = lib.models.at(0).variables;
    BOOST_REQUIRE_EQUAL(variables.size(), 4);
    BOOST_CHECK_EQUAL(variables[0].variable_type, YmlModel::ValueType::BOOL);
    BOOST_CHECK_EQUAL(variables[1].variable_type, YmlModel::ValueType::INTEGER);
    BOOST_CHECK_EQUAL(variables[2].variable_type, YmlModel::ValueType::CONTINUOUS);
    BOOST_CHECK_EQUAL(variables[3].variable_type, YmlModel::ValueType::CONTINUOUS);
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_ports)
{
    const auto lib = parse(libraryWithModel({.ports = R"(
        - id: port_name1
          type: port_type1
        - id: port_name2
          type: port_type2)"}));
    const auto& ports = lib.models.at(0).ports;
    BOOST_REQUIRE_EQUAL(ports.size(), 2);
    BOOST_CHECK_EQUAL(ports[0].id, "port_name1");
    BOOST_CHECK_EQUAL(ports[0].type, "port_type1");
    BOOST_CHECK_EQUAL(ports[1].id, "port_name2");
    BOOST_CHECK_EQUAL(ports[1].type, "port_type2");
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_portfields)
{
    const auto lib = parse(libraryWithModel({.portFieldDefinitions = R"(
        - port: port_name1
          field: field_name1
          definition: definition1
        - port: port_name2
          field: field_name2
          definition: definition2)"}));
    const auto& definitions = lib.models.at(0).port_field_definitions;
    BOOST_REQUIRE_EQUAL(definitions.size(), 2);
    BOOST_CHECK_EQUAL(definitions[0].port, "port_name1");
    BOOST_CHECK_EQUAL(definitions[0].field, "field_name1");
    BOOST_CHECK_EQUAL(definitions[0].definition.input_expr, "definition1");
    BOOST_CHECK_EQUAL(definitions[1].port, "port_name2");
    BOOST_CHECK_EQUAL(definitions[1].field, "field_name2");
    BOOST_CHECK_EQUAL(definitions[1].definition.input_expr, "definition2");
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_constraints)
{
    const auto lib = parse(libraryWithModel({.constraints = R"(
        - id: constraint_name1
          expression: expression1
        - id: constraint_name2
          expression: expression2)"}));
    const auto& constraints = lib.models.at(0).constraints;
    BOOST_REQUIRE_EQUAL(constraints.size(), 2);
    BOOST_CHECK_EQUAL(constraints[0].id, "constraint_name1");
    BOOST_CHECK_EQUAL(constraints[0].expression.input_expr, "expression1");
    BOOST_CHECK_EQUAL(constraints[1].id, "constraint_name2");
    BOOST_CHECK_EQUAL(constraints[1].expression.input_expr, "expression2");
}

BOOST_AUTO_TEST_CASE(model_can_contain_multiple_extra_outputs)
{
    const auto lib = parse(libraryWithModel({.extraOutputs = R"(
        - id: output_name1
          expression: expression1
        - id: output_name2
          expression: expression2)"}));
    const auto& outputs = lib.models.at(0).extra_outputs;
    BOOST_REQUIRE_EQUAL(outputs.size(), 2);
    BOOST_CHECK_EQUAL(outputs[0].id, "output_name1");
    BOOST_CHECK_EQUAL(outputs[0].expression.input_expr, "expression1");
    BOOST_CHECK_EQUAL(outputs[1].id, "output_name2");
    BOOST_CHECK_EQUAL(outputs[1].expression.input_expr, "expression2");
}

BOOST_AUTO_TEST_CASE(model_attributes_can_be_ommited)
{
    const auto lib = parse(library({.models = "- id: model_id"}));
    BOOST_REQUIRE_EQUAL(lib.models.size(), 1);
    const auto& model = lib.models[0];
    BOOST_CHECK_EQUAL(model.id, "model_id");
    BOOST_CHECK_EQUAL(model.description, "");
    BOOST_CHECK(model.parameters.empty());
    BOOST_CHECK(model.variables.empty());
    BOOST_CHECK(model.ports.empty());
    BOOST_CHECK(model.port_field_definitions.empty());
    BOOST_CHECK(model.constraints.empty());
    BOOST_CHECK(model.objectives.empty());
}

BOOST_AUTO_TEST_CASE(test_variable_to_string)
{
    namespace YmlMod = YmlModel;

    BOOST_CHECK_EQUAL(YmlMod::toString(YmlMod::ValueType::CONTINUOUS), "CONTINUOUS");
    BOOST_CHECK_EQUAL(YmlMod::toString(YmlMod::ValueType::INTEGER), "INTEGER");
    BOOST_CHECK_EQUAL(YmlMod::toString(YmlMod::ValueType::BOOL), "BOOL");
    BOOST_CHECK_EQUAL(YmlMod::toString(static_cast<YmlMod::ValueType>(5)), "UNKNOWN");
}
