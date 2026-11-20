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
struct PortFieldSumTimeShiftFixture: VisitorFixture<ReadLinearExpressionVisitor>
{
    Api::FillContext fillContext{0, 2, 0, 2, 0};

    bool outOfBoundsAt(const Node* node, const SystemModel::Component& component, unsigned timeStep)
    {
        EvalVisitor evalVisitor(optimContainer,
                                fillContext,
                                component,
                                &data,
                                scenarioGroupRepository.scenario("group"));
        return hasOutOfBoundsTimeShift(node, timeStep, fillContext, evalVisitor);
    }

    // Transfers the fixture's registry into the expression. Create every node needed by the test
    // before calling this, so all AST nodes remain owned by that one registry.
    SystemModel::Expression makeExpression(const std::string& value, Node* root)
    {
        return SystemModel::Expression(value, NodeRegistry(root, std::move(*this)));
    }
};
} // namespace

BOOST_FIXTURE_TEST_CASE(connected_port_field_sum_checks_peer_time_shifts,
                        PortFieldSumTimeShiftFixture)
{
    SystemModel::PortField field("field");
    SystemModel::PortType portType("type", {field});
    SystemModel::Port port("port", portType);

    TimeShiftNode* peerExpression = create<TimeShiftNode>(create<LiteralNode>(0.),
                                                          create<LiteralNode>(-1));
    PortFieldSumNode* portFieldSum = create<PortFieldSumNode>("port", "field");
    EqualNode* ownerExpression = create<EqualNode>(portFieldSum, create<LiteralNode>(0.));
    SystemModel::Expression peerFieldExpression = makeExpression("time_shifted_peer_field",
                                                                 peerExpression);
    std::vector<SystemModel::PortFieldDefinition> fieldDefinitions;
    fieldDefinitions.emplace_back(port, field, std::move(peerFieldExpression));

    SystemModel::ModelBuilder modelBuilder;
    SystemModel::Model peerModel = modelBuilder.withId("peer")
                                     .withPorts({port})
                                     .withPortFieldDefinitions(std::move(fieldDefinitions))
                                     .build();
    SystemModel::Model ownerModel = modelBuilder.withId("owner").withPorts({port}).build();

    SystemModel::ComponentBuilder componentBuilder;
    SystemModel::Component peer = componentBuilder.withId("peer_component")
                                    .withModel(&peerModel)
                                    .withScenarioGroupId("group")
                                    .build();
    SystemModel::Component owner = componentBuilder.withId("owner_component")
                                     .withModel(&ownerModel)
                                     .withScenarioGroupId("group")
                                     .build();
    owner.addComponentConnection("port", SystemModel::ConnectionEnd(peer, port));

    BOOST_CHECK(outOfBoundsAt(ownerExpression, owner, 0));
    BOOST_CHECK(!outOfBoundsAt(ownerExpression, owner, 1));
}

BOOST_FIXTURE_TEST_CASE(connected_port_field_sum_in_bounds, PortFieldSumTimeShiftFixture)
{
    // Test: positive time shift within bounds (shift of +1 should not be out of bounds
    // at the first timestep but out of bounds at the last one)
    SystemModel::PortField field("field");
    SystemModel::PortType portType("type", {field});
    SystemModel::Port port("port", portType);

    TimeShiftNode* peerExpression = create<TimeShiftNode>(create<LiteralNode>(0.),
                                                          create<LiteralNode>(1.));
    PortFieldSumNode* portFieldSum = create<PortFieldSumNode>("port", "field");
    EqualNode* ownerExpression = create<EqualNode>(portFieldSum, create<LiteralNode>(0.));
    SystemModel::Expression peerFieldExpression = makeExpression("time_shifted_peer_field",
                                                                 peerExpression);
    std::vector<SystemModel::PortFieldDefinition> fieldDefinitions;
    fieldDefinitions.emplace_back(port, field, std::move(peerFieldExpression));

    SystemModel::ModelBuilder modelBuilder;
    SystemModel::Model peerModel = modelBuilder.withId("peer")
                                     .withPorts({port})
                                     .withPortFieldDefinitions(std::move(fieldDefinitions))
                                     .build();
    SystemModel::Model ownerModel = modelBuilder.withId("owner").withPorts({port}).build();

    SystemModel::ComponentBuilder componentBuilder;
    SystemModel::Component peer = componentBuilder.withId("peer_component")
                                    .withModel(&peerModel)
                                    .withScenarioGroupId("group")
                                    .build();
    SystemModel::Component owner = componentBuilder.withId("owner_component")
                                     .withModel(&ownerModel)
                                     .withScenarioGroupId("group")
                                     .build();
    owner.addComponentConnection("port", SystemModel::ConnectionEnd(peer, port));

    // Positive shift of +1 at time step 0 should be in bounds (not out of bounds)
    BOOST_CHECK(!outOfBoundsAt(ownerExpression, owner, 0));
    // Positive shift of +1 at the last time step (2) should be out of bounds (2 + 1 > 2)
    BOOST_CHECK(outOfBoundsAt(ownerExpression, owner, 2));
}

BOOST_FIXTURE_TEST_CASE(connected_port_field_sum_no_shift, PortFieldSumTimeShiftFixture)
{
    // Test: no time shift on the peer field (neutral case should be in bounds)
    SystemModel::PortField field("field");
    SystemModel::PortType portType("type", {field});
    SystemModel::Port port("port", portType);

    LiteralNode* peerExpression = create<LiteralNode>(0.);
    PortFieldSumNode* portFieldSum = create<PortFieldSumNode>("port", "field");
    EqualNode* ownerExpression = create<EqualNode>(portFieldSum, create<LiteralNode>(0.));
    SystemModel::Expression peerFieldExpression = makeExpression("field", peerExpression);
    std::vector<SystemModel::PortFieldDefinition> fieldDefinitions;
    fieldDefinitions.emplace_back(port, field, std::move(peerFieldExpression));

    SystemModel::ModelBuilder modelBuilder;
    SystemModel::Model peerModel = modelBuilder.withId("peer")
                                     .withPorts({port})
                                     .withPortFieldDefinitions(std::move(fieldDefinitions))
                                     .build();
    SystemModel::Model ownerModel = modelBuilder.withId("owner").withPorts({port}).build();

    SystemModel::ComponentBuilder componentBuilder;
    SystemModel::Component peer = componentBuilder.withId("peer_component")
                                    .withModel(&peerModel)
                                    .withScenarioGroupId("group")
                                    .build();
    SystemModel::Component owner = componentBuilder.withId("owner_component")
                                     .withModel(&ownerModel)
                                     .withScenarioGroupId("group")
                                     .build();
    owner.addComponentConnection("port", SystemModel::ConnectionEnd(peer, port));

    // No time shift → should be in bounds (not out of bounds)
    BOOST_CHECK(!outOfBoundsAt(ownerExpression, owner, 0));
}
