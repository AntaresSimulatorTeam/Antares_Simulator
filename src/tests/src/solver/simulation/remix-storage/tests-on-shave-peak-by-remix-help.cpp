#define BOOST_TEST_MODULE shave peaks by remix help

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

struct FindExchangeFixture
{
};

BOOST_FIXTURE_TEST_SUITE(find_exchange, FindExchangeFixture)

BOOST_AUTO_TEST_CASE(dummy) 
{ BOOST_CHECK(true); }

BOOST_AUTO_TEST_SUITE_END()