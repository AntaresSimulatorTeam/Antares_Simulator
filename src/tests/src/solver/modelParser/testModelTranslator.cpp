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

#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/modelParser/model.h"

std::vector<Antares::Solver::ObjectModel::PortType> toPortTypes(
  std::span<const Antares::Solver::ModelParser::PortType> portTypes)
{
    // Convert portTypes to Antares::Solver::ObjectModel::PortType
    std::vector<Antares::Solver::ObjectModel::PortType> out;
    for (const auto& portType: portTypes)
    {
        std::vector<Antares::Solver::ObjectModel::PortField> fields;
        std::transform(portType.fields.begin(),
                       portType.fields.end(),
                       fields.begin(),
                       [](const auto& field)
                       { return Antares::Solver::ObjectModel::PortField{field}; });
        Antares::Solver::ObjectModel::PortType portTypeModel(portType.id,
                                                             portType.description,
                                                             std::move(fields));
        out.push_back(portTypeModel);
    }
    return out;
}

// Given a Antares::Solver::ModelParser::Library object, return a
// Antares::Solver::ObjectModel::Library object
Antares::Solver::ObjectModel::Library convert(const Antares::Solver::ModelParser::Library& library)
{
    Antares::Solver::ObjectModel::LibraryBuilder builder;
    std::vector<Antares::Solver::ObjectModel::PortType> portTypes = toPortTypes(library.port_types);
    Antares::Solver::ObjectModel::Library lib = builder.withId(library.id)
                                                  .withDescription(library.description)
                                                  .withPortType(portTypes)
                                                  //.withModel(library.models)
                                                  .build();
    return lib;
}

// Test empty library
BOOST_AUTO_TEST_CASE(test_library_empty)
{
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_CHECK(lib.id().empty());
    BOOST_CHECK(lib.description().empty());
    BOOST_CHECK(lib.portTypes().empty());
    BOOST_CHECK(lib.models().empty());
}

// Test library with id and description
BOOST_AUTO_TEST_CASE(test_library_id_and_description)
{
    Antares::Solver::ModelParser::Library library;
    library.id = "test_id";
    library.description = "test_description";
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_CHECK_EQUAL(lib.id(), "test_id");
    BOOST_CHECK_EQUAL(lib.description(), "test_description");
}

// Test library with port types
BOOST_AUTO_TEST_CASE(test_library_port_types_empty_fileds)
{
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ModelParser::PortType portType1{"port1", "flow port", {}};
    Antares::Solver::ModelParser::PortType portType2{"port2", "impedance port", {}};
    library.port_types = {portType1, portType2};
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_REQUIRE_EQUAL(lib.portTypes().size(), 2);
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").id(), "port1");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").description(), "flow port");
    BOOST_CHECK(lib.portTypes().at("port1").fields().empty());
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").id(), "port2");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").description(), "impedance port");
    BOOST_CHECK(lib.portTypes().at("port2").fields().empty());
}
