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

BOOST_FIXTURE_TEST_CASE(reports_all_location_errors, Fixture)
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

    const std::string firstError
      = "Model 'base model': In expression 'first + second': Error for variable 'first': "
        "Location doesn't match the expression location (variable location: master, "
        "expression location: subproblems)";
    const std::string secondError
      = "Model 'base model': In expression 'first + second': Error for variable 'second': "
        "Location doesn't match the expression location (variable location: master, "
        "expression location: subproblems)";
    BOOST_CHECK_EXCEPTION(checkModel(model, system),
                          LocationError,
                          [&](const LocationError& e)
                          { return e.what() == firstError + "\n" + secondError; });
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

BOOST_FIXTURE_TEST_CASE(stacks_errors_in_nested_connected_port_fields, Fixture)
{
    PortField field("field");
    PortType portType("port", std::vector<PortField>{field}, {"field", "", "", ""});
    Port port("port", portType);

    Antares::Expressions::Registry<Node> innerRegistry;
    auto* first = innerRegistry.create<VariableNode>("first", 0);
    auto* second = innerRegistry.create<VariableNode>("second", 0);
    auto* reduced = innerRegistry.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                                       innerRegistry.create<VariableNode>("shared",
                                                                                          0));
    auto* innerRoot = innerRegistry.create<SumNode>(innerRegistry.create<SumNode>(first, second),
                                                    reduced);
    Expression innerExpression("first + second + reduced_cost(shared)",
                               Antares::Expressions::NodeRegistry(innerRoot,
                                                                  std::move(innerRegistry)));
    std::vector<Variable> variables;
    variables.push_back({"first", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});
    variables.push_back({"second", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});
    variables.push_back(
      {"shared", {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER_AND_SUBPROBLEMS});
    std::vector<PortFieldDefinition> innerDefinitions;
    innerDefinitions.emplace_back(port, field, std::move(innerExpression));
    auto innerModel = modelBuilder.withId("inner")
                        .withPorts({port})
                        .withVariables(std::move(variables))
                        .withPortFieldDefinitions(std::move(innerDefinitions))
                        .build();

    Antares::Expressions::Registry<Node> middleRegistry;
    auto* middleRoot = middleRegistry.create<PortFieldSumNode>("port", "field");
    Expression middleExpression("sum_connections(port.field)",
                                Antares::Expressions::NodeRegistry(middleRoot,
                                                                   std::move(middleRegistry)));
    std::vector<PortFieldDefinition> middleDefinitions;
    middleDefinitions.emplace_back(port, field, std::move(middleExpression));
    auto middleModel = modelBuilder.withId("middle")
                         .withPorts({port})
                         .withPortFieldDefinitions(std::move(middleDefinitions))
                         .build();
    auto outerModel = modelBuilder.withId("outer").withPorts({port}).build();

    auto inner = componentBuilder.withId("inner-component").withModel(&innerModel).build();
    auto middle = componentBuilder.withId("middle-component").withModel(&middleModel).build();
    auto outer = componentBuilder.withId("outer-component").withModel(&outerModel).build();
    middle.addComponentConnection("port", ConnectionEnd(inner, innerModel.Ports().at("port")));
    outer.addComponentConnection("port", ConnectionEnd(middle, middleModel.Ports().at("port")));
    auto system = systemBuilder.withId("system").withComponents({outer, middle, inner}).build();
    auto* root = registry.create<PortFieldSumNode>("port", "field");

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, outerModel, system, "outer expression"),
      LocationError,
      [](const LocationError& e)
      {
          const std::string message = e.what();
          const std::string context
            = "In model 'outer': In expression 'outer expression': this "
              "'sum_connections(port.field)' "
              "is referencing an expression containing an incorrect location: "
              "In model 'middle': In expression 'sum_connections(port.field)': this "
              "'sum_connections(port.field)' is referencing an expression containing an incorrect "
              "location: Model 'inner':";
          const auto first = message.find(context);
          const auto second = message.find(context, first + context.size());
          const auto third = message.find(context, second + context.size());
          return first == 0 && second != std::string::npos && third != std::string::npos
                 && message.find("Error for variable 'first'") != std::string::npos
                 && message.find("Error for variable 'second'") != std::string::npos
                 && message.find("Error for variable 'shared': reduced_cost") != std::string::npos;
      });
}

BOOST_FIXTURE_TEST_CASE(stacks_errors_across_models, Fixture)
{
    std::vector<Model> models;
    for (const auto& name: {"first", "second"})
    {
        Antares::Expressions::Registry<Node> expressionRegistry;
        auto* node = expressionRegistry.create<VariableNode>(name, 0);
        Expression expression(name,
                              Antares::Expressions::NodeRegistry(node,
                                                                 std::move(expressionRegistry)));
        std::vector<Variable> variables;
        variables.push_back({name, {}, {}, ValueType::FLOAT, {}, {}, Location::MASTER});
        std::vector<Constraint> constraints;
        constraints.emplace_back("constraint", std::move(expression), Location::SUBPROBLEMS);
        models.push_back(modelBuilder.withId(name)
                           .withVariables(std::move(variables))
                           .withConstraints(std::move(constraints))
                           .build());
    }
    auto systemModel = modelBuilder.withId("system-model").build();
    auto system = systemBuilder.withId("system")
                    .withComponents(
                      {componentBuilder.withId("component").withModel(&systemModel).build()})
                    .build();
    Antares::Solver::ModelerData data;
    data.system = std::make_unique<System>(std::move(system));
    data.libraries.push_back(
      LibraryBuilder().withId("library").withModels(std::move(models)).build());

    BOOST_CHECK_EXCEPTION(checkLocations(data),
                          LocationError,
                          [](const LocationError& e)
                          {
                              const std::string message = e.what();
                              return message.find("Model 'first'") != std::string::npos
                                     && message.find("Model 'second'") != std::string::npos;
                          });
}

BOOST_AUTO_TEST_SUITE_END()
