// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"

#include "inmemory-modeler.h"
#include "unit_test_utils.h"

using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::Optimisation;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Test::Modeler;
using namespace std;

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_MultipleComponentsOffset_, LinearProblemBuildingFixture)

/**
 * Test case demonstrating the bug: when multiple components are added to the linear problem,
 * the objective offset should be accumulated (incremented), not overwritten.
 *
 * Expected behavior:
 * - Component A has objective with offset 10
 * - Component B has objective with offset 20
 * - Total expected offset: 10 + 20 = 30
 */
BOOST_AUTO_TEST_CASE(multiple_components_offsets_should_accumulate)
{
    // Create model A with objective that has offset 10
    auto objectiveA = add(variable("x", 0), literal(10));
    createModelWithOneFloatVar("modelA", {}, "x", literal(-50), literal(-40), {}, objectiveA);
    createComponent("modelA", "componentA", {});

    // Create model B with objective that has offset 20
    auto objectiveB = add(variable("y", 0), literal(20));
    createModelWithOneFloatVar("modelB", {}, "y", literal(0), literal(100), {}, objectiveB);
    createComponent("modelB", "componentB", {});

    // Build linear problem which will add both components
    buildLinearProblem();

    // Expected: 10 + 20 = 30
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 30);
}

/**
 * Test with components having no offset and one with offset
 * to verify the offset is not lost
 */
BOOST_AUTO_TEST_CASE(mixed_components_with_and_without_offset)
{
    // Component A: no offset
    auto objectiveA = variable("x", 0);
    createModelWithOneFloatVar("modelA", {}, "x", literal(-10), literal(10), {}, objectiveA);
    createComponent("modelA", "componentA", {});

    // Component B: offset = 42
    auto objectiveB = add(variable("y", 0), literal(42));
    createModelWithOneFloatVar("modelB", {}, "y", literal(-10), literal(10), {}, objectiveB);
    createComponent("modelB", "componentB", {});

    // Component C: no offset
    auto objectiveC = variable("z", 0);
    createModelWithOneFloatVar("modelC", {}, "z", literal(-10), literal(10), {}, objectiveC);
    createComponent("modelC", "componentC", {});

    buildLinearProblem();

    // Expected: 0 + 42 + 0 = 42
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 42);
}

/**
 * Test with multiple components using the same model to verify
 * that the offset is correctly accumulated
 */
BOOST_AUTO_TEST_CASE(multiple_components_same_model_offsets_should_accumulate)
{
    auto objective = add(variable("x", 0), literal(10));
    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);

    createComponent("model", "componentA", {});
    createComponent("model", "componentB", {});

    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 20);
}

BOOST_AUTO_TEST_SUITE_END()
