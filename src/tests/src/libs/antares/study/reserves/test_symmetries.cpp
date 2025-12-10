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
#define BOOST_TEST_MODULE "making groups symmetries from a string"

#define WIN32_LEAN_AND_MEAN
//
#include "antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h"
//
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

using namespace Antares::Data::Symmetries;

BOOST_AUTO_TEST_SUITE(test_of_making_groups_of_symmetries_from_a_string)

BOOST_AUTO_TEST_CASE(_1_symmetry_2_reserves)
{
    std::string symmetryField = "[res_1_up, res_1_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(_2_symmetries_2_reserves)
{
    std::string symmetryField = "[res_1_up, res_1_down],[res_2_up, res_2_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"},
                                                                {"res_2_up", "res_2_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(_symmetries_different_sizes)
{
    std::string symmetryField = "[res_0,res_1],[res_1_up, res_1_down, res_1_bis],[res_2_up, "
                                "res_2_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_0", "res_1"},
                                                                {"res_1_up",
                                                                 "res_1_down",
                                                                 "res_1_bis"},
                                                                {"res_2_up", "res_2_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_inside_brackets_after_a_comma_separator)
{
    std::string symmetryField = "[res_1_up,     res_1_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_right_before_opening_bracket)
{
    std::string symmetryField = "[    res_1_up, res_1_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_right_before_closing_bracket)
{
    std::string symmetryField = "[res_1_up, res_1_down     ]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_between_groups)
{
    std::string symmetryField = "[res_1_up, res_1_down],  [res_2_up, res_2_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"},
                                                                {"res_2_up", "res_2_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_before_a_group)
{
    std::string symmetryField = "  [res_1_up, res_1_down], [res_2_up, res_2_down]";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"},
                                                                {"res_2_up", "res_2_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_has_spaces_everywhere)
{
    std::string symmetryField = "   [ res_1_up, res_1_down  ]  , [ res_2_up   , res_2_down ]   ";

    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up", "res_1_down"},
                                                                {"res_2_up", "res_2_down"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(symmetry_field_contains_only_1_reserve_accepted_grammar)
{
    std::string symmetryField = "[res_1_up]";
    const auto groupsOfSymmetries = makeGroupsOfSymmetries(symmetryField);

    const std::vector<std::set<std::string>> expected_result = {{"res_1_up"}};
    BOOST_CHECK(groupsOfSymmetries == expected_result);
}

BOOST_AUTO_TEST_CASE(
  symmetry_field_contains_unwnated_chars_outside_between_brackets___exception_raised)
{
    std::string symmetryField = "[res_1_up, res_1_down] {I'm not wanted} [res_2_up, res_2_down]";

    BOOST_CHECK_THROW(makeGroupsOfSymmetries(symmetryField), SymmetriesError);
}

BOOST_AUTO_TEST_CASE(
  symmetry_field_contains_contains_an_intruder_string_with_spaces__exception_raised)
{
    std::string symmetryField = "[res_1_up, res_1_down, is this legal]";

    BOOST_CHECK_THROW(makeGroupsOfSymmetries(symmetryField), SymmetriesError);
}

BOOST_AUTO_TEST_CASE(symmetry_field_where_2_brackets_are_opened__exception_raised)
{
    std::string symmetryField = "[res_1_up, res_1_down, [res_2_up, res_2_down,]";

    BOOST_CHECK_THROW(makeGroupsOfSymmetries(symmetryField), SymmetriesError);
}

BOOST_AUTO_TEST_CASE(symmetry_field_where_2_brackets_are_closed__exception_raised)
{
    std::string symmetryField = "[res_1_up, res_1_down] res_2_up, res_2_down,]";

    BOOST_CHECK_THROW(makeGroupsOfSymmetries(symmetryField), SymmetriesError);
}

BOOST_DATA_TEST_CASE(symmetry_field_has_more_invalid_format,
                     boost::unit_test::data::make({"",
                                                   "[]",
                                                   "[ ]",
                                                   "[\t]",
                                                   "[\r]",
                                                   "[\f]",
                                                   "[\v]",
                                                   "[; 3,2,1]",
                                                   "1]",
                                                   "[1,]",
                                                   "[1,,2]",
                                                   "[1, 2], , [3]",
                                                   "[1 2 3  , 11      3]"}),
                     string_value)
{
    std::string symmetryField = string_value;
    BOOST_CHECK_THROW(makeGroupsOfSymmetries(symmetryField), SymmetriesError);
}

BOOST_DATA_TEST_CASE(symmetry_field_has_more_valid_format,
                     boost::unit_test::data::make({"[1,2],[1,2],[3,2,1]",
                                                   "[\r1,\t2]",
                                                   "[\v1\f,\t2],\f\v\t[4]",
                                                   "[\f\v1,2]\t\t",
                                                   "\t\v\t[1  ,2 ],    [   1,      2,3]    ",
                                                   "          [4,5          ]"}),
                     string_value)
{
    std::string symmetryField = string_value;
    BOOST_CHECK_NO_THROW(makeGroupsOfSymmetries(symmetryField));
}

BOOST_AUTO_TEST_SUITE_END()
