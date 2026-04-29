// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN
#define BOOST_TEST_MODULE test api structured linear problem

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <antares/exception/RuntimeError.hpp>
#include <antares/optimisation/linear-problem-api/StructuredLinearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>

using namespace Antares::Optimisation::LinearProblemApi;
using Antares::Error::RuntimeError;

BOOST_AUTO_TEST_SUITE(StructuredLinearProblemTests)

BOOST_AUTO_TEST_CASE(create_empty_problem)
{
    StructuredLinearProblem lp;

    BOOST_CHECK_EQUAL(lp.variableCount(), 0);
    BOOST_CHECK_EQUAL(lp.constraintCount(), 0);
    BOOST_CHECK(lp.isLP());
    BOOST_CHECK(lp.isMinimization());
}

BOOST_AUTO_TEST_CASE(add_mixed_variables)
{
    StructuredLinearProblem lp;

    auto* x = lp.addNumVariable(0.0, 10.0, "x");
    auto* y = lp.addIntVariable(0, 5, "y");

    BOOST_CHECK_EQUAL(lp.variableCount(), 2);
    BOOST_CHECK_CLOSE(x->getLb(), 0.0, 1e-12);
    BOOST_CHECK_CLOSE(x->getUb(), 10.0, 1e-12);
    BOOST_CHECK_CLOSE(y->getLb(), 0.0, 1e-12);
    BOOST_CHECK_CLOSE(y->getUb(), 5.0, 1e-12);

    BOOST_CHECK_EQUAL(x->getName(), "x");
    BOOST_CHECK_EQUAL(y->getName(), "y");
    BOOST_CHECK(!x->isInteger());
    BOOST_CHECK(y->isInteger());

    BOOST_CHECK(!lp.isLP());
}

BOOST_AUTO_TEST_CASE(add_constraints)
{
    StructuredLinearProblem lp;
    (void)lp.addNumVariable(0.0, 10.0, "x");
    (void)lp.addNumVariable(0.0, 10.0, "y");

    auto* c1 = lp.addConstraint(2.0, 5.0, "c1");
    (void)lp.addConstraint(-1e20, 3.0, "c2");

    BOOST_CHECK_EQUAL(lp.constraintCount(), 2);
    BOOST_CHECK_CLOSE(c1->getLb(), 2.0, 1e-12);
    BOOST_CHECK_CLOSE(c1->getUb(), 5.0, 1e-12);
    BOOST_CHECK_EQUAL(c1->getName(), "c1");

    BOOST_CHECK_EQUAL(lp.lookupConstraint("c1"), c1);
    BOOST_CHECK_EQUAL(lp.lookupConstraint("nonexistent"), nullptr);
}

BOOST_AUTO_TEST_CASE(set_constraint_coefficients)
{
    StructuredLinearProblem lp;
    auto* x = lp.addNumVariable(0.0, 10.0, "x");
    auto* y = lp.addNumVariable(0.0, 10.0, "y");
    auto* z = lp.addNumVariable(0.0, 10.0, "z");

    auto* c1 = lp.addConstraint(1.0, 10.0, "c1");
    c1->setCoefficient(x, 2.0);
    c1->setCoefficient(y, -1.0);
    c1->setCoefficient(z, 3.0);

    BOOST_CHECK_CLOSE(c1->getCoefficient(x), 2.0, 1e-12);
    BOOST_CHECK_CLOSE(c1->getCoefficient(y), -1.0, 1e-12);
    BOOST_CHECK_CLOSE(c1->getCoefficient(z), 3.0, 1e-12);

    auto coeffs = c1->getCoefficients();
    BOOST_CHECK_EQUAL(coeffs.size(), 3);

    bool foundX = false;
    bool foundY = false;
    bool foundZ = false;
    for (const auto& [varIdx, coeff]: coeffs)
    {
        if (varIdx == 0 && std::abs(coeff - 2.0) < 1e-12)
        {
            foundX = true;
        }
        if (varIdx == 1 && std::abs(coeff - (-1.0)) < 1e-12)
        {
            foundY = true;
        }
        if (varIdx == 2 && std::abs(coeff - 3.0) < 1e-12)
        {
            foundZ = true;
        }
    }
    BOOST_CHECK(foundX);
    BOOST_CHECK(foundY);
    BOOST_CHECK(foundZ);
}

BOOST_AUTO_TEST_CASE(objective_coefficients)
{
    StructuredLinearProblem lp;
    auto* x = lp.addNumVariable(0.0, 10.0, "x");
    auto* y = lp.addNumVariable(0.0, 10.0, "y");

    lp.setObjectiveCoefficient(x, 5.0);
    lp.setObjectiveCoefficient(y, -3.0);

    BOOST_CHECK_CLOSE(lp.getObjectiveCoefficient(x), 5.0, 1e-12);
    BOOST_CHECK_CLOSE(lp.getObjectiveCoefficient(y), -3.0, 1e-12);

    lp.setObjectiveOffset(42.0);
    BOOST_CHECK_CLOSE(lp.getObjectiveOffset(), 42.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(optimization_direction)
{
    StructuredLinearProblem lp;

    BOOST_CHECK(lp.isMinimization());
    BOOST_CHECK(!lp.isMaximization());

    lp.setMaximization();
    BOOST_CHECK(!lp.isMinimization());
    BOOST_CHECK(lp.isMaximization());

    lp.setMinimization();
    BOOST_CHECK(lp.isMinimization());
    BOOST_CHECK(!lp.isMaximization());
}

BOOST_AUTO_TEST_CASE(infinity)
{
    StructuredLinearProblem lp;
    double inf = lp.infinity();

    BOOST_CHECK_GT(inf, 1e19);
    BOOST_CHECK(std::isinf(inf));
}

BOOST_AUTO_TEST_CASE(modify_bounds)
{
    StructuredLinearProblem lp;
    auto* x = lp.addNumVariable(0.0, 10.0, "x");

    BOOST_CHECK_CLOSE(x->getLb(), 0.0, 1e-12);
    BOOST_CHECK_CLOSE(x->getUb(), 10.0, 1e-12);

    x->setLb(1.0);
    x->setUb(20.0);
    BOOST_CHECK_CLOSE(x->getLb(), 1.0, 1e-12);
    BOOST_CHECK_CLOSE(x->getUb(), 20.0, 1e-12);

    x->setBounds(2.0, 15.0);
    BOOST_CHECK_CLOSE(x->getLb(), 2.0, 1e-12);
    BOOST_CHECK_CLOSE(x->getUb(), 15.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(solve_not_supported)
{
    StructuredLinearProblem lp;
    (void)lp.addNumVariable(0.0, 10.0, "x");

    BOOST_CHECK_THROW(lp.solve(false), RuntimeError);
}

BOOST_AUTO_TEST_CASE(solution_value_not_available)
{
    StructuredLinearProblem lp;
    auto* x = lp.addNumVariable(0.0, 10.0, "x");

    BOOST_CHECK_THROW(x->solutionValue(), RuntimeError);
    BOOST_CHECK_THROW(x->reducedCost(), RuntimeError);
}

BOOST_AUTO_TEST_CASE(dual_not_available)
{
    StructuredLinearProblem lp;
    (void)lp.addNumVariable(0.0, 10.0, "x");
    auto* c = lp.addConstraint(1.0, 10.0, "c");

    BOOST_CHECK_THROW(c->dual(), RuntimeError);
}

BOOST_AUTO_TEST_SUITE_END()
