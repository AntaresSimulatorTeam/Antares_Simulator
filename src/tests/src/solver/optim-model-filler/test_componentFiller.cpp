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

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/expressions/nodes/LiteralNode.h"
#include "antares/solver/modeler/api/linearProblemBuilder.h"
#include "antares/solver/modeler/ortoolsImpl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"

using namespace Antares::Solver::Modeler::Api;
using namespace Antares::Study::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Solver::Nodes;

struct LinearProblemBuildingFixture
{
    ModelBuilder model_builder;
    ComponentBuilder component_builder;
};

static Expression generateExpression(Node* node)
{
    Antares::Solver::Registry<Node> registry;
    Antares::Solver::NodeRegistry node_registry(node, std::move(registry));
    Expression expression("expression", std::move(node_registry));
    return std::move(expression);
}

static std::unique_ptr<ILinearProblem> buildProblem(std::vector<LinearProblemFiller*> fillers)
{
    std::unique_ptr<ILinearProblem>
      pb = std::make_unique<Antares::Solver::Modeler::OrtoolsImpl::OrtoolsLinearProblem>(false,
                                                                                         "scip");
    LinearProblemBuilder linear_problem_builder(fillers);
    LinearProblemData dummy_data;
    FillContext dummy_context = {0, 0};
    linear_problem_builder.build(*(pb.get()), dummy_data, dummy_context);
    return std::move(pb);
}

BOOST_AUTO_TEST_SUITE(_ComponentFiller_)

BOOST_FIXTURE_TEST_CASE(testAddOneVarAllLiteral, LinearProblemBuildingFixture)
{
    LiteralNode lb_node(-5);
    LiteralNode ub_node(10);
    Variable var1 = {"var1",
                     generateExpression(&lb_node),
                     generateExpression(&ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> vec_vars;
    vec_vars.push_back(std::move(var1));
    auto model = model_builder.withId("model").withVariables(std::move(vec_vars)).build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    auto pb = buildProblem({filler.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->getVariable("componentToto.var1");
    BOOST_CHECK(var);
    BOOST_CHECK_EQUAL(var->getLb(), -5);
    BOOST_CHECK_EQUAL(var->getUb(), 10);
    // TODO : check variable type (float, not integer)
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
}

// TODO
// - test with 3 variables (different types: float, bool, int)
// - test with one model, 1 variable, 2 components (and 2 component fillers)
// - test with one model, 1 variable, lb and ub are dependent on component parameters (2 components)
//        in model builder : .withParameters({"p_min", NO, NO})
//        in component builder : use withParameterValues
// - test with ill-defined lb and/or ub

BOOST_AUTO_TEST_SUITE_END()
