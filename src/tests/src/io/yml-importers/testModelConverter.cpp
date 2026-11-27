// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <unit_test_utils.h>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/exception/RuntimeError.hpp"
#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/library.h"

#include "enum_operators.h"

using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::YmlModel;
using namespace Antares::ModelerStudy;
using namespace Antares::Solver::Config;

namespace
{
YmlModel::Model makeModel(std::string id, std::string description = "description")
{
    YmlModel::Model model;
    model.id = std::move(id);
    model.description = std::move(description);
    return model;
}
} // namespace

struct Fixture
{
    YmlModel::Library library;
};

// Test empty library
BOOST_FIXTURE_TEST_CASE(Empty_library_is_valid, Fixture)
{
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK(lib.Id().empty());
    BOOST_CHECK(lib.Description().empty());
    BOOST_CHECK(lib.PortTypes().empty());
    BOOST_CHECK(lib.Models().empty());
}

// Test library with id and description
BOOST_FIXTURE_TEST_CASE(library_id_description_properly_translated, Fixture)

{
    library.id = "test_id";
    library.description = "test_description";
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK_EQUAL(lib.Id(), "test_id");
    BOOST_CHECK_EQUAL(lib.Description(), "test_description");
}

// Test library with port types and fields
BOOST_FIXTURE_TEST_CASE(portType_with_fields_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"port1", "flow port", {"field1", "field2"}, "", {}};
    YmlModel::PortType portType2{"port2", "impedance port", {"field3", "field4"}, "", {}};
    library.port_types = {portType1, portType2};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port1").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[0].Id(), "field1");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[1].Id(), "field2");
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port2").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[0].Id(), "field3");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[1].Id(), "field4");
}

bool emptyFields(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "This port type doesn't contains fields: port1");
    return true;
}

bool portTypeAlreadyExists(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Port type with this id already exists: port2");
    return true;
}

// Test port types for exceptions
BOOST_FIXTURE_TEST_CASE(port_type_error_cases, Fixture)
{
    YmlModel::PortType portType1{"port1", "empty port", {}, "", {}};
    library.port_types = {portType1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, emptyFields);

    // same name
    YmlModel::PortType portType2{"port2", "flow port", {"field1", "field2"}, "", {}};
    YmlModel::PortType portType3{"port2", "impedance port", {"field3", "field4"}, "", {}};
    library.port_types = {portType2, portType3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, portTypeAlreadyExists);
}

// Test library with models
BOOST_FIXTURE_TEST_CASE(model_with_one_objective_properly_translated, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}};
    model1.objectives = {{"objective-id", ExpressionLineNumber{"param1", 0}, "subproblems"}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.Models().size(), 1);
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Id(), "model1");
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Objectives()[0].expression().Value(), "param1");
}

// Test library with models and parameters
BOOST_FIXTURE_TEST_CASE(model_parameters_properly_translated, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}, {"param2", false, false}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Parameters().size(), 2);
    auto& parameter1 = model.Parameters().at("param1");
    auto& parameter2 = model.Parameters().at("param2");
    BOOST_CHECK_EQUAL(parameter1.Id(), "param1");
    BOOST_CHECK(parameter1.isTimeDependent());
    BOOST_CHECK(!parameter1.isScenarioDependent());
    BOOST_CHECK_EQUAL(parameter2.Id(), "param2");
    BOOST_CHECK(!parameter2.isTimeDependent());
    BOOST_CHECK(!parameter2.isScenarioDependent());
}

// Test library with models and variables
BOOST_FIXTURE_TEST_CASE(model_variables_properly_translated, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"pmax", true, false}};
    model1.variables = {{"var1",
                         ExpressionLineNumber{"7", 0},
                         ExpressionLineNumber{"pmax", 0},
                         ValueType::BOOL,
                         true,
                         true,
                         "master"},
                        {"var2",
                         ExpressionLineNumber{"999.99", 0},
                         ExpressionLineNumber{"var1", 0},
                         ValueType::INTEGER,
                         true,
                         true,
                         "subproblems"}};
    model1.objectives = {{"objective-id", ExpressionLineNumber{"var1", 0}, "subproblems"}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    const auto& vars = model.Variables();
    BOOST_REQUIRE_EQUAL(vars.size(), 2);
    const auto var_1 = std::ranges::find_if(vars, [](const auto& v) { return v.Id() == "var1"; });
    const auto var_2 = std::ranges::find_if(vars, [](const auto& v) { return v.Id() == "var2"; });
    BOOST_CHECK(var_1 != vars.cend());
    BOOST_CHECK_EQUAL(var_1->LowerBound().Value(), "7");
    BOOST_CHECK_EQUAL(var_1->UpperBound().Value(), "pmax");
    BOOST_CHECK_EQUAL(var_1->Type(), SystemModel::ValueType::BOOL);
    BOOST_CHECK(var_1->location() == Location::MASTER);
    BOOST_CHECK(var_2 != vars.cend());
    BOOST_CHECK_EQUAL(var_2->LowerBound().Value(), "999.99");
    BOOST_CHECK_EQUAL(var_2->UpperBound().Value(), "var1");
    BOOST_CHECK_EQUAL(var_2->Type(), SystemModel::ValueType::INTEGER);
    BOOST_CHECK(var_2->location() == Location::SUBPROBLEMS);
}

// wrong variable ValueType
BOOST_FIXTURE_TEST_CASE(wrong_value_type, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}, {"param2", false, false}};
    model1.variables = {{"varP",
                         ExpressionLineNumber{"7", 0},
                         ExpressionLineNumber{"param2", 0},
                         static_cast<ValueType>(5),
                         true,
                         true,
                         "master"}};
    library.models = {model1};
    BOOST_CHECK_THROW(ModelConverter::convert(library), InputError);
}

// max/min operators with only parameters (and literals) as arguments are allowed in variable
// bounds : these expressions are evaluated before the linear optimization, since parameters are
// known at that point.
BOOST_FIXTURE_TEST_CASE(variable_bound_with_max_min_on_parameters_is_accepted, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"pmin", true, false}, {"pmax", true, false}};
    model1.variables = {{"var1",
                         ExpressionLineNumber{"max(pmin, 10)", 0},
                         ExpressionLineNumber{"min(pmax, 50)", 0},
                         ValueType::CONTINUOUS,
                         true,
                         true,
                         "subproblems"}};
    library.models = {model1};

    auto lib = ModelConverter::convert(library);
    auto& m = lib.Models().at("model1");
    const auto& var = m.Variables().front();
    BOOST_CHECK_EQUAL(var.LowerBound().Value(), "max(pmin, 10)");
    BOOST_CHECK_EQUAL(var.UpperBound().Value(), "min(pmax, 50)");
}

// Variables must still be rejected in function nodes within variable bounds, to keep bounds
// independent of the solution of the linear problem.
BOOST_FIXTURE_TEST_CASE(variable_bound_with_max_min_on_a_variable_is_rejected, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"pmin", true, false}};
    model1.variables = {{"var1",
                         ExpressionLineNumber{"0", 0},
                         ExpressionLineNumber{"max(var1, pmin)", 0},
                         ValueType::CONTINUOUS,
                         true,
                         true,
                         "subproblems"}};
    library.models = {model1};

    std::string err_msg = "'FunctionNode::max' is not allowed to contain 'VariableNode' in "
                          "expression 'max(var1, pmin)'";
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, containsMessage(err_msg));
}

// Test library with models and ports
BOOST_FIXTURE_TEST_CASE(model_ports_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"abc"}, "", {}};
    library.port_types = {portType1};

    auto model1 = makeModel("model1");
    model1.ports = {{"port1", "flow"}, {"port2", "flow"}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    [[maybe_unused]] auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Ports().size(), 2);
    auto& port1 = model.Ports().at("port1");
    auto& port2 = model.Ports().at("port2");
    BOOST_CHECK_EQUAL(port1.Id(), "port1");
    BOOST_CHECK_EQUAL(port1.Type().Id(), lib.PortTypes().at(port1.Type().Id()).Id());
    BOOST_CHECK_EQUAL(port2.Id(), "port2");
}

bool portAlreadyExists(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Port with this id already exists: port1");
    return true;
}

bool typeNotFound(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "For the port: port2 , port type not found: not flow");
    return true;
}

// Test ports errors
BOOST_FIXTURE_TEST_CASE(ports_errors_cases, Fixture)
{
    // test port already exists
    YmlModel::PortType portType1{"flow", "description", {"abc"}, "", {}};
    library.port_types = {portType1};

    // test port type not found
    auto model = makeModel("model2");
    model.ports = {{"port2", "not flow"}};
    library.models = {model};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, typeNotFound);
}

// Test library with models and constraints
BOOST_FIXTURE_TEST_CASE(model_constraints_properly_translated, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"expression1", true, false},
                         {"expression2", true, false},
                         {"expression3", true, false}};
    model1.constraints = {
      {"constraint1", ExpressionLineNumber{"expression1", 0}, "subproblems", ""},
      {"constraint2", ExpressionLineNumber{"expression2", 0}, "master-and-subproblems", ""}};
    model1.binding_constraints = {
      {"constraint3", ExpressionLineNumber{"expression3", 0}, "master", ""}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Constraints().size(), 3);
    auto& constraint1 = model.Constraints().at(0);
    auto& constraint2 = model.Constraints().at(1);
    auto& constraint3 = model.Constraints().at(2);
    BOOST_CHECK_EQUAL(constraint1.Id(), "constraint1");
    BOOST_CHECK_EQUAL(constraint1.expression().Value(), "expression1");
    BOOST_CHECK_EQUAL(constraint2.Id(), "constraint2");
    BOOST_CHECK_EQUAL(constraint2.expression().Value(), "expression2");
    BOOST_CHECK_EQUAL(constraint3.Id(), "constraint3");
    BOOST_CHECK_EQUAL(constraint3.expression().Value(), "expression3");
    BOOST_CHECK(constraint1.location() == Location::SUBPROBLEMS);
    BOOST_CHECK(constraint2.location() == Location::MASTER_AND_SUBPROBLEMS);
    BOOST_CHECK(constraint3.location() == Location::MASTER);
}

bool constraintAlreadyExists(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Constraint with this id already exists: constraint1");
    return true;
}

// Test with 2 models
BOOST_FIXTURE_TEST_CASE(multiple_models_properly_translated, Fixture)
{
    auto ymlModel_1 = makeModel("model1");
    ymlModel_1.parameters = {{"param1", true, false}, {"param2", false, false}};
    ymlModel_1.variables = {{"varP",
                             ExpressionLineNumber{"7", 0},
                             ExpressionLineNumber{"param2", 0},
                             ValueType::CONTINUOUS,
                             true,
                             true,
                             "master-and-subproblems"}};

    auto ymlModel_2 = makeModel("model2");
    ymlModel_2.variables = {{"var1",
                             ExpressionLineNumber{"7", 0},
                             ExpressionLineNumber{"8", 0},
                             ValueType::BOOL,
                             true,
                             true,
                             "master"},
                            {"var2",
                             ExpressionLineNumber{"9999.9", 0},
                             ExpressionLineNumber{"var1", 0},
                             ValueType::INTEGER,
                             true,
                             true,
                             "subproblems"}};

    library.models = {ymlModel_1, ymlModel_2};

    SystemModel::Library lib = ModelConverter::convert(library);

    BOOST_REQUIRE_EQUAL(lib.Models().size(), 2);

    auto& model1 = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model1.Parameters().size(), 2);
    BOOST_REQUIRE_EQUAL(model1.Variables().size(), 1);

    auto& model2 = lib.Models().at("model2");
    BOOST_REQUIRE_EQUAL(model2.Parameters().size(), 0);
    BOOST_REQUIRE_EQUAL(model2.Variables().size(), 2);

    auto& var11 = model1.Variables()[0];
    auto& var21 = model2.Variables()[0];
    auto& var22 = model2.Variables()[1];
    BOOST_CHECK(var11.location() == Location::MASTER_AND_SUBPROBLEMS);
    BOOST_CHECK(var21.location() == Location::MASTER);
    BOOST_CHECK(var22.location() == Location::SUBPROBLEMS);
}

// Test library with ports field definitions
BOOST_FIXTURE_TEST_CASE(model_port_field_definitions_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"field1"}, "", {}};
    library.port_types = {portType1};

    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}};
    model1.ports = {{"port1", "flow"}};
    model1.port_field_definitions = {{"port1", "field1", ExpressionLineNumber{"param1", 0}}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    auto& pfd1 = model.PortFieldDefinitions().at(
      SystemModel::PortFieldKey{.portId = "port1", .fieldId = "field1"});
    BOOST_CHECK_EQUAL(pfd1.port().Id(), "port1");
    BOOST_CHECK_EQUAL(pfd1.field().Id(), "field1");
    BOOST_CHECK_EQUAL(pfd1.Definition().Value(), "param1");
}

BOOST_FIXTURE_TEST_CASE(port_fields_definitions_forbid_usage_of_sum_connections, Fixture)
{
    YmlModel::PortType portType{"my-port-type", "description", {"field"}, "", {}};
    library.port_types = {portType};

    auto model = makeModel("my-model");
    model.ports = {{"port", "my-port-type"}};
    model.port_field_definitions = {
      {"port", "field", ExpressionLineNumber{"sum_connections(port.field)", 0}}};
    library.models = {model};

    std::string err_msg = "'PortFieldSumNode' is not allowed in expression "
                          "'sum_connections(port.field)'";
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, containsMessage(err_msg));
}

namespace
{
struct SumConnectionsCase
{
    std::string name;
    std::vector<std::string> ports; // all of type my-port-type
    std::string definedPort;        // port-field-definition: <definedPort>.field1
    std::string constraint;         // binding constraint expression
    std::string conflictingPort;    // empty: no conflict expected
    std::string conflictingField;
};

std::ostream& operator<<(std::ostream& os, const SumConnectionsCase& c)
{
    return os << c.name;
}
} // namespace

BOOST_DATA_TEST_CASE_F(
  Fixture,
  port_field_definition_vs_sum_connections_conflicts,
  boost::unit_test::data::make<SumConnectionsCase>({
    {"same_port", {"port"}, "port", "sum_connections(port.field1) = 0", "port", "field1"},
    {"other_port_names",
     {"port1", "port2"},
     "port1",
     "sum_connections(port1.field1) = 0",
     "port1",
     "field1"},
    {"different_field", {"port"}, "port", "sum_connections(port.field2) = 0", "", ""},
    {"different_port", {"port1", "port2"}, "port1", "sum_connections(port2.field1) = 0", "", ""},
  }),
  c)
{
    library.port_types = {
      YmlModel::PortType{"my-port-type", "description", {"field1", "field2"}, "", {}}};

    auto model = makeModel("my-model");
    for (const auto& port: c.ports)
    {
        model.ports.push_back({port, "my-port-type"});
    }
    model.port_field_definitions = {{c.definedPort, "field1", ExpressionLineNumber{"0", 0}}};
    model.binding_constraints = {
      {"constraint1", ExpressionLineNumber{c.constraint, 0}, "subproblems", ""}};
    library.models = {model};

    if (c.conflictingPort.empty())
    {
        BOOST_CHECK_NO_THROW(ModelConverter::convert(library));
        return;
    }
    const std::string err_msg = "In model 'my-model', field '" + c.conflictingField + "' of port '"
                                + c.conflictingPort
                                + "' is defined in a port-field-definition and also used in a "
                                  "sum_connections in binding constraint 'constraint1'. "
                                  "A field cannot be both a sender and a receiver in the "
                                  "same model.";
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, containsMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(
  area_connection_price_field_forbidden_in_sum_connections_in_binding_constraint,
  Fixture)
{
    YmlModel::PortType portType{"my-port-type",
                                "description",
                                {"flow", "price"},
                                "",
                                YmlModel::AreaConnection{.inject_to_balance = "flow",
                                                         .spillage_bound = "",
                                                         .unsupplied_energy_bound = "",
                                                         .price = "price"}};
    library.port_types = {portType};

    auto model = makeModel("my-model");
    model.ports = {{"port", "my-port-type"}};
    model.binding_constraints = {{"constraint1",
                                  ExpressionLineNumber{"sum_connections(port.price) = 0", 0},
                                  "subproblems",
                                  ""}};
    library.models = {model};

    std::string err_msg = "In model 'my-model', field 'price' of port 'port' holds the dual "
                          "value of a legacy area's balance equation and can only be used in "
                          "extra-outputs, not in 'constraint1'.";
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, containsMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(area_connection_price_field_allowed_in_extra_output, Fixture)
{
    YmlModel::PortType portType{"my-port-type",
                                "description",
                                {"flow", "price"},
                                "",
                                YmlModel::AreaConnection{.inject_to_balance = "flow",
                                                         .spillage_bound = "",
                                                         .unsupplied_energy_bound = "",
                                                         .price = "price"}};
    library.port_types = {portType};

    auto model = makeModel("my-model");
    model.ports = {{"port", "my-port-type"}};
    model.extra_outputs = {{"profit", ExpressionLineNumber{"sum_connections(port.price)", 0}}};
    library.models = {model};

    BOOST_CHECK_NO_THROW(ModelConverter::convert(library));
}

BOOST_FIXTURE_TEST_CASE(in_port_fields_definitions__min_operator_accepts_a_variable, Fixture)
{
    YmlModel::PortType portType{"my-port-type", "description", {"field"}, "", {}};
    library.port_types = {portType};

    std::string location = "master-and-subproblems";

    Variable spillage = {"spillage",
                         ExpressionLineNumber{"0", 0},
                         ExpressionLineNumber{"100", 0},
                         ValueType::CONTINUOUS,
                         true,
                         true,
                         location};
    Variable ens = {"ens",
                    ExpressionLineNumber{"0", 0},
                    ExpressionLineNumber{"100", 0},
                    ValueType::CONTINUOUS,
                    true,
                    true,
                    location};

    auto model = makeModel("my-model");
    model.variables = {spillage, ens};
    model.ports = {{"port", "my-port-type"}};
    model.port_field_definitions = {
      {"port", "field", ExpressionLineNumber{"min(spillage, ens)", 0}}};
    library.models = {model};

    BOOST_CHECK_NO_THROW(ModelConverter::convert(library));
}

bool portNotFoundForDef(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "In port-field-definitions, port not found: port2");
    return true;
}

bool fieldNotFoundForDef(const InputError& ex)
{
    BOOST_CHECK_EQUAL(ex.what(),
                      "In port-field-definitions, for port: port2 , field not found: field2");
    return true;
}

bool portInDef(const InputError& ex)
{
    BOOST_CHECK_EQUAL(
      ex.what(),
      "In port-field-definitions, for port: port4 , found another port in the definition: port3");
    return true;
}

// Test library ports field definitions errors
BOOST_FIXTURE_TEST_CASE(port_field_definition_error_cases, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"field1"}, "", {}};
    library.port_types = {portType1};

    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}};
    model1.ports = {{"port1", "flow"}};
    model1.port_field_definitions = {{"port2", "field1", ExpressionLineNumber{"param1", 0}}};
    library.models = {model1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, portNotFoundForDef);

    auto model2 = makeModel("model2");
    model2.parameters = {{"param2", true, false}};
    model2.ports = {{"port2", "flow"}};
    model2.port_field_definitions = {{"port2", "field2", ExpressionLineNumber{"param2", 0}}};
    library.models = {model2};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, fieldNotFoundForDef);

    auto model3 = makeModel("model3");
    model3.parameters = {{"param3", true, false}};
    model3.ports = {{"port3", "flow"}, {"port4", "flow"}};
    model3.port_field_definitions = {{"port4", "field1", ExpressionLineNumber{"port3.field1", 0}},
                                     {"port3", "field1", ExpressionLineNumber{"", 0}}};
    library.models = {model3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library), InputError, portInDef);
}

// Test one model with extra outputs
BOOST_FIXTURE_TEST_CASE(model_extra_outputs_properly_translated, Fixture)
{
    auto model1 = makeModel("model1");
    model1.parameters = {{"param1", true, false}};
    model1.variables = {{"var1",
                         ExpressionLineNumber{"7", 0},
                         ExpressionLineNumber{"8", 0},
                         ValueType::CONTINUOUS,
                         true,
                         true,
                         "subproblems"}};
    model1.extra_outputs = {{"output1", ExpressionLineNumber{"5 * param1", 0}},
                            {"output2", ExpressionLineNumber{"param1 / var1 * 95.4", 0}}};

    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);

    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.ExtraOutputs().size(), 2);
    auto& output1 = model.ExtraOutputs().at("output1");
    auto& output2 = model.ExtraOutputs().at("output2");
    BOOST_CHECK_EQUAL(output1.Id(), "output1");
    BOOST_CHECK_EQUAL(output1.expression().Value(), "5 * param1");
    BOOST_CHECK_EQUAL(output2.Id(), "output2");
    BOOST_CHECK_EQUAL(output2.expression().Value(), "param1 / var1 * 95.4");
}
