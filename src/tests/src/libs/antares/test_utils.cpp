/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE test utils
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <string>
#include <antares/utils/utils.h>


namespace {

template <class T>
T beautify(const T& in) {
    T out;
    Antares::BeautifyName(out, in);
    return out;
}

constexpr auto beautifyStd = beautify<std::string>;
constexpr auto beautifyYuni = beautify<Yuni::String>;

std::string transformNameToId(const AnyString& name)
{
    std::string res;
    Antares::TransformNameIntoID(name, res);
    return res;
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

BOOST_AUTO_TEST_CASE(test_transform_name_into_id)
{
    BOOST_CHECK(transformNameToId("NA!ME") == "na me");
    BOOST_CHECK(transformNameToId("name!") == "name");
    BOOST_CHECK(transformNameToId("!name") == "name");
}
