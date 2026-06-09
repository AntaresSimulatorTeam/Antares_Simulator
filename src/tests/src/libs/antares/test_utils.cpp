// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test utils
#include <filesystem>
#include <string>

#include <boost/test/unit_test.hpp>

#include <yuni/io/file.h>

#include <antares/utils/utils.h>

namespace fs = std::filesystem;
namespace tt = boost::test_tools;
using namespace Antares;

BOOST_AUTO_TEST_SUITE(utils)

BOOST_AUTO_TEST_CASE(test_string_conversion)
{
    Yuni::String yuniStr = "hello";
    std::string stdStr = yuniStr;
    BOOST_TEST(stdStr == "hello");

    Yuni::String yuniConvertedBack = stdStr;
    BOOST_TEST(yuniConvertedBack == "hello");
}

BOOST_AUTO_TEST_CASE(test_transform_name_into_id)
{
    BOOST_CHECK(transformNameIntoID("NA!ME") == "na me");
    BOOST_CHECK(transformNameIntoID("name!") == "name");
    BOOST_CHECK(transformNameIntoID("!name") == "name");
}

BOOST_AUTO_TEST_CASE(yuni_absolute_vs_std_absolute)
{
    fs::path pathToFile("abc.txt");

    Yuni::String yuniAbs;
    Yuni::IO::MakeAbsolute(yuniAbs, pathToFile.string());

    BOOST_CHECK(fs::absolute(pathToFile).string() == yuniAbs);
}

BOOST_AUTO_TEST_CASE(yuni_normalize_vs_std_lexically_normal)
{
    const auto helper = [](fs::path&& path)
    {
        Yuni::String yuniNorm;
        Yuni::IO::Normalize(yuniNorm, path.string());
        BOOST_CHECK_MESSAGE(path.lexically_normal().string() == yuniNorm,
                            std::string("Check failed for ") + path.string());
    };
    helper(fs::path("a/./b/.."));
    helper(fs::path("a/.///b/../"));
}

BOOST_AUTO_TEST_CASE(isZero)
{
    BOOST_CHECK(!Utils::isZero(1.e-5));
    BOOST_CHECK(Utils::isZero(1.e-7));
}

BOOST_AUTO_TEST_CASE(round)
{
    BOOST_TEST(Utils::round(2.1, 0) == 2, tt::tolerance(1e-9));
    BOOST_TEST(Utils::round(2.1, 1) == 2.1, tt::tolerance(1e-9));
    BOOST_TEST(Utils::round(2.1, 2) == 2.1, tt::tolerance(1e-9));
}

BOOST_AUTO_TEST_CASE(ceil)
{
    BOOST_TEST(Utils::ceil(2.4) == 3, tt::tolerance(1e-9));
    BOOST_TEST(Utils::ceil(2.4 / 10) == 1, tt::tolerance(1e-9));

    BOOST_TEST(Utils::ceil(2.6) == 3, tt::tolerance(1e-9));
    BOOST_TEST(Utils::ceil(2.6 / 10) == 1, tt::tolerance(1e-9));
}

BOOST_AUTO_TEST_CASE(floor)
{
    BOOST_TEST(Utils::floor(2.4) == 2, tt::tolerance(1e-9));
    BOOST_TEST(Utils::floor(2.4 / 10) == 0, tt::tolerance(1e-9));

    BOOST_TEST(Utils::floor(2.6) == 2, tt::tolerance(1e-9));
    BOOST_TEST(Utils::floor(2.6 / 10) == 0, tt::tolerance(1e-9));
}

BOOST_AUTO_TEST_CASE(all_one_OK)
{
    std::vector<std::pair<unsigned, std::string>> list = {{1, ""}, {1, ""}, {1, ""}};
    BOOST_CHECK(Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_identical_values_OK)
{
    std::vector<std::pair<unsigned, std::string>> list = {{4, ""}, {4, ""}, {4, ""}, {4, ""}};
    BOOST_CHECK(Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_identical_values_and_one_OK)
{
    std::vector<std::pair<unsigned, std::string>> list = {{4, ""}, {4, ""}, {4, ""}, {1, ""}};
    BOOST_CHECK(Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_one_and_identical_values_OK)
{
    std::vector<std::pair<unsigned, std::string>> list = {{1, ""}, {4, ""}, {4, ""}, {4, ""}};
    BOOST_CHECK(Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_two_distinct_values_of_which_one_KO)
{
    std::vector<std::pair<unsigned, std::string>> list = {{1, ""},
                                                          {2, ""},
                                                          {1, ""},
                                                          {2, ""},
                                                          {1, ""},
                                                          {3, ""}};
    BOOST_CHECK(!Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_three_distinct_values_KO)
{
    std::vector<std::pair<unsigned, std::string>> list = {{1, ""},
                                                          {2, ""},
                                                          {1, ""},
                                                          {3, ""},
                                                          {2, ""},
                                                          {1, ""}};
    BOOST_CHECK(!Utils::checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(split_string_ts_cluster_gen)
{
    char delimiter1 = ';';
    char delimiter2 = '.';

    using stringPair = std::pair<std::string, std::string>;
    std::vector<stringPair> v;

    // only one pair of area cluster
    v = splitStringIntoPairs("abc.def", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));

    // two pairs
    v = splitStringIntoPairs("abc.def;ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));
    BOOST_CHECK(v[1] == stringPair("ghi", "jkl"));

    // first pair isn't valid
    v = splitStringIntoPairs("abcdef;ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("ghi", "jkl"));

    // second pair isn't valid
    v = splitStringIntoPairs("abc.def;ghijkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));

    // no semi colon
    v = splitStringIntoPairs("abc.def.ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def.ghi.jkl"));

    // no separator
    v.clear();
    v = splitStringIntoPairs("abcdef", delimiter1, delimiter2);
    BOOST_CHECK(v.empty());
}

BOOST_AUTO_TEST_CASE(path_is_empty___path_is_valid)
{
    fs::path path;
    BOOST_CHECK(Utils::isPathValid(path.string()));
}

BOOST_AUTO_TEST_CASE(path_has_only_ascii_chars___path_is_valid)
{
    fs::path path("path-with-only-ascii-chars");
    BOOST_CHECK(Utils::isPathValid(path.string()));
}

BOOST_AUTO_TEST_CASE(on_Win_path_containing_non_ascii_chars_not_valid__path_valid_otherwise)
{
    fs::path path("hé je suis français");
#if defined(_WIN32)
    BOOST_CHECK(!Utils::isPathValid(path.string()));
#else
    BOOST_CHECK(Utils::isPathValid(path.string()));
#endif
}

BOOST_AUTO_TEST_SUITE_END()
