#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

#include "optim/api/LinearProblemBuilder.h"

using namespace Antares::optim::api;

BOOST_FIXTURE_TEST_SUITE(TEST_NEW_WORKFLOW)

BOOST_AUTO_TEST_CASE(milp_two_mc_single_unit_single_scenario)
{

}