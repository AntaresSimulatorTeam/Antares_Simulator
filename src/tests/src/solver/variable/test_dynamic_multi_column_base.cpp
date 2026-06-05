// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test dynamic multi column base"
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/economy/dynamic_multi_column_base.h"

using namespace Antares::Solver::Variable::Economy;

struct TestColumnDescriptor
{
    std::string caption;
    std::string unit;
};

struct StubDynamicTraits
{
    static std::string Caption()
    {
        return "STUB";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Stub for testing";
    }

    using ResultsType = Results<std::tuple<R::AllYears::Average>>;

    static constexpr uint8_t decimal = 0;

    static std::vector<ColumnDescriptor> buildColumnDescriptors([[maybe_unused]] Data::Area* area)
    {
        return {{"GROUP_A", "MWh"}, {"GROUP_B", "MWh"}, {"GROUP_C", "MWh"}};
    }

    static void setHourlyValues(
      VCardDynamicMultiColumn<StubDynamicTraits>::IntermediateValuesBaseType& pValues,
      const State& state,
      unsigned int)
    {
        for (auto& col: pValues)
        {
            col.hour[state.hourInTheYear] = 100.0;
        }
    }
};

using StubVCard = VCardDynamicMultiColumn<StubDynamicTraits>;
using StubDynamic = DynamicMultiColumnBase<StubDynamicTraits>;

BOOST_AUTO_TEST_SUITE(dynamic_multi_column_base)

BOOST_AUTO_TEST_CASE(metadata)
{
    BOOST_CHECK_EQUAL(StubVCard::Caption(), "STUB");
    BOOST_CHECK_EQUAL(StubVCard::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(StubVCard::decimal), 0);
    BOOST_CHECK_EQUAL(static_cast<int>(StubVCard::columnCount), Category::dynamicColumns);
}

BOOST_AUTO_TEST_CASE(column_descriptors)
{
    auto descriptors = StubDynamicTraits::buildColumnDescriptors(nullptr);
    BOOST_CHECK_EQUAL(descriptors.size(), 3);
    BOOST_CHECK_EQUAL(descriptors[0].caption, "GROUP_A");
    BOOST_CHECK_EQUAL(descriptors[0].unit, "MWh");
    BOOST_CHECK_EQUAL(descriptors[1].caption, "GROUP_B");
    BOOST_CHECK_EQUAL(descriptors[2].caption, "GROUP_C");
}

BOOST_AUTO_TEST_SUITE_END()
