/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#define BOOST_TEST_MODULE "making groups of hours from a string"

#define WIN32_LEAN_AND_MEAN
//
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"
//
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

using namespace Antares::Data::ShortTermStorage;

BOOST_AUTO_TEST_SUITE(test_of_making_groups_of_hours_from_a_string)

BOOST_AUTO_TEST_CASE(first_quite_simple_hours_field)
{
    std::string hoursField = "[1, 2], [3, 4]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{1, 2}, {3, 4}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_inside_brackets__after_a_comma_separator)
{
    std::string hoursField = "[5,   1]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{5, 1}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_right_before_opening_bracket)
{
    std::string hoursField = "[  7, 6]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{7, 6}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_right_before_closing_bracket)
{
    std::string hoursField = "[9, 0  ]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{9, 0}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_between_groups)
{
    std::string hoursField = "[5,1] ,  [8,7]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{5, 1}, {8, 7}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_before_a_group)
{
    std::string hoursField = "  [5,1], [8,7]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{5, 1}, {8, 7}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_has_spaces_everywhere)
{
    std::string hoursField = " [  5,   1] , [ 8, 7   ]  ";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{5, 1}, {8, 7}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(hours_field_contains_groups_of_different_sizes)
{
    std::string hoursField = "[5, 1, 98], [8, 79, 54, 32], [109]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {{5, 1, 98}, {8, 79, 54, 32}, {109}};
    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(
  hours_field_contains_unwnated_chars_outside_between_brackets___exception_raised)
{
    std::string hoursField = "[5, 1, 23] {I'm not wanted} [4, 19]";
    BOOST_CHECK_THROW(makeGroupsOfHours(hoursField), ShortTermStorageAdditionalConstraintsError);
}

BOOST_AUTO_TEST_CASE(hours_field_with_group_contains_an_intruder_char__exception_raised)
{
    // 'I' is the intruder
    std::string hoursField = "[5, 4, I 19]";

    BOOST_CHECK_THROW(makeGroupsOfHours(hoursField), ShortTermStorageAdditionalConstraintsError);
}

BOOST_AUTO_TEST_CASE(hours_field_where_2_brackets_are_opened__exception_raised)
{
    std::string hoursField = "[5, 1, 23, [4, 19]";

    BOOST_CHECK_THROW(makeGroupsOfHours(hoursField), ShortTermStorageAdditionalConstraintsError);
}

BOOST_AUTO_TEST_CASE(hours_field_where_2_brackets_are_closed__exception_raised)
{
    std::string hoursField = "[5, 1, 23] 4, 19]";

    BOOST_CHECK_THROW(makeGroupsOfHours(hoursField), ShortTermStorageAdditionalConstraintsError);
}

BOOST_AUTO_TEST_CASE(large_hours_field___can_happen_in_real_life)
{
    std::string hoursField;
    hoursField += "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, "
                  "23, 24], ";
    hoursField += "[25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, "
                  "44, 45, 46, 47, 48], ";
    hoursField += "[49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, "
                  "68, 69, 70, 71, 72], ";
    hoursField += "[73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, "
                  "92, 93, 94, 95, 96], ";
    hoursField += "[97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, "
                  "113, 114, 115, 116, 117, 118, 119, 120]";

    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector<std::set<int>> expected_result = {
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24},
      {25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
       37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48},
      {49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
       61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72},
      {73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
       85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96},
      {97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108,
       109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}};

    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_AUTO_TEST_CASE(very_large_hours_field___can_happen_in_real_life)
{
    std::string hoursField;
    int i = 0;
    std::set<int> expected_rhs;
    hoursField += "[" + std::to_string(i);
    expected_rhs.insert(i);
    while (i < 100000)
    {
        hoursField += "," + std::to_string(i);
        expected_rhs.insert(i);
        ++i;
    }
    hoursField += "]";
    const auto groupsOfHours = makeGroupsOfHours(hoursField);

    const std::vector expected_result = {expected_rhs};

    BOOST_CHECK(groupsOfHours == expected_result);
}

BOOST_DATA_TEST_CASE(hours_field_has_more_invalid_format,
                     boost::unit_test::data::make({"",
                                                   "[]",
                                                   "[ ]",
                                                   "[\t]",
                                                   "[\r]",
                                                   "[\f]",
                                                   "[\v]",
                                                   "[1, nol]",
                                                   "[; 3,2,1]",
                                                   "[1, 12345678901]",
                                                   "[1, 12345",
                                                   "1]",
                                                   "[1,]",
                                                   "[1,,2]",
                                                   "[a]",
                                                   "[1, 2], , [3]",
                                                   "[1 2 3  , 11      3]"}),
                     string_value)
{
    std::string hoursField = string_value;
    BOOST_CHECK_THROW(makeGroupsOfHours(hoursField), ShortTermStorageAdditionalConstraintsError);
}

BOOST_DATA_TEST_CASE(hours_field_has_more_valid_format,
                     boost::unit_test::data::make({"[1],[1],[3,2,1]",
                                                   "[\r1,\t2]",
                                                   "[\v1\f,\t2],\f\v\t[4]",
                                                   "[\f\v1]\t\t",
                                                   "\t\v\t[1   ],    [   1,      2,3]    ",
                                                   "          [4,5          ]"}),
                     string_value)
{
    std::string hoursField = string_value;
    BOOST_CHECK_NO_THROW(makeGroupsOfHours(hoursField));
}

BOOST_AUTO_TEST_SUITE_END()
