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

#include "antares/modeler/optimConfig/optimConfig.h"

BOOST_AUTO_TEST_SUITE(test_optim_config_creation)

using namespace Antares::Modeler::Config;

BOOST_AUTO_TEST_CASE(TestVariable)
{
    Variable var("var1", Location::MASTER);
    BOOST_CHECK(var.id() == "var1");
    BOOST_CHECK(var.location() == Location::MASTER);
}

BOOST_AUTO_TEST_CASE(TestObjective)
{
    Objective obj("obj1", Location::SUBPROBLEMS);
    BOOST_CHECK(obj.id() == "obj1");
    BOOST_CHECK(obj.location() == Location::SUBPROBLEMS);
}

BOOST_AUTO_TEST_CASE(TestModelDecomposition)
{
    std::vector<Variable> variables = {Variable("var1", Location::MASTER)};
    std::vector<Objective> objectives = {Objective("obj1", Location::SUBPROBLEMS)};
    ModelDecomposition md(variables, objectives);

    BOOST_CHECK(md.variables().size() == 1);
    BOOST_CHECK(md.objectives().size() == 1);
    BOOST_CHECK(md.variables()[0].id() == "var1");
    BOOST_CHECK(md.objectives()[0].id() == "obj1");
}

BOOST_AUTO_TEST_CASE(TestModel)
{
    std::vector<Variable> variables = {Variable("var1", Location::MASTER)};
    std::vector<Objective> objectives = {Objective("obj1", Location::SUBPROBLEMS)};
    ModelDecomposition md(variables, objectives);
    Model model("model1", md);

    BOOST_CHECK(model.id() == "model1");
    BOOST_CHECK(model.modelDecomposition().variables().size() == 1);
    BOOST_CHECK(model.modelDecomposition().objectives().size() == 1);
}

BOOST_AUTO_TEST_CASE(TestOptimConfigUniqueIDs)
{
    std::vector<Model> models;
    models.push_back(Model("model1", ModelDecomposition({}, {})));
    models.push_back(Model("model2", ModelDecomposition({}, {})));

    OptimConfig config(models);

    BOOST_CHECK(config.models().size() == 2);
}

BOOST_AUTO_TEST_CASE(TestOptimConfigDuplicateIDs)
{
    std::vector<Model> models;
    models.push_back(Model("model1", ModelDecomposition({}, {})));
    models.push_back(Model("model1", ModelDecomposition({}, {}))); // Duplicate ID

    std::vector<std::string> libraries = {"lib1", "lib2"};
    BOOST_CHECK_THROW(OptimConfig config(models), std::runtime_error);
}
} // namespace Antares::Modeler::Config
