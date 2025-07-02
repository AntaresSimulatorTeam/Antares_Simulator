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

#define WIN32_LEAN_AND_MEAN

#include <map>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/solver/optim-model-filler/ComponentFiller.h"

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(DimensionsSuite)

BOOST_AUTO_TEST_CASE(PartialKeyGetters)
{
    PartialKey pk("component", "variable");
    BOOST_CHECK_EQUAL(pk.getComponent(), "component");
    BOOST_CHECK_EQUAL(pk.getVariable(), "variable");
}

BOOST_AUTO_TEST_CASE(FullKeyGetters_2ArgsConstructor)
{
    FullKey k("component", "variable");
    BOOST_CHECK_EQUAL(k.getComponent(), "component");
    BOOST_CHECK_EQUAL(k.getVariable(), "variable");

    BOOST_CHECK(!k.getScenario());
    BOOST_CHECK(!k.getTimestep());
}

BOOST_AUTO_TEST_CASE(FullKeyGetters_4ArgsConstructor)
{
    FullKey k("component", "variable", 3, 4);
    BOOST_CHECK_EQUAL(k.getComponent(), "component");
    BOOST_CHECK_EQUAL(k.getVariable(), "variable");

    BOOST_CHECK_EQUAL(*k.getScenario(), 3);
    BOOST_CHECK_EQUAL(*k.getTimestep(), 4);
}

BOOST_AUTO_TEST_CASE(FullKeyCompare)
{
    FullKey k1("component", "a");
    FullKey k2("component", "a");
    BOOST_CHECK(k1 == k2);
    BOOST_CHECK(k1 <= k2);

    FullKey k3("component", "b");
    BOOST_CHECK(k3 != k1);
    BOOST_CHECK(k1 < k3);

    FullKey k4("komponent", "a");
    BOOST_CHECK(k4 != k1);
}

BOOST_AUTO_TEST_CASE(IntegerInterval_count)
{
    int count = 0;
    Antares::Optimization::IntegerInterval interval(0, 2);
    BOOST_CHECK_EQUAL(interval.size(), 3);
    for (int x: interval)
    {
        count++;
    }
    BOOST_CHECK_EQUAL(count, 3);
}

BOOST_AUTO_TEST_CASE(no_scenarios)
{
    Dimensions dim({}, Antares::Optimization::IntegerInterval(0, 2));
    BOOST_CHECK_EQUAL(dim.getNumberOfTimesteps(), 3);
}

std::pair<std::map<std::pair<int, int>, std::string>, VariableDictionary> namesFromDimensions(
  const Dimensions& dim)
{
    VariableDictionary vdict;
    std::map<std::pair<int, int>, std::string> names;
    vdict.addVariable(
      dim,
      PartialKey("component", "variable"),
      [&names](const TimeAndScenario& timeAndScenario, const std::string& name)
      {
          names[std::pair(timeAndScenario.scenario, timeAndScenario.timestep)] = name;
          return nullptr;
      });
    return {names, vdict};
}

BOOST_AUTO_TEST_CASE(addVariable_no_ts_no_sc)
{
    const auto [names, dict] = namesFromDimensions({});
    BOOST_CHECK_EQUAL(names.size(), 1);
    BOOST_CHECK_EQUAL(names.at(std::pair(0, 0)), "component.variable");

    BOOST_CHECK_NO_THROW(dict("component", "variable"));
}

BOOST_AUTO_TEST_CASE(addVariable_no_ts_multiple_sc)
{
    const auto [names, dict] = namesFromDimensions({IntegerInterval(0, 2), {}});
    BOOST_CHECK_EQUAL(names.size(), 3);
    BOOST_CHECK_EQUAL(names.at(std::pair(0, 0)), "component.variable_s0");

    BOOST_CHECK_NO_THROW(dict("component", "variable", 1, 0));
}

BOOST_AUTO_TEST_CASE(addVariable_multiple_ts_no_sc)
{
    const auto [names, dict] = namesFromDimensions({{}, IntegerInterval(0, 2)});
    BOOST_CHECK_EQUAL(names.size(), 3);
    BOOST_CHECK_EQUAL(names.at(std::pair(0, 0)), "component.variable_t0");

    BOOST_CHECK_NO_THROW(dict("component", "variable", 0, 2));
}

BOOST_AUTO_TEST_CASE(addVariable_multiple_ts_sc)
{
    const auto [names, dict] = namesFromDimensions({IntegerInterval(0, 2), IntegerInterval(0, 4)});
    BOOST_CHECK_EQUAL(names.at(std::pair(0, 0)), "component.variable_s0_t0");
    BOOST_CHECK_EQUAL(names.at(std::pair(2, 3)), "component.variable_s2_t3");
    BOOST_CHECK(!names.contains(std::pair(3, 3)));

    BOOST_CHECK_THROW(dict("component", "variable", 3, 2), std::out_of_range);
    BOOST_CHECK_THROW(dict("component", "variable", 2, 5), std::out_of_range);
}
BOOST_AUTO_TEST_SUITE_END()
