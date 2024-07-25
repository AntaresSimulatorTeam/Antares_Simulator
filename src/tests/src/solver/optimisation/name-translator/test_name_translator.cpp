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

#define BOOST_TEST_MODULE test_name_translator
#define WIN32_LEAN_AND_MEAN

#include <typeinfo>

#include <boost/test/unit_test.hpp>

#include "antares/solver/utils/name_translator.h"

BOOST_AUTO_TEST_SUITE(translations)

BOOST_AUTO_TEST_CASE(ReturnedNullName)
{
    auto nul_names = NameTranslator::create(false);
    auto* base_ptr = nul_names.get();
    const auto& expected_type_info = typeid(NullName);
    BOOST_CHECK(expected_type_info == typeid(*base_ptr));
}

BOOST_AUTO_TEST_CASE(ReturnedRealName)
{
    auto real_names = NameTranslator::create(true);
    auto* base_ptr = real_names.get();
    const auto& expected_type_info = typeid(RealName);

    BOOST_CHECK(expected_type_info == typeid(*base_ptr));
}

BOOST_AUTO_TEST_CASE(NullNameIsNotRealName)
{
    auto real_names = NameTranslator::create(true);
    auto* base_ptr = real_names.get();
    const auto& expected_type_info = typeid(NullName);

    BOOST_CHECK(expected_type_info != typeid(*base_ptr));
}

BOOST_AUTO_TEST_CASE(NullNameWithEmptyInput)
{
    auto nul_names = NameTranslator::create(false);
    std::vector<std::string> names;
    std::vector<char*> names_ptr;
    BOOST_CHECK(nul_names->translate(names, names_ptr) == names_ptr.data());
    // size of names_ptr has not changed
    BOOST_CHECK(0 == names_ptr.size());
}

BOOST_AUTO_TEST_CASE(NullNameWith2Names)
{
    auto nul_names = NameTranslator::create(false);
    std::vector<std::string> names{"name1", "name2"};
    std::vector<char*> names_ptr;
    BOOST_CHECK(nul_names->translate(names, names_ptr) == names_ptr.data());
    // size of names_ptr should be updated to names.size()
    BOOST_CHECK(names.size() == names_ptr.size());
    BOOST_CHECK(names_ptr[0] == nullptr);
    BOOST_CHECK(names_ptr[1] == nullptr);
}

BOOST_AUTO_TEST_CASE(RealNameWithEmptyInput)
{
    auto nul_names = NameTranslator::create(true);
    std::vector<std::string> names;
    std::vector<char*> names_ptr;
    BOOST_CHECK(nul_names->translate(names, names_ptr) == names_ptr.data());
    // size of names_ptr has not changed
    BOOST_CHECK(0 == names_ptr.size());
}

BOOST_AUTO_TEST_CASE(RealNameWith2Names)
{
    auto nul_names = NameTranslator::create(true);
    std::vector<std::string> names{"name1", "name2"};
    std::vector<char*> names_ptr;
    BOOST_CHECK(nul_names->translate(names, names_ptr) == names_ptr.data());
    // size of names_ptr should be updated to names.size()
    BOOST_CHECK(names.size() == names_ptr.size());
    BOOST_CHECK(names_ptr[0] == names[0]);
    BOOST_CHECK(names_ptr[1] == names[1]);
}

BOOST_AUTO_TEST_SUITE_END()
