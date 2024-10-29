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

#include "antares/solver/expressions/Registry.hxx"
#include "antares/solver/expressions/visitors/AstDOTStyleVisitor.h"
#include "antares/solver/libObjectModel/model.h"
#include "antares/solver/modelConverter/convertorVisitor.h"

using namespace Antares::Solver;

/* BOOST_AUTO_TEST_CASE(empty_expression) */
/* { */
/*     ObjectModel::Model model; */
/*     Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry; */

/*     std::string expression = ""; */

/*     auto* node = ModelConverter::convertExpressionToNode(expression, registry, model); */
/* } */

BOOST_AUTO_TEST_CASE(simple_expression)
{
    ObjectModel::Model model;
    Antares::Solver::Registry<Nodes::Node> registry;

    std::string expression = "1 * 2";
    auto* nodeMult = ModelConverter::convertExpressionToNode(expression, registry, model);

    expression = "6 / 3";
    auto* nodeDiv = ModelConverter::convertExpressionToNode(expression, registry, model);

    std::ofstream out("/tmp/abc.dot");
    Visitors::AstDOTStyleVisitor dot;
    dot(out, nodeDiv);
}
