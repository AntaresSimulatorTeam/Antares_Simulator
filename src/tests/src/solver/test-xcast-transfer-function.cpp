// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE xcast_transfer_function

#include <array>
#include <cmath>

#include <boost/test/unit_test.hpp>

#include "transfer-function.h"

BOOST_AUTO_TEST_CASE(transfer_function_preserves_float_rounding)
{
    const auto [slope, intercept] = Antares::TSGenerator::XCast::transferFunctionCoefficients(
      74.7683334f,
      85.7018280f,
      78.9353638f,
      21.2983189f);
    const float result = slope * 76.8518524f + intercept;

    BOOST_CHECK_EQUAL(std::round(result), 53.f);
}

BOOST_AUTO_TEST_CASE(find_segment_with_maximum_number_of_points)
{
    std::array<std::array<float, 2>, 100> points{};
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        points[i][0] = static_cast<float>(i);
    }
    using Antares::TSGenerator::XCast::findTransferSegment;
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 98.5f, 0).index, 98);
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 0.5f, 98).index, 0);
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 100.f, 98).index, 99);
}

BOOST_AUTO_TEST_CASE(shared_knot_preserves_historical_search_cursor)
{
    using namespace Antares::TSGenerator::XCast;
    const std::array<std::array<float, 2>, 5> points{
      {{-10.f, 0.1f}, {1.f, 0.1f}, {2.f, 0.5f}, {3.f, 0.1f}, {10.f, 0.1f}}};
    unsigned int cursor = 0;
    auto match = findTransferSegment(points, 5, 1.5f, cursor);
    BOOST_REQUIRE_EQUAL(match.index, 1);
    cursor = match.searchOffset;
    match = findTransferSegment(points, 5, 2.5f, cursor);
    BOOST_REQUIRE_EQUAL(match.index, 2);
    cursor = match.searchOffset;
    match = findTransferSegment(points, 5, 2.f, cursor);
    BOOST_REQUIRE_EQUAL(match.index, 1);
    const auto [slope, intercept] = transferFunctionCoefficients(1.f, 0.1f, 2.f, 0.5f);
    BOOST_CHECK_EQUAL(std::round(slope * 2.f + intercept), 1.f);
    const auto [rightSlope, rightIntercept] = transferFunctionCoefficients(2.f, 0.5f, 3.f, 0.1f);
    BOOST_CHECK_EQUAL(std::round(rightSlope * 2.f + rightIntercept), 0.f);
    BOOST_CHECK_EQUAL(findTransferSegment(points, 5, 11.f, cursor).index, 4);
}

BOOST_AUTO_TEST_CASE(transfer_segment_reverse_crossing_and_endpoints)
{
    using namespace Antares::TSGenerator::XCast;
    const std::array<std::array<float, 2>, 5> points{
      {{-10.f, 0.f}, {1.f, 0.f}, {2.f, 0.5f}, {3.f, 1.f}, {10.f, 1.f}}};
    unsigned int cursor = 0;
    for (const auto& [value, expected]: std::array<std::pair<float, unsigned int>, 4>{
           {{2.5f, 2}, {1.5f, 1}, {-10.f, 0}, {10.f, 3}}})
    {
        const auto result = findTransferSegment(points, points.size(), value, cursor);
        BOOST_REQUIRE_EQUAL(result.index, expected);
        cursor = result.searchOffset;
    }
    const auto outside = findTransferSegment(points, points.size(), -11.f, cursor);
    BOOST_CHECK_EQUAL(outside.index, 4);
    BOOST_CHECK_EQUAL(outside.searchOffset, cursor);
}
