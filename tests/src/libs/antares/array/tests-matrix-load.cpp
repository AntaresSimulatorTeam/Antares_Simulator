#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include<tests-matrix-load.h>

namespace utf = boost::unit_test;


// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(test_suite_1)

BOOST_AUTO_TEST_CASE(test_1)
{
	BOOST_REQUIRE_EQUAL(true, true);
}

BOOST_AUTO_TEST_SUITE_END()


