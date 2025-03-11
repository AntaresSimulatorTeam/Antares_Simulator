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

#include <antares/io/inputs/yml-system/converter.h>
#include <antares/io/inputs/yml-system/parser.h>
#include <antares/study/system-model/system.h>
#include "yaml-cpp/yaml.h"


using namespace std::string_literals;
using namespace Antares::IO::Inputs;
using namespace Antares::Study;

BOOST_AUTO_TEST_CASE(no_connections_is_allowed)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK(systemObj.connections.empty());
}

BOOST_AUTO_TEST_CASE(empty_connections_is_allowed)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections: []
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK(systemObj.connections.empty());
}

/**
* Idealy we would like to throw an error if connection is not a map
*/
BOOST_AUTO_TEST_CASE(connection_is_map)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections: true
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK(systemObj.connections.empty());
}

BOOST_AUTO_TEST_CASE(connection_is_read_properly)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - component1: N
                  port1: injection_port
                  component2: D
                  port2: other_port
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.connections[0].component1, "N");
    BOOST_CHECK_EQUAL(systemObj.connections[0].port1, "injection_port");
    BOOST_CHECK_EQUAL(systemObj.connections[0].component2, "D");
    BOOST_CHECK_EQUAL(systemObj.connections[0].port2, "other_port");
}

BOOST_AUTO_TEST_CASE(missing_port2_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - component1: N
                  port1: injection_port
                  component2: D
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), YAML::InvalidNode);
}

BOOST_AUTO_TEST_CASE(missing_port1_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - component1: N
                  port2: injection_port
                  component2: D
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), YAML::InvalidNode);
}

BOOST_AUTO_TEST_CASE(missing_component1_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - port2: N
                  port1: injection_port
                  component2: D
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), YAML::InvalidNode);
}

BOOST_AUTO_TEST_CASE(missing_component2_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - component1: N
                  port1: injection_port
                  port2: D
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), YAML::InvalidNode);
}

/**
 * Test that an extra field in the connection does not cause an error
 * Test the implementation more than the expected behaviour
 */
BOOST_AUTO_TEST_CASE(extra_field_does_not_cause_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
            connections:
                - component1: N
                  port1: injection_port
                  component2: D
                  port2: other_port
                  component3: R
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.connections[0].component1, "N");
    BOOST_CHECK_EQUAL(systemObj.connections[0].port1, "injection_port");
    BOOST_CHECK_EQUAL(systemObj.connections[0].component2, "D");
    BOOST_CHECK_EQUAL(systemObj.connections[0].port2, "other_port");
}