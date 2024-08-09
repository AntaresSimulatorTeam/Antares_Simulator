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
#define BOOST_TEST_MODULE test optim api ortools

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/optim/ortoolsImpl/linearProblem.h>

#include <ortools/linear_solver/linear_solver.h>

BOOST_AUTO_TEST_SUITE(optim_api)

using namespace Antares::Solver::Optim;

BOOST_AUTO_TEST_CASE(basicLinearProblemAdd)
{
    auto pb = std::make_shared<OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");
    pb->addIntVariable(0, 1, "a");
    pb->addNumVariable(0, 1, "b");

    pb->addConstraint(0, 1, "c");
}

BOOST_AUTO_TEST_CASE(basicLinearProblemGet)
{
    auto pb = std::make_shared<OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");
    pb->addVariable(0, 1, true, "a");

    auto var = pb->getVariable("a");
}

BOOST_AUTO_TEST_SUITE_END()
