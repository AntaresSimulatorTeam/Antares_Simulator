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

#include "antares/exception/RuntimeError.hpp"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/portType.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

#include <boost/test/unit_test.hpp>

using namespace Antares::ModelerStudy::SystemModel;

BOOST_AUTO_TEST_SUITE(_ModelerLibrary_)

BOOST_AUTO_TEST_CASE(model_with_same_objects_of_same_id)
{
    ModelBuilder model_builder;

    // Two parameters with same ID
    model_builder.withId("model");
    model_builder.withParameters({Parameter("param1", TimeDependent::NO, ScenarioDependent::NO),
                                  Parameter("param1", TimeDependent::NO, ScenarioDependent::NO)});
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model\" contains multiple objects with ID \"param1\"."));

    // Two variables with same ID
    model_builder.withId("model");
    std::vector<Variable> variables;
    variables.push_back({"var1", {}, {}, ValueType::FLOAT, {}, {}});
    variables.push_back({"var1", {}, {}, ValueType::BOOL, {}, {}});
    model_builder.withVariables(std::move(variables));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model\" contains multiple objects with ID \"var1\"."));

    // Two constraints with same ID
    model_builder.withId("model32");
    std::vector<Constraint> constraints;
    Constraint ct1("ctt", {});
    Constraint ct2("ctt", {});
    constraints.push_back(std::move(ct1));
    constraints.push_back(std::move(ct2));

    model_builder.withConstraints(std::move(constraints));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model32\" contains multiple objects with ID \"ctt\"."));

    // Two ports with same ID
    model_builder.withId("model");
    PortField flow("flow");
    std::vector<PortField> portFields = {flow};
    PortType portType("some-port-type", std::move(portFields));
    Port port1("p", portType);
    Port port2("p", portType);
    model_builder.withPorts({port1, port2});
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage("Model \"model\" contains multiple objects with ID \"p\"."));

    // Two extra outputs with same ID
    model_builder.withId("model312");
    std::vector<ExtraOutput> outputs;
    ExtraOutput eo1("output", {});
    ExtraOutput eo2("output", {});
    outputs.push_back(std::move(eo1));
    outputs.push_back(std::move(eo2));

    model_builder.withExtraOutputs(std::move(outputs));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model312\" contains multiple objects with ID \"output\"."));
}

BOOST_AUTO_TEST_CASE(model_with_different_objects_of_same_id)
{
    ModelBuilder model_builder;

    // Variable with same ID as parameter
    model_builder.withId("model");
    model_builder.withParameters({Parameter("z", TimeDependent::NO, ScenarioDependent::NO)});
    std::vector<Variable> variables;
    variables.push_back({"z", {}, {}, ValueType::FLOAT, {}, {}});
    model_builder.withVariables(std::move(variables));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage("Model \"model\" contains multiple objects with ID \"z\"."));

    // Constraint with same id as parameter
    model_builder.withId("model32");
    model_builder.withParameters({Parameter("x", TimeDependent::NO, ScenarioDependent::NO)});
    std::vector<Constraint> constraints;
    Constraint ct("x", {});
    constraints.push_back(std::move(ct));
    model_builder.withConstraints(std::move(constraints));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model32\" contains multiple objects with ID \"x\"."));
}

BOOST_AUTO_TEST_CASE(model_with_port_ct_same_id)
{
    ModelBuilder model_builder;

    // Port with same ID as constraint
    model_builder.withId("model");
    PortField flow("flow");
    std::vector<PortField> portFields = {flow};
    PortType portType("some-port-type", std::move(portFields));
    Port port("x", portType);
    model_builder.withPorts({port});

    std::vector<Constraint> constraints;
    Constraint ct("x", {});
    constraints.push_back(std::move(ct));
    model_builder.withConstraints(std::move(constraints));

    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage("Model \"model\" contains multiple objects with ID \"x\"."));

    // Extra output with same ID as variable
    model_builder.withId("model__");
    std::vector<Variable> variables2;
    variables2.push_back({"y", {}, {}, ValueType::FLOAT, {}, {}});
    model_builder.withVariables(std::move(variables2));
    std::vector<ExtraOutput> extraOutputs;
    ExtraOutput eo("y", {});
    extraOutputs.push_back(std::move(eo));
    model_builder.withExtraOutputs(std::move(extraOutputs));
    BOOST_CHECK_EXCEPTION(model_builder.build(),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "Model \"model__\" contains multiple objects with ID \"y\"."));
}

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
