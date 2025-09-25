/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#define WIN32_LEAN_AND_MEAN

#define BOOST_TEST_MODULE optim config

#include <boost/test/unit_test.hpp>

#include "antares/solver/modeler/optimConfig/optimConfig.h"

BOOST_AUTO_TEST_SUITE(test_optim_config_builder)

using namespace Antares::Modeler::Config;

BOOST_AUTO_TEST_CASE(WithId)
{
    OptimConfigBuilder builder;
    builder.withId("test_id");
    OptimConfig config = builder.build();
    BOOST_CHECK_EQUAL(config.id, "test_id");
}

BOOST_AUTO_TEST_CASE(WithVariables)
{
    OptimConfigBuilder builder;
    std::vector<Variable> variables = {{"var1", Location::MASTER}, {"var2", Location::SUBPROBLEMS}};
    builder.withVariables(std::move(variables));
    OptimConfig config = builder.build();
    BOOST_CHECK_EQUAL(config.modelDecomposition.variables.size(), 2);
    BOOST_CHECK_EQUAL(config.modelDecomposition.variables[0].id, "var1");
    BOOST_CHECK(config.modelDecomposition.variables[0].location == Location::MASTER);
    BOOST_CHECK_EQUAL(config.modelDecomposition.variables[1].id, "var2");
    BOOST_CHECK(config.modelDecomposition.variables[1].location == Location::SUBPROBLEMS);
}

BOOST_AUTO_TEST_CASE(WithObjectives)
{
    OptimConfigBuilder builder;
    std::vector<Objective> objectives = {{"obj1", Location::MASTER},
                                         {"obj2", Location::MASTER_AND_SUBPROBLEMS}};
    builder.withObjectives(std::move(objectives));
    OptimConfig config = builder.build();
    BOOST_CHECK_EQUAL(config.modelDecomposition.objectives.size(), 2);
    BOOST_CHECK_EQUAL(config.modelDecomposition.objectives[0].id, "obj1");
    BOOST_CHECK(config.modelDecomposition.objectives[0].location == Location::MASTER);
    BOOST_CHECK_EQUAL(config.modelDecomposition.objectives[1].id, "obj2");
    BOOST_CHECK(config.modelDecomposition.objectives[1].location
                == Location::MASTER_AND_SUBPROBLEMS);
}

BOOST_AUTO_TEST_CASE(Build)
{
    OptimConfigBuilder builder;
    builder.withId("test_id");
    std::vector<Variable> variables = {{"var1", Location::MASTER}, {"var2", Location::SUBPROBLEMS}};
    builder.withVariables(std::move(variables));
    std::vector<Objective> objectives = {{"obj1", Location::MASTER},
                                         {"obj2", Location::MASTER_AND_SUBPROBLEMS}};
    builder.withObjectives(std::move(objectives));
    OptimConfig config = builder.build();
    BOOST_CHECK_EQUAL(config.id, "test_id");
    BOOST_CHECK_EQUAL(config.modelDecomposition.variables.size(), 2);
    BOOST_CHECK_EQUAL(config.modelDecomposition.objectives.size(), 2);
}

} // namespace Antares::Modeler::Config
