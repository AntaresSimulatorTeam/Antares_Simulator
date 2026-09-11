// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE miscgen_names
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <string_view>

#include <boost/test/unit_test.hpp>

#include <antares/study/area/constants.h>

using namespace Antares::Data;

BOOST_AUTO_TEST_SUITE(MiscGenComponentNames)

BOOST_AUTO_TEST_CASE(names_are_ordered_as_the_enum)
{
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhCHP], std::string_view("combined_heat_power"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhBioMass], std::string_view("biomass"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhBioGaz], std::string_view("biogas"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhWaste], std::string_view("waste"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhGeoThermal], std::string_view("geothermal"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhOther], std::string_view("other"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhPSP], std::string_view("pumped_storage_power"));
    BOOST_CHECK_EQUAL(miscGenComponentNames[fhhRowBalance], std::string_view("rest_world"));
}

BOOST_AUTO_TEST_CASE(suffix_carries_the_leading_underscore)
{
    BOOST_CHECK_EQUAL(miscGenComponentSuffix(fhhCHP), std::string("_combined_heat_power"));
    BOOST_CHECK_EQUAL(miscGenComponentSuffix(fhhPSP), std::string("_pumped_storage_power"));
    BOOST_CHECK_EQUAL(miscGenComponentSuffix(fhhRowBalance), std::string("_rest_world"));
}

BOOST_AUTO_TEST_CASE(component_id_is_area_plus_name)
{
    BOOST_CHECK_EQUAL(miscGenComponentId("france", fhhCHP),
                      std::string("france_combined_heat_power"));
    BOOST_CHECK_EQUAL(miscGenComponentId("france", fhhPSP),
                      std::string("france_pumped_storage_power"));
}

BOOST_AUTO_TEST_SUITE_END()
