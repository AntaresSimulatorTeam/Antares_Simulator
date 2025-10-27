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

#include <boost/test/unit_test.hpp>

#include <antares/exception/RuntimeError.hpp>
#include <antares/io/inputs/yml-optim-config/converter.h>
#include <antares/modeler/optimConfig/optimConfig.h>

namespace IOYML = Antares::IO::Inputs::YmlOptimConfig;

namespace Antares::Modeler::Config
{
inline std::ostream& operator<<(std::ostream& os, Location loc)
{
    switch (loc)
    {
    case Location::MASTER:
        return os << "MASTER";
    case Location::MASTER_AND_SUBPROBLEMS:
        return os << "MASTER_AND_SUBPROBLEMS";
    case Location::SUBPROBLEMS:
        return os << "SUBPROBLEMS";
    }
    throw Error::RuntimeError("Unknown Location enum value");
}
} // namespace Antares::Modeler::Config

BOOST_AUTO_TEST_SUITE(OptimConfigConverterSuite)

BOOST_AUTO_TEST_CASE(ConvertEmptyConfig)
{
    IOYML::OptimConfig ymlConfig;
    Antares::Modeler::Config::OptimConfig converted = IOYML::OptimConfigConverter::convert(
      ymlConfig);
    BOOST_CHECK_EQUAL(converted.models().size(), 0);
}

BOOST_AUTO_TEST_CASE(ConvertSingleModel)
{
    IOYML::OptimConfig ymlConfig = {
      {IOYML::Model{"model1",
                    {IOYML::Variable{"var1", "master"}, IOYML::Variable{"var2", "subproblems"}},
                    {IOYML::Objective{"obj1", "master_and_subproblems"}}}}};

    Antares::Modeler::Config::OptimConfig converted = IOYML::OptimConfigConverter::convert(
      ymlConfig);
    BOOST_CHECK_EQUAL(converted.models().size(), 1);

    const auto& model = converted.models()[0];
    BOOST_CHECK_EQUAL(model.id(), "model1");

    const auto& variables = model.modelDecomposition().variables();
    BOOST_CHECK_EQUAL(variables.size(), 2);
    BOOST_CHECK_EQUAL(variables[0].id(), "var1");
    BOOST_CHECK_EQUAL(variables[0].location(), Antares::Modeler::Config::Location::MASTER);
    BOOST_CHECK_EQUAL(variables[1].id(), "var2");
    BOOST_CHECK_EQUAL(variables[1].location(), Antares::Modeler::Config::Location::SUBPROBLEMS);

    const auto& objectives = model.modelDecomposition().objectives();
    BOOST_CHECK_EQUAL(objectives.size(), 1);
    BOOST_CHECK_EQUAL(objectives[0].id(), "obj1");
    BOOST_CHECK_EQUAL(objectives[0].location(),
                      Antares::Modeler::Config::Location::MASTER_AND_SUBPROBLEMS);
}

BOOST_AUTO_TEST_CASE(ConvertMultipleModels)
{
    IOYML::OptimConfig ymlConfig = {
      {IOYML::Model{"model1",
                    {IOYML::Variable{"var1", "master"}},
                    {IOYML::Objective{"obj1", "master"}}},
       IOYML::Model{"model2",
                    {IOYML::Variable{"var2", "subproblems"}},
                    {IOYML::Objective{"obj2", "master_and_subproblems"}}}}};

    Antares::Modeler::Config::OptimConfig converted = IOYML::OptimConfigConverter::convert(
      ymlConfig);
    BOOST_CHECK_EQUAL(converted.models().size(), 2);

    BOOST_CHECK_EQUAL(converted.models()[0].id(), "model1");
    BOOST_CHECK_EQUAL(converted.models()[1].id(), "model2");
}

BOOST_AUTO_TEST_CASE(ConvertInvalidLocation)
{
    IOYML::OptimConfig ymlConfig = {
      {IOYML::Model{"model1", {IOYML::Variable{"var1", "invalid_location"}}, {}}}};

    BOOST_CHECK_THROW(IOYML::OptimConfigConverter::convert(ymlConfig), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
