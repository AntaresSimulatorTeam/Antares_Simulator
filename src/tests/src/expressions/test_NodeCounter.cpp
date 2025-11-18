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

#include <antares/expressions/nodes/NodeCounter.h>

#include "unit_test_utils.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

class MockNode: public Nodes::Node
{
private:
    std::string name_;

public:
    explicit MockNode(const std::string& name):
        name_(name)
    {
    }

    std::string name() const override
    {
        return name_;
    }
};

BOOST_AUTO_TEST_SUITE(NodeCounterTestSuite)

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
    NodeCounter counter;

    BOOST_TEST(counter.empty() == true);
    BOOST_TEST(counter.nodeCount() == 0);
    BOOST_TEST(counter.nbNodesPerType().empty() == true);
}

BOOST_AUTO_TEST_CASE(test_getNodeID_first_call)
{
    NodeCounter counter;
    MockNode node("TestNode");

    unsigned int id = counter.getNodeID(&node);

    BOOST_TEST(id == 1);
    BOOST_TEST(counter.nodeCount() == 1);
}

BOOST_AUTO_TEST_CASE(test_getNodeID_multiple_calls_same_node)
{
    NodeCounter counter;
    MockNode node("TestNode");

    unsigned int id1 = counter.getNodeID(&node);
    unsigned int id2 = counter.getNodeID(&node);
    unsigned int id3 = counter.getNodeID(&node);

    BOOST_TEST(id1 == 1);
    BOOST_TEST(id2 == 1);
    BOOST_TEST(id3 == 1);
    BOOST_TEST(counter.nodeCount() == 1);
}

BOOST_AUTO_TEST_CASE(test_getNodeID_multiple_different_nodes)
{
    NodeCounter counter;
    MockNode node1("Node1");
    MockNode node2("Node2");
    MockNode node3("Node3");

    unsigned int id1 = counter.getNodeID(&node1);
    unsigned int id2 = counter.getNodeID(&node2);
    unsigned int id3 = counter.getNodeID(&node3);

    BOOST_TEST(id1 == 1);
    BOOST_TEST(id2 == 2);
    BOOST_TEST(id3 == 3);
    BOOST_TEST(counter.nodeCount() == 3);
}

BOOST_AUTO_TEST_CASE(test_reset)
{
    NodeCounter counter;
    MockNode node1("Node1");
    MockNode node2("Node2");

    counter.getNodeID(&node1);
    counter.getNodeID(&node2);
    counter.computeNumberNodesPerType();

    BOOST_TEST(counter.empty() == false);
    BOOST_TEST(counter.nodeCount() == 2);
    BOOST_TEST(counter.nbNodesPerType().size() == 2);

    counter.reset();

    BOOST_TEST(counter.empty() == true);
    BOOST_TEST(counter.nodeCount() == 0);
    BOOST_TEST(counter.nbNodesPerType().empty() == true);
}

BOOST_AUTO_TEST_CASE(test_empty)
{
    NodeCounter counter;

    // Test initial state
    BOOST_TEST(counter.empty() == true);

    // Test after adding nodes but before computeNumberNodesPerType
    MockNode node("TestNode");
    counter.getNodeID(&node);
    BOOST_TEST(counter.empty() == true); // Should still be empty

    // Test after computeNumberNodesPerType
    counter.computeNumberNodesPerType();
    BOOST_TEST(counter.empty() == false);

    // Test after reset
    counter.reset();
    BOOST_TEST(counter.empty() == true);
}

BOOST_AUTO_TEST_CASE(test_computeNumberNodesPerType_single_node_type)
{
    NodeCounter counter;
    MockNode node1("SameType");
    MockNode node2("SameType");
    MockNode node3("SameType");

    counter.getNodeID(&node1);
    counter.getNodeID(&node2);
    counter.getNodeID(&node3);

    counter.computeNumberNodesPerType();

    const auto& nbNodes = counter.nbNodesPerType();
    BOOST_TEST(nbNodes.size() == 1);
    BOOST_TEST(nbNodes.at("SameType") == 3);
    BOOST_TEST(counter.empty() == false);
}

BOOST_AUTO_TEST_CASE(test_computeNumberNodesPerType_multiple_node_types)
{
    NodeCounter counter;
    MockNode node1("TypeA");
    MockNode node2("TypeB");
    MockNode node3("TypeA");
    MockNode node4("TypeC");
    MockNode node5("TypeB");

    counter.getNodeID(&node1);
    counter.getNodeID(&node2);
    counter.getNodeID(&node3);
    counter.getNodeID(&node4);
    counter.getNodeID(&node5);

    counter.computeNumberNodesPerType();

    const auto& nbNodes = counter.nbNodesPerType();
    BOOST_TEST(nbNodes.size() == 3);
    BOOST_TEST(nbNodes.at("TypeA") == 2);
    BOOST_TEST(nbNodes.at("TypeB") == 2);
    BOOST_TEST(nbNodes.at("TypeC") == 1);
}

BOOST_AUTO_TEST_CASE(test_computeNumberNodesPerType_empty)
{
    NodeCounter counter;

    counter.computeNumberNodesPerType();

    const auto& nbNodes = counter.nbNodesPerType();
    BOOST_TEST(nbNodes.empty() == true);
    BOOST_TEST(counter.empty() == true);
}

BOOST_AUTO_TEST_CASE(test_computeNumberNodesPerType_multiple_calls)
{
    NodeCounter counter;
    MockNode node1("TypeA");
    MockNode node2("TypeB");

    counter.getNodeID(&node1);
    counter.getNodeID(&node2);

    // First call
    counter.computeNumberNodesPerType();
    const auto& nbNodes1 = counter.nbNodesPerType();
    BOOST_TEST(nbNodes1.size() == 2);
    BOOST_TEST(nbNodes1.at("TypeA") == 1);
    BOOST_TEST(nbNodes1.at("TypeB") == 1);

    // Add more nodes and call again
    MockNode node3("TypeA");
    MockNode node4("TypeA");
    counter.getNodeID(&node3);
    counter.getNodeID(&node4);

    counter.computeNumberNodesPerType();
    const auto& nbNodes2 = counter.nbNodesPerType();
    BOOST_TEST(nbNodes2.size() == 2);
    BOOST_TEST(nbNodes2.at("TypeA") == 3); // Should accumulate
    BOOST_TEST(nbNodes2.at("TypeB") == 1);
}

BOOST_AUTO_TEST_CASE(test_nodeCount)
{
    NodeCounter counter;

    BOOST_TEST(counter.nodeCount() == 0);

    MockNode node1("Node1");
    counter.getNodeID(&node1);
    BOOST_TEST(counter.nodeCount() == 1);

    MockNode node2("Node2");
    counter.getNodeID(&node2);
    BOOST_TEST(counter.nodeCount() == 2);

    // Same node shouldn't increment count
    counter.getNodeID(&node1);
    BOOST_TEST(counter.nodeCount() == 2);

    counter.reset();
    BOOST_TEST(counter.nodeCount() == 0);
}

BOOST_AUTO_TEST_CASE(test_contains_method)
{
    NodeCounter counter;

    // Test with empty counter
    BOOST_TEST(counter.contains("AnyType") == false);

    // Add nodes and compute
    MockNode node1("TypeA");
    MockNode node2("TypeB");
    counter.getNodeID(&node1);
    counter.getNodeID(&node2);
    counter.computeNumberNodesPerType();

    // Test contains with existing types
    BOOST_TEST(counter.contains("TypeA") == true);
    BOOST_TEST(counter.contains("TypeB") == true);
    BOOST_TEST(counter.contains("TypeC") == false);
}

BOOST_AUTO_TEST_CASE(test_null_node_handling)
{
    NodeCounter counter;

    // Test avec un pointeur null
    BOOST_CHECK_EXCEPTION(counter.getNodeID(nullptr),
                          std::invalid_argument,
                          checkMessage("cannot get node id from nullptr"));
}

BOOST_AUTO_TEST_CASE(test_nbNodesPerType_const_accessor)
{
    NodeCounter counter;
    MockNode node("TestNode");

    counter.getNodeID(&node);
    counter.computeNumberNodesPerType();

    // Test que l'accesseur retourne bien une référence const
    const auto& nbNodes = counter.nbNodesPerType();
    BOOST_TEST(nbNodes.size() == 1);
    BOOST_TEST(nbNodes.at("TestNode") == 1);

    // Vérifier que c'est bien const (ne peut pas être modifié)
    static_assert(std::is_const_v<std::remove_reference_t<decltype(nbNodes)>>,
                  "nbNodesPerType should return const reference");
}

BOOST_AUTO_TEST_SUITE_END()
