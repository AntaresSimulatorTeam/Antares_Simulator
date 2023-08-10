#define BOOST_TEST_MODULE test utils
#include <boost/test/unit_test.hpp>

#include <string>
#include <antares/utils.h>


namespace {

std::string beautify(const std::string& in) {
    std::string out;
    Antares::BeautifyName(out, in);
    return out;
}
}


BOOST_AUTO_TEST_CASE(test)
{
    BOOST_TEST(beautify("test") == "test");
    BOOST_TEST(beautify("tést") == "t st");
    BOOST_TEST(beautify("tÿst") == "t st");
    BOOST_TEST(std::string("tÿst").size() == 5);
}
