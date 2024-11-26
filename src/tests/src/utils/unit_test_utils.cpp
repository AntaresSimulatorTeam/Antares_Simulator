#include "unit_test_utils.h"

#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

std::function<bool(const std::exception&)> checkMessage(std::string expected_message)
{
    return [expected_message](const std::exception& e)
    {
        BOOST_CHECK_EQUAL(e.what(), expected_message);
        return true;
    };
}
