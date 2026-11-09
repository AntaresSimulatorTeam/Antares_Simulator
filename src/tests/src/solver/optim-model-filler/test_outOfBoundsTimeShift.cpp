// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/connection.h"

#include "visitorFixture.hpp"

namespace
{
struct OutOfBoundsFixture: VisitorFixture<ReadLinearExpressionVisitor>
{
    Api::FillContext fillContext{0, 2, 0, 2, 0};

    // sum_connections(port.field, no_connection_expr = var1[t-1])
    Node* sumWithShiftedFallback()
    {
        auto* shifted = create<TimeShiftNode>(create<ParameterNode>("param_ts"),
                                              create<LiteralNode>(-1));
        return create<PortFieldSumNode>("port", "field", shifted);
    }

    bool outOfBoundsAt(const Node* node, unsigned timeStep)
    {
        EvalVisitor evalVisitor(optimContainer,
                                fillContext,
                                components[0],
                                &data,
                                scenarioGroupRepository.scenario("group"));
        return hasOutOfBoundsTimeShift(node, components[0], timeStep, fillContext, evalVisitor);
    }
};
} // namespace

BOOST_AUTO_TEST_SUITE(_hasOutOfBoundsTimeShift_)

BOOST_FIXTURE_TEST_CASE(unconnected_port_checks_the_no_connection_expression, OutOfBoundsFixture)
{
    auto* node = sumWithShiftedFallback();
    BOOST_CHECK(outOfBoundsAt(node, 0));
    BOOST_CHECK(!outOfBoundsAt(node, 1));
}

BOOST_FIXTURE_TEST_CASE(connected_port_ignores_the_no_connection_expression, OutOfBoundsFixture)
{
    SystemModel::Component other = components[0];
    SystemModel::Port port("port", SystemModel::PortType("type", {}));
    components[0].addComponentConnection("port", SystemModel::ConnectionEnd(&other, &port));

    auto* node = sumWithShiftedFallback();
    BOOST_CHECK(!outOfBoundsAt(node, 0));
    BOOST_CHECK(!outOfBoundsAt(node, 1));
}

BOOST_FIXTURE_TEST_CASE(shift_outside_the_sum_is_still_detected_when_port_is_connected,
                        OutOfBoundsFixture)
{
    SystemModel::Component other = components[0];
    SystemModel::Port port("port", SystemModel::PortType("type", {}));
    components[0].addComponentConnection("port", SystemModel::ConnectionEnd(&other, &port));

    auto* shifted = create<TimeShiftNode>(create<ParameterNode>("param_ts"),
                                          create<LiteralNode>(-1));
    auto* node = create<SumNode>(std::vector<Node*>{sumWithShiftedFallback(), shifted});
    BOOST_CHECK(outOfBoundsAt(node, 0));
    BOOST_CHECK(!outOfBoundsAt(node, 1));
}

BOOST_AUTO_TEST_SUITE_END()
