// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/utils/basis_status.h>

#include "basis_status_impl.h"
#include "ortools/linear_solver/linear_solver.h"

using namespace operations_research;

namespace Test
{
class BasisStatus
{
public:
    BasisStatus(const Antares::Optimization::BasisStatus& b):
        StatutDesVariables(b.impl->StatutDesVariables),
        StatutDesContraintes(b.impl->StatutDesContraintes)
    {
    }

    using Status = operations_research::MPSolver::BasisStatus;
    const std::vector<Status>& StatutDesVariables;
    const std::vector<Status>& StatutDesContraintes;
};
} // namespace Test

BOOST_AUTO_TEST_CASE(basisStatusExtract)
{
    // CLP_LINEAR_PROGRAMMING should be always available
    MPSolver solver("foo", MPSolver::CLP_LINEAR_PROGRAMMING);
    auto x = solver.MakeNumVar(0, 1, "x");
    auto c = solver.MakeRowConstraint(0, 2, "c");
    auto obj = solver.MutableObjective();
    obj->SetCoefficient(x, 1);
    c->SetCoefficient(x, 1);
    Antares::Optimization::BasisStatus status;
    solver.Solve();
    status.extractBasis(&solver);

    // Variables
    Test::BasisStatus test(status);
    BOOST_CHECK_EQUAL(test.StatutDesVariables.size(), 1);
    BOOST_CHECK_EQUAL(test.StatutDesVariables[0], MPSolver::AT_LOWER_BOUND);

    // Constraints
    BOOST_CHECK_EQUAL(test.StatutDesContraintes.size(), 1);
    BOOST_CHECK_EQUAL(test.StatutDesContraintes[0], MPSolver::BASIC);
}
