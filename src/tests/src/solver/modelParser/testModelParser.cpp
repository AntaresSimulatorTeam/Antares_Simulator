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

#define BOOST_TEST_MODULE modelParser

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/modelParser/parser.h"

// Test empty library
BOOST_AUTO_TEST_CASE(test_empty_library)
{
    Antares::Solver::ModelParser::Parser parser;
    auto library = R"(
        library:
            id: ""
            description: ""
            port-types: []
    )";
    Antares::Solver::ModelParser::Library libraryObj = parser.parse(library);
    BOOST_CHECK(libraryObj.id.empty());
    BOOST_CHECK(libraryObj.description.empty());
    BOOST_CHECK(libraryObj.port_types.empty());
    BOOST_CHECK(libraryObj.models.empty());
}

// Test library with id and description
BOOST_AUTO_TEST_CASE(test_library_id_description)
{
    Antares::Solver::ModelParser::Parser parser;
    auto library = R"(
        library:
            id: "test_id"
            description: "test_description"
            port-types: []
        )";
    Antares::Solver::ModelParser::Library libraryObj = parser.parse(library);
    BOOST_CHECK(libraryObj.id == "test_id");
    BOOST_CHECK(libraryObj.description == "test_description");
    BOOST_CHECK(libraryObj.port_types.empty());
    BOOST_CHECK(libraryObj.models.empty());
}

// Test library with port types
BOOST_AUTO_TEST_CASE(test_library_port_types)
{
    Antares::Solver::ModelParser::Parser parser;
    auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types:
                - id: "porttype_id"
                  description: "porttype_description"
                  fields:
                      - name: "port_name"
        )";
    Antares::Solver::ModelParser::Library libraryObj = parser.parse(library);
    BOOST_CHECK(libraryObj.port_types.size() == 1);
    BOOST_CHECK(libraryObj.port_types[0].id == "porttype_id");
    BOOST_CHECK(libraryObj.port_types[0].description == "porttype_description");
    BOOST_CHECK(libraryObj.port_types[0].fields.size() == 1);
    BOOST_CHECK(libraryObj.port_types[0].fields[0] == "port_name");
}
