// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <utility>

namespace Antares::TSGenerator::XCast
{
// The generator historically computed these coefficients with float operands.
inline std::pair<float, float> transferFunctionCoefficients(float x0, float y0, float x1, float y1)
{
    return {(y1 - y0) / (x1 - x0), (y0 * x1 - y1 * x0) / (x1 - x0)};
}

// Returns the number of segments when the value lies outside the function's domain.
template<class Points>
unsigned int findTransferSegment(const Points& points,
                                 unsigned int pointCount,
                                 float value,
                                 unsigned int previousSegment)
{
    const unsigned int segmentCount = pointCount - 1;
    for (unsigned int i = 0; i < segmentCount; ++i)
    {
        const unsigned int segment = (i + previousSegment) % segmentCount;
        if (value >= points[segment][0] && value <= points[segment + 1][0])
        {
            return segment;
        }
    }
    return segmentCount;
}
} // namespace Antares::TSGenerator::XCast
