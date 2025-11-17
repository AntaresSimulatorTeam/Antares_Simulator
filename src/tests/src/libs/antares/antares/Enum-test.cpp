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

#define WINDOWS_LEAN_AND_MEAN

#define BOOST_TEST_MODULE "EnumTest"

#include <initializer_list>
#include <string>

#include <antares/enums/Enum.hpp>

namespace Antares::Data::Enum
{
enum class MyEnum
{
    Value1,
    Value2,
    Value3
};

template<>
const std::initializer_list<std::string>& getNames<MyEnum>()
{
    static const std::initializer_list<std::string> il = {"Value1", "Value2", "Value3"};
    return il;
}

std::ostream& operator<<(std::ostream& os, MyEnum e)
{
    return os << toString(e);
}
} // namespace Antares::Data::Enum

#include <boost/test/unit_test.hpp>

#include <antares/exception/AssertionError.hpp>
#include "antares/enums/Enum.hxx"

BOOST_AUTO_TEST_SUITE(EnumTest)

BOOST_AUTO_TEST_CASE(FromString)
{
    BOOST_CHECK_EQUAL(Antares::Data::Enum::fromString<Antares::Data::Enum::MyEnum>("Value1"),
                      Antares::Data::Enum::MyEnum::Value1);
    BOOST_CHECK_EQUAL(Antares::Data::Enum::fromString<Antares::Data::Enum::MyEnum>("Value2"),
                      Antares::Data::Enum::MyEnum::Value2);
    BOOST_CHECK_EQUAL(Antares::Data::Enum::fromString<Antares::Data::Enum::MyEnum>("Value3"),
                      Antares::Data::Enum::MyEnum::Value3);
}

BOOST_AUTO_TEST_CASE(FromStringInvalid)
{
    BOOST_CHECK_THROW(Antares::Data::Enum::fromString<Antares::Data::Enum::MyEnum>("Invalid"),
                      Antares::Data::AssertionError);
}

BOOST_AUTO_TEST_CASE(ToString)
{
    BOOST_CHECK_EQUAL(Antares::Data::Enum::toString(Antares::Data::Enum::MyEnum::Value1), "Value1");
    BOOST_CHECK_EQUAL(Antares::Data::Enum::toString(Antares::Data::Enum::MyEnum::Value2), "Value2");
    BOOST_CHECK_EQUAL(Antares::Data::Enum::toString(Antares::Data::Enum::MyEnum::Value3), "Value3");
}

BOOST_AUTO_TEST_CASE(ToStringInvalid)
{
    BOOST_CHECK_THROW(Antares::Data::Enum::toString(static_cast<Antares::Data::Enum::MyEnum>(42)),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(EnumList)
{
    const auto list = Antares::Data::Enum::enumList<Antares::Data::Enum::MyEnum>();
    std::list<Antares::Data::Enum::MyEnum> expected = {Antares::Data::Enum::MyEnum::Value1,
                                                       Antares::Data::Enum::MyEnum::Value2,
                                                       Antares::Data::Enum::MyEnum::Value3};
    BOOST_CHECK_EQUAL_COLLECTIONS(list.begin(), list.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_SUITE_END()
