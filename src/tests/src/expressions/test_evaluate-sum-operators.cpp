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
 * along with Antares_Simulator. If not, see
 * <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>

#include "UtilMocks.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Optimisation;
using namespace Antares::ModelerStudy::SystemModel;

// =================================
// Fixture to build AST nodes tree
// =================================
struct build_AST_fixture
{
    Node* literal(double value);
    Node* allTimeSum(Node* node);

private:
    Registry<Nodes::Node> registry_;
};

Node* build_AST_fixture::literal(double value)
{
    return registry_.create<LiteralNode>(value);
}

Node* build_AST_fixture::allTimeSum(Node* node)
{
    return registry_.create<AllTimeSumNode>(node);
}

// =============================
// Fixture to build EvalVisitor
// =============================
struct build_eval_visitor_fixture
{
    build_eval_visitor_fixture();

    LinearProblemDataImpl::LinearProblemData data;
    MockLinearProblem linearProblem;
    OptimEntityContainer optimEntityContainer;
    LinearProblemApi::FillContext fillCtx;

    std::unique_ptr<Visitors::EvalVisitor> evaluator;

private:
    Model model_;
    Component component_;
    ScenarioGroupRepository scenarioGroupRepository_;
};

build_eval_visitor_fixture::build_eval_visitor_fixture():
    linearProblem(true),
    fillCtx(1, 3, 1, 3, 1),
    model_(createModelWithoutParameters()),
    component_(createComponent(model_)),
    scenarioGroupRepository_(getscenarioGroupRepository(component_)),
    optimEntityContainer(linearProblem, &data, &scenarioGroupRepository_)
{
    optimEntityContainer.addFromSystemComponents({component_});
    evaluator = std::make_unique<Visitors::EvalVisitor>(optimEntityContainer, fillCtx, component_);
}

// =================================================
// Test fixture : made from both previous fixtures
// =================================================
struct tests_fixture: build_AST_fixture, build_eval_visitor_fixture
{
};

BOOST_FIXTURE_TEST_CASE(sum_a_literal_over_time_span, tests_fixture)
{
    // Expression : sum(1.)
    Node* one = literal(1.);
    Node* someOfOnes = allTimeSum(one);

    auto evalResult = evaluator->dispatch(someOfOnes);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 3.);
}
