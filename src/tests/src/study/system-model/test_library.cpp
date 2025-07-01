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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/study/system-model/portType.h"

using namespace Antares::ModelerStudy::SystemModel;

BOOST_AUTO_TEST_SUITE(_ModelerLibrary_)

BOOST_AUTO_TEST_CASE(port_type_basic)
{
    PortField field1("field1");
    PortField field2("field2");
    std::vector fields = {field1, field2};
    PortType portType("myId", std::move(fields), "");
    BOOST_CHECK_EQUAL(portType.Id(), "myId");
    BOOST_REQUIRE_EQUAL(portType.Fields().size(), 2);
    BOOST_CHECK_EQUAL(portType.Fields()[0].Id(), "field1");
    BOOST_CHECK_EQUAL(portType.Fields()[1].Id(), "field2");
    BOOST_CHECK_EQUAL(portType.AreaConnectionFieldId().has_value(), false);
}

BOOST_AUTO_TEST_CASE(port_type_with_area_connection)
{
    PortField field1("firstField");
    PortField field2("secondField");
    std::vector fields = {field1, field2};
    PortType portType("portTypeId", std::move(fields), "secondField");
    BOOST_CHECK_EQUAL(portType.Id(), "portTypeId");
    BOOST_REQUIRE_EQUAL(portType.Fields().size(), 2);
    BOOST_CHECK_EQUAL(portType.Fields()[0].Id(), "firstField");
    BOOST_CHECK_EQUAL(portType.Fields()[1].Id(), "secondField");
    BOOST_CHECK_EQUAL(portType.AreaConnectionFieldId().has_value(), true);
    BOOST_CHECK_EQUAL(portType.AreaConnectionFieldId().value(), "secondField");
}

BOOST_AUTO_TEST_CASE(port_type_with_area_connection_error)
{
    auto shouldThrow = []
    {
        PortField field1("firstField");
        std::vector fields = {field1};
        return PortType("portTypeId", std::move(fields), "secondField");
    };
    BOOST_CHECK_EXCEPTION(shouldThrow(),
                          std::invalid_argument,
                          checkMessage("Field \"secondField\" selected for area connections was "
                                       "not defined in PortType \"portTypeId\"."));
}

BOOST_AUTO_TEST_SUITE_END()
