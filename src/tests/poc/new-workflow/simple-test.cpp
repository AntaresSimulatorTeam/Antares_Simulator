#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

#include "antares/optim/api/LinearProblemBuilder.h"
#include "antares/optim/impl/LinearProblemImpl.h"
#include "../include/Battery.h"
#include "../include/Thermal.h"

using namespace Antares::optim::api;

BOOST_FIXTURE_TEST_SUITE(TEST_NEW_WORKFLOW)

BOOST_AUTO_TEST_CASE(milp_two_mc_single_unit_single_scenario)
{

    vector<int> timesteps{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int timestep = 60;

    LinearProblemImpl linearProblem(true, "xpress");
    LinearProblemBuilder linearProblemBuilder(&linearProblem);
    Battery battery(timesteps, timestep, 100, 1000, 500);

    vector<double> pCost{1, 1, 1, 5, 5, 8, 8, 1, 1, 1};
    Thermal thermal(...);
    //Eod eod(...);
    //Objectif objectif(...);

    //linearProblemBuilder.addFiller(&battery);
    //linearProblemBuilder.addFiller(&thermal);
    //linearProblemBuilder.addFiller(&eod);
    //linearProblemBuilder.addFiller(&objectif);

    linearProblemBuilder.build(nullptr);
    auto solution = linearProblemBuilder.solve();
}