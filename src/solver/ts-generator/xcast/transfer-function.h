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

struct TransferSegment
{
    unsigned int index;
    unsigned int searchOffset;
};

// Preserve the historical search order and cursor (the offset, not the segment
// index). Skip the wraparound pair, which is not a valid function segment.
template<class Points>
TransferSegment findTransferSegment(const Points& points,
                                    unsigned int pointCount,
                                    float value,
                                    unsigned int previousOffset)
{
    if (pointCount < 2)
    {
        return {0, previousOffset};
    }
    const unsigned int segmentCount = pointCount - 1;
    for (unsigned int i = 0; i < pointCount; ++i)
    {
        const unsigned int segment = (i + previousOffset) % pointCount;
        if (segment < segmentCount && value >= points[segment][0]
            && value <= points[segment + 1][0])
        {
            return {segment, i};
        }
    }
    return {segmentCount, previousOffset};
}
} // namespace Antares::TSGenerator::XCast
