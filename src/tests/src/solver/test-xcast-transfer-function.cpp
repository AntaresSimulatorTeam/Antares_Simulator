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
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 98.5f, 0), 98);
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 0.5f, 98), 0);
    BOOST_CHECK_EQUAL(findTransferSegment(points, 100, 100.f, 98), 99);
}
