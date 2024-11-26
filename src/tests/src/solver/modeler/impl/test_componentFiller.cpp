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
#include "antares/solver/optimisation/ComponentFiller.h"
#include "antares/study/system-model/component.h"

using namespace Antares::Solver::Modeler::Api;
using namespace Antares::Study::SystemModel;
using namespace Antares::Optimization;

static Antares::Study::SystemModel::Component createComponent(std::string id)
{
    Antares::Study::SystemModel::ModelBuilder model_builder;
    auto model = model_builder.withId("model").build();
    Antares::Study::SystemModel::ComponentBuilder component_builder;
    auto component = component_builder.withId(id)
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();
    return component;
}

BOOST_AUTO_TEST_SUITE(_ComponentFiller_)

BOOST_AUTO_TEST_CASE(testAddOneVarAllLiteral)
{
    std::unique_ptr<ILinearProblem>
      pb = std::make_unique<Antares::Solver::Modeler::OrtoolsImpl::OrtoolsLinearProblem>(false,
                                                                                         "SCIP");

    ModelBuilder model_builder;

    Antares::Solver::Nodes::LiteralNode lb_node(-5);
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> lb_registry;
    Antares::Solver::NodeRegistry lb_node_registry(&lb_node, std::move(lb_registry));

    Antares::Solver::Nodes::LiteralNode ub_node(10);
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> ub_registry;
    Antares::Solver::NodeRegistry ub_node_registry(&ub_node, std::move(ub_registry));

    Expression lb("-5", std::move(lb_node_registry));
    Expression ub("10", std::move(ub_node_registry));

    auto model = model_builder.withId("model")
                   .withVariables({{"var1", std::move(lb), std::move(ub), ValueType::FLOAT}})
                   .build();
    ComponentBuilder component_builder;
    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    ComponentFiller component_filler(component);
    LinearProblemBuilder linear_problem_builder({&component_filler});

    LinearProblemData dummy_data;
    FillContext dummy_context(0, 0);
    linear_problem_builder.build(*(pb.get()), dummy_data, dummy_context);

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

BOOST_AUTO_TEST_SUITE_END()
