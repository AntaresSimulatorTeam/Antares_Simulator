// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE extractSTS_functions
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/common-hydro-remix.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/parts/short-term-storage/series.h"

using namespace Antares::Solver::Simulation;

constexpr double TOLERANCE = 1e-9;

BOOST_AUTO_TEST_SUITE(tests_extractSTS_functions)

BOOST_AUTO_TEST_CASE(extract_first_week_from_start)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 50.0;

    auto result = extractSTSpmax(props, 0);

    BOOST_CHECK_EQUAL(result.size(), 168);
    BOOST_CHECK_CLOSE(result[0], 50.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[167], 50.0, TOLERANCE);
}

BOOST_AUTO_TEST_CASE(extract_week_starting_at_hour_24)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 75.0;

    auto result = extractSTSpmax(props, 24);

    BOOST_CHECK_EQUAL(result.size(), 168);
    BOOST_CHECK_CLOSE(result[0], 75.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[167], 75.0, TOLERANCE);
}

BOOST_AUTO_TEST_CASE(extract_week_with_nominal_capacity_50)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    stsSeries->maxWithdrawalModulation.insert(stsSeries->maxWithdrawalModulation.end(), 162, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 50.0;

    auto result = extractSTSpmax(props, 0);

    BOOST_CHECK_CLOSE(result[0], 25.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[1], 30.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[2], 35.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[3], 40.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[4], 45.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[5], 50.0, TOLERANCE);
}

BOOST_AUTO_TEST_CASE(extract_week_with_nominal_capacity_200)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 200.0;

    auto result = extractSTSpmax(props, 0);

    BOOST_CHECK_CLOSE(result[0], 200.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[9], 200.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[19], 200.0, TOLERANCE);
}

BOOST_AUTO_TEST_CASE(extract_week_starting_at_hour_168_non_constant)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    stsSeries->maxWithdrawalModulation[168] = 0.5; // test non-constant
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 100.0;

    auto result = extractSTSpmax(props, 168);

    BOOST_CHECK_EQUAL(result.size(), 168);
    BOOST_CHECK_CLOSE(result[0], 50.0, TOLERANCE);
    BOOST_CHECK_CLOSE(result[167], 100.0, TOLERANCE);
}

BOOST_AUTO_TEST_CASE(modulation_factors_of_1_return_full_capacity)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 150.0;

    auto result = extractSTSpmax(props, 0);

    for (int i = 0; i < 168; ++i)
    {
        BOOST_CHECK_CLOSE(result[i], 150.0, TOLERANCE);
    }
}

BOOST_AUTO_TEST_CASE(modulation_factors_of_0_return_zero_capacity)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 0.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 50.0;

    auto result = extractSTSpmax(props, 0);

    for (int i = 0; i < 168; ++i)
    {
        BOOST_CHECK_CLOSE(result[i], 0.0, TOLERANCE);
    }
}

BOOST_AUTO_TEST_CASE(verify_withdrawalNominalCapacity_used_not_efficiency)
{
    PROPERTIES props;
    auto stsSeries = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    stsSeries->maxWithdrawalModulation.assign(8760, 1.0);
    props.series = stsSeries;
    props.withdrawalNominalCapacity = 80.0;
    props.withdrawalEfficiency = 0.5;

    auto result = extractSTSpmax(props, 0);

    BOOST_CHECK_CLOSE(result[0], 80.0, TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()
