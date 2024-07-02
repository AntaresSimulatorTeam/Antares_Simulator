/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE test utils
#include <filesystem>
#include <string>

#include <boost/test/unit_test.hpp>

#include <yuni/io/file.h>

#include <antares/utils/utils.h>

namespace fs = std::filesystem;

namespace
{

template<class T>
T beautify(const T& in)
{
    T out;
    Antares::BeautifyName(out, in);
    return out;
}

constexpr auto beautifyStd = beautify<std::string>;
constexpr auto beautifyYuni = beautify<Yuni::String>;

} // namespace

BOOST_AUTO_TEST_SUITE(utils)

BOOST_AUTO_TEST_CASE(test_beautify_name_std)
{
    // Just checking that both std and yuni strings have similar behaviours with UTF8 chars...
    BOOST_TEST(std::string("tÿst").size() == 5);
    BOOST_TEST(Yuni::String("tÿst").size() == 5);

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

BOOST_AUTO_TEST_CASE(test_transform_name_into_id)
{
    BOOST_CHECK(Antares::transformNameIntoID("NA!ME") == "na me");
    BOOST_CHECK(Antares::transformNameIntoID("name!") == "name");
    BOOST_CHECK(Antares::transformNameIntoID("!name") == "name");
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

BOOST_AUTO_TEST_SUITE_END()
