// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/ExpressionsNodes.h"

namespace Antares::Expressions::Visitors
{
class EvalVisitor; // forward declaration to avoid include cycles
class VariabilityVisitor;

// Normalize a possibly negative or out-of-range time index into [0, size).
int normalizeTimeIndex(int timeIndex, int size);

// Decomposition of the window [from, to] over a cyclic block of `size` steps:
// the window covers every index of the block `fullCycles` times, plus the `remainder` indices
// starting at `first` (wrapping around the block), once each.
// A window that is not longer than the block has fullCycles == 0, so that its indices are summed
// in window order.
struct TimeSumWindow
{
    long long fullCycles = 0;
    long long remainder = 0;
    int first = 0;
};

// Empty window (from > to, or size <= 0) gives fullCycles == 0 and remainder == 0.
inline TimeSumWindow splitTimeSumWindow(int from, int to, int size)
{
    if (from > to || size <= 0)
    {
        return {};
    }
    // 64-bit arithmetic: to - from + 1 can exceed INT_MAX
    const long long length = static_cast<long long>(to) - from + 1;
    const int first = normalizeTimeIndex(from, size);
    if (length <= size)
    {
        return {0, length, first};
    }
    return {length / size, length % size, first};
}

// Index of the offset-th step after `first`, wrapped on [0, size). 64-bit: first + offset can
// exceed INT_MAX.
inline int wrapTimeSumIndex(int first, long long offset, int size)
{
    return static_cast<int>((static_cast<long long>(first) + offset) % size);
}

// Resolve a bound used in a time-sum when no variability check is required.
int resolveTimeSumBound(const Nodes::Node* bound, EvalVisitor& visitor, unsigned localTimeStep);

// Check that a time-sum bound is constant in time.
void checkTimeSumBoundVariability(const Nodes::Node* bound, VariabilityVisitor& variabilityVisitor);

} // namespace Antares::Expressions::Visitors
