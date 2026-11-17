// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE xcast_transfer_function

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
