// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/connection.h"

#include "visitorFixture.hpp"

namespace bdata = boost::unit_test::data;

namespace
{
// Time steps of the fixture go from 0 to 2
struct PortFieldSumTimeShiftFixture: VisitorFixture<ReadLinearExpressionVisitor>
{
    using ParameterValues = std::map<std::string, SystemModel::ParameterTypeAndValue>;

    Api::FillContext fillContext{0, 2, 0, 2, 0};

    SystemModel::PortField field{"field"};
    SystemModel::PortType portType{"type", {field}};
    SystemModel::Port port{"port", portType};

    // ConnectionEnd holds references: models and components need stable addresses
    std::vector<std::unique_ptr<SystemModel::Model>> models;
    std::vector<std::unique_ptr<SystemModel::Component>> componentsStorage;

    SystemModel::Component& owner = addComponent("owner", nullptr);

    // Adds a component with a port "port", whose field "field" is defined by fieldExpression
    // (no definition if nullptr). Create every node of fieldExpression before calling this: the
    // fixture's registry is transferred to the component's expression.
    SystemModel::Component& addComponent(const std::string& id,
                                         Node* fieldExpression,
                                         const ParameterValues& parameterValues = {})
    {
        std::vector<SystemModel::PortFieldDefinition> fieldDefinitions;
        if (fieldExpression)
        {
            fieldDefinitions.emplace_back(
              port,
              field,
              SystemModel::Expression(id + "_field",
                                      NodeRegistry(fieldExpression, std::move(*this))));
        }
        std::vector<SystemModel::Parameter> parameters;
        for (const auto& [parameterId, value]: parameterValues)
        {
            parameters.emplace_back(parameterId,
                                    SystemModel::TimeDependent::NO,
                                    SystemModel::ScenarioDependent::NO);
        }

        SystemModel::ModelBuilder modelBuilder;
        auto& model = *models.emplace_back(std::make_unique<SystemModel::Model>(
          modelBuilder.withId(id + "_model")
            .withPorts({port})
            .withParameters(std::move(parameters))
            .withPortFieldDefinitions(std::move(fieldDefinitions))
            .build()));

        SystemModel::ComponentBuilder componentBuilder;
        return *componentsStorage.emplace_back(
          std::make_unique<SystemModel::Component>(componentBuilder.withId(id)
                                                     .withModel(&model)
                                                     .withScenarioGroupId("group")
                                                     .withParameterValues(parameterValues)
                                                     .build()));
    }

    void connect(SystemModel::Component& from, SystemModel::Component& to)
    {
        from.addComponentConnection("port", SystemModel::ConnectionEnd(to, port));
    }

    // Expression of the owner: port.field == 0 (or shift(port.field, outerShift) == 0)
    Node* ownerExpression(std::optional<double> outerShift = std::nullopt)
    {
        Node* portFieldSum = create<PortFieldSumNode>("port", "field");
        if (outerShift)
        {
            portFieldSum = create<TimeShiftNode>(portFieldSum, create<LiteralNode>(*outerShift));
        }
        return create<EqualNode>(portFieldSum, create<LiteralNode>(0.));
    }

    Node* shifted(double shift)
    {
        return create<TimeShiftNode>(create<LiteralNode>(0.), create<LiteralNode>(shift));
    }

    bool outOfBoundsAt(const Node* node, unsigned timeStep)
    {
        EvalVisitor evalVisitor(optimContainer,
                                fillContext,
                                owner,
                                &data,
                                scenarioGroupRepository.scenario("group"));
        return hasOutOfBoundsTimeShift(node, timeStep, fillContext, evalVisitor);
    }
};

struct ShiftCase
{
    double peerShift;
    unsigned timeStep;
    bool expectedOutOfBounds;
};

std::ostream& operator<<(std::ostream& os, const ShiftCase& c)
{
    return os << "{peerShift=" << c.peerShift << ", timeStep=" << c.timeStep
              << ", expectedOutOfBounds=" << c.expectedOutOfBounds << "}";
}
} // namespace

BOOST_AUTO_TEST_SUITE(out_of_bounds_time_shift_port)

BOOST_DATA_TEST_CASE_F(PortFieldSumTimeShiftFixture,
                       connected_port_field_shift,
                       bdata::make(std::vector<ShiftCase>{
                         // Negative shift: out of bounds at the first step, in bounds afterwards
                         {-1., 0, true},
                         {-1., 1, false},
                         // Positive shift: in bounds at the first step, out of bounds at the last
                         {1., 0, false},
                         {1., 2, true},
                         // Neutral case
                         {0., 0, false}}),
                       c)
{
    auto* expression = ownerExpression();
    auto& peer = addComponent("peer", shifted(c.peerShift));
    connect(owner, peer);

    BOOST_CHECK_EQUAL(outOfBoundsAt(expression, c.timeStep), c.expectedOutOfBounds);
}

BOOST_FIXTURE_TEST_CASE(no_connection_is_in_bounds, PortFieldSumTimeShiftFixture)
{
    BOOST_CHECK(!outOfBoundsAt(ownerExpression(), 0));
}

BOOST_FIXTURE_TEST_CASE(only_second_connection_is_shifted, PortFieldSumTimeShiftFixture)
{
    auto* expression = ownerExpression();
    auto& peerA = addComponent("peerA", create<LiteralNode>(0.));
    auto& peerB = addComponent("peerB", shifted(-1.));
    connect(owner, peerA);
    connect(owner, peerB);

    BOOST_CHECK(outOfBoundsAt(expression, 0));
    BOOST_CHECK(!outOfBoundsAt(expression, 1));
}

BOOST_FIXTURE_TEST_CASE(peer_field_is_itself_a_port_field_sum, PortFieldSumTimeShiftFixture)
{
    auto* expression = ownerExpression();
    auto& farPeer = addComponent("farPeer", shifted(-1.));
    auto& nearPeer = addComponent("nearPeer", create<PortFieldSumNode>("port", "field"));
    connect(owner, nearPeer);
    connect(nearPeer, farPeer);

    BOOST_CHECK(outOfBoundsAt(expression, 0));
    BOOST_CHECK(!outOfBoundsAt(expression, 1));
}

BOOST_FIXTURE_TEST_CASE(shift_nested_in_larger_peer_expression, PortFieldSumTimeShiftFixture)
{
    // peer field: 1 + shift(0, -1)
    auto* expression = ownerExpression();
    auto& peer = addComponent("peer", create<SumNode>(create<LiteralNode>(1.), shifted(-1.)));
    connect(owner, peer);

    BOOST_CHECK(outOfBoundsAt(expression, 0));
    BOOST_CHECK(!outOfBoundsAt(expression, 1));
}

BOOST_FIXTURE_TEST_CASE(shift_value_is_evaluated_in_the_peer_context, PortFieldSumTimeShiftFixture)
{
    // "peer_shift" only exists on the peer: evaluating it in the owner's context would throw
    auto* expression = ownerExpression();
    auto& peer = addComponent("peer",
                              create<TimeShiftNode>(create<LiteralNode>(0.),
                                                    create<ParameterNode>("peer_shift")),
                              {{"peer_shift",
                                {.id = "peer_shift",
                                 .type = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                                 .value = "-1."}}});
    connect(owner, peer);

    BOOST_CHECK(outOfBoundsAt(expression, 0));
    BOOST_CHECK(!outOfBoundsAt(expression, 1));
}

BOOST_FIXTURE_TEST_CASE(nested_time_shifts_compose, PortFieldSumTimeShiftFixture)
{
    // shift(shift(0, +1), +1): total shift of +2, last time step is 2
    Node* expression = create<TimeShiftNode>(shifted(1.), create<LiteralNode>(1.));

    BOOST_CHECK(!outOfBoundsAt(expression, 0));
    BOOST_CHECK(outOfBoundsAt(expression, 1));
}

BOOST_FIXTURE_TEST_CASE(outer_shift_composes_with_connected_port_field_shift,
                        PortFieldSumTimeShiftFixture)
{
    // Owner: shift(port.field, +1). Peer field: shift(0, +1). Total shift: +2
    auto* expression = ownerExpression(1.);
    auto& peer = addComponent("peer", shifted(1.));
    connect(owner, peer);

    BOOST_CHECK(!outOfBoundsAt(expression, 0));
    // 1 + 1 + 1 = 3 > 2: only detected if the outer shift is carried into the peer field
    BOOST_CHECK(outOfBoundsAt(expression, 1));
}

BOOST_AUTO_TEST_SUITE_END()
