// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#define BOOST_TEST_MODULE modeler checks

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/checks/checkLocation.h>
#include "antares/study/system-model/model.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::Checks;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Solver::Config;

struct Fixture
{
    ModelBuilder modelBuilder;
    SystemBuilder systemBuilder;
    ComponentBuilder componentBuilder;

    Antares::Expressions::Registry<Node> registry;
};

BOOST_AUTO_TEST_SUITE(check_location)

BOOST_FIXTURE_TEST_CASE(reports_first_location_error, Fixture)
{
    Antares::Expressions::Registry<Node> expressionRegistry;
    Node* first = expressionRegistry.create<VariableNode>("first", 0);
    Node* second = expressionRegistry.create<VariableNode>("second", 0);
    Node* root = expressionRegistry.create<SumNode>(first, second);
    Antares::Expressions::NodeRegistry nodeRegistry(root, std::move(expressionRegistry));
    Expression expression("first + second", std::move(nodeRegistry));

    std::vector<Variable> variables;
    variables.push_back({"first", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});
    variables.push_back({"second", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});
    std::vector<Constraint> constraints;
    constraints.push_back({"constraint", std::move(expression), Location::SUBPROBLEMS});
    auto model = modelBuilder.withVariables(std::move(variables))
                   .withConstraints(std::move(constraints))
                   .withId("base model")
                   .build();
    auto system = systemBuilder
                    .withComponents(
                      {componentBuilder.withModel(&model).withId("component").build()})
                    .withId("system")
                    .build();

    const std::string expected =
      "Model 'base model': In expression 'first + second': Error for variable 'first': "
      "Location doesn't match the expression location (variable location: master, "
      "expression location: subproblems)";
    BOOST_CHECK_EXCEPTION(checkModel(model, system),
                          LocationError,
                          [&expected](const LocationError& e) { return e.what() == expected; });
}

BOOST_FIXTURE_TEST_CASE(one_var_good_one_var_throw, Fixture)
{
    Node* goodLocVar = registry.create<VariableNode>("var1", 0);
    Node* badLocVar = registry.create<VariableNode>("var2", 0);
    Node* root = registry.create<SumNode>(goodLocVar, badLocVar);

    std::vector<Variable> variables;
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}, Location::SUBPROBLEMS});
    variables.push_back({"var2", {}, {}, ValueType::BOOL, {}, {}, Location::MASTER});

    auto model = modelBuilder.withVariables(std::move(variables)).withId("base model").build();
    auto system = systemBuilder
                    .withComponents(
                      {componentBuilder.withModel(&model).withId("component").build()})
                    .withId("system")
                    .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, model, system, "var1 + var2"),
      LocationError,
      containsMessage("Model 'base model': In expression 'var1 + var2': Error for variable "
                      "'var2': Location doesn't match the expression location (variable "
                      "location: master, expression location: subproblems)"));
}

BOOST_FIXTURE_TEST_CASE(reduced_cost_throw, Fixture)
{
    Node* root = registry.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                               registry.create<VariableNode>("var", 0));

    std::vector<Variable> variables;
    variables.push_back(
      {"var", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER_AND_SUBPROBLEMS});

    auto model = modelBuilder.withVariables(std::move(variables)).withId("base model").build();
    auto system = systemBuilder
                    .withComponents(
                      {componentBuilder.withModel(&model).withId("component").build()})
                    .withId("system")
                    .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, model, system, "reduced_cost(var)"),
      LocationError,
      containsMessage("Model 'base model': In expression 'reduced_cost(var)': Error for variable "
                      "'var': reduced_cost can only be used on variables located in subproblems"));
}

BOOST_FIXTURE_TEST_CASE(dual_throw, Fixture)
{
    Node* root = registry.create<FunctionNode>(FunctionNodeType::dual,
                                               registry.create<ParameterNode>("constraint"),
                                               registry.create<LiteralNode>(0));

    std::vector<Constraint> constraints;
    constraints.push_back({"constraint", {}, Location::MASTER_AND_SUBPROBLEMS});
    auto model = modelBuilder.withConstraints(std::move(constraints)).withId("base model").build();
    auto system = systemBuilder
                    .withComponents(
                      {componentBuilder.withModel(&model).withId("component").build()})
                    .withId("system")
                    .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, model, system, "dual(constraint)"),
      LocationError,
      containsMessage("Model 'base model': In expression 'dual(constraint)': Error for constraint "
                      "'constraint': dual can only be used on constraints located in subproblems"));
}

BOOST_FIXTURE_TEST_CASE(portfield_ok_var_throw, Fixture)
{
    // expressions setup
    Node* pfNode = registry.create<PortFieldNode>("port", "field");
    Node* varNodeBad = registry.create<VariableNode>("var2", 0);
    Node* root = registry.create<SumNode>(pfNode, varNodeBad);

    Antares::Expressions::Registry<Node> registry2;
    Node* varNode = registry2.create<VariableNode>("var1", 0);
    Antares::Expressions::NodeRegistry node_registry(varNode, std::move(registry2));
    Expression pfExpression("var1", std::move(node_registry));

    // var setup
    std::vector<Variable> variables;
    variables.push_back(
      {"var1", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER_AND_SUBPROBLEMS});
    variables.push_back({"var2", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});

    // ports setup
    PortField portfield("field");
    std::vector portFields = {portfield};
    PortType portType("port", std::move(portFields), {"field", "", "", ""});
    Port port("port", portType);
    std::vector<PortFieldDefinition> portFieldDefs;
    portFieldDefs.emplace_back(port, portfield, std::move(pfExpression));

    auto model = modelBuilder.withVariables(std::move(variables))
                   .withPortFieldDefinitions(std::move(portFieldDefs))
                   .withId("base model")
                   .build();
    auto system = systemBuilder
                    .withComponents(
                      {componentBuilder.withModel(&model).withId("component").build()})
                    .withId("system")
                    .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::MASTER_AND_SUBPROBLEMS, model, system, "port.field + var2"),
      LocationError,
      containsMessage("Model 'base model': In expression 'port.field + var2': Error for variable "
                      "'var2': Location doesn't match the expression location (variable "
                      "location: master, expression location: master-and-subproblems)"));
}

BOOST_FIXTURE_TEST_CASE(porfieldsum_throw, Fixture)
{
    // expressions setup
    Node* pfNode = registry.create<PortFieldNode>("port", "field");
    Node* pfSumNode = registry.create<PortFieldSumNode>("port", "field");

    Antares::Expressions::Registry<Node> registry2;
    Node* varNode = registry2.create<VariableNode>("var1", 0);
    Antares::Expressions::NodeRegistry node_registry(varNode, std::move(registry2));
    Expression pfExpression("var1", std::move(node_registry));

    // var setup
    std::vector<Variable> variables;
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});

    // ports setup
    PortField portfield("field");
    std::vector portFields = {portfield};
    PortType portType("port", std::move(portFields), {"field", "", "", ""});
    Port port("port", portType);
    std::vector<PortFieldDefinition> portFieldDefs;
    portFieldDefs.emplace_back(port, portfield, std::move(pfExpression));

    auto model1 = modelBuilder.withPorts({port}).withId("model1").build();

    auto model2 = modelBuilder.withVariables(std::move(variables))
                    .withId("model2")
                    .withPorts({port})
                    .withPortFieldDefinitions(std::move(portFieldDefs))
                    .build();

    auto component1 = componentBuilder.withModel(&model1).withId("comp1").build();
    auto component2 = componentBuilder.withModel(&model2).withId("comp2").build();

    component1.addComponentConnection("port", ConnectionEnd(component2, model2.Ports().at("port")));

    auto system = systemBuilder.withComponents({component1, component2}).withId("system").build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(pfSumNode,
                      Location::SUBPROBLEMS,
                      model1,
                      system,
                      "sum_connections(port.field)"),
      LocationError,
      containsMessage(
        "In model 'model1': In expression 'sum_connections(port.field)': this "
        "'sum_connections(port.field)' is referencing an expression containing an incorrect "
        "location: Model "
        "'model2': In expression 'var1': Error for variable 'var1': Location doesn't match the "
        "expression location (variable location: master, expression location: subproblems)"));
}

BOOST_AUTO_TEST_SUITE_END()
