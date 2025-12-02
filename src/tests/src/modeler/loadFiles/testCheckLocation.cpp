/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/checkLocation.h>
#include "antares/study/system-model/model.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::LoadFiles;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Modeler::Config;

struct Fixture
{
    ModelBuilder modelBuilder;
    Antares::Expressions::Registry<Node> registry;
};

BOOST_AUTO_TEST_SUITE(check_location)

BOOST_FIXTURE_TEST_CASE(one_var_good_one_var_throw, Fixture)
{
    Node* goodLocVar = registry.create<VariableNode>("var1", 0);
    Node* badLocVar = registry.create<VariableNode>("var2", 0);
    Node* root = registry.create<SumNode>(goodLocVar, badLocVar);

    std::vector<Variable> variables;
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}});
    variables.push_back({"var2", {}, {}, ValueType::BOOL, {}, {}});
    variables[0].setLocation(Location::SUBPROBLEMS);
    variables[1].setLocation(Location::MASTER);

    auto model = modelBuilder.withVariables(std::move(variables)).withId("base model").build();

    BOOST_CHECK_EXCEPTION(checkExpression(root, Location::SUBPROBLEMS, model, "var1 + var2"),
                          LocationError,
                          checkMessage(
                            "Model 'base model': In expression 'var1 + var2': Error for variable "
                            "'var2': Location doesn't match the expression location (variable "
                            "location: master, expression location: subproblems)"));
}

BOOST_FIXTURE_TEST_CASE(reduced_cost_throw, Fixture)
{
    Node* root = registry.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                               registry.create<VariableNode>("var", 0));

    std::vector<Variable> variables;
    variables.push_back({"var", {}, {}, ValueType::FLOAT, {}, {}});
    variables[0].setLocation(Location::MASTER_AND_SUBPROBLEMS);

    auto model = modelBuilder.withVariables(std::move(variables)).withId("base model").build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, model, "reduced_cost(var)"),
      LocationError,
      checkMessage("Model 'base model': In expression 'reduced_cost(var)': Error for variable "
                   "'var': reduced_cost can only be used on variables located in subproblems"));
}

BOOST_FIXTURE_TEST_CASE(dual_throw, Fixture)
{
    Node* root = registry.create<FunctionNode>(FunctionNodeType::dual,
                                               registry.create<ParameterNode>("constraint"),
                                               registry.create<LiteralNode>(0));

    std::vector<Constraint> constraints;
    constraints.push_back({"constraint", {}});
    constraints[0].setLocation(Location::MASTER_AND_SUBPROBLEMS);
    auto model = modelBuilder.withConstraints(std::move(constraints)).withId("base model").build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::SUBPROBLEMS, model, "dual(constraint)"),
      LocationError,
      checkMessage("Model 'base model': In expression 'dual(constraint)': Error for constraint "
                   "'constraint': dual can only be used on constraints located in subproblems"));
}

BOOST_FIXTURE_TEST_CASE(portfield_throw, Fixture)
{
    // expressions setup
    Node* pfNode = registry.create<PortFieldNode>("port", "field");
    Antares::Expressions::Registry<Node> registry2;
    Node* varNode = registry2.create<VariableNode>("var1", 0);
    Antares::Expressions::NodeRegistry node_registry(varNode, std::move(registry2));
    Expression pfExpression("var1", std::move(node_registry));

    // var setup
    std::vector<Variable> variables;
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}});
    variables[0].setLocation(Location::SUBPROBLEMS);

    // ports setup
    PortField portfield("field");
    std::vector portFields = {portfield};
    PortType portType("port", std::move(portFields), "field");
    Port port("port", portType);
    std::vector<PortFieldDefinition> portFieldDefs;
    portFieldDefs.emplace_back(port, portfield, std::move(pfExpression));

    auto model = modelBuilder.withVariables(std::move(variables))
                   .withPortFieldDefinitions(std::move(portFieldDefs))
                   .withId("base model")
                   .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(pfNode, Location::MASTER, model, "port.field"),
      LocationError,
      checkMessage(
        "In model 'base model': In expression 'port.field': this port field definition "
        "'port.field': is referencing a expression with a bad locationModel 'base model': In "
        "expression 'var1': Error for variable 'var1': Location doesn't match the expression "
        "location (variable location: subproblems, expression location: master)"));
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
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}});
    variables.push_back({"var2", {}, {}, ValueType::FLOAT, {}, {}});
    variables[0].setLocation(Location::MASTER_AND_SUBPROBLEMS);
    variables[1].setLocation(Location::MASTER);

    // ports setup
    PortField portfield("field");
    std::vector portFields = {portfield};
    PortType portType("port", std::move(portFields), "field");
    Port port("port", portType);
    std::vector<PortFieldDefinition> portFieldDefs;
    portFieldDefs.emplace_back(port, portfield, std::move(pfExpression));

    auto model = modelBuilder.withVariables(std::move(variables))
                   .withPortFieldDefinitions(std::move(portFieldDefs))
                   .withId("base model")
                   .build();

    BOOST_CHECK_EXCEPTION(
      checkExpression(root, Location::MASTER_AND_SUBPROBLEMS, model, "port.field + var2"),
      LocationError,
      checkMessage("Model 'base model': In expression 'port.field + var2': Error for variable "
                   "'var2': Location doesn't match the expression location (variable "
                   "location: master, expression location: master-and-subproblems)"));
}
BOOST_AUTO_TEST_SUITE_END()
