// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/InactiveComponentsAnalyzerBuilder.h"

#include "in-memory-study.h"

using Antares::Optimization::BuildInactiveComponentsAnalyzer;

namespace
{
struct AnalyzerBuilderFixture: public StudyBuilder
{
    AnalyzerBuilderFixture();

    Area* area = nullptr;
};

AnalyzerBuilderFixture::AnalyzerBuilderFixture()
{
    simulationBetweenDays(0, 7);
    area = addAreaToStudy("area");
}
} // namespace

BOOST_FIXTURE_TEST_SUITE(InactiveComponentsAnalyzerBuilder_tests, AnalyzerBuilderFixture)

// A load series can have several chronicles (columns). Only the ones actually
// drawn by the scenario builder / TS-number generator for this study's MC
// years should matter for the "is this component active" decision - a
// non-zero chronicle that's never selected must not keep the area active.
BOOST_AUTO_TEST_CASE(load_is_all_zero_when_only_the_selected_chronicle_is_zero)
{
    setNumberMCyears(2);

    TimeSeriesConfigurer loadTS(area->load.series);
    loadTS.setDimensions(2).fillColumnWith(0, 42.0).fillColumnWith(1, 0.0);

    // Both MC years select the all-zero column (1), even though column 0 is non-zero.
    area->load.series.timeseriesNumbers[0] = 1;
    area->load.series.timeseriesNumbers[1] = 1;

    const auto analyzer = BuildInactiveComponentsAnalyzer(*study);

    BOOST_CHECK(analyzer->loadIsAllZero(0));
}

BOOST_AUTO_TEST_CASE(load_is_not_all_zero_when_a_selected_chronicle_is_non_zero)
{
    setNumberMCyears(2);

    TimeSeriesConfigurer loadTS(area->load.series);
    loadTS.setDimensions(2).fillColumnWith(0, 42.0).fillColumnWith(1, 0.0);

    // Year 0 selects the non-zero column, year 1 selects the all-zero one.
    area->load.series.timeseriesNumbers[0] = 0;
    area->load.series.timeseriesNumbers[1] = 1;

    const auto analyzer = BuildInactiveComponentsAnalyzer(*study);

    BOOST_CHECK(!analyzer->loadIsAllZero(0));
}

BOOST_AUTO_TEST_SUITE_END()
