#define BOOST_TEST_MODULE test utils
#include <boost/test/unit_test.hpp>

#include <string>
#include <antares/utils.h>


namespace {

std::string beautifyStd(const std::string& in) {
    std::string out;
    Antares::BeautifyName(out, in);
    return out;
}

Yuni::String beautifyYuni(const Yuni::String in) {
    Yuni::String out;
    Antares::BeautifyName(out, in);
    return out;
}
}

BOOST_AUTO_TEST_CASE(test_beautify_name_std)
{
    //Just checking that both std and yuni strings have similar behaviours with UTF8 chars...
    BOOST_TEST(std::string("tÿst").size() == 5);
    BOOST_TEST(Yuni::String ("tÿst").size() == 5);

    BOOST_TEST(beautifyStd("test") == "test");
    BOOST_TEST(beautifyStd("tést") == "t st");
    BOOST_TEST(beautifyStd("tÿst") == "t st");
}

BOOST_AUTO_TEST_CASE(test_beautify_name_yuni)
{
    BOOST_TEST(beautifyYuni("test") == "test");
    BOOST_TEST(beautifyYuni("tést") == "t st");
    BOOST_TEST(beautifyYuni("tÿst") == "t st");
}

BOOST_AUTO_TEST_CASE(test_string_conversion)
{
    Yuni::String yuniStr = "hello";
    std::string stdStr = yuniStr;
    BOOST_TEST(stdStr == "hello");

    Yuni::String yuniConvertedBack = stdStr;
    BOOST_TEST(yuniConvertedBack == "hello");
}
