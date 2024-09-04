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

using namespace Antares::Solver::Optim;

struct Fixture
{
    ~Fixture() = default;

    Fixture()
    {
        pb = std::make_unique<OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");
    }

    std::unique_ptr<OrtoolsImpl::OrtoolsLinearProblem> pb;

    void createProblemMaximize();
};

void Fixture::createProblemMaximize()
{
    auto* var1 = pb->addNumVariable(0, 1, "var1");
    auto* var2 = pb->addNumVariable(0, 1, "var2");
    auto* constraint = pb->addConstraint(1, 1, "constraint");

    constraint->setCoefficient(var1, 1);
    constraint->setCoefficient(var2, 1);

    pb->setObjectiveCoefficient(var1, 1);
    pb->setObjectiveCoefficient(var1, 1);
    pb->setMaximization();
}

BOOST_AUTO_TEST_SUITE(optim_api)

BOOST_AUTO_TEST_CASE(solverMip)
{
    auto pb = std::make_unique<OrtoolsImpl::OrtoolsLinearProblem>(true, "sirius");
}

BOOST_FIXTURE_TEST_CASE(basicLinearProblemAdd, Fixture)
{
    pb->addIntVariable(0, 1, "var1");
    pb->addNumVariable(0, 1, "var2");

    pb->addConstraint(0, 1, "constraint");
}

BOOST_FIXTURE_TEST_CASE(linearProblemGetAndConstraintSetCoeff, Fixture)
{
    pb->addVariable(0, 1, true, "var");
    pb->addConstraint(0, 1, "constraint");

    auto* var = pb->getVariable("var");
    auto* cons = pb->getConstraint("constraint");

    BOOST_CHECK_EQUAL(var->getName(), "var");
    BOOST_CHECK_EQUAL(cons->getName(), "constraint");

    cons->setCoefficient(var, 3.2);
    BOOST_CHECK_EQUAL(cons->getCoefficient(var), 3.2);

    BOOST_CHECK_THROW(cons->setCoefficient(nullptr, 0), std::bad_cast);
    BOOST_CHECK_THROW(cons->getCoefficient(nullptr), std::bad_cast);
}

bool correctMessage(const std::exception& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), std::string("Element name already exists in linear problem"));
    return true;
}

BOOST_FIXTURE_TEST_CASE(nameOrConstraintAlreadyExists, Fixture)
{
    pb->addNumVariable(0, 1, "var");
    BOOST_CHECK_EXCEPTION(pb->addNumVariable(0, 1, "var"), std::exception, correctMessage);

    pb->addConstraint(0, 1, "constraint");
    BOOST_CHECK_EXCEPTION(pb->addConstraint(0, 1, "constraint"), std::exception, correctMessage);
}

BOOST_FIXTURE_TEST_CASE(getVarOrConstraintDoesntExist, Fixture)
{
    BOOST_CHECK_THROW(pb->getVariable(""), std::out_of_range);
    BOOST_CHECK_THROW(pb->getConstraint(""), std::out_of_range);
}

BOOST_FIXTURE_TEST_CASE(maximizeMinimize, Fixture)
{
    pb->setMinimization();
    BOOST_CHECK(pb->isMinimization());

    pb->setMaximization();
    BOOST_CHECK(pb->isMaximization());
}

BOOST_FIXTURE_TEST_CASE(mipVariableBounds, Fixture)
{
    auto* var = pb->addNumVariable(0, 1, "var");

    var->setLb(-4);
    var->setUb(7);

    BOOST_CHECK_EQUAL(var->getLb(), -4);
    BOOST_CHECK_EQUAL(var->getUb(), 7);

    var->setBounds(2, 13);

    BOOST_CHECK_EQUAL(var->getLb(), 2);
    BOOST_CHECK_EQUAL(var->getUb(), 13);

    auto* sameVar = pb->getVariable("var");
    BOOST_CHECK_EQUAL(sameVar->getLb(), 2);
    BOOST_CHECK_EQUAL(sameVar->getUb(), 13);
}

BOOST_FIXTURE_TEST_CASE(mipConstraintBounds, Fixture)
{
    auto* constraint = pb->addConstraint(0, 1, "var");

    constraint->setLb(-4);
    constraint->setUb(7);

    BOOST_CHECK_EQUAL(constraint->getLb(), -4);
    BOOST_CHECK_EQUAL(constraint->getUb(), 7);

    constraint->setBounds(2, 13);

    BOOST_CHECK_EQUAL(constraint->getLb(), 2);
    BOOST_CHECK_EQUAL(constraint->getUb(), 13);
}

BOOST_FIXTURE_TEST_CASE(objectiveCoeff, Fixture)
{
    auto* var = pb->addVariable(0, 1, true, "var");
    pb->setObjectiveCoefficient(var, 1);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 1);

    BOOST_CHECK_THROW(pb->getObjectiveCoefficient(nullptr), std::bad_cast);
    BOOST_CHECK_THROW(pb->setObjectiveCoefficient(nullptr, 0), std::bad_cast);
}

BOOST_FIXTURE_TEST_CASE(infeasibleProblem, Fixture)
{
    auto* var1 = pb->addIntVariable(0, 10, "var1");
    auto* var2 = pb->addNumVariable(0, 10, "var2");
    auto* constraint = pb->addConstraint(1, 1, "constraint");

    constraint->setCoefficient(var1, 1);
    constraint->setCoefficient(var2, 1);

    pb->setObjectiveCoefficient(var1, 1);
    pb->setObjectiveCoefficient(var2, 1);
    pb->setMinimization();

    auto* solution = pb->solve(true);

    BOOST_CHECK(solution->getStatus() == Api::MipStatus::MIP_ERROR);

    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 0);
    BOOST_CHECK_EQUAL(solution->getOptimalValue(var1), 0);
}

BOOST_FIXTURE_TEST_CASE(problemMaximize, Fixture)
{
    createProblemMaximize();
    auto* solution = pb->solve(false);

    BOOST_CHECK(solution->getStatus() == Api::MipStatus::OPTIMAL);

    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 1);
}

BOOST_FIXTURE_TEST_CASE(solutionOpimalValues, Fixture)
{
    createProblemMaximize();
    auto* solution = pb->solve(true);

    auto* var1 = pb->getVariable("var1");
    BOOST_CHECK_EQUAL(solution->getOptimalValue(var1), 1);

    auto* var2 = pb->getVariable("var2");

    std::vector<Api::IMipVariable*> v = {var1, var2};
    auto res = solution->getOptimalValues(v);

    auto* varNotInSolution = pb->addNumVariable(0, 1, "f");
    BOOST_CHECK_EQUAL(solution->getOptimalValue(varNotInSolution), 0);

    BOOST_CHECK_EQUAL(solution->getOptimalValue(nullptr), 0);
}

BOOST_AUTO_TEST_CASE(simpleProblem)
{
    auto problem = std::make_unique<OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");

    auto* P1 = problem->addNumVariable(0, 80, "P1");
    auto* P2 = problem->addNumVariable(0, 200, "P2");

    auto* EOD = problem->addConstraint(100, 100, "EOD");
    EOD->setCoefficient(P1, 1);
    EOD->setCoefficient(P2, 1);

    problem->setObjectiveCoefficient(P1, 10);
    problem->setObjectiveCoefficient(P2, 20);

    problem->setMinimization();

    auto* solution = problem->solve(true);

    std::cout << "Optimal value for P1: " << solution->getOptimalValue(P1) << std::endl;
    std::cout << "Optimal value for P2: " << solution->getOptimalValue(P2) << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
